#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#include "mush.h"
#include "debug.h"

/*
 * This is the "jobs" module for Mush.
 * It maintains a table of jobs in various stages of execution, and it
 * provides functions for manipulating jobs.
 * Each job contains a pipeline, which is used to initialize the processes,
 * pipelines, and redirections that make up the job.
 * Each job has a job ID, which is an integer value that is used to identify
 * that job when calling the various job manipulation functions.
 *
 * At any given time, a job will have one of the following status values:
 * "new", "running", "completed", "aborted", "canceled".
 * A newly created job starts out in with status "new".
 * It changes to status "running" when the processes that make up the pipeline
 * for that job have been created.
 * A running job becomes "completed" at such time as all the processes in its
 * pipeline have terminated successfully.
 * A running job becomes "aborted" if the last process in its pipeline terminates
 * with a signal that is not the result of the pipeline having been canceled.
 * A running job becomes "canceled" if the jobs_cancel() function was called
 * to cancel it and in addition the last process in the pipeline subsequently
 * terminated with signal SIGKILL.
 *
 * In general, there will be other state information stored for each job,
 * as required by the implementation of the various functions in this module.
 */
typedef enum job_status
{
    NEW,
    RUNNNING,
    COMPLETED,
    ABORTED,
    CANCELED
} job_status;

typedef struct job
{
    int id;
    int pgid;
    job_status status;
    int term;
    PIPELINE *pline;
    struct job *next;
    struct job *prev;
} job;

struct job_table
{
    job *head;
    int count;
} job_table;

void show_job_status(FILE *f, job *j)
{
    fputc('<', f);
    switch (j->status)
    {
    case 0:
        fputs("new", f);
        break;
    case 1:
        fputs("running", f);
        break;
    case 2:
        fputs("completed", f);
        break;
    case 3:
        fputs("aborted", f);
        break;
    case 4:
        fputs("canceled", f);
        break;
    default:
        printf("show_job_status FAILURE!");
        abort();
    }
    fputs(">\t", f);
    fflush(f);
}

job *get_job(int jobid)
{
    if (!job_table.head)
        return NULL;
    for (job *curr = job_table.head; curr; curr = curr->next)
    {
        if (jobid == curr->id)
            return curr;
    }
    return NULL;
}

job *get_child(job *parent, job *head)
{
    if (!head)
        head = job_table.head;
    for (job *curr = head; curr; curr = curr->next)
    {
        if (parent->id == curr->pgid)
            return curr;
    }
    return NULL;
}

void add_job(job *j)
{
    job_table.count++;
    if (!job_table.head)
    {
        job_table.head = j;
        return;
    }
    job *former_head = job_table.head;
    j->next = former_head;
    former_head->prev = j;
    job_table.head = j;
}
/**
 * @brief  Initialize the jobs module.
 * @details  This function is used to initialize the jobs module.
 * It must be called exactly once, before any other functions of this
 * module are called.
 *
 * @return 0 if initialization is successful, otherwise -1.
 */
int jobs_init(void)
{
    // job_table.head->id = getpid();
    // // job_table.head->pg.id = getppid();
    // job_table.head->status = RUNNNING;
    // job_table.count = 1;
    return 0;
}

/**
 * @brief  Finalize the jobs module.
 * @details  This function is used to finalize the jobs module.
 * It must be called exactly once when job processing is to be terminated,
 * before the program exits.  It should cancel all jobs that have not
 * yet terminated, wait for jobs that have been cancelled to terminate,
 * and then expunge all jobs before returning.
 *
 * @return 0 if finalization is completely successful, otherwise -1.
 */
int jobs_fini(void)
{
    int a;
    for (job *curr = job_table.head->next; curr; curr = curr->next)
    {
        if (curr->status < COMPLETED)
        {
            a = jobs_cancel(curr->id);
            if (a)
                return -1;
            a = jobs_wait(curr->id);
            if (a)
                return -1;
        }
        a = jobs_expunge(curr->id);
        if (a)
            return -1;
    }
    return 0;
}

/**
 * @brief  Print the current jobs table.
 * @details  This function is used to print the current contents of the jobs
 * table to a specified output stream.  The output should consist of one line
 * per existing job.  Each line should have the following format:
 *
 *    <jobid>\t<pgid>\t<status>\t<pipeline>
 *
 * where <jobid> is the numeric job ID of the job, <status> is one of the
 * following strings: "new", "running", "completed", "aborted", or "canceled",
 * and <pipeline> is the job's pipeline, as printed by function show_pipeline()
 * in the syntax module.  The \t stand for TAB characters.
 *
 * @param file  The output stream to which the job table is to be printed.
 * @return 0  If the jobs table was successfully printed, -1 otherwise.
 */
int jobs_show(FILE *file)
{
    if (!job_table.count)
        return 0;
    int count = -1;
    for (job *curr = job_table.head; curr; curr = curr->next, count++)
    {
        fprintf(file, "<%d>\t<%d>\t", curr->id, curr->pgid);
        show_job_status(file, curr);
        fputc('<', file);
        show_pipeline(file, curr->pline);
        fputc('<', file);
        if (curr->next)
            fputc('\n', file);
    }
    fflush(file);
    if (count == job_table.count)
        return 0;
    return -1;
}

/**
 * @brief  Create a new job to run a pipeline.
 * @details  This function creates a new job and starts it running a specified
 * pipeline.  The pipeline will consist of a "leader" process, which is the direct
 * child of the process that calls this function, plus one child of the leader
 * process to run each command in the pipeline.  All processes in the pipeline
 * should have a process group ID that is equal to the process ID of the leader.
 * The leader process should wait for all of its children to terminate before
 * terminating itself.  The leader should return the exit status of the process
 * running the last command in the pipeline as its own exit status, if that
 * process terminated normally.  If the last process terminated with a signal,
 * then the leader should terminate via SIGABRT.
 *
 * If the "capture_output" flag is set for the pipeline, then the standard output
 * of the last process in the pipeline should be redirected to be the same as
 * the standard output of the pipeline leader, and this output should go via a
 * pipe to the main Mush process, where it should be read and saved in the data
 * store as the value of a variable, as described in the assignment handout.
 * If "capture_output" is not set for the pipeline, but "output_file" is non-NULL,
 * then the standard output of the last process in the pipeline should be redirected
 * to the specified output file.   If "input_file" is set for the pipeline, then
 * the standard input of the process running the first command in the pipeline should
 * be redirected from the specified input file.
 *
 * @param pline  The pipeline to be run.  The jobs module expects this object
 * to be valid for as long as it requires, and it expects to be able to free this
 * object when it is finished with it.  This means that the caller should not pass
 * a pipeline object that is shared with any other data structure, but rather should
 * make a copy to be passed to this function.
 *
 * @return  -1 if the pipeline could not be initialized properly, otherwise the
 * value returned is the job ID assigned to the pipeline.
 */
int jobs_run(PIPELINE *pline)
{
    // job *leader = calloc(1, sizeof(job));
    // add_job(leader);
    // leader->pline = pline;
    // leader->status = RUNNNING;
    // int leader_to_main_pipe[2];
    // if (pipe(leader_to_main_pipe) < 0)
    // {
    //     printf("error piping");
    //     return -1;
    // }
    // if ((leader->id = fork()) == 0)
    // { // this goes in to leader (main process' child)
    //     // leader->pgid = leader->id;
    //     close(leader_to_main_pipe[0]);             // close read
    //     job *first_child = calloc(1, sizeof(job)); // allocate the first child job
    //     add_job(first_child);                      // add it to the job table
    //     first_child->pgid = getpid();              // set its pgid to be the leader process id
    //     first_child->status = RUNNNING;            // set it to running
    //     job *next_child;                           // declare the next_child
    //     if (!pline->commands->next)                // if there is going to be a second commands
    //     {
    //         next_child = calloc(1, sizeof(job));             // allocate the second child job
    //         add_job(next_child);                             // add the second job to the table
    //         next_child->pgid = getpid();                     // set its pgid to the leader id
    //         next_child->pline = calloc(1, sizeof(PIPELINE)); // allocate its pipeline to connect it to the first child
    //         int first_child_pipe[2];                         // declare an array for fd for pipe
    //         if (pipe(first_child_pipe) < 0)                  // create a pipe
    //         {
    //             printf("pipe error");
    //             return -1;
    //         }

    //         next_child->pline->input_file = first_child_pipe[0]; // set the next childs input file to be the read side of the pipe

    //         if (dup2(first_child_pipe[1], STDOUT_FILENO) < 0) // set the output to go to the read side of the pipe
    //         {
    //             printf("dup2 error");
    //             return -1;
    //         }
    //     }
    //     if ((first_child->id = fork()) == 0) // fork in to the first child process
    //     {                                    // this goes in to the leaders child (main process' grandchild)

    //         if (pline->input_file) // if there is an input file
    //         {
    //             FILE *input = open(pline->input_file, "r"); // open the input file
    //             if (dup2(input, STDIN_FILENO) < 0)          // set the input to read from the input file
    //             {
    //                 printf("dup2 error");
    //                 return -1;
    //             };
    //         }
    //         if (!pline->commands->next && !pline->capture_output && pline->output_file) // if this is the last command and there is an output file
    //         {
    //             FILE *output = open(pline->output_file, "w"); // open the output file
    //             if (dup2(output, STDOUT_FILENO) < 0)          // set the output to go to the output file
    //             {
    //                 printf("dup2 error");
    //                 return -1;
    //             }
    //         }
    //         char *command = pline->commands->args; // get the first arg (which is the actual command eg. echo)
    //         char **argv = pline->commands->args;   // get all arg for that command (eg. ["ls", "-l"])
    //         if (execvp(command, argv) < 0)         // execute the command
    //         {
    //             printf("FAILURE");
    //         }
    //     }
    //     for (COMMAND *cmd = pline->commands->next; cmd; cmd = cmd->next) // loop through the commands starting at the second command
    //     {                                                                // loop through each command to branch from leader and execute
    //         job *child = next_child;                                     // set the current child to be the next from the previous loop or the second command if this is the first loop
    //         if (cmd->next)                                               // if this is not the last command
    //         {
    //             next_child = calloc(1, sizeof(job));             // allocate a job for the next command
    //             add_job(next_child);                             // add it the next child to the job table
    //             next_child->pgid = getpid();                     // set its pgid to be the leader process id
    //             next_child->pline = calloc(1, sizeof(PIPELINE)); // allocate the pipeline for the next child to connect the current child to the next child
    //             int child_pipe[2];                               // declare fd for the pipe
    //             if (dup2(child->pline->input_file, STDIN_FILENO) < 0)
    //             { // set the input of to be the output from the previous child
    //                 printf("dup2 error");
    //                 return -1;
    //             }
    //             if (pipe(child_pipe) < 0) // pipe from current child to next child
    //             {
    //                 printf("pipe error");
    //                 return -1;
    //             }
    //             child->pline->output_file = child_pipe[1];              // set the current childs output to be the write side of the pipe to the next child
    //             next_child->pline->input_file = child_pipe[0];          // set the next childs input to be the read side of the pipe that connects current to next child
    //             if (dup2(child->pline->output_file, STDOUT_FILENO) < 0) // set the write side of the pipe to be the output
    //             {
    //                 printf("dup2 error");
    //                 return -1;
    //             }
    //         }

    //         // if (write(leader_to_main_pipe, child, sizeof(job)) < 0)
    //         //     return -1;
    //         child->status = RUNNNING; // set childs status to running

    //         if ((child->id = fork()) == 0)
    //         { // this goes in to the leaders child (main process' grandchild)
    //             if (!child->next && !pline->capture_output && pline->output_file)
    //             {
    //                 FILE *output = open(pline->output_file, "w");
    //                 if (dup2(output, STDOUT_FILENO) < 0)
    //                 {
    //                     printf("dup2 error");
    //                     return -1;
    //                 }
    //             }
    //             char *command = cmd->args;
    //             char *argv[] = cmd->args;
    //             if (execvp(cmd->args, cmd->args) < 0)
    //             {
    //                 printf("FAILURE");
    //             }
    //         }
    //         else
    //         {
    //         }
    //     }
    // }
    // else
    // { // stay within main process
    //     close(leader_to_main_pipe[1]);
    //     job *j;
    //     if (read(leader_to_main_pipe[0], j, sizeof(job)))
    //         ;
    //     add_job(j);
    //     // free_pipeline(pline);
    // }
    abort();
}

/**
 * @brief  Wait for a job to terminate.
 * @details  This function is used to wait for the job with a specified job ID
 * to terminate.  A job has terminated when it has entered the COMPLETED, ABORTED,
 * or CANCELED state.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(),
 * or -1 if any error occurs that makes it impossible to wait for the specified job.
 */
int jobs_wait(int jobid)
{
    job *j = get_job(jobid);
    if (!j)
        return -1;
    if (j->status >= COMPLETED)
        return j->pgid;

    for (job *child = get_child(j, NULL); child; child = get_child(j, child))
    {
        if (jobs_wait(child->id) < 0)
            return -1;
    }
    waitpid(j->id, &j->term, 0);
    if (WEXITSTATUS(j->term))
        j->status = COMPLETED;
    // else if ()
    return j->term;
}

/**
 * @brief  Poll to find out if a job has terminated.
 * @details  This function is used to poll whether the job with the specified ID
 * has terminated.  This is similar to jobs_wait(), except that this function returns
 * immediately without waiting if the job has not yet terminated.
 *
 * @param  jobid  The job ID of the job to wait for.
 * @return  the exit status of the job leader, as returned by waitpid(), if the job
 * has terminated, or -1 if the job has not yet terminated or if any other error occurs.
 */
int jobs_poll(int jobid)
{
    job *j = get_job(jobid);
    int wstatus;
    if (!j)
        return -1;
    if (j->status < COMPLETED)
        return j->pgid;
    else
        return waitpid(j->id, &wstatus, 0);
    return -1;
}

/**
 * @brief  Expunge a terminated job from the jobs table.
 * @details  This function is used to expunge (remove) a job that has terminated from
 * the jobs table, so that space in the table can be used to start some new job.
 * In order to be expunged, a job must have terminated; if an attempt is made to expunge
 * a job that has not yet terminated, it is an error.  Any resources (exit status,
 * open pipes, captured output, etc.) that were being used by the job are finalized
 * and/or freed and will no longer be available.
 *
 * @param  jobid  The job ID of the job to expunge.
 * @return  0 if the job was successfully expunged, -1 if the job could not be expunged.
 */
int jobs_expunge(int jobid)
{
    job *j = get_job(jobid);
    job_table.count--;
    job *prev = j->prev;
    job *next = j->next;
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
    free_pipeline(j->pline);
    free(j);
    return 0;
}

/**
 * @brief  Attempt to cancel a job.
 * @details  This function is used to attempt to cancel a running job.
 * In order to be canceled, the job must not yet have terminated and there
 * must not have been any previous attempt to cancel the job.
 * Cancellation is attempted by sending SIGKILL to the process group associated
 * with the job.  Cancellation becomes successful, and the job actually enters the canceled
 * state, at such subsequent time as the job leader terminates as a result of SIGKILL.
 * If after attempting cancellation, the job leader terminates other than as a result
 * of SIGKILL, then cancellation is not successful and the state of the job is either
 * COMPLETED or ABORTED, depending on how the job leader terminated.
 *
 * @param  jobid  The job ID of the job to cancel.
 * @return  0 if cancellation was successfully initiated, -1 if the job was already
 * terminated, a previous attempt had been made to cancel the job, or any other
 * error occurred.
 */
int jobs_cancel(int jobid)
{
    // TO BE IMPLEMENTED
    abort();
}

/**
 * @brief  Get the captured output of a job.
 * @details  This function is used to retrieve output that was captured from a job
 * that has terminated, but that has not yet been expunged.  Output is captured for a job
 * when the "capture_output" flag is set for its pipeline.
 *
 * @param  jobid  The job ID of the job for which captured output is to be retrieved.
 * @return  The captured output, if the job has terminated and there is captured
 * output available, otherwise NULL.
 */
char *jobs_get_output(int jobid)
{
    // TO BE IMPLEMENTED
    abort();
}

/**
 * @brief  Pause waiting for a signal indicating a potential job status change.
 * @details  When this function is called it blocks until some signal has been
 * received, at which point the function returns.  It is used to wait for a
 * potential job status change without consuming excessive amounts of CPU time.
 *
 * @return -1 if any error occurred, 0 otherwise.
 */
int jobs_pause(void)
{
    // TO BE IMPLEMENTED
    abort();
}

#include <stdlib.h>
#include <stdio.h>

#include "mush.h"
#include "debug.h"

/********************************GLOBAL VARIABLES******************************************/

typedef struct STMT_item
{
    STMT *value;
    struct STMT_item *next;
    struct STMT_item *prev;

} STMT_item;

struct
{
    int pc;
    STMT_item *sentinell;
} program_store;

/********************************MY FUNCTIONS******************************************/
STMT_item *get_stmt(int pos)
{
    if (program_store.sentinell->next == program_store.sentinell)
        return NULL;
    STMT_item *min;
    int min_val;
    int first_loop = 1;
    for (STMT_item *curr = program_store.sentinell->next; curr != program_store.sentinell; curr = curr->next)
        if (curr->value->lineno >= pos && (first_loop || curr->value->lineno < min_val))
        {
            first_loop = 0;
            min = curr;
            min_val = min->value->lineno;
        }
    if (!first_loop)
        return min;
    return NULL;
}

void program_store_init()
{
    if (!program_store.sentinell)
    {
        program_store.sentinell = calloc(1, sizeof(STMT_item));
        program_store.sentinell->next = program_store.sentinell->prev = program_store.sentinell;
    }
}
static int firstrun;
/********************************REQUIRED FUNCTIONS******************************************/
/*
 * This is the "program store" module for Mush.
 * It maintains a set of numbered statements, along with a "program counter"
 * that indicates the current point of execution, which is either before all
 * statements, after all statements, or in between two statements.
 * There should be no fixed limit on the number of statements that the program
 * store can hold.
 */

/**
 * @brief  Output a listing of the current contents of the program store.
 * @details  This function outputs a listing of the current contents of the
 * program store.  Statements are listed in increasing order of their line
 * number.  The current position of the program counter is indicated by
 * a line containing only the string "-->" at the current program counter
 * position.
 *
 * @param out  The stream to which to output the listing.
 * @return  0 if successful, -1 if any error occurred.
 */
int prog_list(FILE *out)
{
    program_store_init();
    int arrow = 0;
    if (!out)
        return -1;
    if (program_store.sentinell->next == program_store.sentinell)
        return 0;
    if (!program_store.pc)
    {
        arrow = 1;
        fputs("-->\n", out);
    }
    // show_stmt(out, program_store.sentinell->next->value);
    int prev = 0;
    for (STMT_item *curr = get_stmt(1); curr; curr = get_stmt(curr->value->lineno + 1))
    {
        if ((prev > program_store.pc && curr->value->lineno <= program_store.pc))
        {
            arrow = 1;
            fputs("-->\n", out);
        }
        show_stmt(out, curr->value);
        prev = curr->value->lineno;
        // if (!get_stmt(curr->value->lineno + 1))
        // {
        //     if (!arrow)
        //         fputs("-->\n", out);
        //     debug("prog list error");
        //     return -1;
        // }
    }
    if (!arrow)
        fputs("-->\n", out);
    return 0;
}

/**
 * @brief  Insert a new statement into the program store.
 * @details  This function inserts a new statement into the program store.
 * The statement must have a line number.  If the line number is the same as
 * that of an existing statement, that statement is replaced.
 * The program store assumes the responsibility for ultimately freeing any
 * statement that is inserted using this function.
 * Insertion of new statements preserves the value of the program counter:
 * if the position of the program counter was just before a particular statement
 * before insertion of a new statement, it will still be before that statement
 * after insertion, and if the position of the program counter was after all
 * statements before insertion of a new statement, then it will still be after
 * all statements after insertion.
 *
 * @param stmt  The statement to be inserted.
 * @return  0 if successful, -1 if any error occurred.
 */
int prog_insert(STMT *stmt)
{
    debug("prog_insert()");
    program_store_init();

    STMT_item *s = calloc(1, sizeof(STMT_item));
    s->value = stmt;
    // free_stmt(stmt);
    if (program_store.sentinell->next == program_store.sentinell)
    {
        s->next = s->prev = program_store.sentinell;
        program_store.sentinell->next = program_store.sentinell->prev = s;
    }
    else
    {
        for (STMT_item *curr = program_store.sentinell->next; curr != program_store.sentinell; curr = curr->next)
        {
            if (s->value->lineno > curr->value->lineno)
            {
                STMT_item *next = curr->next;
                curr->next = s;
                s->prev = curr;
                s->next = next;
                next->prev = s;
                return 0;
            }
            else if (s->value->lineno == curr->value->lineno)
            {
                STMT_item *prev = curr->prev;
                STMT_item *next = curr->next;
                prog_delete(curr->value->lineno, curr->value->lineno);
                prev->next = s;
                s->prev = prev;
                next->prev = s;
                s->next = next;
                return 0;
            }
        }
        s->prev = program_store.sentinell->prev;
        s->next = program_store.sentinell;
        program_store.sentinell->prev->next = s;
        program_store.sentinell->prev = s;
    }
    return 0;
}

/**
 * @brief  Delete statements from the program store.
 * @details  This function deletes from the program store statements whose
 * line numbers fall in a specified range.  Any deleted statements are freed.
 * Deletion of statements preserves the value of the program counter:
 * if before deletion the program counter pointed to a position just before
 * a statement that was not among those to be deleted, then after deletion the
 * program counter will still point the position just before that same statement.
 * If before deletion the program counter pointed to a position just before
 * a statement that was among those to be deleted, then after deletion the
 * program counter will point to the first statement beyond those deleted,
 * if such a statement exists, otherwise the program counter will point to
 * the end of the program.
 *
 * @param min  Lower end of the range of line numbers to be deleted.
 * @param max  Upper end of the range of line numbers to be deleted.
 */
int prog_delete(int min, int max)
{
    debug("prog_delete()");
    program_store_init();
    if (program_store.sentinell->next == program_store.sentinell)
        return 0;
    int maxitem = 0;
    for (STMT_item *curr = program_store.sentinell->next; curr != program_store.sentinell; curr = curr->next)
    {
        if (curr->value->lineno >= min && curr->value->lineno <= max)
        {
            STMT_item *prev = curr->prev;
            STMT_item *next = curr->next;
            prev->next = next;
            next->prev = prev;
            free_stmt(curr->value);
            free(curr);
            curr = prev;
        }
        else if (curr->value->lineno < min)
        {
            if (curr->value->lineno > maxitem)
                maxitem = curr->value->lineno;
            //     if (program_store.pc < max && program_store.pc > min)
            //         program_store.pc = curr->value->lineno;
            //     break;
        }
    }
    if (program_store.pc < max && program_store.pc > min)
        program_store.pc = maxitem;
    return 0;
}

/**
 * @brief  Reset the program counter to the beginning of the program.
 * @details  This function resets the program counter to point just
 * before the first statement in the program.
 */
void prog_reset(void)
{
    debug("prog_reset()");
    program_store_init();
    firstrun = 1;
    program_store.pc = 0;
}

/**
 * @brief  Fetch the next program statement.
 * @details  This function fetches and returns the first program
 * statement after the current program counter position.  The program
 * counter position is not modified.  The returned pointer should not
 * be used after any subsequent call to prog_delete that deletes the
 * statement from the program store.
 *
 * @return  The first program statement after the current program
 * counter position, if any, otherwise NULL.
 */
STMT *prog_fetch(void)
{

    program_store_init();
    STMT_item *s;
    s = get_stmt(program_store.pc);
    if (!s)
        return NULL;
    debug("prog_fetch(): %d", s->value->lineno);
    return s->value;
}

/**
 * @brief  Advance the program counter to the next existing statement.
 * @details  This function advances the program counter by one statement
 * from its original position and returns the statement just after the
 * new position.  The returned pointer should not be used after any
 * subsequent call to prog_delete that deletes the statement from the
 * program store.
 *
 * @return The first program statement after the new program counter
 * position, if any, otherwise NULL.
 */
STMT *prog_next()
{
    debug("prog_next()");
    program_store_init();
    STMT *pc = prog_fetch();
    STMT_item *new_pc = get_stmt((program_store.pc + 1));
    if (!new_pc || !new_pc->value)
    {
        program_store.pc++;
        return pc;
    }
    if (firstrun)
    {
        STMT_item *new_pc_next = get_stmt((new_pc->value->lineno + 1));
        program_store.pc = new_pc_next->value->lineno;
        firstrun = 0;
    }
    else
    {
        program_store.pc = new_pc->value->lineno;
    }
    return new_pc->value;
}
/*
 * @brief  Perform a "go to" operation on the program store.
 * @details  This function performs a "go to" operation on the program
 * store, by resetting the program counter to point to the position just
 * before the statement with the specified line number.
 * The statement pointed at by the new program counter is returned.
 * If there is no statement with the specified line number, then no
 * change is made to the program counter and NULL is returned.
 * Any returned statement should only be regarded as valid as long
 * as no calls to prog_delete are made that delete that statement from
 * the program store.
 *
 * @return  The statement having the specified line number, if such a
 * statement exists, otherwise NULL.
 */
STMT *prog_goto(int lineno)
{
    program_store_init();
    STMT_item *new_pc = get_stmt(lineno);
    if (!new_pc || !new_pc->prev || new_pc->value->lineno != lineno)
        return NULL;
    program_store.pc = lineno;
    debug("prog_goto(%d)", program_store.pc);
    int prev = 0;
    STMT_item *prev_item;
    for (STMT_item *curr = get_stmt(1); curr; curr = get_stmt(curr->value->lineno + 1))
    {
        if (curr->value->lineno <= lineno && curr->value->lineno > prev)
        {
            prev_item = curr;
            prev = prev_item->value->lineno;
        }
    }
    // if (!prev_item->value)
    // {
    //     return new_pc->value;
    // }
    return prev_item->value;
}
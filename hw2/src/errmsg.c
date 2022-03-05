/*********************/
/* errmsg.c          */
/* for Par 3.20      */
/* Copyright 1993 by */
/* Adam M. Costello  */
/*********************/

/* This is ANSI C code. */

#include "errmsg.h" /* Makes sure we're consistent with the declarations. */

// char errmsg[163];

char *outofmem = "Out of memory.\n";

static char *errormsg;
static int error = 0;
void set_error(char *msg)
{
    errormsg = calloc(1, sizeof(msg));
    strcpy(errormsg, msg);
    error = 1;
}

int is_error()
{
    return error;
}

int report_error(FILE *file)
{
    if (!file)
        return 1;
    if (is_error())
    {
        fprintf(file, "%s", errormsg);
        clear_error();
        exit(EXIT_FAILURE);
    }
    else
        return 0;
}

void clear_error()
{
    error = 0;
    free(errormsg);
}

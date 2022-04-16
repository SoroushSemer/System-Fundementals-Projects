#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntax.h"
#include "debug.h"
/*
 * This is the "data store" module for Mush.
 * It maintains a mapping from variable names to values.
 * The values of variables are stored as strings.
 * However, the module provides functions for setting and retrieving
 * the value of a variable as an integer.  Setting a variable to
 * an integer value causes the value of the variable to be set to
 * a string representation of that integer.  Retrieving the value of
 * a variable as an integer is possible if the current value of the
 * variable is the string representation of an integer.
 */

typedef struct variable
{
    char *name;
    VALUE_TYPE type;
    union
    {
        long integer;
        char *string;
    } value;
    struct variable *next;
} variable;

struct data_store
{
    variable *first;
} data_store;
/**
 * @brief  Get the current value of a variable as a string.
 * @details  This function retrieves the current value of a variable
 * as a string.  If the variable has no value, then NULL is returned.
 * Any string returned remains "owned" by the data store module;
 * the caller should not attempt to free the string or to use it
 * after any subsequent call that would modify the value of the variable
 * whose value was retrieved.  If the caller needs to use the string for
 * an indefinite period, a copy should be made immediately.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @return  A string that is the current value of the variable, if any,
 * otherwise NULL.
 */
char *store_get_string(char *var)
{
    debug("store_get_string()");
    if (!var)
        return NULL;
    if (!data_store.first)
        return NULL;
    for (variable *curr = data_store.first; curr; curr = curr->next)
    {
        int same = 1;
        int i;
        for (i = 0; var[i] != '\0'; i++)
        {
            if (curr->name[i] != var[i])
            {
                same = 0;
                break;
            }
        }
        if (same && var[i] == curr->name[i])
        {
            if (curr->type == NUM_VALUE_TYPE)
            {
                char *s = calloc(64, sizeof(char));
                snprintf(s, 64, "%ld", curr->value.integer);
                debug("IM returning : %s which is supposed to be %ld", s, curr->value.integer);
                return s;
            }
            else if (curr->type == STRING_VALUE_TYPE)
            {
                return curr->value.string;
            }
        }
    }
    return NULL;
}

/**
 * @brief  Get the current value of a variable as an integer.
 * @details  This retrieves the current value of a variable and
 * attempts to interpret it as an integer.  If this is possible,
 * then the integer value is stored at the pointer provided by
 * the caller.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @param  valp  Pointer at which the returned value is to be stored.
 * @return  If the specified variable has no value or the value
 * cannot be interpreted as an integer, then -1 is returned,
 * otherwise 0 is returned.
 */
int store_get_int(char *var, long *valp)
{
    debug("store_get_int()");
    if (!var)
        return -1;
    if (!data_store.first)
        return -1;
    for (variable *curr = data_store.first; curr; curr = curr->next)
    {
        int same = 1;
        int i;
        for (i = 0; var[i] != '\0'; i++)
        {
            if (curr->name[i] != var[i])
            {
                same = 0;
                break;
            }
        }
        if (same && var[i] == curr->name[i])
        {
            if (curr->type == NUM_VALUE_TYPE)
            {
                *valp = curr->value.integer;
                return 0;
            }
            else if (curr->type == STRING_VALUE_TYPE)
            {
                int len = strlen(curr->value.string);
                if (atoi(curr->value.string))
                {
                    *valp = atoi(curr->value.string);
                    return 0;
                }
                else if (len == 1 && curr->value.string[0] == '0')
                {
                    *valp = 0;
                    return 0;
                }
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}

/**
 * @brief  Set the value of a variable as an integer.
 * @details  This function sets the current value of a specified
 * variable to be a specified integer.  If the variable already
 * has a value, then that value is replaced.  Ownership of the variable
 * string is not transferred to the data store module as a result of
 * this call; the data store module makes such copies of this string
 * as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set.
 */
int store_set_int(char *var, long val)
{
    debug("store_set_int(%s, %ld)", var, val);
    if (!var)
        return -1;
    if (!data_store.first)
    {
        variable *v = calloc(1, sizeof(variable));
        data_store.first = v;
        v->name = strdup(var);
        v->type = NUM_VALUE_TYPE;
        v->value.integer = val;
        return 0;
    }
    for (variable *curr = data_store.first; curr; curr = curr->next)
    {
        int same = 1;
        int i;
        for (i = 0; var[i] != '\0'; i++)
        {
            if (curr->name[i] != var[i])
            {
                same = 0;
                break;
            }
        }
        if (same && var[i] == curr->name[i])
        {
            if (curr->type == STRING_VALUE_TYPE)
            {
                free(curr->value.string);
            }
            curr->type = NUM_VALUE_TYPE;
            // curr->value.string = NULL;
            curr->value.integer = val;
            return 0;
        }
        if (!curr->next)
        {
            variable *v = calloc(1, sizeof(variable));
            curr->next = v;
            v->name = strdup(var);
            v->type = NUM_VALUE_TYPE;
            v->value.integer = val;
            return 0;
        }
    }
    return -1;
}
/**
 * @brief  Set the value of a variable as a string.
 * @details  This function sets the current value of a specified
 * variable to be a specified string.  If the variable already
 * has a value, then that value is replaced.  If the specified
 * value is NULL, then any existing value of the variable is removed
 * and the variable becomes un-set.  Ownership of the variable and
 * the value strings is not transferred to the data store module as
 * a result of this call; the data store module makes such copies of
 * these strings as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set, or NULL if the variable is to become
 * un-set.
 */
int store_set_string(char *var, char *val)
{
    debug("store_set_string(%s, %s)", var, val);
    if (!var)
        return -1;

    if (!data_store.first)
    {
        if (val)
        {
            variable *v = calloc(1, sizeof(variable));
            data_store.first = v;
            v->name = strdup(var);
            v->type = STRING_VALUE_TYPE;
            v->value.string = strdup(val);
            return 0;
        }
        else
        {
            variable *v = calloc(1, sizeof(variable));
            data_store.first = v;
            v->name = strdup(var);
            v->type = NO_VALUE_TYPE;
            return 0;
        }
    }
    for (variable *curr = data_store.first; curr; curr = curr->next)
    {
        int same = 1;
        int i;
        for (i = 0; var[i] != '\0'; i++)
        {
            if (curr->name[i] != var[i])
            {
                same = 0;
                break;
            }
        }
        if (same && var[i] == curr->name[i])
        {
            if (curr->type == STRING_VALUE_TYPE)
            {
                free(curr->value.string);
            }
            if (val)
            {
                curr->type = STRING_VALUE_TYPE;
                curr->value.string = strdup(val);
                // curr->value.integer = 0;
                return 0;
            }
            else
            {
                curr->type = NO_VALUE_TYPE;
                // curr->value.integer = 0;
                curr->value.string = NULL;
                return 0;
            }
        }
        if (!curr->next)
        {
            variable *v = calloc(1, sizeof(variable));
            curr->next = v;
            v->name = strdup(var);
            v->type = STRING_VALUE_TYPE;
            v->value.string = strdup(val);
            return 0;
        }
    }
    return -1;
}

/**
 * @brief  Print the current contents of the data store.
 * @details  This function prints the current contents of the data store
 * to the specified output stream.  The format is not specified; this
 * function is intended to be used for debugging purposes.
 *
 * @param f  The stream to which the store contents are to be printed.
 */
void store_show(FILE *f)
{
    fputc('{', f);
    if (!data_store.first)
    {
        fputc('}', f);
        fflush(f);
        return;
    }
    variable *curr = data_store.first;
    while (curr)
    {

        switch (curr->type)
        {
        case NUM_VALUE_TYPE:
            fputs(curr->name, f);
            fputc('=', f);
            fprintf(f, "%ld", curr->value.integer);
            break;
        case STRING_VALUE_TYPE:
            fputs(curr->name, f);
            fputc('=', f);
            fputs(curr->value.string, f);
            break;
        case NO_VALUE_TYPE:
            break;
        default:
            printf("error at store show");
            abort();
            break;
        }
        if (curr->next && curr->type != NO_VALUE_TYPE)
            fputc(',', f);
        curr = curr->next;
        if (!curr)
        {
            fputc('}', f);
            fflush(f);
        }
    }

    fflush(f);
    // loop through data_store and print in "{JOB=0,STATUS=0,x=abcd,y=ifejio\n,abcd=5}" format
}

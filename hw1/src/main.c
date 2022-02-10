#include <stdio.h>
#include <stdlib.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif


int main(int argc, char **argv)
{

    //TESTS BEGIN


    ARGO_STRING s;
    s.capacity = 0;
    s.length = 0;
    argo_append_char(&s, 0);
    argo_append_char(&s, 0x0061);
    argo_append_char(&s, 0x0020);
    argo_append_char(&s, 0x005c);
    argo_append_char(&s, 0x002f);
    argo_append_char(&s, 0x005c);
    argo_append_char(&s, 0x0008);
    argo_append_char(&s, 0x000c);
    argo_append_char(&s, 0x000a);
    argo_append_char(&s, 0x0009);
    argo_append_char(&s, 0x000b);
    argo_write_string(&s, stdout);
    
    printf("\n");
    ARGO_STRING numS;
    numS.capacity = 0;
    numS.length = 0;
    argo_append_char(&numS, 0x0031);
    argo_append_char(&numS, 0x002e);
    argo_append_char(&numS, 0x0032);
    argo_append_char(&numS, 0x0033);
    argo_append_char(&numS, 0x0034);

    ARGO_NUMBER n;
    n.string_value = numS;
    n.int_value = 1; 
    n.float_value = 1.234; 
    n.valid_string= 1; 
    n.valid_int= 0; 
    n.valid_float= 0;
    argo_write_number(&n, stdout);


    //TESTS END


    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);

    

    if(global_options == HELP_OPTION)
        USAGE(*argv, EXIT_SUCCESS);

    else if(global_options == VALIDATE_OPTION){
        printf("\nWERE VALIDATING\n");
    }
    else if(global_options >= CANONICALIZE_OPTION){
        printf("\nWERE CANONICALIZING\n");
    }
    fprintf(stdout,"%x\n", global_options);
    // TO BE IMPLEMENTED
    return EXIT_FAILURE;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */

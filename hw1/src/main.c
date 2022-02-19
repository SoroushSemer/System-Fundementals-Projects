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


    // ARGO_STRING s;
    // s.capacity = 0;
    // s.length = 0;
    // argo_append_char(&s, 0);
    // argo_append_char(&s, 0x0061);
    // argo_append_char(&s, 0x0020);
    // argo_append_char(&s, 0x005c);
    // argo_append_char(&s, 0x002f);
    // argo_append_char(&s, 0x005c);
    // argo_append_char(&s, 0x0008);
    // argo_append_char(&s, 0x000c);
    // argo_append_char(&s, 0x000a);
    // argo_append_char(&s, 0x0009);
    // argo_append_char(&s, 0x000b);
    // argo_write_string(&s, stdout);
    


    // printf("\n");
    // ARGO_STRING numS;
    // numS.capacity = 0;
    // numS.length = 0;
    // argo_append_char(&numS, 0x0031);
    // argo_append_char(&numS, 0x002e);
    // argo_append_char(&numS, 0x0032);
    // argo_append_char(&numS, 0x0033);
    // argo_append_char(&numS, 0x0034);


    // ARGO_NUMBER n;
    // n.string_value = numS;
    // n.int_value = 1; 
    // n.float_value = 1.234; 
    // n.valid_string= 1; 
    // n.valid_int= 0; 
    // n.valid_float= 0;
    // argo_write_number(&n, stdout);

    // ARGO_VALUE v1;
    // v1.type = 3;
    // v1.content.string = s;

    // printf("\n");
    // argo_write_value(&v1, stdout);

    // ARGO_VALUE v2;
    // v2.type = 3;
    // v2.content.string = numS;

    // printf("\n");
    // argo_write_value(&v2, stdout);

    // ARGO_VALUE v3;
    // v3.type = 2;
    // v3.content.number = n;

    // printf("\n");
    // argo_write_value(&v3, stdout);

    // v1.prev = &v3;
    // v1.next = &v2;
    // v2.prev = &v1;
    // v2.next = &v3;
    // v3.prev = &v2;
    // v3.next = &v1;
    
    // ARGO_VALUE v4;
    // v4.type = 3;
    // v4.content.string = s;
    // v4.name = s;

    // printf("\n");
    // argo_write_value(&v4, stdout);

    // ARGO_VALUE v5;
    // v5.type = 3;
    // v5.content.string = numS;
    // v5.name= numS;

    // printf("\n");
    // argo_write_value(&v5, stdout);

    // ARGO_VALUE v6;
    // v6.type = 2;
    // v6.content.number = n;
    // v6.name = numS;

    // printf("\n");
    // argo_write_value(&v6, stdout);

    // v4.prev = &v6;
    // v4.next = &v5;
    // v5.prev = &v4;
    // v5.next = &v6;
    // v6.prev = &v5;
    // v6.next = &v4;

    // ARGO_ARRAY a;
    // a.element_list = &v1;

    // ARGO_VALUE arrayV;
    // arrayV.type = 5;
    // arrayV.content.array = a; 
    
    // printf("\n");
    // argo_write_value(&arrayV, stdout);
    
    // ARGO_OBJECT o;
    // o.member_list = &v4;

    // ARGO_VALUE object;
    // object.type = 4;
    // object.content.object = o; 

    // printf("\n");
    // argo_write_value(&object, stdout);

    //TESTS END


    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);

    

    if(global_options == HELP_OPTION)
        USAGE(*argv, EXIT_SUCCESS);

    else if(global_options == VALIDATE_OPTION){
        ARGO_VALUE* argo_value = argo_read_value(stdin);  
        if(!argo_value){
            fputs("ERROR Reading", stderr);
            return EXIT_FAILURE;
        }
        else
            return EXIT_SUCCESS;
        // printf("\nWERE VALIDATING\n");
    }
    else if(global_options >= CANONICALIZE_OPTION){
        ARGO_VALUE* argo_value = argo_read_value(stdin);  
        if(!argo_value){
            fputs("ERROR Reading\n", stderr);
            return EXIT_FAILURE;
            }
        else{
            int x = argo_write_value(argo_value, stdout);
            if(!x)
                return EXIT_SUCCESS;
            else
                return EXIT_FAILURE;
        }
        // printf("\nWERE CANONICALIZING\n");
    }
    // fprintf(stdout,"%x\n", global_options);
    // TO BE IMPLEMENTED
    return EXIT_FAILURE;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */

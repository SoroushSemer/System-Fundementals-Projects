#include <stdlib.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */
static int strEquals(char *s1, char *s2){
    int i = 0;
    while(s1[i] != '\0' && s2[i] != '\0'){
        if(s1[i] != s2[i])
            return 0;
        i++;
    }
    return 1;
}
static int strToInt(char *s){
    int i = 0;
    int num = 0;
    while(s[i] != '\0'){
        num*=10;
        if(s[i]<48 || s[i]>57) return -1;
        num+= s[i]-48;
        i++;
    }
    return num;
}


int validargs(int argc, char **argv) {
    // TO BE IMPLEMENTED
    if(argc <= 1){
        fprintf(stderr, "Invalid number of args\n");
        return -1;
    }

    if(strEquals(argv[1], "-h")){
        global_options=0x80000000;
        return 0;
    }
    if(argc >4){
        fprintf(stderr, "Invalid number of args\n");
        return -1;
    }

    if(strEquals(argv[1], "-v")){
        global_options=0x40000000;
        if(argc > 2){
            fprintf(stderr, "Invalid arg\n");
            return -1;
        }
        return 0;

    }

    if(strEquals(argv[1], "-c")){
        global_options=0x20000000;
         if(argc >=3){
            if(strEquals(argv[2], "-p")){     
            global_options=0x30000000;
                if(argc >= 4){
                    int indent = strToInt(argv[3]);
                    if(strToInt(argv[3])<0){
                        global_options=0x0;
                        fprintf(stderr, "Invalid indent number\n");
                        return -1;
                    }
                    global_options+=+indent;
                }
                else
                    global_options+=4;
            }
            else{
                global_options=0x0;
                fprintf(stderr, "Invalid arg\n");
                return -1;
            }
        }
        return 0;
    }

    global_options = 0x0;
    fprintf(stderr, "Invalid args\n");

    return -1;

    //abort();
}

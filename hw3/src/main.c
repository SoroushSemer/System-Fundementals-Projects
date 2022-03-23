#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[])
{
    sf_set_magic(0x0);

    // printf("allocating double of size %ld bytes\n", sizeof(double));
    // double *ptr = sf_malloc(sizeof(double));

    // *ptr = 1234.5678;

    // printf("address: 0x%.8lx\n", (long int)ptr);
    // printf("value: %f\n", *ptr);

    // sf_show_heap();
    // printf("\n----------------------\n");

    // printf("allocating char[1000] of size %ld bytes\n", sizeof(char[1000]));
    // char **ptr2 = sf_malloc(sizeof(char[1000]));

    // *ptr2 = "This is a 1000 character string";

    // printf("address: 0x%.8lx\n", (long int)ptr2);
    // printf("value: %s\n", *ptr2);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");

    // printf("allocating char[940] of size %ld bytes\n", sizeof(char[940]));
    // char **ptr3 = sf_malloc(sizeof(char[940]));

    // *ptr3 = "This is a 940 character string";

    // printf("address: 0x%.8lx\n", (long int)ptr3);
    // printf("value: %s\n", *ptr3);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");
    // sf_free(ptcd

    // printf("allocating char[98304] of size %ld bytes\n", sizeof(char[98304]));
    // char **ptr4 = sf_malloc(sizeof(char[98304]));
    // if (!ptr4)
    //     return EXIT_FAILURE;
    // *ptr4 = "This is should fail";

    // printf("address: 0x%.8lx\n", (long int)ptr4);
    // printf("value: %s\n", *ptr4);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");

    printf("allocating char[4032] of size %ld bytes\n", sizeof(char[4032]));
    char **ptr5 = sf_malloc(sizeof(char[4032]));
    if (!ptr5)
        return EXIT_FAILURE;
    *ptr5 = "This is should get 4 pages";

    printf("address: 0x%.8lx\n", (long int)ptr5);
    printf("value: %s\n", *ptr5);

    // sf_show_blocks();
    sf_show_heap();
    printf("\n----------------------\n");
    // sf_free(ptcd

    return EXIT_SUCCESS;
}

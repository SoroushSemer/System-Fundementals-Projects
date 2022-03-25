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

    // // printf("allocating char[98304] of size %ld bytes\n", sizeof(char[98304]));
    // // char **ptr4 = sf_malloc(sizeof(char[98304]));
    // // if (!ptr4)
    // //     return EXIT_FAILURE;
    // // *ptr4 = "This is should fail";

    // // printf("address: 0x%.8lx\n", (long int)ptr4);
    // // printf("value: %s\n", *ptr4);

    // // // sf_show_blocks();
    // // sf_show_heap();
    // // printf("\n----------------------\n");

    // printf("allocating char[4032] of size %ld bytes\n", sizeof(char[4032]));
    // char **ptr5 = sf_malloc(sizeof(char[4032]));
    // if (!ptr5)
    //     return EXIT_FAILURE;
    // *ptr5 = "This is should get 4 pages";

    // printf("address: 0x%.8lx\n", (long int)ptr5);
    // printf("value: %s\n", *ptr5);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");

    // printf("allocating int of size %ld bytes\n", sizeof(int));
    // int *ptr6 = sf_malloc(sizeof(int));
    // if (!ptr6)
    //     return EXIT_FAILURE;
    // *ptr6 = 4;

    // printf("value: %d\n", *ptr6);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");

    // sf_free(ptr6);

    // sf_show_heap();
    // printf("\n----------------------\n");

    // printf("allocating int of size %ld bytes\n", sizeof(int));
    // int *ptr7 = sf_malloc(sizeof(int));
    // if (!ptr7)
    //     return EXIT_FAILURE;
    // *ptr7 = 4;

    // printf("value: %d\n", *ptr7);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");

    // sf_free(ptr5);

    // sf_show_heap();
    // printf("\n----------------------\n");

    // printf("allocating char[2000] of size %ld bytes\n", sizeof(char[2000]));
    // char **ptr8 = sf_malloc(sizeof(char[2000]));

    // *ptr8 = "This is a 2000 character string";

    // printf("address: 0x%.8lx\n", (long int)ptr8);
    // printf("value: %s\n", *ptr8);

    // // sf_show_blocks();
    // sf_show_heap();
    // printf("\n----------------------\n");

    // sf_free(ptr2);

    // sf_show_heap();
    // printf("\n----------------------\n");

    // sf_free(ptr8);

    // sf_show_heap();
    // printf("\n----------------------\n");

    // size_t sz_u = 216, sz_v = 150, sz_w = 50, sz_x = 150, sz_y = 200, sz_z = 250;
    // void *u = sf_malloc(sz_u);
    // sf_show_heap();

    // /* void *v = */ sf_malloc(sz_v);
    // sf_show_heap();

    // void *w = sf_malloc(sz_w);
    // sf_show_heap();

    // /* void *x = */ sf_malloc(sz_x);
    // sf_show_heap();

    // void *y = sf_malloc(sz_y);
    // sf_show_heap();

    // /* void *z = */ sf_malloc(sz_z);
    // sf_show_heap();

    // sf_free(u);
    // sf_show_heap();

    // sf_free(w);
    // sf_show_heap();

    // sf_free(y);
    // sf_show_heap();

    // size_t sz_x = sizeof(double) * 8, sz_y = sizeof(int);

    // int *x = sf_malloc(sz_x);
    // *x = 20;
    // sf_show_heap();

    // int *y = sf_realloc(x, sz_y);
    // *y = 5;
    // sf_show_heap();

    // size_t sz_x = sizeof(int), sz_y = 10, sz_x1 = sizeof(int) * 20;
    // void *x = sf_malloc(sz_x);
    // /* void *y = */ sf_malloc(sz_y);
    // x = sf_realloc(x, sz_x1);
    // sf_show_heap();

    size_t sz_x = sizeof(double) * 8, sz_y = sizeof(int);
    int *x = sf_malloc(sz_x);
    *x = 20;
    sf_show_heap();

    int *y = sf_realloc(x, sz_y);
    *y = 5;
    // sf_free(ptcd
    sf_show_heap();

    // int *pp = sf_malloc(1400);
    // *pp = 1;
    // sf_show_heap();
    return EXIT_SUCCESS;
}

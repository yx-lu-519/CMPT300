#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kallocator.h"

int main(int argc, char* argv[]) {
    initialize_allocator(10000, FIRST_FIT);
    //initialize_allocator(10000, BEST_FIT);
    //initialize_allocator(10000, WORST_FIT);
    //printf("Using best fit algorithm on memory size 100\n");

    int* p[50] = {NULL};
    for(int i=9; i>0; i--) {
        p[i] = kalloc(sizeof(int)*i);
        if(p[i] == NULL) {
            printf("Allocation failed\n");
            continue;
        }
        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
    }

    print_statistics();

    for(int i=4; i>1; i--) {

        printf("Freeing p[%d]\n", i);
        kfree(p[i]);
        p[i] = NULL;
    }

    printf("available_memory %d\n", available_memory());
    printf("\n/////////////////////////////////////\n");

    int* num[20] = {NULL};
    for(int i=9; i>8 ;i--) {
        num[i] = kalloc(sizeof(int)*9);
        if(num[i] == NULL) {
            printf("Allocation failed\n");
            continue;
        }
        *(num[i]) = i;
        printf("num[%d] = %p ; *num[%d] = %d\n", i, num[i], i, *(num[i]));
    }

    print_statistics();

    printf("\n/////////////////////////////////////\n");
    void* before[100] = {NULL};
    void* after[100] = {NULL};
    compact_allocation(before, after);
    print_statistics();

    // You can assume that the destroy_allocator will always be the 
    // last funciton call of main function to avoid memory leak 
    // before exit

    destroy_allocator();

    return 0;
}

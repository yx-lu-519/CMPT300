#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include "process_ancestors.h"

// Sys-call number:
#define _PROCESS_ANCESTORS_TEST_ 342

void test_process_ancestors()
{
    printf("Testing process_ancestors syscall...\n");

    const long INVALID_SIZE = (rand()%1000)*(-1);
    const long VALID_SIZE = rand()%1000 + 1;
    process_info_t arr_process[VALID_SIZE];
    long num_filled;

    printf("INVALID_SIZE: %li\n", INVALID_SIZE);
    printf("VALID_SIZE: %li\n\n", VALID_SIZE);

    int result = syscall(_PROCESS_ANCESTORS_TEST_, arr_process, VALID_SIZE, &num_filled);
    printf("\n");
    for(int i=0; i<num_filled; i++)
    {
        printf("arr_process[%d]: ", i);
        printf("pid: %li, name: %s, state: %li, uid: %li, nvcsw: %li, nivscw: %li, num_children: %li, num_siblings: %li\n",
            arr_process[i].pid, arr_process[i].name, arr_process[i].state, arr_process[i].uid, arr_process[i].nvcsw, arr_process[i].nivcsw, arr_process[i].num_children, arr_process[i].num_siblings);
    }
    printf("\n");
    assert(result == 0);
    assert(num_filled <= VALID_SIZE);

    result = syscall(_PROCESS_ANCESTORS_TEST_, arr_process, INVALID_SIZE, &num_filled);
    assert(result == -1);
    assert(errno == EINVAL);
    result = syscall(_PROCESS_ANCESTORS_TEST_, arr_process, VALID_SIZE, NULL);
    assert(result == -1);
    assert(errno == EFAULT);
    result = syscall(_PROCESS_ANCESTORS_TEST_, NULL, VALID_SIZE, &num_filled);
    assert(result == -1);
    assert(errno == EFAULT);

    printf("\nprocess_ancestors test complete!\n");
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    printf("Testing process_ancestors syscalls...\n");

    test_process_ancestors();

    printf("All tests passed!\n");
    
    return 0;
}
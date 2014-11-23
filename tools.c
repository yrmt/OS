/*
 *
 * Utility functions.
 * Author: Youri Mouton.
 *
 */
#include "global.h"

void * clean_ptr(void * pointer) {
    if (pointer != NULL) {
        free(pointer);
    }
    return pointer = NULL;
}

bool checkyesno(const char *msg) {
    char        input[BUFSIZ];
    char        res;

    do {

        printf("%s %s", msg, " ? ([o]ui/[n]on) : ");
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%c", &res);

    } while (res != 'n' && res != 'o');

    if (res == 'n') {
        listen = false;
    }
    
    return listen;
}

void cleanup(int fd) {
    struct stat         info;

    printf("\ncleaning up...\n");

    // close the file descriptor
    if (fd != FAIL) {
        if (close(fd) == FAIL) {
            printf("Couldn't close file descriptor %d\n", fd);
        }
    }

    // delete the fifo file if it exists
    if (unlink(FIFO_FILE) < 0) {
        if (stat(FIFO_FILE, &info) == FAIL) {
            printf("The fifo file doesn't exist\n");
        } else {
            printf("Couldn't delete fifo\n");
        }
        exit(EXIT_FAILURE);
    }

    // actually exit
    exit(EXIT_SUCCESS);

}

void fatal(int fd, const char * restrict format, ...) {    
    va_list     args;

    va_start(args, format);
    fprintf(stderr, RED"FATAL: "NOR);
    vfprintf(stderr, format, args);
    va_end(args);

    cleanup(fd);
}
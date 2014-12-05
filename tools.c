/*
 *
 * Utility functions mostly taken from OpenBSD.
 * Author: Youri Mouton.
 *
 */
#include "global.h"

bool listen;
int input;
int output;
int nb;
char **requests;

void * xmalloc(size_t size) {
    void *ptr;

    if (size == 0) {
        fatal("xmalloc: zero size\n");
    }

    ptr = malloc(size);

    if (ptr == NULL) {
        fatal("xmalloc: out of memory (allocating %zu bytes)", size);
    }

    return ptr;
}

void * xrealloc(void *ptr, size_t nmemb, size_t size) {
    void *new_ptr;
    size_t new_size = nmemb * size;

    if (new_size == 0)
        fatal("xrealloc: zero size");
    if (SIZE_T_MAX / nmemb < size)
        fatal("xrealloc: nmemb * size > SIZE_T_MAX");
    if (ptr == NULL)
        new_ptr = malloc(new_size);
    else
        new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL)
        fatal("xrealloc: out of memory (new_size %zu bytes)",
                new_size);
    return new_ptr;
}

void cleanPtr(int count, char ** array) {

    for (int i = 0; i < count; i++) {
        if (array[i] != NULL) {
            free(array[i]);
            array[i] = NULL;
        }
    }

    if (array != NULL) {
        free(array);
        array = NULL;
    }
}

bool checkyesno(const char *msg) {
    char        inp[BUFSIZ];
    char        res;

    do {

        printf("\n%s%s", msg, " ? (["RED"y"NOR"]es/["RED"n"NOR"]o) : ");
        fgets(inp, sizeof(inp), stdin);
        sscanf(inp, "%c", &res);

    } while (res != 'y' && res != 'n');

    if (res == 'y') {
        listen = false;
    }

    return listen;
}

void delete(const char * pathname) {
    struct stat         info;

    if (stat(pathname, &info) == FAIL) {
        fatal("File %s doesn't exist!\n", pathname);
    } else {
        if (unlink(pathname) == -1) {
            fatal("Failed to delete %s !\n", pathname);
        }
    }
}

void cleanup(int state) {

    if (state == EXIT_SUCCESS || state == SIGINT) {
        listen = checkyesno("Really quit");
    } else if (state == EXIT_FAILURE) {
        listen = false;
    }

    if (!listen) {

        printf("\ncleaning up... \n");

        if (input != -1 || output != -1) {

            if (close(input) == FAIL) {
                printf("Couldn't close input file descriptor %d\n", input);
            }

            if (close(output) == FAIL) {
                printf("Couldn't close output file descriptor %d\n", output);
            }
        }

        delete(FIFO_FILE);

        delete(FIFO_FILE_OUT);

        cleanPtr(nb, requests);
    }

}

int fatal(const char * format, ...) {    

    va_list     args;

    va_start(args, format);

    fprintf(stderr, RED"FATAL: "NOR);
    vfprintf(stderr, format, args);

    va_end(args);

    cleanup(EXIT_FAILURE);

    exit(EXIT_FAILURE);
}

#include "global.h"

bool listen = true;

void cleanup(int fd) {
    struct stat info;

    // close the fifo file
    if (close(fd) == -1) {
        printf("Couldn't close fifo.\n");
    }

    // delete the fifo file if it exists
    if (unlink(FIFO_FILE) < 0) {
        if (stat(FIFO_FILE, &info) == -1) {
            printf("The fifo file doesn't exist\n");
        } else {
            printf("Couldn't delete fifo\n");
        }
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char const **argv)
{
    // fifo file descriptor number
    int input = 0;
    int result = 0;
    // message to read
    struct request  req;
    fd_set      readset;
    struct timeval   tv;

    // setup signal, so if programs exits 
    // abruptly, the fifo still gets cleanup up
    sigset(SIGINT, &cleanup);

    // create the fifo
    if (mkfifo(FIFO_FILE, 0777) == -1) {
        printf("Was unable to create input fifo\n");
        return -1;
    }

    // open the fifo
    if ((input = open(FIFO_FILE, O_RDONLY)) == -1) {
        printf("Was unable to open the server's fifo\n");
        cleanup(input);
        return -1;
    }

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (listen) {
        FD_ZERO(&readset);
        FD_SET(input, &readset);
        result = select (input+1, &readset, NULL, NULL, NULL);

        if (result > 0) {
            // read and write the message
            if (read(input, &req, sizeof(req)) == -1) {
                printf("Was unable to read the message\n");
                cleanup(input);
            } else {
                if (strncmp(req.msg, PILOT_REQUEST, BUFSIZ) == 0) {
                    /*if (write(STDOUT_FILENO, req.msg, strlen(req.msg)) == -1) {
                      printf("Was unable to write the message\n");
                      cleanup(input);
                      return -1;
                      }
                      if (write(STDOUT_FILENO, &req.pid, sizeof(req.pid)) == -1) {
                      printf("Was unable to get the pid\n");
                      cleanup(input);
                      return -1;
                      }*/
                    printf("REQ from %s : %s (size=%ld)\n", req.pid, req.msg, req.siz);
                    //write(0, req.msg, strlen(req.msg));
                }
            }
        }
    }
    // exit gracefully
    cleanup(input);
}

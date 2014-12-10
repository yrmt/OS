#include "global.h"

#define MAX_TRY 20

int input = -1;
int output = -1;
bool listen = true;
int nb = 0;
char **requests = NULL;

int atis(char * atisMsg) {
    int fichierMeteo = 0;
    int atisSize = 0;
    char buf[MSG_SIZE];
    char noMeteoMsg[] = "the meteo server is unreachable...";
    char busyMeteoMsg[] = "the meteo server is busy...";

    fichierMeteo = open(FICHIERMETEO, O_RDONLY);

    if (fichierMeteo == FAIL) {

        printf("Unable to open meteo file\n");
        memcpy(atisMsg, noMeteoMsg, sizeof(noMeteoMsg));
        atisSize = sizeof(noMeteoMsg);

    } else if (exists(FICHIERLOCK)) {

        printf("The meteo server is busy\n");
        memcpy(atisMsg, busyMeteoMsg, sizeof(busyMeteoMsg));
        atisSize = sizeof(busyMeteoMsg);

    } else {

        atisSize = (int)read(fichierMeteo, buf, sizeof(buf));

        if (atisSize == FAIL) {

            fatal("Unable to read from %s\n", FICHIERMETEO);

        } else {

            memcpy(atisMsg, buf, atisSize);

        }
    }

    return atisSize;
}

void createFifos(void) {
    if (mkfifo(FIFO_FILE, S_IRUSR | S_IWUSR) == FAIL) {

        fatal("Unable to create input fifo\n");

    }

    if (mkfifo(FIFO_FILE_OUT, S_IRUSR | S_IWUSR) == FAIL) {

        fatal("Unable to create output fifo\n");

    }
}

void openFifos(void) {
    if ((input = open(FIFO_FILE, O_RDWR)) == FAIL) {

        fatal("Unable to open the server's fifo\n");

    }

    if ((output = open(FIFO_FILE_OUT, O_RDWR)) == FAIL) {

        fatal("Unable to open server ouput fifo %s\n", FIFO_FILE_OUT);

    }
}

void operations(void) {
    // poll in the input server
    // for incomind data and for 
    // client disconnects.
    struct pollfd       fd[] = {
        { input, POLLIN | POLLHUP, 0 }
    };

    // request packet
    char                requestPacket[MSG_SIZE];
    requests=           NULL;

    // atis
    char                atisMsg[MSG_SIZE];

    printf("^C to exit\n- Listening...\n");

    while (listen) {

        // poll the input fifo every second
        int fifoActions = poll(fd, 1, 1000);

        if (fifoActions == FAIL) {

            if (listen) {
                printf("%s\n", "polling again...");
            }

        } else if (fifoActions == 0) {

            printf("- Listening...\n");

        } else {

            int packet = (int)read(input, requestPacket, MSG_SIZE);

            if (packet == FAIL) {

                if (fd[0].revents & POLLHUP) {

                    close(input);

                    if ((input = open(FIFO_FILE, O_RDWR)) == FAIL) {

                        fatal("Unable to open the input fifo.\n");

                    }
                }

            } else {

                // assign read string
                requests = xrealloc(requests, nb+1, MSG_SIZE);
                requests[nb] = xmalloc(MSG_SIZE);
                memcpy(requests[nb], requestPacket, packet);

                if (memcmp(requests[nb], PILOT_REQUEST, 
                            sizeof(PILOT_REQUEST)) == 0)  {

                    printf("< Got Request nr. %d\n", nb);

                    size_t tailleMsg = (size_t)atis(atisMsg);

                    printf("> Sending ATIS \"%s\" to %d\n", atisMsg, nb);

                    if (write(output, atisMsg, tailleMsg) == FAIL) {

                        fatal("Failed to send message...\n");

                    }

                    nb++;
                } else {

                    fatal("No valid messages intercepted\n");

                }
            }
        }
    }
}

int main(void) {
    // setup signal, so if programs gets 
    // interrupted, files can still be 
    // cleaned up and FIFOs removed.
    signal(SIGINT, &cleanup);

    createFifos();

    openFifos();

    operations();

    return EXIT_SUCCESS;
}

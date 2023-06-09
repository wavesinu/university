#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define SERVER_KEY 0x21033161
#define CLIENT1_KEY 0x21033162
#define CLIENT2_KEY 0x21033163

#define CLIENT1_TYPE 100
#define CLIENT2_TYPE 200

struct Message {
    long type;
    char text[256];
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./client <client number>\n");
        exit(1);
    }

    int clientNumber = atoi(argv[1]);
    int myQueue, myType, serverQueue;

    if (clientNumber == 1) {
        myQueue = msgget(CLIENT1_KEY, IPC_CREAT | 0666);
        myType = CLIENT1_TYPE;
    } else if (clientNumber == 2) {
        myQueue = msgget(CLIENT2_KEY, IPC_CREAT | 0666);
        myType = CLIENT2_TYPE;
    } else {
        fprintf(stderr, "Invalid client number. Use 1 or 2.\n");
        exit(1);
    }

    serverQueue = msgget(SERVER_KEY, 0666);

    printf("Message queue 0x%x 생성\n", clientNumber == 1 ? CLIENT1_KEY : CLIENT2_KEY);

    struct Message msg;

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(myQueue, &readfds);

        select(myQueue + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(myQueue, &readfds)) {
            if (msgrcv(myQueue, &msg, sizeof(msg.text), 0, IPC_NOWAIT) >= 0) {
                printf("recv from client%d: %s\n", clientNumber == 1 ? 2 : 1, msg.text);
                if (strcmp(msg.text, "quit") == 0) {
                    break;
                }
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            printf("Text to send: ");
            fgets(msg.text, sizeof(msg.text), stdin);
            msg.text[strcspn(msg.text, "\n")] = '\0'; // Remove newline character

            printf("send to client%d: %s\n", clientNumber == 1 ? 2 : 1, msg.text);

            msg.type = myType;
            msgsnd(serverQueue, &msg, sizeof(msg.text), 0);

            if (strcmp(msg.text, "quit") == 0) {
                break;
            }
        }
    }

    msgctl(myQueue, IPC_RMID, NULL);

    return 0;
}
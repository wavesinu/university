//학번: 202103316
//이름: 조이수
//완성여부: server 코드는 완성됨.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY 0x21033161
#define CLIENT1_KEY 0x21033162
#define CLIENT2_KEY 0x21033163

#define CLIENT1_TYPE 100
#define CLIENT2_TYPE 200

struct Message {
    long type;
    char text[256];
};

int main() {
    int serverQueue = msgget(SERVER_KEY, IPC_CREAT | 0666);
    int client1Queue = msgget(CLIENT1_KEY, IPC_CREAT | 0666);
    int client2Queue = msgget(CLIENT2_KEY, IPC_CREAT | 0666);

    printf("Message queue 0x%x 생성\n", SERVER_KEY);

    struct Message msg;
    int quitCount = 0;

    while (1) {
        if (msgrcv(serverQueue, &msg, sizeof(msg.text), 0, 0) < 0) {
            perror("msgrcv");
            exit(1);
        }

        if (msg.type == CLIENT1_TYPE) {
            printf("recv from client1: %s\n", msg.text);
            if (strcmp(msg.text, "quit") == 0) {
                quitCount++;
            }

            if (quitCount < 2) {
                msg.type = CLIENT2_TYPE;
                printf("send to client2: %s\n", msg.text);
                msgsnd(client2Queue, &msg, sizeof(msg.text), 0);
            }
        } else if (msg.type == CLIENT2_TYPE) {
            printf("recv from client2: %s\n", msg.text);
            if (strcmp(msg.text, "quit") == 0) {
                quitCount++;
            }

            if (quitCount < 2) {
                msg.type = CLIENT1_TYPE;
                printf("send to client1: %s\n", msg.text);
                msgsnd(client1Queue, &msg, sizeof(msg.text), 0);
            }
        }

        if (quitCount == 2) {
            strcpy(msg.text, "quit");
            msg.type = CLIENT1_TYPE;
            msgsnd(client1Queue, &msg, sizeof(msg.text), 0);

            msg.type = CLIENT2_TYPE;
            msgsnd(client2Queue, &msg, sizeof(msg.text), 0);

            msgctl(serverQueue, IPC_RMID, NULL);
            exit(0);
        }
    }

    return 0;
}
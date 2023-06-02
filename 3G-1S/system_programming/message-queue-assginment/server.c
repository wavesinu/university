#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY 0x2021033161
#define CLIENT1_KEY 0x2021033162
#define CLIENT2_KEY 0x2021033163

#define CLIENT1_MSG_TYPE 100
#define CLIENT2_MSG_TYPE 200

struct message {
    long type;
    char text[1024];
};

int main(int argc, char *argv[]) {
    int server_queue, client1_queue, client2_queue;
    struct message msg;

    // Create the message queues
    server_queue = msgget(SERVER_KEY, IPC_CREAT | 0666);
    client1_queue = msgget(CLIENT1_KEY, IPC_CREAT | 0666);
    client2_queue = msgget(CLIENT2_KEY, IPC_CREAT | 0666);

    printf("message queue 0x%x created\n", SERVER_KEY);

    while (1) {
        // Server receives the message from client 1 and sends it to client 2
        msgrcv(server_queue, &msg, sizeof(msg.text), CLIENT1_MSG_TYPE, 0);
        if (strcmp(msg.text, "quit\n") == 0) {
            printf("Client 1 has quit\n");
            strcpy(msg.text, "quit\n");
            msgsnd(client2_queue, &msg, sizeof(msg.text), 0);
            break;
        }
        printf("recv from client1 : %s", msg.text);
        msg.type = CLIENT2_MSG_TYPE;
        msgsnd(client2_queue, &msg, sizeof(msg.text), 0);

        // Server receives the message from client 2 and sends it to client 1
        msgrcv(server_queue, &msg, sizeof(msg.text), CLIENT2_MSG_TYPE, 0);
        if (strcmp(msg.text, "quit\n") == 0) {
            printf("Client 2 has quit\n");
            strcpy(msg.text, "quit\n");
            msgsnd(client1_queue, &msg, sizeof(msg.text), 0);
            break;
        }
        printf("recv from client2 : %s", msg.text);
        msg.type = CLIENT1_MSG_TYPE;
        msgsnd(client1_queue, &msg, sizeof(msg.text), 0);
    }

    // Cleanup the message queues
    msgctl(server_queue, IPC_RMID, NULL);
    msgctl(client1_queue, IPC_RMID, NULL);
    msgctl(client2_queue, IPC_RMID, NULL);

    return 0;
}
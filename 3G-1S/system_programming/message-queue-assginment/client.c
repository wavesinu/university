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
    int client_queue;
    struct message msg;

    if (argc != 2) {
        printf("Usage: %s <client ID>\n", argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "client1") == 0) {
        client_queue = msgget(CLIENT1_KEY, IPC_CREAT | 0666);
        msg.type = CLIENT1_MSG_TYPE;
    } else if (strcmp(argv[1], "client2") == 0) {
        client_queue = msgget(CLIENT2_KEY, IPC_CREAT | 0666);
        msg.type = CLIENT2_MSG_TYPE;
    } else {
        printf("Invalid client ID\n");
        exit(1);
    }

    printf("message queue 0x%x created\n", client_queue);

    while (1) {
        // Prompt the user to enter a message
        printf("Text to send: ");
        fgets(msg.text, sizeof(msg.text), stdin);

        // Send the message to the server
        int server_queue = msgget(SERVER_KEY, 0);
        msgsnd(server_queue, &msg, sizeof(msg.text), 0);

        // Receive the message from the other client and output it
        msgrcv(client_queue, &msg, sizeof(msg.text), msg.type, 0);
        if (strcmp(msg.text, "quit\n") == 0) {
            printf("Other client has quit\n");
            break;
        }
        printf("recv from other client : %s", msg.text);
    }

    // Send the quit message to the other client and remove the message queue
    strcpy(msg.text, "quit\n");
    msgsnd(client_queue == CLIENT1_KEY ? msgget(CLIENT2_KEY, 0) : msgget(CLIENT1_KEY, 0), &msg, sizeof(msg.text), 0);
    msgctl(client_queue, IPC_RMID, NULL);

    return 0;
}
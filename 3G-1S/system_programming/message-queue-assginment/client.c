#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SERVER_KEY 0x2021033161
#define CLIENT1_KEY 0x2021033162
#define CLIENT2_KEY 0x2021033163

#define CLIENT1_MTYPE 100
#define CLIENT2_MTYPE 200

struct msgbuf
{
    long mtype;
    char mtext[200];
};

int main(int argc, char *argv[])
{
    int client_id, server_id;
    struct msgbuf buf;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <client number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_number = atoi(argv[1]);
    if (client_number == 1)
    {
        buf.mtype = CLIENT1_MTYPE;
        client_id = msgget(CLIENT1_KEY, 0666 | IPC_CREAT);
    }
    else if (client_number == 2)
    {
        buf.mtype = CLIENT2_MTYPE;
        client_id = msgget(CLIENT2_KEY, 0666 | IPC_CREAT);
    }
    else
    {
        fprintf(stderr, "Invalid client number\n");
        exit(EXIT_FAILURE);
    }

    server_id = msgget(SERVER_KEY, 0666 | IPC_CREAT);

    if (client_id == -1 || server_id == -1)
    {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("message queue 0x%x 생성\n", (client_number == 1) ? CLIENT1_KEY : CLIENT2_KEY);

    while (1)
    {
        printf("Text to send: ");
        fgets(buf.mtext, sizeof(buf.mtext), stdin);
        buf.mtext[strcspn(buf.mtext, "\n")] = '\0'; // Remove newline character

        msgsnd(server_id, &buf, sizeof(buf.mtext), 0);
        printf("send to client%d: %s\n", (client_number == 1) ? 2 : 1, buf.mtext);

        msgrcv(client_id, &buf, sizeof(buf.mtext), 0, 0);
        printf("recv from client%d: %s\n", (client_number == 1) ? 2 : 1, buf.mtext);

        if (strcmp(buf.mtext, "quit") == 0)
        {
            msgsnd(server_id, &buf, sizeof(buf.mtext), 0);
            msgctl(client_id, IPC_RMID, NULL);
            printf("client message queue removed\n");
            break;
        }
    }

    return 0;
}
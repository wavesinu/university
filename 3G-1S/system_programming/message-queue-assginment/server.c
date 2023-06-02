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

int main()
{
    int server_id, client1_id, client2_id;
    struct msgbuf buf;

    int quit_cnt = 0;

    server_id = msgget(SERVER_KEY, 0666 | IPC_CREAT);
    client1_id = msgget(CLIENT1_KEY, 0666 | IPC_CREAT);
    client2_id = msgget(CLIENT2_KEY, 0666 | IPC_CREAT);

    if (server_id == -1 || client1_id == -1 || client2_id == -1)
    {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    printf("message queue 0x%08lx 생성\n", SERVER_KEY);

    while (quit_cnt < 2)
    {
        msgrcv(server_id, &buf, sizeof(buf.mtext), 0, 0);

        if (buf.mtype == CLIENT1_MTYPE)
        {
            printf("recv from client1: %s\n", buf.mtext);
            msgsnd(client2_id, &buf, sizeof(buf.mtext), 0);
            printf("send to client2: %s\n", buf.mtext);
        }
        else if (buf.mtype == CLIENT2_MTYPE)
        {
            printf("recv from client2: %s\n", buf.mtext);
            msgsnd(client1_id, &buf, sizeof(buf.mtext), 0);
            printf("send to client1: %s\n", buf.mtext);
        }

        if (strcmp(buf.mtext, "quit") == 0)
        {
            quit_cnt++;
        }
    }

    msgctl(server_id, IPC_RMID, NULL);
    printf("server message queue removed\n");

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define SERVER_KEY 0x2021033161
#define CLIENT1_KEY 0x2021033162
#define CLIENT2_KEY 0x2021033163
#define CLIENT1_MTYPE 100
#define CLIENT2_MTYPE 200
#define MAX_TEXT_SIZE 100

struct message
{
    long mtype;
    char mtext[MAX_TEXT_SIZE];
};

int main()
{
    int server_msgid = msgget(SERVER_KEY, IPC_CREAT | 0666);
    if (server_msgid == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("Message queue %lx 생성\n", SERVER_KEY);

    struct message msgbuf;
    int quit_count = 0;

    while (1)
    {
        if (msgrcv(server_msgid, &msgbuf, sizeof(struct msg), 0, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }
        if (strcmp(msgbuf.mtext, "quit") == 0)
        {
            quit_count++;
            if (quit_count == 2)
                break;
        }
        int target_msgid;
        if (msgbuf.mtype == CLIENT1_MTYPE)
        {
            msgbuf.mtype = CLIENT2_MTYPE;
            target_msgid = msgget(CLIENT2_KEY, 0666);
            if (target_msgid == -1)
            {
                perror("msgget");
                exit(1);
            }
            printf("recv from client1 : %s\n", msgbuf.mtext);
            printf("send to client2 : %s\n", msgbuf.mtext);
        }
        else
        {
            msgbuf.mtype = CLIENT1_MTYPE;
            target_msgid = msgget(CLIENT1_KEY, 0666);
            if (target_msgid == -1)
            {
                perror("msgget");
                exit(1);
            }
            printf("recv from client2 : %s\n", msgbuf.mtext);
            printf("send to client1 : %s\n", msgbuf.mtext);
        }
        int status = msgsnd(target_msgid, &msgbuf, sizeof(struct msg), 0);
        if (status == -1)
        {
            perror("msgsnd");
            exit(1);
        }

        if (msgbuf.mtext == CLIENT1_MTYPE)
            msgbuf.mtype = CLIENT2_MTYPE;
        else
            msgbuf.mtype = CLIENT1_MTYPE;
    }
    if (msgctl(server_msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }
    printf("Server message queue removed\n");
    return 0;
}

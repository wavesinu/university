#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define SERVER_KEY 0x21033161
#define CLIENT1_KEY 0x21033162
#define CLIENT2_KEY 0x21033163

#define CLIENT1_MTYPE 100
#define CLIENT2_MTYPE 200
#define MAX_TEXT_SIZE 100

struct message
{
    long mtype;
    char mtext[MAX_TEXT_SIZE];
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: client <client_number>\n");
        exit(1);
    }
    int client_number = atoi(argv[1]);
    key_t my_key = (client_number == 1) ? CLIENT1_KEY : CLIENT2_KEY;
    long my_mtype = (client_number == 1) ? CLIENT1_MTYPE : CLIENT2_MTYPE;

    int my_msgid = msgget(my_key, IPC_CREAT | 0666);
    if (my_msgid == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("Message queue %x 생성\n", my_key);

    int server_msgid = msgget(SERVER_KEY, 0666);
    if (server_msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    struct message msgbuf;
    msgbuf.mtype = my_mtype;

    while (1)
    {
        printf("Text to send: ");
        fgets(msgbuf.mtext, MAX_TEXT_SIZE, stdin);
        msgbuf.mtext[strcspn(msgbuf.mtext, "\n")] = '\0'; 

        int status = msgsnd(server_msgid, &msgbuf, sizeof(struct message), 0);
        if (status == -1)
        {
            perror("msgsnd");
            exit(1);
        }
        printf("send to client%d : %s\n", (client_number == 1) ? 2 : 1, msgbuf.mtext);

        if (strcmp(msgbuf.mtext, "quit") == 0)
        {
            if (msgrcv(my_msgid, &msgbuf, sizeof(struct message), 0, 0) == -1)
            {
                perror("msgrcv");
                exit(1);
            }
            printf("Client%d message queue removed\n", client_number);
            break;
        }

        if (msgrcv(my_msgid, &msgbuf, sizeof(struct message), 0, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }
        printf("recv from client%d : %s\n", (client_number == 1) ? 2 : 1, msgbuf.mtext);
    }
    return 0;
}

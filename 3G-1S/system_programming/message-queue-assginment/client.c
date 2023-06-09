#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define SERVER_KEY 0x21033161
#define CLIENT1_KEY 0x21033162
#define CLIENT2_KEY 0x21033163

#define CLIENT1_MTYPE 100
#define CLIENT2_MTYPE 200
#define MAX_TEXT_SIZE 1024

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

    int server_msgid = msgget(SERVER_KEY, 0);
    if (server_msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    struct message msg_buffer;
    while (1)
    {
        printf("Text to send: ");
        fgets(msg_buffer.mtext, MAX_TEXT_SIZE, stdin);
        msg_buffer.mtype = my_mtype;
        if (msgsnd(server_msgid, &msg_buffer, sizeof(struct message), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
        if (strcmp(msg_buffer.mtext, "quit\n") == 0)
        {
            if (msgctl(my_msgid, IPC_RMID, NULL) == -1)
            {
                perror("msgctl");
                exit(1);
            }
            break;
        }
        if (msgrcv(my_msgid, &msg_buffer, sizeof(struct message), 0, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }
        printf("recv from client%d : %s", (client_number == 1) ? 2 : 1, msg_buffer.mtext);
    }

    exit(0);
}
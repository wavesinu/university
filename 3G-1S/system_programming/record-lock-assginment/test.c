#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Account
{
    char acc_no[6];
    char name[10];
    int balance;
} Account;

void handle_operation(char *acc_no, char op, int amount)
{
    int fd = open("account.dat", O_RDWR);
    struct flock lock;
    Account acc;

    int found = 0;

    while (read(fd, &acc, sizeof(Account)) > 0)
    {
        if (strcmp(acc.acc_no, acc_no) == 0)
        {
            found = 1;
            
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = lseek(fd, -sizeof(Account), SEEK_CUR);
            lock.l_len = sizeof(Account);

            fcntl(fd, F_SETLKW, &lock);

            if (op == 'w')
            {
                acc.balance -= amount;
                printf("pid: %d\tacc_no: %s\twithdraw: %d\tbalance: %d\n", getpid(), acc_no, amount, acc.balance);
            }
            else if (op == 'd')
            {
                acc.balance += amount;
                printf("pid: %d\tacc_no: %s\tdeposit: %d\tbalance: %d\n", getpid(), acc_no, amount, acc.balance);
            }
            else if (op == 'i')
            {
                printf("pid: %d\tacc_no: %s\tinquiry\tbalance: %d\n", getpid(), acc_no, acc.balance);
            }

            lseek(fd, -sizeof(Account), SEEK_CUR);
            write(fd, &acc, sizeof(Account));

            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            break;
        }
    }
    if (!found)
    {
        printf("pid: %d\tacc_no: %s\taccount not found\n", getpid(), acc_no);
    }

    close(fd);
}

void handle_operations()
{
    FILE *file = fopen("operation.dat", "r");
    char acc_no[6];
    char op;
    int amount;

    while (fscanf(file, "%s %c %d", acc_no, &op, &amount) != EOF)
    {
        handle_operation(acc_no, op, amount);
    }

    fclose(file);
}

void print_account_balances()
{
    int fd = open("account.dat", O_RDONLY);
    Account acc;

    while (read(fd, &acc, sizeof(Account)) > 0)
    {
        printf("acc_no: %s\tbalance: %d\n", acc.acc_no, acc.balance);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    int i;
    int child_process_count = 10;
    pid_t pid;

    for (i = 0; i < child_process_count; i++)
    {
        pid = fork();

        if (pid < 0)
        {
            fprintf(stderr, "Fork failed");
            return 1;
        }
        else if (pid == 0)
        {
            handle_operations();
            exit(0);
        }
    }

    while (child_process_count > 0)
    {
        wait(NULL);
        child_process_count--;
    }

    print_account_balances();

    return 0;
}

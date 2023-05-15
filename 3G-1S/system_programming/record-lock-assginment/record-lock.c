#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

typedef struct
{
    char acc_no[6];
    char name[10];
    int balance;
} Account;

typedef struct
{
    char acc_no[6];
    char optype;
    int amount;
} Operation;

void process_job(char *acc_no, char op, int amount);

int main(int argc, char *argv[])
{
    FILE *operation_fp = fopen("operation.dat", "r");
    if (operation_fp == NULL)
    {
        perror("Failed to open operation file");
        return 1;
    }

    Operation operation;
    int process_id = 0;

    // create child processes and assign unique process IDs
    while (fscanf(operation_fp, "%s %c %d", operation.acc_no, &operation.optype, &operation.amount) != EOF)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            process_job(operation.acc_no, operation.optype, operation.amount);
            exit(0);
        }
        else if (pid < 0)
        {
            perror("Failed to fork");
            return 1;
        }

        // Assign unique process ID
        printf("Child process created with PID: %d\n", pid);
        process_id++;
    }

    fclose(operation_fp);

    // wait for all child processes to finish
    for (int i = 0; i < process_id; i++)
    {
        wait(NULL);
    }

    // print all account balances
    int fd = open("account.dat", O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open account file");
        return 1;
    }

    Account account;
    while (read(fd, &account, sizeof(Account)) > 0)
    {
        printf("acc_no: %s balance: %d\n", account.acc_no, account.balance);
    }

    close(fd);

    return 0;
}

void process_job(char *acc_no, char op, int amount)
{
    int fd = open("account.dat", O_RDWR);
    if (fd == -1)
    {
        perror("Failed to open account file");
        exit(1);
    }

    struct flock lock;
    Account acc;
    int found = 0;

    while (read(fd, &acc, sizeof(Account)) > 0)
    {
        if (strcmp(acc.acc_no, acc_no) == 0)
        {
            found = 1;
            lock.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(Account);

            if (op == 'i')
            {
                lock.l_type = F_RDLCK;
            }
            else
            {
                lock.l_type = F_WRLCK;
            }

            if (fcntl(fd, F_SETLKW, &lock) == -1)
            {
                perror("fcntl failed");
                exit(1);
            }

            switch (op)
            {
            case 'w':
                acc.balance -= amount;
                break;
            case 'd':
                acc.balance += amount;
                break;
            case 'i':
                break;
            default:
                fprintf(stderr, "Invalid operation type: %c\n", op);
                exit(1);
            }

            if (op != 'i')
            {
                lseek(fd, lock.l_start, SEEK_SET);
                write(fd, &acc, sizeof(Account));
            }

            lock.l_type = F_UNLCK;
            if (fcntl(fd, F_SETLK, &lock) == -1)
            {
                perror("Unlock failed");
                exit(1);
            }

            break;
        }
    }

    if (!found)
    {
        pid_t pid = getpid();
        switch (op)
        {
        case 'w':
            printf("pid: %d\tacc_no: %s\twithdraw: %d\tacc_no %s 계좌 없음\n", pid, acc_no, amount, acc_no);
            break;
        case 'd':
            printf("pid: %d\tacc_no: %s\tdeposit: %d\tacc_no %s 계좌 없음\n", pid, acc_no, amount, acc_no);
            break;
        case 'i':
            printf("pid: %d\tacc_no: %s\tinquiry\t\tacc_no %s 계좌 없음\n", pid, acc_no, acc_no);
            break;
        default:
            fprintf(stderr, "Invalid operation type: %c\n", op);
            exit(1);
        }
    }
    usleep(rand() % 1000000);

    close(fd);
}

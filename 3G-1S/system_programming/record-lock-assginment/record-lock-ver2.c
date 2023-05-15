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

void process_job(pid_t pid, const char *operation_file, const char *account_file);

int main(int argc, char *argv[])
{
    pid_t pid[10];
    // create 10 child processes
    for (int i = 0; i < 10; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        { // child process
            process_job(getpid(), "operation.dat", "account.dat");
        }
        else if (pid[i] < 0)
        {
            perror("Failed to fork");
            exit(1);
        }
    }
    // wait for all child processes to finish
    for (int i = 0; i < 10; i++)
    {
        waitpid(pid[i], NULL, 0);
    }
    // print all account balances
    int fd = open("account.dat", O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open account file");
        exit(1);
    }
    Account account;
    while (read(fd, &account, sizeof(Account)) > 0)
    {
        printf("acc_no: %s balance: %d\n", account.acc_no, account.balance);
    }
    close(fd);
    return 0;
}
void process_job(pid_t pid, const char *operation_file, const char *account_file)
{
    srand(time(NULL) ^ (getpid() << 16));
    FILE *operation_fp = fopen(operation_file, "r");
    if (operation_fp == NULL)
    {
        perror("Failed to open operation file");
        exit(1);
    }
    Operation operation;
    while (fscanf(operation_fp, "%5s %c %d", operation.acc_no, &operation.optype, &operation.amount) != EOF)
    {
        int fd = open(account_file, O_RDWR, 0666);
        if (fd == -1)
        {
            perror("Failed to open account file");
            exit(1);
        }
        Account account;
        struct flock lock;
        lock.l_whence = SEEK_SET;
        lock.l_len = sizeof(Account);
        int found = 0;
        while (read(fd, &account, sizeof(Account)) > 0)
        {
            if (strcmp(account.acc_no, operation.acc_no) == 0)
            {
                found = 1;
                lock.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(Account);
                // Set the lock type based on operation type
                if (operation.optype == 'i')
                {
                    lock.l_type = F_RDLCK; // Read lock for inquiry
                }
                else
                {
                    lock.l_type = F_WRLCK; // Write lock for withdrawal and deposit
                }
                if (fcntl(fd, F_SETLKW, &lock) == -1)
                {
                    fprintf(stderr, "fcntl failed, process: %d, error: %d\n", pid, errno);
                    exit(1);
                }
                switch (operation.optype)
                {
                case 'w':
                    account.balance -= operation.amount;
                    printf("pid: %d\tacc_no: %s\twithdraw: %d\tbalance: %d\n", pid, account.acc_no, operation.amount, account.balance);
                    break;
                case 'd':
                    account.balance += operation.amount;
                    printf("pid: %d\tacc_no: %s\tdeposit: %d\tbalance: %d\n", pid, account.acc_no, operation.amount, account.balance);
                    break;
                case 'i':
                    printf("pid: %d\tacc_no: %s\tinquiry\t\tbalance: %d\n", pid, account.acc_no, account.balance);
                    break;
                default:
                    fprintf(stderr, "Invalid operation type: %c\n", operation.optype);
                    exit(1);
                }
                if (operation.optype != 'i')
                {
                    lseek(fd, lock.l_start, SEEK_SET);
                    write(fd, &account, sizeof(Account));
                }
                lock.l_type = F_UNLCK;
                if (fcntl(fd, F_SETLK, &lock) == -1)
                {
                    fprintf(stderr, "Unlock failed, process: %d, error: %d\n", pid, errno);
                    exit(1);
                }
                break;
            }
        }
        if (!found)
        {
            printf("pid: %d\tacc_no: %s\tacc_no %s 계좌 없음\n", pid, operation.acc_no, operation.acc_no);
        }
        usleep(rand() % 1000001);
        close(fd);
    }
    fclose(operation_fp);
    exit(0);
}
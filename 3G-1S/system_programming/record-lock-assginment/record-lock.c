#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void process_job(pid_t pid, const char *operation_file, const char *account_file);

int main(int argc, char *argv[])
{
    pid_t pid[10];
    for (int i = 0; i < 10; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            process_job(getpid(), "operation.dat", "account.dat");
            exit(0);
        }
        else if (pid[i] < 0)
        {
            perror("fork failed");
            exit(1);
        }
    }

    for (int i = 0; i < 10; i++)
        waitpid(pid[i], NULL, 0);

    int fd = open("account.dat", O_RDONLY, 0666);
    if (fd == -1)
    {
        perror("Failed to open account file");
        exit(1);
    }

    char acc_no[6];
    char name[10];
    int balance;

    ssize_t size;
    while ((size = read(fd, acc_no, sizeof(acc_no))) > 0)
    {
        if (size < sizeof(acc_no))
        {
            break;
        }
        read(fd, name, sizeof(name));
        read(fd, &balance, sizeof(balance));

        if (acc_no[0] != '\0')
        {
            printf("acc_no: %s\tbalance: %d\n", acc_no, balance);
        }
    }
    if (size == -1)
    {
        perror("Failed to read account file");
        exit(1);
    }

    close(fd);
    return 0;
}

void process_job(pid_t pid, const char *operation_file, const char *account_file)
{
    srand(time(NULL) ^ (pid << 16));
    FILE *operation_fp = fopen(operation_file, "r");
    if (operation_fp == NULL)
    {
        perror("Failed to open operation file");
        exit(1);
    }

    char operation_acc_no[6];
    char operation_optype;
    int operation_amount;

    while (fscanf(operation_fp, "%5s %c %d", operation_acc_no, &operation_optype, &operation_amount) == 3)
    {
        int fd = open(account_file, O_RDONLY, 0666);
        if (fd == -1)
        {
            perror("Failed to open account file");
            exit(1);
        }

        char account_acc_no[6];
        char account_name[10];
        int account_balance;

        struct flock lock;

        lock.l_whence = SEEK_SET;
        lock.l_len = sizeof(account_acc_no) + sizeof(account_name) + sizeof(account_balance);

        int found = 0;

        while (read(fd, account_acc_no, sizeof(account_acc_no)) > 0 &&
               read(fd, account_name, sizeof(account_name)) > 0 &&
               read(fd, &account_balance, sizeof(account_balance)) > 0)
        {
            if (strcmp(account_acc_no, operation_acc_no) == 0)
            {
                found = 1;
                lock.l_start = lseek(fd, 0, SEEK_CUR) - lock.l_len;

                if (operation_optype == 'i')
                    lock.l_type = F_RDLCK;
                else
                    lock.l_type = F_WRLCK;
                if (fcntl(fd, F_SETLKW, &lock) == -1)
                {
                    fprintf(stderr, "fcntl failed, process: %d, error: %d\n", pid, errno);
                    exit(1);
                }

                off_t offset = lseek(fd, lock.l_start, SEEK_SET);
                if (offset == -1)
                {
                    perror("lseek failed");
                    exit(1);
                }

                if (read(fd, account_acc_no, sizeof(account_acc_no)) == -1 ||
                    read(fd, account_name, sizeof(account_name)) == -1 ||
                    read(fd, &account_balance, sizeof(account_balance)) == -1)
                {
                    perror("read failed");
                    exit(1);
                }

                // lseek(fd, lock.l_start, SEEK_SET);
                // read(fd, account_acc_no, sizeof(account_acc_no));
                // read(fd, account_name, sizeof(account_name));
                // read(fd, &account_balance, sizeof(account_balance));

                switch (operation_optype)
                {
                case 'w':
                    account_balance -= operation_amount;
                    printf("pid: %d\tacc_no: %s\twithdraw: %d\tbalance: %d\n", pid, account_acc_no, operation_amount, account_balance);
                    break;
                case 'd':
                    account_balance += operation_amount;
                    printf("pid: %d\tacc_no: %s\tdeposit: %d\tbalance: %d\n", pid, account_acc_no, operation_amount, account_balance);
                    break;
                case 'i':
                    printf("pid: %d\tacc_no: %s\tinquiry\t\tbalance: %d\n", pid, account_acc_no, account_balance);
                    break;
                default:
                    fprintf(stderr, "Invalid operation type: %c\n", operation_optype);
                    exit(1);
                }

                if (lseek(fd, lock.l_start, SEEK_SET) == -1)
                {
                    perror("lseek failed");
                    exit(1);
                }
                if (write(fd, account_acc_no, sizeof(account_acc_no)) == -1 ||
                    write(fd, account_name, sizeof(account_name)) == -1 ||
                    write(fd, &account_balance, sizeof(account_balance)) == -1)
                {
                    perror("write failed");
                    exit(1);
                }
                // lseek(fd, lock.l_start, SEEK_SET);
                // write(fd, account_acc_no, sizeof(account_acc_no));
                // write(fd, account_name, sizeof(account_name));
                // write(fd, &account_balance, sizeof(account_balance));


                lock.l_type = F_UNLCK;

                if (fcntl(fd, F_SETLKW, &lock) == -1)
                {
                    fprintf(stderr, "fcntl failed, process: %d, error: %d\n", pid, errno);
                    exit(1);
                }
                break;
            }
        }
        if (!found)
        {
            printf("pid: %d\tacc_no: %s\t", pid, operation_acc_no);
            switch (operation_optype)
            {
            case 'w':
                printf("withdraw: %d\t", operation_amount);
                break;
            case 'd':
                printf("deposit: %d\t", operation_amount);
                break;
            case 'i':
                printf("inquiry\t\t");
                break;
            default:
                fprintf(stderr, "Invalid operation type: %c\n", operation_optype);
                exit(1);
            }
            printf("acc_no %s 계좌 없음\n", operation_acc_no);
        }
        usleep(rand() % 1000001);
        close(fd);
    }
    fclose(operation_fp);
    exit(0);
}

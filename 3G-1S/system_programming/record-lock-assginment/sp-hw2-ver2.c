#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_CHILD 10
#define MAX_RECORD 100

// 계좌 정보를 저장하는 구조체
typedef struct
{
    char acc_no[6];
    char name[10];
    int balance;
} Account;

// 명령 정보를 저장하는 구조체
typedef struct
{
    char acc_no[6];
    char optype;
    int amount;
} Operation;

// 파일에서 계좌 정보를 읽어옴
void read_accounts(Account accounts[], int *num_account)
{
    FILE *fp = fopen("account.dat", "r");
    if (fp == NULL)
    {
        printf("Failed to open account.dat\n");
        exit(1);
    }

    // 파일에서 각 record를 읽어와서 구조체 배열에 저장
    int i = 0;
    while (fread(&accounts[i], sizeof(Account), 1, fp) == 1)
    {
        i++;
    }
    *num_account = i;

    fclose(fp);
}

// 파일에서 명령 정보를 읽어옴
void read_operations(Operation operations[], int *num_operation)
{
    FILE *fp = fopen("operation.dat", "r");
    if (fp == NULL)
    {
        printf("Failed to open operation.dat\n");
        exit(1);
    }

    // 파일에서 각 라인을 읽어와서 구조체 배열에 저장
    int i = 0;
    while (fscanf(fp, "%s %c", operations[i].acc_no, &operations[i].optype) != EOF)
    {
        if (operations[i].optype == 'w' || operations[i].optype == 'd')
        {
            fscanf(fp, "%d", &operations[i].amount);
        }
        i++;
    }
    *num_operation = i;

    fclose(fp);
}

// 계좌 정보를 파일에 저장
void write_accounts(Account accounts[], int num_account)
{
    FILE *fp = fopen("account.dat", "w");
    if (fp == NULL)
    {
        printf("Failed to open account.dat\n");
        exit(1);
    }

    // 구조체 배열의 각 record를 파일에 쓰기
    for (int i = 0; i < num_account; i++)
    {
        fwrite(&accounts[i], sizeof(Account), 1, fp);
    }

    fclose(fp);
}

// 임의의 sleep time을 생성해서 sleep
void random_sleep()
{
    srand(time(NULL));
    int sleep_time = rand() % 1000000;
    usleep(sleep_time);
}

// 인자로 전달된 명령을 수행하고 결과를 출력
void execute_operation(Operation operation, Account accounts[], int num_account, pid_t pid)
{

    int found = 0;
    int balance = 0;

    // 해당 계좌가 있는지 검사
    for (int i = 0; i < num_account; i++)
    {
        if (strcmp(operation.acc_no, accounts[i].acc_no) == 0)
        {
            found = 1;
            balance = accounts[i].balance;
            break;
        }
    }

    // 해당 계좌가 없으면 결과를 출력하고 함수 종료
    if (!found)
    {
        printf("pid: %d\tacc_no: %s\t", pid, operation.acc_no);
        printf("No such account\n");
        return;
    }

    // 명령에 따라 계좌 정보를 업데이트하고 결과를 출력
    switch (operation.optype)
    {
    case 'w':
        balance -= operation.amount;
        break;
    case 'd':
        balance += operation.amount;
        break;
    case 'i':
        break;
    default:
        printf("Invalid operation type\n");
        return;
    }
    for (int i = 0; i < num_account; i++)
    {
        if (strcmp(operation.acc_no, accounts[i].acc_no) == 0)
        {
            accounts[i].balance = balance;
            break;
        }
    }

    printf("pid: %d\tacc_no: %s\t", pid, operation.acc_no);
    if (operation.optype == 'i')
    {
        printf("inquiry\t\tbalance: %d\n", balance);
    }
    else if (operation.optype == 'w')
    {
        printf("withdraw: %d\tbalance: %d\n", operation.amount, balance);
    }
    else if (operation.optype == 'd')
    {
        printf("deposit: %d\tbalance: %d\n", operation.amount, balance);
    }
}

int main()
{
    Account accounts[MAX_RECORD];
    Operation operations[MAX_RECORD];
    int num_account, num_operation;

    read_accounts(accounts, &num_account);
    read_operations(operations, &num_operation);

    // child process 생성
    pid_t pid[MAX_CHILD];
    for (int i = 0; i < MAX_CHILD; i++)
    {
        pid[i] = fork();

        if (pid[i] == -1)
        {
            printf("Failed to fork\n");
            exit(1);
        }
        else if (pid[i] == 0)
        { // child process
            for (int j = 0; j < num_operation; j++)
            {
                random_sleep();
                execute_operation(operations[j], accounts, num_account, getpid());
            }
            exit(0);
        }
    }

    // parent process가 모든 child process의 종료를 기다림
    int status;
    for (int i = 0; i < MAX_CHILD; i++)
    {
        waitpid(pid[i], &status, 0);
    }

    // 결과 출력
    printf("\nFinal balances:\n");
    for (int i = 0; i < num_account; i++)
    {
        printf("acc_no: %s\tbalance: %d\n", accounts[i].acc_no, accounts[i].balance);
    }

    write_accounts(accounts, num_account);

    return 0;
}
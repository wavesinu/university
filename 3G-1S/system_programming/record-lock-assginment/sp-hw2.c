#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t operation_mutex = PTHREAD_MUTEX_INITIALIZER;

void *process_func(void *arg)
{
    char *account_file = "account.dat";
    char *operation_file = "operation.dat";
    char *operation = (char *)arg;

    // account.dat 파일 lock
    pthread_mutex_lock(&account_mutex);
    FILE *account_fp = fopen(account_file, "r");
    if (account_fp == NULL)
    {
        printf("Error: Failed to open account file.\n");
        exit(1);
    }

    // 계좌 정보 처리

    fclose(account_fp);
    pthread_mutex_unlock(&account_mutex);

    // operation.dat 파일 lock
    pthread_mutex_lock(&operation_mutex);
    FILE *operation_fp = fopen(operation_file, "r");
    if (operation_fp == NULL)
    {
        printf("Error: Failed to open operation file.\n");
        exit(1);
    }

    // 명령어 처리

    fclose(operation_fp);
    pthread_mutex_unlock(&operation_mutex);

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads[10];
    int i;

    for (i = 0; i < 10; i++)
    {
        if (pthread_create(&threads[i], NULL, process_func, "some_operation") != 0)
        {
            printf("Error: Failed to create thread.\n");
            exit(1);
        }
    }

    for (i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

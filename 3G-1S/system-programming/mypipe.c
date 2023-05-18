#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc == 1){ // No arguments
        exit(EXIT_FAILURE);
    }
    else if (argc == 2){ // One argument
        execlp(argv[1], argv[1], NULL);

        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    else if (argc == 3){ // arg =  prog1, prog2
        int fd[2];
        pid_t pid;

        if (pipe(fd) == -1){
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0){ // Child process
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);

            execlp(argv[1], argv[1], NULL);

            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
        else
        { // Parent process
            close(fd[1]);
            wait(NULL);
            dup2(fd[0], STDIN_FILENO);

            execlp(argv[2], argv[2], NULL);

            perror(argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "worng number of arguments");
        exit(EXIT_FAILURE);
    }

    return 0;
}

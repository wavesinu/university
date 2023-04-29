#include <stdio.h>
#include <unistd.h>
#include <string.h>

void pipe_exec(int nargc, char *nargv[])
{
	int pfd[2];

	pipe(pfd);

	// fork and setup a pipe
	if (fork()==0) {
		close(0);
		dup(pfd[0]);
		close(pfd[1]);
		close(pfd[0]);

		if (nargc > 1) pipe_exec(--nargc, ++nargv);
		else execl(nargv[0], nargv[0], nargv[1], NULL);
	}
	else {
		close(1);
		dup(pfd[1]);
		close(pfd[0]);
		close(pfd[1]);
	}

    return;
}

int main(int argc, char *argv[])
{
	int n;
	char buf;

	if (argc > 1) // need to add a pipe
		pipe_exec(--argc, ++argv);

	while ((n = read(0, &buf, 1)) > 0) write(1, &buf, n);

	return 0;
}
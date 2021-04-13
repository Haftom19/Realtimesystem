
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void sig_handler(int signum)
{
	printf("\nkeyboard handler function\n");
	signal(SIGINT, SIG_DFL); // Re Register signal handler for default action
}
int main()
{
	int NUM_CHILD = 4;
	int pid;
	pid = fork();

	signal(SIGINT, sig_handler); // Register signal handler

	for (int i = 0; i < NUM_CHILD; i++)
	{
		sleep(10); //sleep 10 seconds
		if (pid > 0)
		{														 //parent process
			printf("I'm the parent with pid [%d]!\n", getpid()); //
		}
		else if (pid == 0)
		{ //child process
			printf("I'm the child with id [%d] My pid is [%d]\n", getpid(), getppid());
			kill(pid, SIGTERM);
			exit(0);
		}
		else if (pid < 0)
		{ //error process
			perror("Ouch!  Unable to fork() child process!\n");
			exit(1);
		}
	}
	printf("process terminated");

	for (int i = 0; i < NUM_CHILD; i++)
	{
		wait(NULL);
	}
	return 0;
}
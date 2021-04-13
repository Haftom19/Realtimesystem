
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
	int NUM_CHILD = 4;
	int pid;
	pid = fork();
	for (int i = 0; i < NUM_CHILD; i++)
	{
		sleep(1); //delay one second
		if (pid > 0)
		{														 //parent process
			printf(" parent [%d]:-  parent process has been created!\n", getpid()); //
		}
		else if (pid == 0)
		{ //child process
			printf(" child [%d]:- child process created from parent [%d]\n", getpid(), getppid());
			sleep(10); //sleep 10  seconds
			exit(0);
		}
		else if (pid < 0)
		{ //error process
			perror("Ouch!  Unable to fork() child process!\n");
			kill(0, SIGTERM);
			exit(1);
		}
	}
	printf("All process has been terminated\n");
	int N_process = 0;
	while (wait(NULL) > 0)
	{
		++N_process;
	}
	sleep(1);
	printf("numbered of created child\n", N_process);
	return 0;
}
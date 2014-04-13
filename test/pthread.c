#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

void CloseMysql(int sig)
{
	if(sig == SIGINT)
		printf("**********\n");
}

void *thread_one(void *arg)
{
	int times = 10;
	signal(SIGINT, CloseMysql);
	while(times--)
	{
		printf("+++++11111111++++++\n");
		sleep(1);
	}
}

void *thread_two(void *arg)
{
	int times = 10;
	while(times--)
	{
		printf("++++++33333333333++++++\n");
		sleep(1);
	}
}

int main(void)
{
	pthread_t pid_one, pid_two;
	void *status;
	pthread_create(&pid_one, NULL, thread_one, NULL);
	pthread_create(&pid_two, NULL, thread_two, NULL);
	pthread_join(pid_one, &status);
	pthread_join(pid_two, &status);
	return 0;
}

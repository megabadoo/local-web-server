#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>

void *functionC(void *ptr);
void handler (int status);   /* definition of signal handler */
int  counter = 0;

main()
{
	int rc1, rc2;
	pthread_t thread1, thread2;

	// First set up the signal handler
	struct sigaction sigold, signew;

	signew.sa_handler=handler;
	sigemptyset(&signew.sa_mask);
	sigaddset(&signew.sa_mask,SIGINT);
	signew.sa_flags = SA_RESTART;
	sigaction(SIGINT,&signew,&sigold);

	/* Create independent threads each of which will execute functionC */

	if( (rc1=pthread_create( &thread1, NULL, &functionC, (void *)1)) )
	{
		printf("Thread creation failed: %d\n", rc1);
	}

	if( (rc2=pthread_create( &thread2, NULL, &functionC, (void *)2)) )
	{
		printf("Thread creation failed: %d\n", rc2);
	}

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */

	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL); 

	exit(0);
}

void handler (int status)
{
	printf("received signal %d\n",status);
}

void *functionC(void *ptr)
{
	int thnum = (long)ptr;
	for(;;) {
		sleep(1);
		counter++;
		printf("I am thread %d counter %d\n",thnum,counter);
	}
}

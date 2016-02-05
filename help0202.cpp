#include <iostream>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string>

std::queue<int> work;
sem_t work_to_do;
sem_t space_on_q;
sem_t mutex;

struct thread_params {
	long thread_id;
	std::string dir;
	
};

void* serve(void* arg){
	
	//long thread_id = long(arg);
	struct thread_params* tp = (struct thread_params*) arg;
	std::cout << "I'm thread " << tp->thread_id << std::endl;
	std::cout << "\t" << tp->dir << std::endl;
	for(;;){

	sem_wait(&work_to_do);
	sem_wait(&mutex);

	int my_conn = work.front();
	work.pop();

	std::cout << tp->thread_id << " working on " << my_conn << std::endl;

	sem_post(&mutex);
	sem_post(&space_on_q);

	//perform normal request serving stuff
	}
}

int main(int argc, char* argv[]){
	
	int queue_size = 20;
	sem_init(&space_on_q, 0, queue_size);
	sem_init(&work_to_do, 0, 0);
	sem_init(&mutex, 0, 1);		

	int num_threads = 10;
	std::cout << "threads hello!" << std::endl;
	pthread_t threads[num_threads];
	std::string dir = "something/";

	for(long i =0; i < num_threads; i++){

		struct thread_params tp;
		tp.thread_id = i;
		tp.dir = dir;
		int ret_val = pthread_create(&threads[i],
		0,
		serve,
		(void*) &tp);
	}
	

	//socket()
	//bind()
	//listen()




	
	for(int i=0;;i++){
		//accept (returns an int, push that int on the queue)
		sem_wait(&space_on_q);
		sem_wait(&mutex);

		sleep(1);
		work.push(i);
		std::cout << "pushed " << i << std::endl;

		sem_post(&mutex);
		sem_post(&work_to_do);
	}

	return 0;
}

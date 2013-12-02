#include "cs537.h"
#include "request.h"
#include <pthread.h>
#include <stdio.h>
pthread_cond_t empty;
pthread_cond_t fill;
pthread_mutex_t mutex;

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too

 int algo;
 int buff_put_ind = 0;
 int buff_get_ind = 0;
 int buff_count=0;
 int* buffer; 
 int buffer_sz=0;

void getargs(int *port,int *thread_num, int *buffer_sz,int argc, char *argv[])
{
    if (argc != 5) {
	fprintf(stderr, "Usage: server <portnum> <threads> <buffers> <schedulalg>\n");
	exit(1);
    }
    *port = atoi(argv[1]);
    *thread_num = atoi(argv[2]);
    *buffer_sz = atoi(argv[3]);
    if(strcmp("FIFO",argv[4])==0)
    	algo = 0;
    else if(strcmp("SFNF",argv[4])==0)
	algo=1;
    else if(strcmp("SFF",argv[4])==0)
	algo=2;
    else{
	printf("Invalid scheduling algorithm :%s\n",argv[4]);
	exit(1);
    }		
    printf("Port = %d\nThreads = %d\nBuffer = %d\nAlgo = %s\n",port[0],thread_num[0],buffer_sz[0],argv[4]);

}

 void put(int value) {
 buffer[buff_put_ind] = value;
 buff_put_ind = (buff_put_ind + 1) % buffer_sz;
 buff_count++;
 }

 int get() {
 int tmp = buffer[buff_get_ind];
 buff_get_ind = (buff_get_ind + 1) % buffer_sz;
 buff_count--;
 return tmp;
 }

void *producer(void *arg,int fd) {
pthread_mutex_lock(&mutex);
while (buff_count == buffer_sz)
	pthread_cond_wait(&empty, &mutex);
put(fd);
pthread_cond_signal(&fill);
pthread_mutex_unlock(&mutex);
return NULL;
}

void *consumer(void *arg) {
while(1){
pthread_mutex_lock(&mutex);
while (buff_count == 0)
	pthread_cond_wait(&fill, &mutex);
int temp = get();
pthread_cond_signal(&empty);
pthread_mutex_unlock(&mutex);
	requestHandle(temp);
	Close(temp);
}
}

int main(int argc, char *argv[])
{
    int listenfd, port, clientlen,thread_num;
    struct sockaddr_in clientaddr;
    int connfd;

    getargs(&port,&thread_num,&buffer_sz,argc, argv);
    buffer = malloc(sizeof(int)*buffer_sz); //allocate memory to buffer
    pthread_t p[thread_num];	// create worker threads
    int i=0;
    for(i=0;i<thread_num;i++){
	pthread_create(&p[i],NULL,consumer,NULL);
    }
    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        producer(NULL,connfd);
        //put(connfd);
   	//int temp=get();
	printf("%d\n",connfd);
	// 
	// CS537: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. However, for SFF, you may have to do a little work
	// here (e.g., a stat() on the filename) ...
	// 
//	requestHandle(temp);

//	Close(connfd);
    }
 
}


    


 

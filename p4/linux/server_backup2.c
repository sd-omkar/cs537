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

struct server_reg_t {
 int algo;
 int buff_put_ind;
 int buff_get_ind;
 int buff_count;
 int* buffer;
 int buffer_sz;
 int* valid;
 int* size_filename;
 int* size_file;
 int port;
 int thread_num;
};

void getargs(struct server_reg_t* server_reg,int argc, char *argv[])
{
    if (argc != 5) {
	fprintf(stderr, "Usage: server <portnum> <threads> <buffers> <schedulalg>\n");
	exit(1);
    }
    server_reg->port = atoi(argv[1]);
    server_reg->thread_num = atoi(argv[2]);
    server_reg->buffer_sz = atoi(argv[3]);
    if(strcmp("FIFO",argv[4])==0)
    	server_reg->algo = 0;
    else if(strcmp("SFNF",argv[4])==0)
	server_reg->algo=1;
    else if(strcmp("SFF",argv[4])==0)
	server_reg->algo=2;
    else{
	printf("Invalid scheduling algorithm :%s\n",argv[4]);
	exit(1);
    }		
    printf("Port = %d\nThreads = %d\nBuffer = %d\nAlgo = %s\n",server_reg->port,server_reg->thread_num,server_reg->buffer_sz,argv[4]);
}

void put(int value,struct server_reg_t* server_reg) {

 switch(server_reg->algo){
 case 0: server_reg->buffer[server_reg->buff_put_ind] = value;
	 server_reg->buff_put_ind = (server_reg->buff_put_ind + 1) % server_reg->buffer_sz;
	 server_reg->buff_count++;
  	 break;
/* case 1: for(i=0;i<buffer_sz;i++){
	 	if(valid[i]==0){
			filename_size[i]=getFilenameSize(value);
		}
	 }
	 buffer[buff_put_ind] = value;
	 buff_count++;
  	 break;*/
 default:break;
 }
 }

 int get(struct server_reg_t* server_reg) {
 int tmp; 
 switch(server_reg->algo){
 case 0: tmp = server_reg->buffer[server_reg->buff_get_ind];
	 server_reg->buff_get_ind = (server_reg->buff_get_ind + 1) % server_reg->buffer_sz;
	 server_reg->buff_count--;
	 break;
 
 default:break;
 }
 return tmp;
 }

void producer(int fd,struct server_reg_t* server_reg){
pthread_mutex_lock(&mutex);
while (server_reg->buff_count == server_reg->buffer_sz)
	pthread_cond_wait(&empty, &mutex);
put(fd,server_reg);
pthread_cond_signal(&fill);
pthread_mutex_unlock(&mutex);
//return NULL;
}

void *consumer(void* arg) {
while(1){
struct server_reg_t* server_reg=(struct server_reg_t*)arg;
pthread_mutex_lock(&mutex);
while (server_reg->buff_count == 0)
	pthread_cond_wait(&fill, &mutex);
int temp = get(server_reg);
pthread_cond_signal(&empty);
pthread_mutex_unlock(&mutex);
	requestHandle(temp);
	Close(temp);
}
}

int main(int argc, char *argv[])
{   
    struct server_reg_t server_reg;
    int listenfd, clientlen;
    struct sockaddr_in clientaddr;
    int connfd;

    server_reg.buff_put_ind = 0;
    server_reg.buff_get_ind = 0;
    server_reg.buff_count=0;
    server_reg.buffer_sz=0;
 
    getargs(&server_reg,argc,argv);
    server_reg.buffer = malloc(sizeof(int)*server_reg.buffer_sz); //allocate memory to buffer
    server_reg.valid = malloc(sizeof(int)*server_reg.buffer_sz); //allocate memory to store valid flags
    server_reg.size_filename = malloc(sizeof(int)*server_reg.buffer_sz); //allocate memory to store sizes of filenames
    server_reg.size_file = malloc(sizeof(int)*server_reg.buffer_sz); //allocate memory to store file size
    pthread_t p[server_reg.thread_num];	// create worker threads
    int i=0;
    for(i=0;i<server_reg.thread_num;i++){
	pthread_create(&p[i],NULL,consumer,&server_reg);
    }
    listenfd = Open_listenfd(server_reg.port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
	printf("%d\n",connfd);
        producer(connfd,&server_reg);
        //put(connfd);
   	//int temp=get();
//	printf("%d\n",connfd);
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

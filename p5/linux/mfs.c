#include "mfs.h"
#include "udp.h"
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
static int port_num;
struct sockaddr_in saddr;    	
static int sd;
MFS_Transmit_t message;
char buffer[70];
int transmit(){
        return UDP_Write(sd, &saddr, (char*) &message, sizeof(MFS_Transmit_t));	
}
return_struct_t reply_rec;

int MFS_Init(char *hostname, int port){
INIT:
//	printf("MFS_Init CKP\n");
	port_num=port;
	sd = UDP_Open(0);
	assert(sd > -1);
    	int rc = UDP_FillSockAddr(&saddr,hostname,port_num);
	saddr=saddr;
   	assert(rc == 0);
        struct sockaddr_in raddr;

//	MFS_Transmit_t message;
	sprintf(message.name,"Init");
	message.transmit=0;
//INIT: 

	rc = transmit();

	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);


	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	int rc2 = select(sd+1,&r,NULL,NULL,&t);
	printf("Running %s on %d and select returned =%d\n",hostname,port_num,rc);
	
	if(rc2>0){
	        rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	//	printf("TIMEOUT\n");
	//	exit(1);
	}
	else{
//		printf("TIMEOUT\n");
		goto INIT;
	}
	return(0);
}
int MFS_Lookup(int pinum, char *name){
	if(strlen(name)>=60){
		printf("Too long name\n");
		return -1;
	}

	message.transmit = 1;
	message.pinum=pinum;
	sprintf(message.name,name);
	int rc;
LOOKUP:
	rc=transmit();
//	if(rc<0){
//		printf("Write error\n");
//		return(-1);
//	}
        struct sockaddr_in raddr;

//	printf("MFS_Lookup CKP\n");

	///////////////////////////////////////
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);


	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
        rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	}
	else{
//		printf("TIMEOUT\n");
		goto LOOKUP;
	}
	return(reply_rec.return_val);
}
int MFS_Stat(int inum, MFS_Stat_t *m){
	message.transmit = 2;
	message.inum=inum;
	sprintf(message.name,"Stat");
	int rc;
STAT:
	rc=transmit();
//	if(rc<0){
//		printf("Write error\n");
//		return(-1);
//	}
        struct sockaddr_in raddr;
	///////////////////////////////////////
	struct timeval t;
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);
	t.tv_sec=5;
	t.tv_usec=0;
	int rc2 = select(sd+1,&r,NULL,NULL,&t);
	if(rc2>0){
        rc = UDP_Read(sd, &raddr, (char*)m, sizeof(MFS_Stat_t));
        rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	}
	else{
//		printf("TIMEOUT\n");
		goto STAT;
	}
//	printf("Returned size = %d, type = %d\n",m->size,m->type);

  //      printf("CLIENT:: read %d bytes (message: %s) (return val:%d)\n", rc, reply_rec.reply,reply_rec.return_val);

//	printf("MFS_Stat CKP\n");
	return(reply_rec.return_val);

}
int MFS_Write(int inum, char *buffer, int block){
	if(block>13)
		return -1;
	message.transmit = 3;
	message.inum = inum;
	message.block = block;
	sprintf(message.name,"Write");
	int rc;
WRITE:
	rc = transmit();
        struct sockaddr_in raddr;
//	if(rc<0){
//		printf("WRITE failed\n");
//		return -1;
//	}
//	printf("MFS_Write CKP\n");
	///////////////////////////////////////
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);
	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
       		 rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	}
	else{
//		printf("TIMEOUT\n");
		goto WRITE;
	}
WRITE2:
	rc = UDP_Write(sd, &saddr, (char*) buffer, MFS_BLOCK_SIZE);
//	fd_set r;
//	FD_ZERO(&r);
//	FD_SET(sd,&r);
//	struct timeval t;
//	t.tv_sec=5;
//	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
        rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	}
	else{
//		printf("TIMEOUT\n");
		goto WRITE2;
	}

	return(reply_rec.return_val);

}
int MFS_Read(int inum, char *buffer, int block){
	if(block>13)
		return -1;
	message.transmit = 4;
	message.inum = inum;
	message.block = block;
	sprintf(message.name,"Read");
	int rc;
READ:
	rc = transmit();
        struct sockaddr_in raddr;
//	if(rc<0){
//		printf("READ failed\n");
//		return -1;
//	}
//	printf("MFS_Read CKP = %d\n",reply_rec.return_val);

	///////////////////////////////////////
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);
	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
	rc = UDP_Read(sd, &raddr, (char*) buffer, MFS_BLOCK_SIZE);
	rc = UDP_Read(sd, &raddr, (char*) &reply_rec, sizeof(MFS_Transmit_t));
	}
	else{
	//	printf("TIMEOUT\n");
		goto READ;
	}

	return(reply_rec.return_val);

}
int MFS_Creat(int pinum, int type, char *name){
	if(strlen(name)>=60){
		return -1;
	}
	message.transmit=5;
	message.pinum = pinum;
	message.type = type;
	sprintf(message.name,name);
	int rc;

CREAT:
	rc = transmit();
        struct sockaddr_in raddr;
//	printf("MFS_Creat CKP\n");

	///////////////////////////////////////
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);
	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
        rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	}
	else{
	//	printf("TIMEOUT\n");
		goto CREAT;
	}
	/////////////////
	return(reply_rec.return_val);

}
int MFS_Unlink(int pinum, char *name){
	if(strlen(name)>=60){
		return -1;
	}
	message.transmit = 6;
	message.pinum = pinum;
	sprintf(message.name,name);
	int rc ;
UNLINK:
	rc= transmit();
	struct sockaddr_in raddr;
//	printf("MFS_Unlink CKP\n");
	///////////////////////////////////////
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);
	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
	rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	}
	else{
	//	printf("TIMEOUT\n");
		goto UNLINK;
	}
	/////////////////
	return(reply_rec.return_val);

}
int MFS_Shutdown(){
//	printf("MFS_Shutdown CKP\n");
	//char temp[20];
//	MFS_Transmit_t message;
	sprintf(message.name,"Shutdown");
	message.transmit=7;
        int rc;
	struct sockaddr_in raddr;
SHUTDOWN:
	rc= transmit();
//	if(rc<0)
//		printf("ShutDown write failed!!\n");
//	exit(0);
	fd_set r;
	FD_ZERO(&r);
	FD_SET(sd,&r);


	struct timeval t;
	t.tv_sec=5;
	t.tv_usec=0;
	rc = select(sd+1,&r,NULL,NULL,&t);
	if(rc>0){
	        rc = UDP_Read(sd, &raddr, (char*)&reply_rec, sizeof(return_struct_t));
	//	printf("TIMEOUT\n");
	//	exit(1);
	}
	else{
	//	printf("TIMEOUT\n");
		goto SHUTDOWN;
	}
	return(0);

}


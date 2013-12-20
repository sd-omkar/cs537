#include <stdio.h>
#include "mfs.h"

int main(int argc,char *argv[]){
	char message[100];
	char buffer[MFS_BLOCK_SIZE],buffer2[MFS_BLOCK_SIZE],rbuffer[MFS_BLOCK_SIZE],rbuffer2[MFS_BLOCK_SIZE];
	int i=0;
	MFS_Stat_t testing;
	for(i=0;i<MFS_BLOCK_SIZE;i++){
		buffer[i]='a';
		buffer2[i]='b';
	}
	sprintf(message,"mumble-23.cs.wisc.edu");
	int rc = MFS_Init(message,10001);
/*
//	rc = MFS_Lookup(0,"helloworld.c");
	rc = MFS_Creat(0,MFS_REGULAR_FILE,"test");
	//rc = MFS_Creat(0,MFS_DIRECTORY,"newfile");
	
	int inode = MFS_Lookup(0,"test");
	printf("inode = %d\n",inode);
	rc = MFS_Write(inode,buffer,0);
	printf("WRITE return = %d\n",rc);
	rc = MFS_Read(inode,rbuffer,0);
	printf("Read return = %d\n",rc);
	for(i=0;i<MFS_BLOCK_SIZE;i++)
		printf("%c\n",rbuffer[i]);*/
//	rc = MFS_Stat(inode,&testing);
//	printf("type = %d size= %d\n",testing.type,testing.size);

//	rc = MFS_Creat(inode,MFS_REGULAR_FILE,"testfile");
//	rc = MFS_Stat(inode,&testing);
//
/*
	printf("type = %d size= %d\n",testing.type,testing.size);
	rc = MFS_Unlink(0,"testdir");
	printf("Unlink return code: %d\n",rc);
	rc = MFS_Unlink(inode,"testfile");
	printf("Unlink return code: %d\n",rc);
	rc = MFS_Lookup(inode,"testfile");
	printf("Lookup return  = %d\n",rc);
	rc = MFS_Stat(inode,&testing);
	printf("type = %d size= %d\n",testing.type,testing.size);
	rc = MFS_Unlink(0,"testdir");
	printf("Unlink return code: %d\n",rc);*/
//	inode = MFS_Lookup(0,"newfile");
//	printf("inode returned after unlink = %d\n",inode);	

//	rc = MFS_Write(inode,buffer,0);
//	printf("RC = %d\n",rc);
//	rc = MFS_Write(inode,buffer2,13);
//	rc = MFS_Write(inode,buffer2,0);	
//	rc = MFS_Read(inode,rbuffer,0);

//	rc = MFS_Read(inode,rbuffer2,13);
//	for(i=0;i<MFS_BLOCK_SIZE;i++){
//		if((rbuffer2[i]-rbuffer[i])!=-1){
//			printf("Error at %d is %c and %c\n",i,rbuffer[i],rbuffer2[i]);
//			break;
//		}
//	}		
/*	rc = MFS_Creat(0,MFS_REGULAR_FILE,"newfile2.txt");
	rc = MFS_Creat(0,MFS_REGULAR_FILE,"newfile3.txt");
	rc = MFS_Creat(0,MFS_REGULAR_FILE,"newfile4.txt");
	rc = MFS_Creat(0,MFS_REGULAR_FILE,"newfile5.txt");
*/
//	rc = MFS_Write(inode,buffer,3);
//	printf("Write value = %d\n",rc);
//	MFS_Stat_t m;
//	rc = MFS_Stat(inode,&m);	
//	printf("STAT: return value:%d, size= %d, type = %d\n",rc,m.size,m.type);
	MFS_Shutdown();
	return(0);


}

#include <stdio.h>
#include "udp.h"
#include <string.h>
#include "mfs.h"
#define BUFFER_SIZE (4096)
#include <fcntl.h>

int lookup(int pinum,char* filename,int fd);
int stat_rec(int inum,MFS_Stat_t* m,int fd);
int mfs_create(int pinum,int type,char* name, int fd);
int mfs_write(int inum,char* buffer,int block,int fd);
int mfs_read(int inum,char* buffer,int block,int fd);
int mfs_unlink(int pinum, char *name,int fd);
int unlink_lookup(int inode,int fd);
int main(int argc, char *argv[])
{
    if(argc!=3){
	printf("Usage: server [port-num] [file-system-image]\n");
	exit(1);
    }
    int port_num=atoi(argv[1]);
	printf("Chetan's server starting %s on %d\n",argv[2],port_num);

//////////////////////////////////////////////////////
////////// FILE OPENING; CREATING IF DOESN'T EXIST////
//////////////////////////////////////////////////////
        int fd = open(argv[2],O_RDWR,S_IRWXU|S_IRUSR);
        if(fd>=0){
                printf("File exists, file %s opened\n",argv[2]);
                goto A;
        }
        else
                printf("File does not exist, hence creating the file %s\n",argv[2]);


        fd = open(argv[2],O_RDWR|O_CREAT,S_IRWXU|S_IRUSR);
        int i,j,k;
	char buffer[MFS_BLOCK_SIZE];
        //checkpoint - EOF
        int write_buf = 257*4 + 4096*4;
        write(fd,(const void*)(&write_buf),4);

        //pointers to inode maps created
        for(i=0;i<256;i++){
                write_buf = (257*4)+(i*16*4);
                write(fd,(const void*)(&write_buf),4);
        }
        write_buf=-1;
        off_t imap0_offset = lseek(fd, 0 ,SEEK_CUR );
        for(i=0;i<4096;i++)
                write(fd,(const void*)(&write_buf),4);

        off_t block0_offset = lseek( fd, 0, SEEK_CUR ) ;
        MFS_Stat_t block0;
        block0.type = MFS_DIRECTORY;
        block0.size = MFS_BLOCK_SIZE;

        MFS_DirEnt_t dir1;
        dir1.name[0] = '.';
        dir1.name[1] = '\0';
        dir1.inum = 0;
        write(fd,(const void*)(&dir1),sizeof(MFS_DirEnt_t));
        dir1.name[1] = '.';
        dir1.name[2] = '\0';
        write(fd,(const void*)(&dir1),sizeof(MFS_DirEnt_t));
        dir1.inum = -1;
        for(i=0;i<62;i++)
        write(fd,(const void*)(&dir1),sizeof(MFS_DirEnt_t));

        off_t inode0_offset = lseek (fd, 0 , SEEK_CUR);

        // writing the INODE0 - for the root directory
        write(fd ,(const void *)(&block0),sizeof(MFS_Stat_t));
        int temp_ptr = (int) block0_offset;
        write(fd, (const void *)(&temp_ptr),4);
        temp_ptr = -1;
        for(i=0;i<13;i++)
                write(fd, (const void *)(&temp_ptr),4);
        //resetting the checkpoint region - point to the EOF
        off_t eof_offset = lseek (fd, 0, SEEK_CUR);
        lseek (fd, 0 , SEEK_SET);
        temp_ptr = (int) eof_offset;
        write (fd ,&eof_offset, 4);

        //update the imap
        lseek(fd, imap0_offset, SEEK_SET);
        temp_ptr = (int) inode0_offset;
        write (fd,(const void*)&temp_ptr,4);


///////////////////////////////////////////////////////
//////////// FILE OPEN/CREATE FINISHED ////////////////
///////////////////////////////////////////////////////
int nop=0;
A:
//    printf("reached the label\n");
	nop =nop;
    int sd = UDP_Open(port_num);
    if(sd <0){
	UDP_Close(sd);
    	sd=UDP_Open(sd);
    }
    assert(sd > -1);

    printf("                                SERVER:: waiting in loop\n");
    MFS_Transmit_t message_rec;
	    return_struct_t reply_rec;
	    reply_rec.return_val=0;
	    sprintf(reply_rec.reply, "reply");
    while (1) {
	struct sockaddr_in s;
//	char buffer[BUFFER_SIZE];
	int rc = UDP_Read(sd, &s, (char*)&message_rec, sizeof(MFS_Transmit_t));
	if (rc > 0) {
//	    printf("    SERVER:: read %d bytes\n", rc);
		switch(message_rec.transmit){
		case 0:	
//			printf("Server INIT:Client %s\n",message_rec.name);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
			break;

		case 1:
//			printf("Server LOOKUP:Client %s\n",message_rec.name);
			
			sprintf(reply_rec.reply,"Server received lookup");
			reply_rec.return_val = lookup(message_rec.pinum,message_rec.name,fd);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
			if(rc<0)
				printf("Write error\n");
			break;
		case 2:
//			printf("Server STAT:Client %s\n",message_rec.name);
			sprintf(reply_rec.reply,"Server received stat");
			MFS_Stat_t msg_stat;
			reply_rec.return_val = stat_rec(message_rec.inum,&msg_stat,fd);
	      	        rc = UDP_Write(sd, &s,(char*) &msg_stat, sizeof(MFS_Stat_t));
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
			if(rc<0)
				printf("Write error\n");

			break;
		case 3:
//			printf("Server WRITE:Client %s\n",message_rec.name);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
			rc = UDP_Read(sd, &s, (char*)buffer, MFS_BLOCK_SIZE);
			reply_rec.return_val = mfs_write(message_rec.inum,buffer,message_rec.block,fd);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
			break;
		case 4:
	//		printf("Server READ:Client %s\n",message_rec.name);
			reply_rec.return_val = mfs_read(message_rec.inum,buffer,message_rec.block,fd);
			rc = UDP_Write(sd, &s, (char*)buffer, MFS_BLOCK_SIZE);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
//			printf("Server READ:Client %s\n",message_rec.name);
			break;
		case 5:
//			printf("Server CREATE:Client  %s\n",message_rec.name);
			sprintf(reply_rec.reply,"Server received create");
			reply_rec.return_val = mfs_create(message_rec.pinum,message_rec.type,message_rec.name,fd);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
		if(rc<0)
				printf("Write error\n");
			break;
		case 6:
			reply_rec.return_val = mfs_unlink(message_rec.pinum,message_rec.name,fd);
			rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
//			printf("Server UNLINK:Client %s\n",message_rec.name);
			break;
		case 7:
//			printf("Server SHUTDOWN:Client %s\n",message_rec.name);
	      	        rc = UDP_Write(sd, &s,(char*) &reply_rec, sizeof(return_struct_t));
			exit(0);
			break;
		default: break;
		}	
	
	}
    }


  /*  while (1) {
	struct sockaddr_in s;
	char buffer[BUFFER_SIZE];
	int rc = UDP_Read(sd, &s, buffer, BUFFER_SIZE);
	if (rc > 0) {
	    printf("                                SERVER:: read %d bytes (message: '%s')\n", rc, buffer);
	    char reply[BUFFER_SIZE];
	    sprintf(reply, "reply");
	    rc = UDP_Write(sd, &s, reply, BUFFER_SIZE);
	}
    }
*/
        printf("Closing file \n");
        //close the file
        close(fd);     

    return 0;
}

int lookup(int pinum,char* filename,int fd){
	if((pinum<0)||(pinum>4095)){
		printf("Invalid pinum\n");
		return(-1);
	}
	int preimap = pinum / 16;
	int temp_addr;
	
	lseek(fd,0,SEEK_SET);
	////////////////////////////////////////
	lseek(fd,(preimap*4)+4,SEEK_SET);
	int rc = read(fd, (void*)&temp_addr, 4);
	if(rc<0){
		printf("Read error\n");
		return -1;
	}
	
	/////////////////////////////////////////
	lseek(fd,temp_addr+((pinum%16)*4),SEEK_SET);
	rc = read(fd, (void*)&temp_addr, 4);
	if(rc<0){
		printf("Read error\n");
		return -1;
	}
	if(temp_addr == -1){
		printf("pinum doesn't exist\n");
		return -1;
	}

	///////////////////////////////////////
	lseek(fd,temp_addr,SEEK_SET);
	MFS_Stat_t node0;
	MFS_DirEnt_t data0;
	rc = read(fd, (void*)&node0, sizeof(MFS_Stat_t));
	if(rc<0){
		printf("Read error\n");
		return -1;
	}
	if(node0.type != MFS_DIRECTORY){
		printf("Not a directory\n");
		return -1;
	}
//	else
//		printf("Directory hit\n");	
	int ptrs[14];
	////////////////////////////
	int i,j;
	rc = read(fd, (void*)&ptrs, 14*4);
	if(rc<0){
		printf("Read error\n");
		return -1;	
	}
	for(i=0;i<14;i++){
		if(ptrs[i]==-1)
			continue;
//		else
//			printf("Found entry %d\n",i);
		lseek(fd,ptrs[i],SEEK_SET);
		for(j=0;j<64;j++){
			rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
			if(data0.inum==-1)
				continue;
			else{
//				printf("Comparing %s with %s\n",data0.name,filename);
				if(strcmp(data0.name,filename)==0)
					return data0.inum;
			}
		}
	}	
	return -1;
}
int stat_rec(int inum,MFS_Stat_t* m,int fd){
	if((inum<0)||(inum>4095)){
		printf("Invalid inum\n");
		return(-1);
	}
	int preimap = inum / 16;
	int temp_addr;
	
	lseek(fd,0,SEEK_SET);
	////////////////////////////////////////
	lseek(fd,(preimap*4)+4,SEEK_SET);
	int rc = read(fd,(void*)&temp_addr, 4);
	if(rc<0){
		printf("Read error\n");
		return -1;
	}
	
	/////////////////////////////////////////
	lseek(fd,temp_addr+((inum%16)*4),SEEK_SET);
	rc = read(fd, (void*)&temp_addr, 4);
	if(rc<0){
		printf("Read error\n");
		return -1;
	}
	if(temp_addr == -1){
		printf("inum doesn't exist\n");
		return -1;
	}

	///////////////////////////////////////
	lseek(fd,temp_addr,SEEK_SET);
//	MFS_Stat_t node0;
	//MFS_DirEnt_t data0;
	rc = read(fd, (void*)m, sizeof(MFS_Stat_t));
	if(rc<0){
		printf("Read error\n");
		return -1;
	}
	if(m->type == MFS_DIRECTORY){
//		printf("It's a directory\n");
		return 0;
	}
	else if(m->type== MFS_REGULAR_FILE){
//		printf("Directory hit\n");
		return 0;
	}
	else{
//		printf("Invalid iNode\n");
		return -1;
	}	
	return -1;
}
int inode_available(int inum,int fd){
	lseek(fd,0,SEEK_SET);
        int preimap = inum / 16;
        int temp_addr;
        lseek(fd,(preimap*4)+4,SEEK_SET);
        int rc = read(fd, (void*)&temp_addr, 4);
        if(rc<0){
                printf("Read error\n");
                return -1;
        }
        lseek(fd,temp_addr+((inum%16)*4),SEEK_SET);
        rc = read(fd, (void*)&temp_addr, 4);
        if(rc<0){
                printf("Read error\n");
                return -1;
        }
        if(temp_addr == -1){
        //        printf("inode available = %d\n",inum);
                return 0;
        }

return -1;
}

int mfs_create(int pinum,int type,char* name, int fd){
int i,j;
int ptrs[14],ptrs_specific;
	MFS_Stat_t node0;
	MFS_DirEnt_t data0;
int new_inode=-1;
	for(i=0;i<4096;i++){
		if(inode_available(i,fd)==0){
			new_inode = i;
			break;
		}
	}	
int flag=0;
int checkpoint,new_file,new_dirdata,new_nodedata,new_mapdata,old_dirdata,old_mapdata,old_nodedata,old_nodedata2,new_nodedata2,new_mapdata2,old_mapdata2,new_dirent;
switch(type){
case(MFS_REGULAR_FILE):
	lseek(fd,0,SEEK_SET);
	read(fd,(void*)&checkpoint,4);
	lseek(fd,checkpoint,SEEK_SET);
	new_file = checkpoint;
	MFS_Stat_t newfiledir;
	newfiledir.size = 0;
	newfiledir.type = MFS_REGULAR_FILE;
	write(fd,(void*)&newfiledir,sizeof(MFS_Stat_t));
	int temp = -1;
	for(i=0;i<14;i++)
		write(fd,(void*)&temp ,4);	
	new_dirdata = (int) lseek(fd,0,SEEK_CUR);

	if((pinum<0)||(pinum>4095)){
//		printf("Invalid pinum\n");
		return(-1);
	}
	int preimap = pinum / 16;
	int temp_addr;
	
	lseek(fd,0,SEEK_SET);
	////////////////////////////////////////
	lseek(fd,(preimap*4)+4,SEEK_SET);
	int rc = read(fd, (void*)&temp_addr,4);
//	if(rc<0){
//		printf("Read error\n");
//		return -1;
//	}
	
	/////////////////////////////////////////
	old_mapdata = temp_addr;
	lseek(fd,temp_addr+((pinum%16)*4),SEEK_SET);
	rc = read(fd, (void*)&temp_addr, 4);
	old_nodedata = temp_addr;
//	if(rc<0){
//		printf("Read error\n");
//		return -1;
//	}
	if(temp_addr == -1){
//		printf("pinum doesn't exist\n");
		return -1;
	}

	///////////////////////////////////////
	lseek(fd,temp_addr,SEEK_SET);
	rc = read(fd, (void*)&node0, sizeof(MFS_Stat_t));
//	if(rc<0){
//		printf("Read error\n");
//		return -1;
//	}
	if(node0.type != MFS_DIRECTORY){
//		printf("Not a directory\n");
		return -1;
	}
//	else
//		printf("Directory hit\n");	
	
	////////////////////////////
	rc = read(fd, (void*)&ptrs, 14*4);
	if(rc<0){
//		printf("Read error\n");
		return -1;	
	}
	for(i=0;i<14;i++){
		/*
		if(ptrs[i]==-1)
			continue;
		else{
			old_dirdata = ptrs[i];
			printf("Found entry %d\n",i);
		}*/
		flag=0;
		if(ptrs[i]==-1){
			ptrs[i]=new_dirdata;
			flag=1;
//			printf("Creating new data block for directories\n");
			ptrs_specific = i;
			old_dirdata = ptrs[i];
			goto D;
		}
			old_dirdata = ptrs[i];
		lseek(fd,ptrs[i],SEEK_SET);
		for(j=0;j<64;j++){
			rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
			if((data0.inum!=-1)&&(strcmp(data0.name,name)==0)){
//				printf("Already exists\n");	
				return 0;
			}
		}
		lseek(fd,ptrs[i],SEEK_SET);
		for(j=0;j<64;j++){
//			off_t temp_write = lseek(fd,0,SEEK_CUR);
			rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
			if(data0.inum==-1){
				ptrs_specific = j;
				goto B;
//				lseek(fd,temp_write,SEEK_SET);
//				rc=write(fd,name,60);
//				rc=write(fd,&new_inode,4);
//				return 0;
			}
		}
	}
	return -1;
B:
	for(i=0;i<64;i++){
/////////////////////////////////
	if(flag==0){
	   if(i==ptrs_specific){
		data0.inum = new_inode;
		sprintf(data0.name,name);
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	   }
	   else{
		lseek(fd,old_dirdata+i*64,SEEK_SET);
		rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	   }
	}	
	else{
		if(i==0){
		data0.inum=new_inode;
		sprintf(data0.name,name);
		lseek(fd,new_dirdata,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		}
		else{
		data0.inum =-1;
//		lseek(fd,new_dirdata+i*64,SEEK_SET);
//		rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		}
	}
///////////////////////////////
/*
	   if(i==ptrs_specific){
		data0.inum = new_inode;
		sprintf(data0.name,name);
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	   }
	   else{
		lseek(fd,old_dirdata+i*64,SEEK_SET);
		rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	   }*/

	}
	new_nodedata = (int) lseek(fd,0,SEEK_CUR);
		
	lseek(fd,old_nodedata,SEEK_SET);
	rc=read(fd,(void*)&node0,sizeof(MFS_Stat_t));
	old_nodedata2 = (int) lseek(fd,0,SEEK_CUR);
	lseek(fd,new_nodedata,SEEK_SET);
	rc=write(fd,(void*)&node0,sizeof(MFS_Stat_t));
	new_nodedata2=(int)lseek(fd,0,SEEK_CUR);
	for(i=0;i<14;i++){
		lseek(fd,old_nodedata2+i*4,SEEK_SET);
		rc=read(fd,(void*)&temp_addr,4);
		if(temp_addr==old_dirdata)
			temp_addr = new_dirdata;
		lseek(fd,new_nodedata2+i*4,SEEK_SET);
		rc=write(fd,(void*)&temp_addr,4);
	}

	new_mapdata = (int)lseek(fd,0,SEEK_CUR);
	for(i=0;i<16;i++){
		lseek(fd,old_mapdata+i*4,SEEK_SET);
		rc=read(fd,(void*)&temp_addr,4);
		if(temp_addr==old_nodedata)
			temp_addr = new_nodedata;
		if(i==new_inode)
			temp_addr = new_file;
		lseek(fd,new_mapdata+i*4,SEEK_SET);
		rc=write(fd,(void*)&temp_addr,4);
	}

	new_mapdata2 = (int) lseek(fd,0,SEEK_CUR);
		
	lseek(fd,(pinum/16)*4+4,SEEK_SET);
	rc=write(fd,(void*)&new_mapdata,4);

	
	if(preimap == new_inode/16){
		checkpoint = new_mapdata2;
		goto C;
	}
	preimap = new_inode / 16;
	lseek(fd,0,SEEK_SET);
	lseek(fd,(preimap*4)+4,SEEK_SET);
	rc = read(fd, (void*)&temp_addr,4);

	old_mapdata2 = temp_addr;
	for(i=0;i<16;i++){
		lseek(fd,old_mapdata2+i*4,SEEK_SET);
		rc=read(fd,(void*)&temp_addr,4);
		if(i==new_inode%16)
			temp_addr = new_file;////////new_mapdata2
		lseek(fd,new_mapdata2+i*4,SEEK_SET);
		rc=write(fd,(void*)&temp_addr,4);
	}

	checkpoint = (int) lseek(fd,0,SEEK_CUR);

	lseek(fd,(new_inode/16)*4+4,SEEK_SET);
	rc = write(fd, (void*)&new_mapdata2, 4);
//	old_nodedata = temp_addr;

C:
	lseek(fd,0,SEEK_SET);
	rc=write(fd,(void*)&checkpoint,4);
	break;
///////////////////////////////////////////////////////////////////////////////
case(MFS_DIRECTORY):
	lseek(fd,0,SEEK_SET);
	read(fd,(void*)&checkpoint,4);
	lseek(fd,checkpoint,SEEK_SET);

	new_dirent = checkpoint;
	data0.inum = new_inode;
	sprintf(data0.name,".");
	write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	data0.inum = pinum;
	sprintf(data0.name,"..");
	write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	data0.inum = -1;
	for(i=0;i<62;i++)
		write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	
	new_file = (int)lseek(fd,0,SEEK_CUR);
//	MFS_Stat_t newfiledir;
	newfiledir.size = MFS_BLOCK_SIZE;
	newfiledir.type = MFS_DIRECTORY;
	write(fd,(void*)&newfiledir,sizeof(MFS_Stat_t));
	
	temp = new_dirent;
	write(fd,(void*)&temp,4);
	temp = -1;
	for(i=0;i<13;i++)
		write(fd,(void*)&temp ,4);	
	new_dirdata = (int) lseek(fd,0,SEEK_CUR);

	if((pinum<0)||(pinum>4095)){
//		printf("Invalid pinum\n");
		return(-1);
	}
	preimap = pinum / 16;
//	temp_addr;
	
	lseek(fd,0,SEEK_SET);
	////////////////////////////////////////
	lseek(fd,(preimap*4)+4,SEEK_SET);
 rc = read(fd, (void*)&temp_addr,4);
	if(rc<0){
//		printf("Read error\n");
		return -1;
	}
	
	/////////////////////////////////////////
	old_mapdata = temp_addr;
	lseek(fd,temp_addr+((pinum%16)*4),SEEK_SET);
	rc = read(fd, (void*)&temp_addr, 4);
	old_nodedata = temp_addr;
//	if(rc<0){
//		printf("Read error\n");
//		return -1;
//	}
	if(temp_addr == -1){
//		printf("pinum doesn't exist\n");
		return -1;
	}

	///////////////////////////////////////
	lseek(fd,temp_addr,SEEK_SET);
	rc = read(fd, (void*)&node0, sizeof(MFS_Stat_t));
	if(rc<0){
//		printf("Read error\n");
		return -1;
	}
	if(node0.type != MFS_DIRECTORY){
//		printf("Not a directory\n");
		return -1;
	}
//	else
//		printf("Directory hit\n");	
	
	////////////////////////////
	rc = read(fd, (void*)&ptrs, 14*4);
//	if(rc<0){
//		printf("Read error\n");
//		return -1;	
//	}
	for(i=0;i<14;i++){
		/*
		if(ptrs[i]==-1)
			continue;
		else{
			old_dirdata = ptrs[i];
			printf("Found entry %d\n",i);
		}*/
		flag=0;
		if(ptrs[i]==-1){
			ptrs[i]=new_dirdata;
			flag=1;
//			printf("Creating new data block for directories\n");
			ptrs_specific = i;
			old_dirdata = ptrs[i];
			goto D;
		}
			old_dirdata = ptrs[i];

		lseek(fd,ptrs[i],SEEK_SET);
		for(j=0;j<64;j++){
			rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
			if((data0.inum!=-1)&&(strcmp(data0.name,name)==0)){
//				printf("Already exists\n");	
				return 0;
			}
		}
		lseek(fd,ptrs[i],SEEK_SET);
		for(j=0;j<64;j++){
//			off_t temp_write = lseek(fd,0,SEEK_CUR);
			rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
			if((data0.inum==-1)||(flag==1)){
				ptrs_specific = j;
				goto D;
//				lseek(fd,temp_write,SEEK_SET);
//				rc=write(fd,name,60);
//				rc=write(fd,&new_inode,4);
//				return 0;
			}
		}
	}
	return -1;
D:
	for(i=0;i<64;i++){
	if(flag==0){
	   if(i==ptrs_specific){
		data0.inum = new_inode;
		sprintf(data0.name,name);
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	   }
	   else{
		lseek(fd,old_dirdata+i*64,SEEK_SET);
		rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
	   }
	}	
	else{
		if(i==0){
		data0.inum=new_inode;
		sprintf(data0.name,name);
		lseek(fd,new_dirdata,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		}
		else{
		data0.inum =-1;
//		lseek(fd,new_dirdata+i*64,SEEK_SET);
//		rc=read(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		lseek(fd,new_dirdata+i*64,SEEK_SET);
		rc=write(fd,(void*)&data0,sizeof(MFS_DirEnt_t));
		}
	}
	}
	new_nodedata = (int) lseek(fd,0,SEEK_CUR);
		
	lseek(fd,old_nodedata,SEEK_SET);
	rc=read(fd,(void*)&node0,sizeof(MFS_Stat_t));
	old_nodedata2 = (int) lseek(fd,0,SEEK_CUR);
	lseek(fd,new_nodedata,SEEK_SET);
	rc=write(fd,(void*)&node0,sizeof(MFS_Stat_t));
	new_nodedata2=(int)lseek(fd,0,SEEK_CUR);
	////////////////////////////////////////////////////
	for(i=0;i<14;i++){
		lseek(fd,old_nodedata2+i*4,SEEK_SET);
		rc=read(fd,(void*)&temp_addr,4);
		if(flag==0){
			if(temp_addr==old_dirdata)
				temp_addr = new_dirdata;
		}
		else if(flag==1){
			if(i==ptrs_specific)
				temp_addr = new_dirdata;
		}
		lseek(fd,new_nodedata2+i*4,SEEK_SET);
		rc=write(fd,(void*)&temp_addr,4);
	}

	new_mapdata = (int)lseek(fd,0,SEEK_CUR);
	for(i=0;i<16;i++){
		lseek(fd,old_mapdata+i*4,SEEK_SET);
		rc=read(fd,(void*)&temp_addr,4);
		if(temp_addr==old_nodedata)
			temp_addr = new_nodedata;
		if(i==new_inode)
			temp_addr = new_file;
		lseek(fd,new_mapdata+i*4,SEEK_SET);
		rc=write(fd,(void*)&temp_addr,4);
	}

	new_mapdata2 = (int) lseek(fd,0,SEEK_CUR);
		
	lseek(fd,(pinum/16)*4+4,SEEK_SET);
	rc=write(fd,(void*)&new_mapdata,4);

	
	if(preimap == new_inode/16){
		checkpoint = new_mapdata2;
		goto E;
	}
	preimap = new_inode / 16;
	lseek(fd,0,SEEK_SET);
	lseek(fd,(preimap*4)+4,SEEK_SET);
	rc = read(fd, (void*)&temp_addr,4);

	old_mapdata2 = temp_addr;
	for(i=0;i<16;i++){
		lseek(fd,old_mapdata2+i*4,SEEK_SET);
		rc=read(fd,(void*)&temp_addr,4);
		if(i==new_inode%16)
			temp_addr = new_file;////////new_mapdata2
		lseek(fd,new_mapdata2+i*4,SEEK_SET);
		rc=write(fd,(void*)&temp_addr,4);
	}

	checkpoint = (int) lseek(fd,0,SEEK_CUR);

	lseek(fd,(new_inode/16)*4+4,SEEK_SET);
	rc = write(fd, (void*)&new_mapdata2, 4);
//	old_nodedata = temp_addr;

E:
	lseek(fd,0,SEEK_SET);
	rc=write(fd,(void*)&checkpoint,4);	
	
	break;

default: break;


}
	fsync(fd);
	return (0);
}
int mfs_write(int inum,char* buffer,int block,int fd){	
	//printf("inode = %d\n",inum);
	lseek(fd,0,SEEK_SET);
	int checkpoint,old_map,old_node,new_data,new_node,new_map;
	int temp_addr;
	int overwrite_flag=0;
	int block_ptrs[14];
	MFS_Stat_t old_stat;
	read(fd,(void*)&checkpoint,4);
	int premap = inum/16;
	int i;
	lseek(fd,4+(premap*4),SEEK_SET);
	read(fd,(void*)&old_map,4);
	lseek(fd,old_map+(inum%16)*4,SEEK_SET);
	read(fd,(void*)&old_node,4);
	if(old_node==-1){
		return -1;
	}
	lseek(fd,old_node,SEEK_SET);
	read(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
//	printf("Type = %d, size = %d\n",old_stat.type,old_stat.size);
	if(old_stat.type !=MFS_REGULAR_FILE){
		return -1;
	}
	read(fd,(void*)block_ptrs,14*4);
	
//	for(i=0;i<14;i++)
//		printf("Pointer %d = %d\n",i,block_ptrs[i]);
//	if(block_ptrs[block]!=-1){
//		return -1;
//	}
	new_data = checkpoint;
	lseek(fd,new_data,SEEK_SET);
	write(fd,buffer,MFS_BLOCK_SIZE);

	new_node = (int) lseek(fd,0,SEEK_CUR);
	//if(overwrite_flag==0)
	block_ptrs[block]=new_data;
	for(i=0;i<14;i++){
		if(block_ptrs[i]!=-1)
			old_stat.size =((i+1)* MFS_BLOCK_SIZE);
	}
	write(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
	block_ptrs[block] = new_data;
	write(fd,(void*)block_ptrs,14*4);

	new_map = lseek(fd,0,SEEK_CUR);

	for(i=0;i<16;i++){
		lseek(fd,old_map+i*4,SEEK_SET);
		read(fd,(void*)&temp_addr,4);
		if(i == inum%16){
			temp_addr = new_node;
		}
		lseek(fd,new_map+i*4,SEEK_SET);
		write(fd,(void*)&temp_addr,4);
	}
	checkpoint = lseek(fd, 0, SEEK_CUR);
	lseek(fd,0,SEEK_SET);
	write(fd,(void*)&checkpoint,4);
	lseek(fd,4+(inum/16)*4,SEEK_SET);
	write(fd,(void*)&new_map,4);
	fsync(fd);
	return 0;
}
int mfs_read(int inum,char* buffer,int block,int fd){	

	lseek(fd,0,SEEK_SET);
	int checkpoint,old_map,old_node;
	int block_ptrs[14];
	MFS_Stat_t old_stat;
	read(fd,(void*)&checkpoint,4);
	int premap = inum/16;
	lseek(fd,4+(premap*4),SEEK_SET);
	read(fd,(void*)&old_map,4);
	lseek(fd,old_map+(inum%16)*4,SEEK_SET);
	read(fd,(void*)&old_node,4);
	if(old_node==-1){
		return -1;
	}
	lseek(fd,old_node,SEEK_SET);
	read(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
	if((old_stat.type !=MFS_REGULAR_FILE)&&(old_stat.type != MFS_DIRECTORY)){
		return -1;
	}
	read(fd,(void*)block_ptrs,14*4);
	if(block_ptrs[block]==-1){
		return -1;
	}
	lseek(fd,block_ptrs[block],SEEK_SET);
	read(fd,(void*)buffer,MFS_BLOCK_SIZE);
	int i;
//	for(i=0;i<MFS_BLOCK_SIZE;i++)
//		printf("block = %d and content = %c\n",block,buffer[i]);
	return 0;
}
int unlink_lookup(int inode,int fd){
	int premap = inode/16;
	int old_mapdata,old_nodedata;
	MFS_Stat_t old_stat;
	MFS_DirEnt_t old_dirent;
	int block_ptrs[14];
	int count=0;
	int temp,i,j;
	lseek(fd,4+(premap*4),SEEK_SET);
	read(fd,(void*)&old_mapdata,4);
	lseek(fd,old_mapdata+(inode%16)*4,SEEK_SET);
	read(fd,(void*)&old_nodedata,4);
	if(old_nodedata == -1)
		return 0;
	lseek(fd,old_nodedata,SEEK_SET);
	read(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
	if(old_stat.type==MFS_REGULAR_FILE)
		return 1;
	if(old_stat.type!= MFS_DIRECTORY)
		return 0;
	read(fd,(void*)block_ptrs,14*4);
	for(i=0;i<14;i++){
		if(block_ptrs[i]==-1)
			continue;
		lseek(fd,block_ptrs[i],SEEK_SET);
		
		for(j=0;j<64;j++){
			read(fd,(void*)&old_dirent,sizeof(MFS_DirEnt_t));
			if(old_dirent.inum!=-1)
				count++;
		}			
	}
	if(count>2)
		return 0;
return 1;
}

int mfs_unlink(int pinum, char *name,int fd){
	
	int new_dirdata,new_nodedata,new_mapdata,old_mapdata, old_nodedata, old_dirdata,checkpoint,new_mapdata2,old_mapdata2,new_dir;
	int old_nodespecific,old_dirdataspecific;
	int node_unlink;
	lseek(fd,0,SEEK_SET);
	int rc = read(fd,(void*)&new_dir,4);
	new_dirdata=new_dir;
	checkpoint = new_dir;
	int premap = pinum/16;
	int block_ptrs[14];
	int temp;
	MFS_Stat_t old_stat;
	MFS_DirEnt_t old_dirent;
	lseek(fd,4+(premap*4),SEEK_SET);
	read(fd,(void*)&old_mapdata,4);
	lseek(fd,old_mapdata+(pinum%16)*4,SEEK_SET);
	read(fd,(void*)&old_nodedata,4);

	if(old_nodedata==-1)
		return -1;
	lseek(fd,old_nodedata,SEEK_SET);
//////////////////////////////////////////////
	read(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
	if(old_stat.type!=MFS_DIRECTORY)
		return -1;
	int i,j;
	read(fd,(void*)block_ptrs,14*4);
	for(i=0;i<14;i++){	
		if(block_ptrs[i]!=-1){
			lseek(fd,block_ptrs[i],SEEK_SET);
			for(j=0;j<64;j++){
				read(fd,(void*)&old_dirent,sizeof(MFS_DirEnt_t));
				if((old_dirent.inum!=-1)&&(strcmp(name,old_dirent.name)==0)){
					if(unlink_lookup(old_dirent.inum,fd)==1){
						old_nodespecific = i;
						old_dirdataspecific = j;				
						node_unlink = old_dirent.inum;
						goto G;
					}
					else
						return -1;
				}
					
			}
		}
	}	
	return 0;
G:
	new_dir = new_dirdata;
	for(i=0;i<64;i++){
		lseek(fd,block_ptrs[old_nodespecific]+i*64,SEEK_SET);
		read(fd,(void*)&old_dirent,sizeof(MFS_DirEnt_t));
		if(i==old_dirdataspecific)	
			old_dirent.inum = -1;
		lseek(fd,new_dir+(i*64),SEEK_SET);
		write(fd,(void*)&old_dirent,sizeof(MFS_DirEnt_t));
	}
	new_nodedata = (int)lseek(fd,0,SEEK_CUR);

	lseek(fd,old_nodedata,SEEK_SET);
	read(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
	lseek(fd,new_nodedata,SEEK_SET);
	write(fd,(void*)&old_stat,sizeof(MFS_Stat_t));
	for(i=0;i<14;i++){
		lseek(fd,old_nodedata+sizeof(MFS_Stat_t)+i*4,SEEK_SET);	
		read(fd,(void*)&temp,4);
		if(i==old_nodespecific)
			temp = new_dir;
		lseek(fd,new_nodedata+sizeof(MFS_Stat_t)+i*4,SEEK_SET);	
		write(fd,(void*)&temp,4);
	}	
	
	int flag=0;
	new_mapdata = lseek(fd,0,SEEK_CUR);
	for(i=0;i<16;i++){
		lseek(fd,old_mapdata+i*4,SEEK_SET);	
		read(fd,(void*)&temp,4);
		if(i==pinum%16)
			temp = new_nodedata;
		if(i==node_unlink){
			temp = -1;
			flag = 1;
			//goto H;
		}
		lseek(fd,new_mapdata+i*4,SEEK_SET);	
		write(fd,(void*)&temp,4);
	}	

	if(flag==1)
		goto H;
	new_mapdata2 = lseek(fd,0,SEEK_CUR);

	premap = node_unlink/16;
	lseek(fd,4+(premap*4),SEEK_SET);
	read(fd,(void*)&old_mapdata2,4);
	for(i=0;i<16;i++){
		lseek(fd,old_mapdata2+i*4,SEEK_SET);	
		read(fd,(void*)&temp,4);
		if(i==node_unlink%16)
			temp = -1;
		
		lseek(fd,new_mapdata2+i*4,SEEK_SET);	
		write(fd,(void*)&temp,4);
	}
	checkpoint = lseek(fd,0,SEEK_CUR);	
	lseek(fd,4+(node_unlink/16)*4,SEEK_SET);
	write(fd,(void*)&new_mapdata2,4);
	goto I;

H:
	checkpoint = lseek(fd,0,SEEK_CUR);
I:
	lseek(fd,0,SEEK_SET);
	write(fd,(void*)&checkpoint,4);

	lseek(fd,4+(pinum/16)*4,SEEK_SET);
	write(fd,(void*)&new_mapdata,4);
	
	fsync(fd);
	return 0;
}

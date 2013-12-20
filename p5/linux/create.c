#include "udp.h"
#include <string.h>
#include "mfs.h"
#define BUFFER_SIZE (4096)
#include <fcntl.h>

void main(int argc, char* argv[]){


	int fd = open(argv[1],O_RDWR,S_IRWXU|S_IRUSR);
        if(fd>=0){
		printf("File exists, file %s opened\n",argv[1]);
		goto A;
	}
	else
                printf("File does not exist, hence creating the file %s\n",argv[1]);


	fd = open(argv[1],O_RDWR|O_CREAT,S_IRWXU|S_IRUSR);
	int i,j,k;
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
	//write(fd,(const void*)(&block1),sizeof(MFS_Stat_t));
	//
	//writing the inode containing the directory entries
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

A:
	printf("Closing file \n");
	//close the file
	close(fd);
return;
}

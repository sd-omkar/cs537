#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "cs537.h"
int
main(){
char* my_string = "hello this is cs537";
char* p;
int fd=open("plain.txt","r");
struct stat statbuf;
fstat(fd,&statbuf);


p=strtok(my_string,"e");
//if(p!=NULL)
//printf("%x %x\n",my_string,p);
return 0;
}

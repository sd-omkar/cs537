#include<stdio.h>
#include<fcntl.h>
void main(){
char temp;
int fd = open("test.txt",O_RDWR,S_IRWXU|S_IRUSR);

//while(1){
int read_val = read(fd,&temp,sizeof(char));
lseek(fd,2,SEEK_SET);
read_val = read(fd,&temp,sizeof(char));
if(read_val<=0){
	printf("Cant read further\n");
//	break;
}
printf("%c\n",temp);
//}

return;
}

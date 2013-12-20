#include <stdio.h>
#include "udp.h"
#include "mfs.h"
#include <string.h>

#define BUFFER_SIZE (4096)
char buffer[BUFFER_SIZE];

int
main(int argc, char *argv[])
{
    int port_num=atoi(argv[1]);
    	
//    int sd = UDP_Open(0);
  //  assert(sd > -1);

    struct sockaddr_in saddr;
  //  int rc = UDP_FillSockAddr(&saddr,argv[2],port_num);
    //assert(rc == 0);

    int rc = MFS_Init(argv[2],port_num);
    assert(rc==0);

//    rc = MFS_Shutdown();

    printf("CLIENT:: about to send message (%d)\n", rc);
//    char message[BUFFER_SIZE];
//    sprintf(message, "hello world");
    MFS_Transmit_t message;
	int sd=0;	
	    rc = UDP_Write(sd, &saddr, (char*)&message, sizeof(MFS_Transmit_t));
    // printf("CLIENT:: sent message (%d)\n", rc);
    if (rc > 0) {
	struct sockaddr_in raddr;
	int rc = UDP_Read(sd, &raddr, buffer, BUFFER_SIZE);
	printf("CLIENT:: read %d bytes (message: '%s')\n", rc, buffer);
    }

    return 0;
}



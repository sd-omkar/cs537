#include <stdio.h>
#include "udp.h"

#define BUFFER_SIZE (4096)

int
main(int argc, char *argv[])
{
    int sd = UDP_Open(10005);
    assert(sd > -1);

    printf("                                SERVER:: waiting in loop\n");

    while (1) {
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

    return 0;
}



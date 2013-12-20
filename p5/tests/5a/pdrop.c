#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>
#include <unistd.h>

#if 0
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
      const struct sockaddr *dest_addr, socklen_t addrlen) {
   static ssize_t (*real_sendto)(int, const void*, size_t, int,
         const struct sockaddr*, socklen_t) = NULL;
   static int i = -1;
   static int drop_every = 0;
   if (real_sendto == NULL) {
      real_sendto = dlsym(RTLD_NEXT, "sendto");
   }
   if (drop_every == 0) {
      return real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
   }
   i = (i + 1) % drop_every;
   if (i == 0) {
      fprintf(stderr, "tester: drop packet\n");
      return len;
   } else {
      return real_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
   }
}
#endif

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
      struct sockaddr *src_addr, socklen_t *addrlen) {
   static ssize_t (*real_func)(int, void*, size_t, int,
         struct sockaddr*, socklen_t *) = NULL;
   static int i = -1;
   static int drop_every = 0;
   if (real_func == NULL) {
      real_func = dlsym(RTLD_NEXT, "recvfrom");
   }
     return real_func(sockfd, buf, len, flags, src_addr, addrlen);
   fprintf(stderr, "%d enter recvfrom %d i=%d f=%p %p\n", getpid(), sockfd, i, real_func, recvfrom);
   char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
   ssize_t r;
   do {
      r = real_func(sockfd, buf, len, flags, src_addr, addrlen);
      getnameinfo(src_addr, sizeof(struct sockaddr), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 0);
      fprintf(stderr, "addr=%s %s\n", hbuf, sbuf);
      if (drop_every > 0) {
         i = (i + 1) % drop_every;
         if (i == 0) {
            fprintf(stderr, "tester: drop packet %d %d\n", sockfd, len);
            fprintf(stderr, "%1024c\n", buf);
         }
      }
   } while (drop_every > 0 && i == 0);
   return r;
}

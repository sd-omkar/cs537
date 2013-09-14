#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-s random seed> <-n number of records> <-o output file>\n", prog);
    exit(1);
}

int
main(int argc, char *argv[])
{
    // program assumes a 4-byte key in a 100-byte record
    assert(sizeof(rec_t) == 100);

    // arguments
    int randomSeed  = 0;
    int recordsLeft = 0;
    char *outFile   = "/no/such/file";

    // input params
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "n:s:o:")) != -1) {
  switch (c) {
  case 'n':
      recordsLeft = atoi(optarg);
      break;
  case 's':
      randomSeed  = atoi(optarg);
      break;
  case 'o':
      outFile     = strdup(optarg);
      break;
  default:
      usage(argv[0]);
  }
    }

    // seed random number generator
    srand(randomSeed);

    // open and create output file
    int fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fd < 0) {
  perror("open");
  exit(1);
    }

    rec_t r;
    int i;
    for (i = 0; i < recordsLeft; i++) {
  // fill in random key
  r.key = rand() % (unsigned int) 0xFFFFFFFF;
  // fill in random rest of records
  int j;
  for (j = 0; j < NUMRECS; j++) {
      r.record[j] = rand() % (int)1e3;
  }

  int rc = write(fd, &r, sizeof(rec_t));
  if (rc != sizeof(rec_t)) {
      perror("write");
      exit(1);
      // should probably remove file here but ...
  }
    }

    // ok to ignore error code here, because we're done anyhow...
    (void) close(fd);

    return 0;
}



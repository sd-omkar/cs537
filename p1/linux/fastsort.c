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
#include <errno.h>

// usage
void usage() {
 fprintf(stderr, "Usage: fastsort -i inputfile -o outputfile\n");
 exit(1);
}

// compare function for qsort
int compare (const void *a, const void *b) {
 const rec_t *rec1 = a;
 const rec_t *rec2 = b;
 return ((int)(rec1->key) - (int)(rec2->key));
}

int main (int argc, char *argv[]) {

char *inFile = NULL;
char *outFile = NULL;

// process arguments
int option;
opterr = 0;

while ((option = getopt(argc, argv, "i:o:")) != -1 ) {
 switch (option) {
 case 'i':
  inFile = strdup(optarg);
  break;
 case 'o':
  outFile = strdup(optarg);
  break;
 default:
  usage();
 }
}

// errors for incorrect arguments 
if (argc != 5)
 usage();
if (argc - optind)
 usage();
 
// open input file
int inFD = open(inFile, O_RDONLY);
if (inFD < 0) {
 fprintf(stderr, "Error: Cannot open file %s\n", inFile);
 exit(1);
}

// get number of records
int numRecords;
struct stat buf;
int err = fstat(inFD, &buf);
if (err != 0) {
 fprintf(stderr, strerror(errno));
 exit(1);
}
numRecords = (buf.st_size)/sizeof(rec_t);

// allocate memory
rec_t *records = NULL;
records = (rec_t *)malloc(numRecords*(sizeof(rec_t)));
if (records == NULL) 
 fprintf(stderr, "Error: Cannot allocate memory ");

int bytesRead = read(inFD, records, buf.st_size);
if (bytesRead != buf.st_size) {
 perror("read");
 exit(1);
}

// sort with qsort
qsort(records, numRecords, sizeof(rec_t), compare);

// generate output file
int outFD = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
if (outFD < 0) {
 fprintf(stderr, "Error: Cannot open file %s\n", outFile);
 exit(1);
}
int bytesWritten = write(outFD, records, buf.st_size);
if (bytesWritten != buf.st_size) {
 perror("write");
 exit(1);
}

// cleanup
free(records);
free(inFile);
free(outFile);
close(inFD);
close(outFD);

return 0;
}

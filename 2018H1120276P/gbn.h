#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <unistd.h>     /* for close() and alarm() */
#include <string.h>     /* for memset() */

#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */

#include <signal.h>     /* for sigaction() */
#include <time.h>       /* for random delay in ack */
#include <errno.h>      /* for errno and EINTR */

#define TIMEOUT_SECS    1       /* timeout timer value */
#define DATAMSG         1       /* data message */
#define TEARDOWNMSG     2       /* tear down message */
#define MAXTRIES        16      /* max number of times it can resend */
#define MAX 500000              /* max size of string to echo */
#define MAXDELAY 1500           /* max delay in miliseconds */

int randstart ;             /* required in case we want to start with random sequence number as base */

void CatchAlarm(int ignored);            /* handler for SIGALRM */
char *readFromFile() ;                   /* read data buffer from input.txt */
void delay(int milliseconds);            /* to generate random time delay */

struct segmentPacket {
    int type;
    int seq_no;
    int length;
    int isLastPacket ;
    char data[512];
};

struct ackPacket {
    int type;
    int ack_no;
};

struct segmentPacket createDataPacket (int seq_no, int length, char* data,int isLastPacket);     
struct ackPacket createAckPacket (int ack_type, int base);  
void writeToFile(char *dataBuffer);                         


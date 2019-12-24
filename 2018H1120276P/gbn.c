#include "gbn.h"

void CatchAlarm(int ignored){}

/* struct for data packet */
struct segmentPacket createDataPacket (int seq_no, int length, char *data,int isLastPacket){

    struct segmentPacket pkt;

    pkt.type = 1;
    pkt.seq_no = seq_no;
    pkt.length = length;
    pkt.isLastPacket = isLastPacket ;
    memset(pkt.data, 0, sizeof(pkt.data));
    strcpy(pkt.data, data);

    return pkt;
}

/* create ack packet */
struct ackPacket createAckPacket (int ack_type, int base){
        struct ackPacket ack;
        ack.type = ack_type;
        ack.ack_no = base;
        return ack;
}

/*reading data from file into buffer */
char *readFromFile(char * filename){

    char *source = NULL;
    randstart = rand()%100000 + 1 ;
    FILE *fp = fopen(filename, "r");
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp);
            if (bufsize == -1) {
                printf("file open failed") ;
                exit(1) ;
             }
            source = malloc(sizeof(char) * (bufsize + 1));

            if (fseek(fp, 0L, SEEK_SET) != 0) { 
                printf("file seek failed") ;
                exit(1) ;
             }

            size_t newLen = fread(source, sizeof(char), bufsize, fp);

            if ( ferror( fp ) != 0 ) {
                printf("error reading file");
                exit(1) ;
            } else {
                source[newLen++] = '\0'; 
            }
        }
        fclose(fp);
    }
    free(source); 

    return (char *)source ;
}

/* writing buffer to file */
void writeToFile(char *dataBuffer){
    FILE *fp = fopen("output.txt", "a");
    if (fp == NULL){
        printf("error opening file!\n");
        exit(1);
    }
    const char *text = dataBuffer;
    fprintf(fp, "%s", text);
    fclose(fp);
}

/* generating milliseconds delay */
void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

#include "gbn.h"

int main(int argc, char *argv[])
{
    int sock;                               /* socket descriptor */
    struct sockaddr_in rcvAddr ;                /* socket address of receiving server */
    struct sockaddr_in sndAddr;                 /* socket address of sending server */
    unsigned short port;                    /* server port */
    unsigned int sz ;                       /* address size */
    struct sigaction myHandler;             /* signal handler */
    char *ip;                               /* ip address of server */
    int ws;                                 /* size of window */
    int tries =0;                           /* number of tries packet has been sent */
    char *filename ;                        /* filename of file to send */

    if (argc != 6)    
    {
        printf("Usage: %s <Server IP> <Server Port> <File Name> <Packet Size> <Window Size>\n You gave %d Arguments\n", argv[0], argc);
        exit(1);
    }

    
    ip = argv[1];                           
    port = atoi(argv[2]);                   
    filename = argv[3] ;                    
    sz = atoi(argv[4]);                 
    ws = atoi(argv[5]);         
    
    printf("Attempting to Send to: \n");
    printf("IP:          %s\n", ip);
    printf("Port:        %d\n", port);
    printf("File:        %s\n", filename) ;

    // creating udp socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("socket connection failed") ;
        exit(1) ;
    }

    /* creating server address */
    memset(&rcvAddr, 0, sizeof(rcvAddr));    /* Zero out structure */
    rcvAddr.sin_family = AF_INET;
    rcvAddr.sin_addr.s_addr = inet_addr(ip);  /* Server IP address */
    rcvAddr.sin_port = htons(port);       /* Server port */
 
    /* storing data read from file in buffer */
    char* buffer ;
    buffer = readFromFile(filename) ;
 
    /* number of packets */
    int bufferSize = strlen(buffer) ;
    int noOfPackets = bufferSize / sz ;
   
    if (strlen(buffer) % sz > 0){
        noOfPackets++;
    }

    randstart = 0 ;                     /* can be modified with random number generator in case we want to avoid problems with delayed ack after connection close */
    
    // set base in case we want random offset to start with
    int base = randstart ;           /* highest packet ACK recieved + 1 */
    int seqNum = randstart ;         /* highest packet sent */
    int dataLength = 0;              /* packet size */
    int lastPacketSeqNum = randstart + noOfPackets - 1 ; /* Limit for seq number */

    /* data statistics */
    printf("Window Size: %d\n", ws);
    printf("Packet Size:  %d\n", sz);
    printf("Packets:      %d\n", noOfPackets);

    myHandler.sa_handler = CatchAlarm;
    if (sigemptyset(&myHandler.sa_mask) == -1){ 
        printf("sigemptyset failed");
        exit(1) ;
    }   

    myHandler.sa_flags = 0;

    if (sigaction(SIGALRM, &myHandler, 0) == -1){
        printf("sigaction failed for SIGALRM") ;
        exit(1) ;
    }
     
    /* bool used to keep the program running until teardown ack has been recieved */
    int noTearDownACK = 1;
    while(noTearDownACK){

        /* send packets from base up to window size */
        while(seqNum <= (lastPacketSeqNum) && (seqNum - base) < ws){
            struct segmentPacket dataPacket;

            if(seqNum == (lastPacketSeqNum)){
                /* last packet to transmit, create terminal packet */
                int currentStart = (seqNum-randstart)*sz ;
                int remainingSize = bufferSize - currentStart ;
                char seg_data[remainingSize];
                strncpy(seg_data, (buffer + (seqNum-randstart)*sz), remainingSize );
                dataPacket = createDataPacket(seqNum, remainingSize , seg_data, 1);
                printf("sending terminal packet with sequence number : %d\n",seqNum);
            } else {
                /* creating data packet */
                char seg_data[sz];
                strncpy(seg_data, (buffer + (seqNum-randstart)*sz), sz);
                dataLength = sizeof(seg_data)/sizeof(char) ;
                dataPacket = createDataPacket(seqNum, dataLength, seg_data, 0);
                printf("sending packet with sequence number : %d\n", seqNum);
            }

            /* Send the constructed data packet to the receiver */
            if (sendto(sock, &dataPacket, sizeof(dataPacket), 0,
                 (struct sockaddr *) &rcvAddr, sizeof(rcvAddr)) == -1){
                     printf("sendto failed") ;
                     exit(1) ;
                 }
            
            /* if last packet send then stop the loop */
            if(seqNum == lastPacketSeqNum)
            break ;

            seqNum++;
        }

        alarm(TIMEOUT_SECS);        /* ring alarm to signal after timeout_sec */

        printf("window is full\n");

        /* listening for ACKs, update highest ACK, reset base */
        struct ackPacket ack;
        unsigned int sndSize ;
        while ((recvfrom(sock, &ack, sizeof(ack), 0,
               (struct sockaddr *) &sndAddr, &sndSize)) == -1)
        {
            if (errno == EINTR)     /* Alarm went off  */
            {
                /* reset the sequence number back to the last expected ACK */
                seqNum = base ;
                printf("sequence number :  %d\n" , seqNum) ;
                printf("timeout occurred. resending\n");
                
                if(tries >= 10){
                    printf("tries exceeded\n");
                    exit(1);
                } else {
                    alarm(0);
                    while(seqNum <= (lastPacketSeqNum) && (seqNum - base) < ws){
                        struct segmentPacket dataPacket;

                        if(seqNum == (lastPacketSeqNum)){
                            /* last packet, create terminal packet */
                            int currentStart = (seqNum-randstart)*sz ;
                            int remainingSize = bufferSize - currentStart ;
                            char seg_data[remainingSize];
                            strncpy(seg_data, (buffer + (seqNum-randstart)*sz), remainingSize-1);
                            // seg_data[remainingSize] = '\0' ;
                            dataPacket = createDataPacket(seqNum, remainingSize , seg_data , 1);
                            printf("sending terminal packet with sequence number : %d \n",seqNum);
                        } else {
                            /* creating data packet */
                            char seg_data[sz];
                            strncpy(seg_data, (buffer + (seqNum-randstart)*sz), sz);
                            dataLength = sizeof(seg_data)/sizeof(char) ;
                            dataPacket = createDataPacket(seqNum, dataLength, seg_data,0);
                            printf("sending packet with sequence number: %d\n", seqNum);
                        }
                
                        /* sending packet */
                        if (sendto(sock, &dataPacket, sizeof(dataPacket), 0,
                             (struct sockaddr *) &rcvAddr, sizeof(rcvAddr)) != sizeof(dataPacket)){
                                printf("sendto failed");
                                exit(1) ;
                            }

                        if(seqNum == lastPacketSeqNum)
                        break ;

                        seqNum++;
                    }

                    alarm(TIMEOUT_SECS);
                }
                tries++;
            }
            else{
                printf("recvfrom failed");
                exit(1) ;
            }
        }

        /* 8 is teardown ack */
        if(ack.type != 8){
            printf("-------- recieved teardown ack: %d --------\n", ack.ack_no);
            if(ack.ack_no >= base){
                base = ack.ack_no + 1 ;
            }
        } else {
            printf("-------- recieved terminal ack : %d  --------\n",ack.ack_no);
            noTearDownACK = 0;
        }

        alarm(0);
        tries = 0;
    }

    close(sock);
    exit(0);
}


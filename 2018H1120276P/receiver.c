#include "gbn.h"

int main(int argc, char *argv[])
{
    int sock;                        
    struct sockaddr_in serverAddr;  
    struct sockaddr_in clientAddr;  
    char buffer[MAX];        
    unsigned short port;     
    int recvMsgSize;                 
   
    if (argc != 2){
        printf("Usage:  %s <UDP SERVER PORT> \n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);  
  
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("socket failed") ;
        exit(1) ;
    }
        
    memset(&serverAddr, 0, sizeof(serverAddr));   
    serverAddr.sin_family = AF_INET;                
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(port);      

    if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1){
        printf("bind failed") ;
        exit(1) ;
    }
        
    char dataBuffer[8192];
    randstart = 0 ;
    int base = randstart ;
    int seqNum = randstart ;

    while(1) 
    {
        unsigned int cliAddrLen = sizeof(clientAddr);

        struct segmentPacket dataPacket;

        struct ackPacket ack;

        if ((recvMsgSize = recvfrom(sock, &dataPacket, sizeof(dataPacket), 0,
            (struct sockaddr *) &clientAddr, &cliAddrLen)) == -1){
                printf("recvfrom failed") ;
                exit(1) ;
            }

            seqNum = dataPacket.seq_no;

            if(dataPacket.seq_no == randstart && dataPacket.type == 1){            
                printf("received first packet with sequence number : %d\n",seqNum) ;
                memset(dataBuffer, 0, sizeof(dataBuffer));
                strcpy(dataBuffer, dataPacket.data);
                strcpy(buffer,dataBuffer) ;
                ack = createAckPacket(2, dataPacket.seq_no);
                base = randstart + 1 ;

            } else if (dataPacket.seq_no == base){                              
                /* then concatinate the data sent to the recieving buffer */
                printf("received packet with sequence number : %d\n", dataPacket.seq_no);
                memset(dataBuffer, 0, sizeof(dataBuffer));
                strcpy(dataBuffer, dataPacket.data);
                strcat(buffer,dataBuffer) ;
                ack = createAckPacket(2, dataPacket.seq_no);
                base = dataPacket.seq_no + 1 ;

            } else if (dataPacket.type == 1 && dataPacket.seq_no != base){      /* handling out of sync packet */
                printf("received out of sync packet : %d\n", dataPacket.seq_no);
                /*sending old ack*/
                ack = createAckPacket(2, base);
            }

            /* isLastPacket means that the packet recieved is a termination packet */
            if(dataPacket.type == 1 &&  dataPacket.isLastPacket ){
                
                printf("received terminal packet with sequence number : %d\n", dataPacket.seq_no);
                memset(dataBuffer, 0, sizeof(dataBuffer));
                strcpy(dataBuffer, dataPacket.data);
                // printf("priting whole received buffer : %s\n",dataBuffer) ;
                
                /* create an ACK packet with terminal type 8 */
                ack = createAckPacket(8, dataPacket.seq_no);
                base = randstart ;
            }

            /* sending ack for received packets */
            if(base >= randstart){
                /* generating random delay for acknowledgement */    
                delay(rand()%MAXDELAY) ;
                printf("--------  Sending ACK %d  --------\n", ack.ack_no);

                if (sendto(sock, &ack, sizeof(ack), 0,
                     (struct sockaddr *) &clientAddr, sizeof(clientAddr)) == -1){
                         printf("sendto failed") ;
                         exit(1) ;
                     }

            }else if (base == randstart) {
                /* generating random delay for acknowledgement */
                delay(rand()%MAXDELAY) ;                
                printf("received tear down packet\n");
                printf("Sending Terminal ACK  %d\n", base);

                if (sendto(sock, &ack, sizeof(ack), 0,
                     (struct sockaddr *) &clientAddr, sizeof(clientAddr)) == -1){
                         printf("sendto failed") ;
                         exit(1) ;
                     }
            }

            /* if data packet is terminal packet, display and clear the recieved message */
            if(dataPacket.type == 1 && dataPacket.isLastPacket){
                /* writting received data to output.txt */
                writeToFile(buffer) ;
                /* printing the received message */
                // printf("\n MESSAGE RECIEVED\n %s\n\n", buffer);
                memset(buffer,0,sizeof(buffer)) ;   
                break ;
            }
    }
    close(sock);
    exit(0);
}


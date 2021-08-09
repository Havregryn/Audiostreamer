//
//  GrimsUdpLib.hpp
//  UDPAU
//
//  Created by Hallgrim Bratberg on 08/08/2021.
//

#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define LOCAL_PORT "4953"
#define LOCAL_NAME "grims-mac.local"

#define FRAMES_PR_PACKAGE 512
#define CHANNELS 2

struct AudioMsg{
    uint16_t seqNr;
    int frameCount;
    int channel;
    float data[FRAMES_PR_PACKAGE];
};

class UdpAudioTransceiver{
public:
    UdpAudioTransceiver(){
        
        isTransmitter = true;
        
        if(isTransmitter) {
            setup_transmitter_socket();
        } else {
            setup_receiver_socket();
        }
        
    }
    
    void setIsTransmitter(bool value){
        this->isTransmitter = value;
    }
    
    void receive(){
        if(sockfd > 0){
            numbytes = recvfrom(sockfd, testBuf, 900, 0, (struct sockaddr *)&their_addr, &addr_len);
        }
    }
    
    // Called before copying frames into send buffer
    void initNewAudioMsg(int frameCount, int channel) {
        if(channel == 0) { seqNr++; }
        au_msg.seqNr = this->seqNr;
        au_msg.frameCount = frameCount;
        au_msg.channel = channel;
    }
    
    // Called for each sample of every channel:
    void copySampleToMsg(const float *audioBuffer, int frameNr){
        au_msg.data[frameNr] = audioBuffer[frameNr];
    }
    
    // Called after the samples of one channel has been copied:
    void transmit(){
        char *msg_buf = (char *) &au_msg;
        if(sockfd > 0){
            numbytes = sendto(sockfd, msg_buf, sizeof(au_msg), 0, p->ai_addr, p->ai_addrlen);
        }
    }
    
    void deallocate(){
        close(sockfd);
    }
    
    
    
private:
    bool isTransmitter;
    
    // Socket and udp variables:
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    long numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    
    char testBuf[1000];
    AudioMsg au_msg;
    
    uint16_t seqNr;
    
    
    
    // Private functions:
    
    void *get_in_addr(struct sockaddr *sa){
      if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
      }
      return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
    
    int setup_receiver_socket(void){
        
        // Filling in the hints to getaddrinfo:
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET6;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;
        
        // Getting the address info:
        if((rv = getaddrinfo(NULL, LOCAL_PORT, &hints, &servinfo) != 0)){
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
          return 1;
        }

        // Loop through all results:
        for(p = servinfo; p !=NULL; p = p->ai_next){
          if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("listener: socket");
            continue;
          }
        
          if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("listener: bind");
            continue;
          }
          break;
        }

        if(p == NULL){
          fprintf(stderr, "listener: Failed to bind socket\n");
            return 1;
        }
         
         

        freeaddrinfo(servinfo);
        return 0;
    }
    
    int setup_transmitter_socket(void){
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET6;
        hints.ai_socktype = SOCK_DGRAM;
        
        if((rv = getaddrinfo(LOCAL_NAME, LOCAL_PORT, &hints, &servinfo)) != 0){
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
          return 1;
        }

        /* Looping through all results, use the first working socket */
        for(p = servinfo; p != NULL; p = p->ai_next){
          if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("talker: socket");
            continue;
          }
          break;
        }

        if(p == NULL){
          fprintf(stderr, "talker: Failed to create socket\n");
          return 2;
        }
        
        seqNr = 0;
        
        
        return 0;
    }
};

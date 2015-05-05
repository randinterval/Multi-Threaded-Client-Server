/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D

    This binary will be loaded directly onto a xterm terminal
    and it will be used for direct client-2-client communication..
*/

#include "headers.h"

int sd;


void closeSocket(int signum);

void * receiveMessage(void * arg)
{
    int csfd = *(int *)arg;
    int rb; //Number of bytes Read - This is done to handle termination of socket
    //in a graceful manner because if a socket is disconnected then read returns 0 bytes
    //so, we check against that and terminate if neeeded..
    char msg[INPUT];
    while(1)
    {
        memset(msg,'\0',INPUT);
        rb = read(csfd,&msg,INPUT);
        if(rb<=0)
        {
            close(csfd);
            logger(LOG_INFO,"Disconnected from Client!");
            sleep(2);
            fflush(stdout);
            exit(0);
        }
        printf("%s",msg);
        printf("\n>");
        fflush(stdout);
    }
}

void * sendMessage(void * arg)
{
    char msg[INPUT];
    int sfd = *(int *)arg;
    fflush(stdout);
    int rb ;
    while(1)
    {
        //Sir Just fixed a size here so user can only enter this much
        printf(">");
        memset(msg,'\0',INPUT);
        fgets (msg,INPUT, stdin); //Sir Using fgets instead of scanf becasue scanf breaks on whitespace so then message breaks 2/3 parts
        //fgets is far more approripate for string handling and to get input correctly - morevoer it also avoids buffer overflow as it
        //stops reading in after the number we give to it
        removeline(msg);
        rb = write(sfd,msg,strlen(msg));
        if(rb<=0)
        {
            close(sfd);
            logger(LOG_INFO,"Disconnected from Client!");
            sleep(2);
            fflush(stdout);
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc<=1)
    {
        logger(LOG_ERR,"Invalid Number of Arguments!");
        exit(0);
    }
    signal(SIGINT, closeSocket);
    signal(SIGTERM, closeSocket);
    int sockfd = atoi(argv[1]);
    sd = sockfd;
    pthread_t ptd[2];
    void *status;
    //crate two threads
    pthread_create(&ptd[0],NULL,receiveMessage,(void *)&sockfd);
    pthread_create(&ptd[1],NULL,sendMessage,(void *)&sockfd);
    //join them
    pthread_join(ptd[0],&status);
    pthread_join(ptd[1],&status);
    close(sockfd);
    exit(0);
    return 0;
}



void closeSocket(int signum)
{
    close(sd);
    exit(signum);
}


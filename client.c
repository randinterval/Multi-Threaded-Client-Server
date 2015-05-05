/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D

    Client Code - This will estbalish communication with server
    AND listen for incoming connection from other clients
    3 Threads Total = 2 Read/Write to Server + 1 Listen Thread

*/

#include "headers.h"

int cdesc;
int sdesc;
int clist[CLIENTS];
int clientCount = 0;
int shouldclose = false;

void *newTerminalForConnect(void *);

void disconnect(int signum);

void *sendCommand(void *arg)
{
    char msg[INPUT];
    int sfd = *(int *)arg;
    printf("\n>");
    fflush(stdout);
    while(1)
    {
        //Sir Just fixed a size here so user can only enter this much
        memset(msg,'\0',INPUT);
        fgets (msg,INPUT, stdin); //Sir Using fgets instead of scanf becasue scanf breaks on whitespace so then message breaks 2/3 parts
        //fgets is far more approripate for string handling and to get input correctly - morevoer it also avoids buffer overflow as it
        //stops reading in after the number we give to it
        removeline(msg);
        if(strcmp(msg,"disconnect")==0)
        {
            shouldclose = true;
        }
        write(sfd,msg,strlen(msg));
        if(shouldclose)
        {
            disconnect(0);
        }
    }
}

void * receiveResponse(void *arg)
{
    int client_sockfd = *(int *)arg;
    int rb; //Number of bytes Read - This is done to handle termination of socket
    //in a graceful manner because if a socket is disconnected then read returns 0 bytes
    //so, we check against that and terminate if neeeded..
    char msg[INPUT];
    while(1)
    {
        memset(msg,'\0',INPUT);
        rb = read(client_sockfd,&msg,INPUT);
        if(rb<=0)
        {
            close(client_sockfd);
            logger(LOG_INFO,"[CLIENT] Disconnected from server");
            fflush(stdout);
            exit(0);
        }
        char * token = strdup(msg);
        char * ip[2];
        int i=0;
        for(i=0; i<2; i++)
        {
            ip[i] = strsep(&token," ");
        }
        if(strcmp(ip[0],"ip")==0)
        {
            if(ip[1]!=NULL)
            {
                pthread_t thread;
                pthread_create(&thread,NULL,newTerminalForConnect,(void *)ip[1]);
                printf("\n");
            }
            continue;
        }
        printf("%s",msg);
        if(strlen(msg)>1)
        {
            printf(">");
        }
        fflush(stdout);
    }
}


//This will take as input an ip and setup a new terminal and pass ip to that terminal..
void *newTerminalForConnect(void *arg)
{
    char * ip = (char *) arg;
    printf("\n");
    printf(">");
    fflush(stdout);
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    sockfd = socket (AF_INET, SOCK_STREAM,0); //Returns a file descriptor for our socket
    address.sin_family = AF_INET;
    inet_aton(ip,&address.sin_addr);
    address.sin_port = htons(CLIENTSERVPORT);
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *)&address,len);
    char sdcl[123];
    sprintf(sdcl, "%d", sockfd);
    clist[clientCount++] = sockfd;
    if(result==-1)
    {
        logger(LOG_ERR,"[CLIENT] Unable to connect to Client");
        return NULL;
    }
    int pid = fork();
    if(pid==-1)
    {
        logger(LOG_ERR,"[CLIENT] Error Forking..!");
        exit(0);
    }
    if(pid==0)
    {
        char * cmd[] = {"/usr/bin/xterm", "-e","./clientclient",sdcl,NULL};
        execv(cmd[0],cmd);
    }
    wait(NULL);
    return NULL;
}

void *newTerminalforChat(void *arg)
{

    int sd = *(int *)arg;
    char sdcl[123];
    sprintf(sdcl, "%d", sd);
    int pid = fork();
    if(pid==-1)
    {
        logger(LOG_ERR,"[CLIENT] Error Forking..!");
        exit(0);
    }
    if(pid==0)
    {
        printf("\n");
        printf(">");
        fflush(stdout);
        char * cmd[] = {"/usr/bin/xterm", "-e","./clientclient",sdcl,NULL};
        execv(cmd[0],cmd);
    }
    wait(NULL);
    return NULL;
}



//Sir this thread is to accept connections from other clients...
//So, in this thread we setup a server at port 2500 so other clients can join
void *clientListener(void * arg)
{
    //Socket INIT
    int ssd,csd;
    unsigned int slen, clen;
    struct sockaddr_in saddr;
    struct sockaddr_in caddr;
    ssd = socket(AF_INET,SOCK_STREAM,0);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(CLIENTSERVPORT);
    slen = sizeof(saddr);

    //Socket Binding...
    if(bind(ssd, (struct sockaddr *)&saddr,slen)==-1)
    {
        logger(LOG_ERR,"[CLIENT] Unable to setup listener - Unable to bind on Port %d \n",SERVERPORT);
        exit(1);
    }
    sdesc = ssd;
    pthread_t thread; //Thread 0 for write and 1 for Read
    while(1)
    {
        listen(ssd,CLIENTQUEUE);
        clen = sizeof(caddr);
        csd = accept(ssd,(struct sockaddr *) &caddr,&clen);
        clist[clientCount++] = csd;
        pthread_create(&thread,NULL,newTerminalforChat,(void *)&csd);
    }
    return NULL;
}


int main(int argc, char *argv[])
{

    if(argc<2)
    {
        logger(LOG_ERR,"SERVER IP NOT PROVIDED");
        return -1;
    }
    //Catch both SIGNINT (CTRL+C) and SIGTERM (termination) signals to
    //perform any destructor operations...
    signal(SIGINT,  disconnect);
    signal(SIGTERM, disconnect);
    char * ip = argv[1];
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    sockfd = socket (AF_INET, SOCK_STREAM,0); //Returns a file descriptor for our socket
    address.sin_family = AF_INET;
    inet_aton(ip,&address.sin_addr);
    //address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(SERVERPORT);
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *)&address,len);
    if(result==-1)
    {
        logger(LOG_ERR,"[CLIENT] Unable to Connect to Server");
        exit(1);
    }
    pthread_t ptd[3]; //Thread 1 For Sending Commands to Server - 2 for Recieve
    //Thread Number 3 for setting up client server so others client can directly connect..
    void *status;
    //crate two threads
    cdesc = sockfd;
    pthread_create(&ptd[0],NULL,sendCommand,(void *)&sockfd);
    pthread_create(&ptd[1],NULL,receiveResponse,(void *)&sockfd);
    pthread_create(&ptd[2],NULL,clientListener,(void *)NULL);

    //join them
    pthread_join(ptd[0],&status);
    pthread_join(ptd[1],&status);
    //Close the socket before server exit
    close(sockfd);
    exit(0);
}


void disconnect(int signum)
{
    logger(LOG_INFO,"[CLIENT] Disconnecting and Closing Sockets..");
    close(sdesc);
    close(cdesc);
    int j=0;
    for(j=0; j<clientCount; j++)
    {
        close(clist[j]);
    }
    exit(signum);
}


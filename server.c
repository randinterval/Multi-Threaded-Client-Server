/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D
    Server Code
*/
#include "headers.h"

struct List list;


void closeSocket(int signum);


/* This is the primary thread that communicates with each client.
    It does the following
    1) Show Menu to Display and Login
    2) Register Client
    3) Login
    4) Respond to Client Commands
    5) Terminate gracefully should the client disconnect!
*/

/* This thread recieves messages from client */


void *handleClient(void *arg)
{
    int sd = *(int *)arg; //Socket  Descriptor

    //Array of Available Commands that are supported by the server..
    const char *commands[] = {"help","list","connect","disconnect","clear"};

    //Pre Defined Responses to Client
    char menu [] = "\n\t You may now create or authenticate your user account \n\t 1 - Register\n\t 2 - Sign In \n \t Enter 1 for first option, 2 for second option!\n";
    char * menuop [] = {"1","2"};
    char clear[] = "\033c";

    //Input Buffers
    char msg[INPUT];
    char status[STATUS]; //To Print Status Messages to Client..
    char user[ULIMIT+1];
    char pass[PLIMIT+1];

    //Variable to read how many bytes read so if bytes are <=0 then we disconnect from socket
    //as socket returns 0 if other end disconnects...
    int rb;

    // Logging
    logger(LOG_INFO,"[SERVER] New Client Connected");
    //Authentication and Registration Block!
    int authenticated = false;
    while(!authenticated)
    {
        memset(msg,'\0',INPUT);
        write(sd,menu,strlen(menu));
        rb = read(sd,&msg,INPUT);
        if(rb<=0)
        {
            logger(LOG_INFO,"[SERVER] Client Disconnected");
            close(sd);
            pthread_exit(0);
        }
        if(strcmp(menuop[REGISTER],msg)==0)
        {
            char user[ULIMIT+1];
            char pass[PLIMIT+1];
            char usermsg[] ="[SERVER] Enter Username: ";
            char passmsg[] ="[SERVER] Enter Password: ";
            memset(user,'\0',ULIMIT);
            memset(pass,'\0',PLIMIT);
            write(sd,usermsg,strlen(usermsg));
            rb = read(sd,&user,ULIMIT);
            if(rb<=0)
            {
                logger(LOG_INFO,"[SERVER] Client Disconnected");
                close(sd);
                pthread_exit(0);
            }
            write(sd,passmsg,strlen(passmsg));
            rb = read(sd,&pass,PLIMIT);
            if(rb<=0)
            {
                logger(LOG_INFO,"[SERVER] Client Disconnected");
                close(sd);
                pthread_exit(0);
            }
            int rt = registerUser(user,pass);
            if(rt==true)
            {
                char status[] = "\n\nRegistration Complete - Your account has been created - You may now sign in.\n";
                write(sd,clear,strlen(clear));
                write(sd,status,strlen(status));
                logger(LOG_INFO,"[SERVER] New Account Created - Username: %s",user);

            }
            else if(rt==EXISTINGUSER)
            {
                char status[] = "\n\tRegistration Failed - Username Already Taken\n";
                write(sd,status,strlen(status));
            }
            else
            {
                char status[] = "\n\tRegistration Failed - Please ensure that your username and password  length >=3 and <=20.\n";
                write(sd,status,strlen(status));
            }

        }
        else if(strcmp(menuop[SIGNIN],msg)==0)
        {

            char usermsg[] ="[SERVER] Enter Username: ";
            char passmsg[] ="[SERVER] Enter Password: ";
            memset(user,'\0',ULIMIT);
            memset(pass,'\0',PLIMIT);
            write(sd,usermsg,strlen(usermsg));
            rb = read(sd,&user,ULIMIT);
            if(rb<=0)
            {
                logger(LOG_INFO,"[SERVER] Client Disconnected");
                close(sd);
                pthread_exit(0);
            }
            write(sd,passmsg,strlen(passmsg));
            rb = read(sd,&pass,PLIMIT);
            if(rb<=0)
            {
                logger(LOG_INFO,"[SERVER] Client Disconnected");
                close(sd);
                pthread_exit(0);
            }
            //AVoid User from Logging Again form same username..
            int n = count(&list);
            char **users = getUsers(&list);
            int i=0;
            int islogged = false;
            for(i=0; i<n; i++)
            {
                if(strcmp(users[i],user)==0)
                {
                    char msg [] = "\tAccount already logged in!\n";
                    write(sd,msg,strlen(msg));
                    islogged = true;
                }
            }
            if(islogged)
            {
                islogged = false;
                continue;
            }
            if(authUser(user,pass))
            {
                char help[] ="\tMulti Threaded Server/Client - Operating Systems Project \n \tBy Saad Ahmed L1F12BSCS2342 Section D \n\n\tThe following commands are available \n\n\tlist - get list of active users \n\tconnect [username] - connect to client with the provided username \n \tdisconnect - disconnect from server and other users \n\tclear - clear screen\n\n";

                sprintf(status,"\n\t Welcome, %s - You have been authenticated on our server!\n\n",user);
                write(sd,clear,strlen(clear));
                write(sd,status,strlen(status));
                write(sd,help,strlen(help));
                logger(LOG_INFO,"[SERVER] Connection Established With User %s",user);
                logger(LOG_INFO,"[SERVER] Client now Authenticated as %s",user);
                User * userinfo = malloc(sizeof(User));
                userinfo->sd = sd;
                strcpy(userinfo->username,user);
                addWrapper(&list,userinfo);
                authenticated = true;
            }
            else
            {
                char status[] = "\n\tAuthentication Failed - Please ensure your credentials are valid.\n";
                write(sd,status,strlen(status));
            }

        }
        else
        {
            char invalid [] = "\t Invalid Option Choosen\n\n";
            write(sd,invalid,strlen(invalid));
        }
    }

    while(1)
    {
        memset(msg,'\0',INPUT);
        rb = read(sd,&msg,INPUT);
        if(rb<=0)
        {
            logger(LOG_INFO,"[SERVER] User %s disconnected",user);
            close(sd);
            deleteWrapper(&list,user);
            pthread_exit(0);
        }

        //The following block of code check user command against list of supported command
        //If command is valid - then appropriate response is sent - otherwise debug message is
        //sent!
        if(strcmp(msg,commands[HELP])==0)
        {
            char help[] ="\tMulti Threaded Server/Client - Operating Systems Project \n \tBy Saad Ahmed L1F12BSCS2342 Section D \n\n\tThe following commands are available \n\n\tlist - get list of active users \n\tconnect [username] - connect to client with the provided username \n \tdisconnect - disconnect from server and other users \n\tclear - clear screen\n\n";
            write(sd,help,strlen(help));
        }
        else if(strcmp(msg,commands[LIST])==0)
        {
            int n = count(&list);
            char **users = getUsers(&list);
            int i=0;
            char buffer[BUFSIZ];
            memset(buffer,'\0',BUFSIZ);
            strcat(buffer,"List of Active Users \n");
            for(i=0; i<n; i++)
            {
                if(strcmp(users[i],user)!=0)
                {
                    strcat(buffer,users[i]);
                    strcat(buffer,"\n");
                }
            }
            write(sd,buffer,sizeof(buffer));

        }
        else if(strcmp(msg,commands[DISCONNECT])==0)
        {
            sprintf(status,"[SERVER] User %s disconnected \n",user);
            printf("%s",status);
            fflush(stdout);
            deleteWrapper(&list,user);
            close(sd);
            pthread_exit(0);
        }
        else if(strcmp(msg,commands[CLEAR])==0)
        {
            write(sd,clear,strlen(clear));
        }
        else
        {
            //Split String and test whether its connect command..

            char * token = strdup(msg);
            char *connect[2];
            int i=0;
            for(i=0; i<2; i++)
            {
                connect[i] = strsep(&token," ");
            }
            if(connect[1]!=NULL)
            {
                if(strcmp(connect[0],commands[CONNECT])==0)
                {
                    if(strcmp(connect[1],user)==0)
                    {
                        char buffer[BUFSIZ];
                        memset(buffer,'\0',BUFSIZ);
                        sprintf(buffer,"You cannot establish a connection with your own username \n");
                        write(sd,buffer,sizeof(buffer));
                        continue;
                    }
                    User *userobj;
                    if(searchWrapper(&list,connect[1],&userobj))
                    {
                        char buffer[BUFSIZ];
                        memset(buffer,'\0',BUFSIZ);
                        //getpeername functionality Reference:
                        //http://www.beej.us/guide/bgnet/output/html/multipage/getpeernameman.html

                        struct sockaddr_in otherclient;
                        socklen_t len;
                        len = sizeof(otherclient);
                        if (getpeername(userobj->sd,(struct sockaddr *)&otherclient, &len) == -1)
                        {
                            sprintf(buffer,"User %s is no longer active \n",userobj->username);
                            write(sd,buffer,sizeof(buffer));

                        }
                        else
                        {
                            sprintf(buffer,"ip %s\n",inet_ntoa(otherclient.sin_addr));
                            logger(LOG_INFO,"[SERVER] User %s established direct connection with User %s",user,connect[1]);
                            write(sd,buffer,sizeof(buffer));
                        }
                    }
                    else
                    {
                        char buffer[BUFSIZ];
                        memset(buffer,'\0',BUFSIZ);
                        strcat(buffer,"User Inactive/Invalid Username! \n");
                        write(sd,buffer,sizeof(buffer));
                    }
                    continue;
                }
            }
            char debug [] = "\t Invalid Command or missing parameters - type help to get list of available commands and their parameters \n";
            write(sd,debug,strlen(debug));
        }

    }
}



/*
The main server function/thread will init and setup our main socket listener
Each request will be handled in its own thread so that multiple clients can be accomodated!
So, whenever a new client is connected a new thread is launched to deal with the client
and that thread inits two more threads (Send, Recieve) so communication can be bidirectional..
*/

int ssd, csd;

int main(int argc,char *argv[])
{
    init(&list);
    initMutex();
    //Catch both SIGNINT (CTRL+C) and SIGTERM (termination) signals to
    //perform any destructor operations...
    signal(SIGINT, closeSocket);
    signal(SIGTERM, closeSocket);

    //Socket INIT
    unsigned int slen, clen;
    struct sockaddr_in saddr;
    struct sockaddr_in caddr;
    ssd = socket(AF_INET,SOCK_STREAM,0);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(SERVERPORT);
    slen = sizeof(saddr);

    //Socket Binding...
    if(bind(ssd, (struct sockaddr *)&saddr,slen)==-1)
    {
        logger(LOG_EMERG,"[SERVER] Unable to Bind to Port %d \n",SERVERPORT);
        exit(1);
    }

    pthread_t thread; //Thread 0 for write and 1 for Read
    logger(LOG_INFO,"[SERVER] Initializing Server on Port %d",SERVERPORT);
    logger(LOG_INFO,"[SERVER] Server Initialized and Listening on Port %d",SERVERPORT);
    logger(LOG_NOTICE,"[SERVER] LOG is written to both console and standard log");
    logger(LOG_NOTICE,"[SERVER] You may view standard log by typing cat /var/log/syslog");

    fflush(stdout);
    //Server Main Loop!
    while(1)
    {
        listen(ssd,CLIENTQUEUE);
        clen = sizeof(caddr);
        csd = accept(ssd,(struct sockaddr *) &caddr,&clen);
        pthread_create(&thread,NULL,handleClient,(void *)&csd);
    }


    return 0;
}

void closeSocket(int signum)
{
    //TODO Fix Closure in case server is disconnected while client is connected..
    close(ssd);
    logger(LOG_INFO,"[SERVER] Closing Server Socket and Terminating");
    exit(signum);
}

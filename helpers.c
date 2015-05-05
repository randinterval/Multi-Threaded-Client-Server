#include "headers.h"
/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D

    Implementations of Helper Functions that i have used in my application
    such as authenitcation user, registering user and so on..
*/

/*
To Remove newline When Reading Via getline||fgets (as it reads newline into buffer as well..)
*/


pthread_mutex_t mutex;

void initMutex()
{
    pthread_mutex_init(&mutex,NULL);
}
void removeline(char *p)
{
    int i=0;
    for(i=0; i<strlen(p); i++)
    {
        if(p[i]=='\n')
        {
            p[i]='\0';
        }
    }
}

/*
To Verify User Credentials...
*/
int authUser(char *username, char *password)
{
    //Mutex to ensure file access is exclsuive
    pthread_mutex_lock(&mutex);
    int rt = false;
    FILE *fd;
    fd = fopen("users.txt","r");
    if(!fd)
    {
        printf("Unable to Open Users Text File!\n");
        exit(0);
    }
    char * user = malloc(sizeof(char)*ULIMIT);
    char * pass = malloc(sizeof(char)*PLIMIT);
    memset(user,'\0',ULIMIT);
    memset(pass,'\0',PLIMIT);
    size_t len = 0;
    while (getline(&user, &len, fd)!=-1)
    {
        getline(&pass,&len,fd);
        removeline(user);
        removeline(pass);
        if(strcmp(username,user)==0 && strcmp(password,pass)==0)
        {
            rt = true;
        }
    }
    fclose(fd);
    free(user);
    free(pass);
    pthread_mutex_unlock(&mutex);
    return rt;
}

/*
To Register a new User and add user info to file..
Before Registering we also validate user and password with condition that
both should be at least 3 charactars long!
*/

int registerUser(char *username, char *password)
{
    int rt = true;
    pthread_mutex_lock(&mutex);
    int ulength = strlen(username);
    int plength = strlen(password);
    char * user = malloc(sizeof(char)*ULIMIT);
    if(ulength<3 || ulength>ULIMIT || plength<3 || plength>PLIMIT)
    {
        rt = false;
    }
    if(rt)
    {

        FILE *fd;
        fd = fopen("users.txt","r");
        if(!fd)
        {
            printf("Unable to Open Users Text File!\n");
            exit(0);
        }
        size_t len = 0;
        while (getline(&user, &len, fd)!=-1)
        {
            removeline(user);
            if(strcmp(username,user)==0)
            {
                rt = EXISTINGUSER;
            }
        }
        if(rt!=EXISTINGUSER)
        {
            fclose(fd);
            fd = fopen("users.txt","a");
            if(!fd)
            {
                printf("Unable to Open Users Text File!\n");
                exit(0);
            }
            fprintf(fd,"%s\n", username);
            fprintf(fd,"%s\n", password);
            fclose(fd);
        }
    }
    pthread_mutex_unlock(&mutex);
    free(user);
    return rt;
}
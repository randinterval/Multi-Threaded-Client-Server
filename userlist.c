/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D
    Linked list to hold all the data related to each user
    e.g username, socket descriptor, their address (to get port and ip etc..)
    Linked List Core Source Code Reference: Graded Lab 19
*/

#include "headers.h"

void init(struct List *list)
{
    list->first=0;
    list->last=0;
    list->nsearchers = 0;
    list->ninsertors = 0;
    //Initilizing All Semaphores
    sem_init(&list->Searching,0,1);
    sem_init(&list->Inserting,0,1);
    sem_init(&list->searchCountMutex,0,1);
    sem_init(&list->insertCountMutex,0,1);
    sem_init(&list->insertMutex,0,1);
    sem_init(&list->deleteMutex,0,1);
}

void add(struct List *list,User * user)
{
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->user = user;
    node->next=NULL;
    if(!list->first)
    {
        list->first=node;
        list->last = list->first;
    }
    else
    {
        list->last->next=node;
        list->last=list->last->next;
    }
}

int search(struct List *list, char *username,User **user)
{
    struct Node *itr;
    for (itr = list->first; itr!=NULL; itr=itr->next)
    {
        if (strcmp(itr->user->username,username)==0)
        {
            *user = itr->user;
            return 1;
        }
    }
    return 0;
}

int del(struct List *list, char *username)
{
    struct Node *itr;
    struct Node *secondLast;
    struct Node *temp;
    if(list->first==NULL)
        return 0;
    if (strcmp(list->first->user->username,username)==0)
    {
        if(list->first==list->last)
        {
            free(list->first);
            list->first=list->last=0;
            return 1;
        }
        else
        {
            struct Node *temp = list->first->next;
            free(list->first);
            list->first=temp;
            return 1;
        }
    }

    for (itr=list->first; itr->next!=NULL; secondLast=itr,itr=itr->next)
    {
        if (strcmp(itr->next->user->username,username)==0)
        {
            if(itr->next==list->last)
                list->last=itr;
            temp =itr->next->next;
            free(itr->next);
            itr->next = temp;
            return 1;
        }
    }
    return 0;
}

void print(struct List *list)
{
    struct Node *itr;
    for (itr=list->first; itr!=NULL; itr=itr->next)
    {
        printf("Username is %s \n",itr->user->username);
    }
}

int count(struct List *list)
{
    int sum = 0;
    struct Node *itr;
    for (itr=list->first; itr!=NULL; itr=itr->next)
    {
        sum+=1;
    }
    return sum;
}


char ** getUsers(struct List *list)
{
    char **userlist;
    int n = count(list);
    userlist = malloc(n * sizeof(char*));
    int i=0;
    struct Node *itr;
    for (itr=list->first,i=0; itr!=NULL; itr=itr->next,i++)
    {
        userlist[i] = malloc(sizeof(char)*strlen(itr->user->username));
        strcpy(userlist[i],itr->user->username);
    }
    return userlist;
}

//The Following Wrapper Functions are used to differentiate the syncronization code
//from the core linked list code hence these wrapper functions that i have implemented
//enforce syncronization on top of list  functions

void addWrapper(struct List *list,User * user)
{
    //To Protect ninsertors variable
    sem_wait(&list->insertCountMutex);
    list->ninsertors++;
    //If we are the first inserter then we acquire Inserting to ensure that
    //Deleter cannot proceed as deleter has to get a lock on this semaphore
    //before proceeding..if a deleter already has a lock
    //then the first insertor will blocked on this semaphore and new insertors
    //will be blocked on insertCountMutex as 1st inserter wont be be able to release
    //insertCountMutex if its blocked here..
    if(list->ninsertors==1)
    {
        sem_wait(&list->Inserting);
    }
    sem_post(&list->insertCountMutex);
    //This is to ensure only 1 insertor can enter critical section
    sem_wait(&list->insertMutex);

    //Call the Actual Add Function
    add(list,user);

    sem_post(&list->insertMutex);
    sem_wait(&list->insertCountMutex);
    list->ninsertors--;
    //If we are the last insertor then we release Inserting Mutex
    //which indiciates that no insert operation is going on hence
    //if there are any deletors blocked on this sema they can proceed
    if(list->ninsertors==0)
    {
        sem_post(&list->Inserting);
    }
    sem_post(&list->insertCountMutex);
}

int searchWrapper(struct List *list, char * username, User ** user)
{
    sem_wait(&list->searchCountMutex);
    list->nsearchers++;
    if(list->nsearchers==1)
    {

        //If we are the first Searcher then we acquire Search Sema to ensure that
        //Deleter cannot proceed as deleter has to get a lock on this semaphore
        //before proceeding..if a deleter already has a lock
        //then the first searcher will blocked on this semaphore and new searchers
        //will be blocked on insertCountMutex as 1st inserter wont be be able to release
        //insertCountMutex if its blocked here..
        sem_wait(&list->Searching);
    }
    sem_post(&list->searchCountMutex);

    //Call the Actual Search Function..
    int rt = search(list,username,user);
    sem_wait(&list->searchCountMutex);
    list->nsearchers--;
    if(list->nsearchers==0)
    {
        //If we are the searcher then we release this sema to indiciate
        //that no search operation is going on hence deleter can proceed
        //if no insert is going on as well..
        sem_post(&list->Searching);
    }
    sem_post(&list->searchCountMutex);
    return rt;
}

int deleteWrapper(struct List *list, char *username)
{
    int rt;
    //First We wait to see whether a search operation is going on
    //If it is then we wait
    sem_wait(&list->Searching);
    //If no search operation is happening then we check whether a insert operationg is
    //occuring or not, if it is we wait
    sem_wait(&list->Inserting);
    //Else we simply wait for deletion to complete if there is any (to ensure only 1 delete
    //can occur at any given instance
    sem_wait(&list->deleteMutex);

    //Call the Actual Delete Function..
    rt = del(list,username);

    //Finally after completition we release deletemutex so other deleters can delete
    sem_post(&list->deleteMutex);
    //then we release insert and search mutex so that other inserters and searchers
    //can now run as delete operation is now done..
    sem_post(&list->Inserting);
    sem_post(&list->Searching);

    return rt;
}



typedef struct
{
    int sd;
    char username[ULIMIT];

} User;

struct Node
{
    struct Node *next;
    User * user;
};

struct List
{
    struct Node *first;
    struct Node *last;
    //All the Syncornization Primitives are added into the List
    //Data Structure
    //Syncronization
    int nsearchers;
    int ninsertors;
    sem_t Searching; //To exclude deleter from proceeding during searching
    sem_t Inserting; //To exclude inserter from proceeding during insertion
    sem_t insertMutex; //To Ensure that only 1 insertor can insert at any instance
    sem_t deleteMutex;// To Ensure Mutual Exclusiveness for Deletion Operation (Only one deleter)
    sem_t searchCountMutex; //To protect nserarchers variable
    sem_t insertCountMutex; //To Protect nreaders variable
};

void init(struct List *list);
void add(struct List *list,User * user);
int search(struct List *list, char *username,User **user);
int del(struct List *list, char *username);
void print(struct List *list);
void addWrapper(struct List *list,User * user);
int searchWrapper(struct List *list, char * username, User ** user);
int deleteWrapper(struct List *list, char *username);
int count(struct List *list);
char ** getUsers(struct List *list);

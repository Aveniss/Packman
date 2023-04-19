//
// Created by root on 10/12/22.
//
#include <pthread.h>
#ifndef EMPTY_PROJECT_SERVER_H
#define EMPTY_PROJECT_SERVER_H
#define MAX_BEASTS 5
#define MAX_PLAYERS 2
static int res = 0;
static  int checkPlayer2 = 0;
static char offlineMove  = ' ';
static struct treasure_list treasureList;
pthread_mutex_t addSth;
enum status
{
    TO_BUSH=1,OUT_BUSH=2,NO_BUSH=3,CAMP=4,IN_OBJECT
};

enum direction
{
    LEFT,RIGHT,UP,DOWN
};

struct point{
    int x,y;
};
struct mapInfo
{
    char **board;
    struct point camp;
};
struct beast
{
    struct point pos;
    char ***board;
    enum direction direction;
    enum status status;
    char position;
    struct player *players;
    pthread_mutex_t mutexBeast;
};

struct treasure_node
{
    struct point pos;
    struct treasure_node *next;
    struct treasure_node *prev;
    int coinsNmber;
};


struct treasure_list
{
    struct treasure_node *head;
    struct treasure_node *tail;
};
struct player
{
    int PID,x,y,deathes,carried,brought,id,round;
    enum status status;
    pthread_mutex_t playerMutex;
};
struct server_t
{
    int serverPID,roundNumber,playerNumber,beastNumber;
    struct mapInfo mapInfo;


    struct beast beasts[MAX_BEASTS];
    pthread_t threads[MAX_BEASTS];
    struct player *players;
};

struct infoForPlayer
{
    char ***board;
    struct player *player;
    int *numberOfPlayers;
};
void setBeast(struct server_t *server,struct beast *beast);
void printMap(struct server_t *server);
void setPlayer(char ***board,struct player *player,int pn);
int movePlayer(char ***board,struct player *player,char step);
void printBlock(char x,int i,int j,int option);
void* comunicationWithPlayer(void *package);
void* addSomethingToMap(void *serw);
struct treasure_node* findTreasure(struct treasure_list, int x,int y);
void addTreasure(char ***map, int x,int y, int amountOfCoins);
void deleteUser(struct player *player,char ***board);
int followPlayer(struct beast *beast);
void* moveBeast(void *beast);
int getTreasure(struct treasure_list list,int x,int y);
void freeTreasure(struct  treasure_list list);
void* offlinePlayerMove(void* package);
void* waitingForPlayer(void* package);
#endif //EMPTY_PROJECT_SERVER_H

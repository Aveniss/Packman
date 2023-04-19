#include <ncurses.h>
#include "server.h"
#include "server.c"
#include "reader.h"
#include "reader.c"
#include <unistd.h>
#include <stdlib.h>




int main()
{
    pthread_t con_t,adder_t,offlinePlayer_t,waint_t;
    initscr();
    start_color();
    noecho();
    keypad(stdscr,true);
    refresh();
    struct server_t *server = calloc(1,sizeof(struct server_t));
    server->players = calloc(2,sizeof(struct player));
    server->playerNumber = 0;
    server->beastNumber = 0;
    server->roundNumber = 0;
    (server->players+1)->round = server->roundNumber;
    server->serverPID = getpid();
    treasureList.head=NULL;
    treasureList.tail=NULL;

    readMap("map.txt",&server->mapInfo.board);
    setPlayer(&server->mapInfo.board,server->players,server->playerNumber+1);
    server->playerNumber++;
    struct infoForPlayer *info = calloc(1,sizeof(struct mapInfo));
    info->board = &server->mapInfo.board;
    info->player = server->players;

//    setPlayer(info->board,info->player+1,2);
//    server->playerNumber++;
    info->numberOfPlayers = &server->playerNumber;
    server->beasts[0].board = info->board;
    server->beasts[1].board = info->board;
    server->beasts[2].board = info->board;
    server->beasts[3].board = info->board;
    server->beasts[4].board = info->board;
    pthread_create(&waint_t,NULL,waitingForPlayer,info);
//    addstr("Oczekiwanie na gracza");
//    refresh();
//    while(1)
//    {
//        if(checkPlayer2)
//            break;
//    }
    pthread_create(&con_t,NULL,comunicationWithPlayer,info);
    printMap(server);
    pthread_create(&adder_t,NULL,addSomethingToMap,server);
    pthread_mutex_init(&addSth,NULL);
    pthread_mutex_lock(&addSth);

    pthread_create(&offlinePlayer_t,NULL,offlinePlayerMove,info);

    pthread_mutex_init(&(server->players)->playerMutex,NULL);
    pthread_mutex_lock(&(server->players)->playerMutex);




    while (1)
    {
        if(checkPlayer2 == 1) {
            setPlayer(info->board, info->player + 1, 2);
            checkPlayer2 = 2;
        }
        for (int i = 0; i < server->beastNumber; ++i) {
            pthread_mutex_unlock(&server->beasts[i].mutexBeast);
        }
        pthread_mutex_unlock(&(server->players+1)->playerMutex);
        pthread_mutex_unlock(&addSth);
        if(res)
            break;
        pthread_mutex_unlock(&(server->players)->playerMutex);
        server->roundNumber++;
        (server->players+1)->round = server->roundNumber;
        sleep(1);
        printMap(server);

    }

    free(server->players);
    for (int i = 0; i < 25; ++i) {
        free(*(server->mapInfo.board+i));
    }
    free(server->mapInfo.board);
    free(server);
    freeTreasure(treasureList);
    endwin();
    return 0;
}


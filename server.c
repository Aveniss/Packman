#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include "server.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void printBlock(char x,int i,int j,int option)
{
    init_color(COLOR_CYAN,144,20,227);

    init_pair(1, COLOR_BLACK, COLOR_BLACK);         //WALL
    init_pair(2, COLOR_WHITE, COLOR_WHITE);         //WAY
    init_pair(3, COLOR_WHITE, COLOR_CYAN);          //PLAYER
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);          //COINS
    init_pair(5, COLOR_BLACK, COLOR_WHITE);          //HASH
    init_pair(6, COLOR_GREEN, COLOR_BLUE);          //CAMPSITE
    init_pair(7, COLOR_RED, COLOR_WHITE);          //ENEMY
    init_pair(8, COLOR_GREEN, COLOR_YELLOW);          //TREASURE

    attrset(COLOR_PAIR(option));
    move(i, j);
    addch(x);
    attroff(COLOR_PAIR(option));

}

void printMap(struct server_t *server) {

    if (has_colors()) {
        if (start_color() == OK) {
            init_pair(98,COLOR_WHITE,COLOR_WHITE);
            attrset(COLOR_PAIR(98));

            for (int y = 0; y < 90; y++) {
                mvhline(y, 0, 98, 200);
            }
            attroff(COLOR_PAIR(98));


            for (int i = 0; i < 25; ++i) {
                for (int j = 0; j < 51; ++j) {
                    switch(*(*(server->mapInfo.board+i)+j)){
                        case 'w':printBlock(' ',i,j,1);break;
                        case ' ':printBlock(' ',i,j,2);break;
                        case '1':
                        case '2':printBlock(*(*(server->mapInfo.board+i)+j),i,j,3);break;
                        case '#':printBlock(*(*(server->mapInfo.board+i)+j),i,j,5);break;
                        case 'A':printBlock(*(*(server->mapInfo.board+i)+j),i,j,6);break;
                        case '*':printBlock(*(*(server->mapInfo.board+i)+j),i,j,7);break;
                        case 'c':
                        case 't':
                        case 'T':printBlock(*(*(server->mapInfo.board+i)+j),i,j,4);break;
                        case 'D':printBlock(*(*(server->mapInfo.board+i)+j),i,j,8);break;
                    }
                }
            }
            refresh();
            init_pair(99, COLOR_BLACK, COLOR_WHITE);
            attrset(COLOR_PAIR(99));

            move(1,56);
            printw("Server's PID: %d",server->serverPID);
            move(2,57);
            addstr("Campsite X/Y: 23/11");
            move(3,57);
            printw("Round number: %d",server->roundNumber);
            move(5,56);
            addstr("Parameter:\tPlayer1\tPlayer2");
            move(6,57);
            printw("PID\t\t");
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if(i<server->playerNumber)
                    printw("%d",server->players[i].PID);
                else
                    addch('-');
                if(i!=MAX_PLAYERS-1)
                    printw("\t");
            }
            move(7,57);
            printw("Type\t\t");
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if(i<server->playerNumber)
                    printw("HUMAN");
                else
                    addch('-');
                if(i!=MAX_PLAYERS-1)
                    printw("\t");
            }
            move(8,57);
            printw("Curr X/Y\t");
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if(i<server->playerNumber)
                    printw("%d/%d",server->players[i].x,server->players[i].y);
                else
                    addstr("--/--");
                if(i!=MAX_PLAYERS-1)
                    printw("\t");
            }
            move(9,57);
            printw("Deaths\t\t");
            for (int i = 0; i < MAX_PLAYERS; ++i) {
                if(i<server->playerNumber)
                    printw("%d",server->players[i].deathes);
                else
                    addch('-');
                if(i!=MAX_PLAYERS-1)
                    printw("\t");
            }

            move(11,56);
            addstr("Coins");
            move(12,57);
            addstr("carried\t");
            for (int i = 0; i < server->playerNumber; ++i) {
                if(i<server->playerNumber)
                    printw("%d",server->players[i].carried);
                else
                    addch('-');
                if(i!=server->playerNumber-1)
                    printw("\t");
            }
            move(13,57);
            addstr("brought\t");
            for (int i = 0; i < server->playerNumber; ++i) {
                if(i<server->playerNumber)
                    printw("%d",server->players[i].brought);
                else
                    addch('-');
                if(i!=server->playerNumber-1)
                    printw("\t");
            }



            move(16, 56);
            addstr("Legend:");
            move(17, 57);
            attroff(COLOR_PAIR(99));
            init_pair(33, COLOR_WHITE, COLOR_CYAN);
            attrset(COLOR_PAIR(33));
            addstr("1234");
            attroff(COLOR_PAIR(33));
            init_pair(99, COLOR_BLACK, COLOR_WHITE);
            attrset(COLOR_PAIR(99));
            move(17, 61);
            addstr(" - players");

            move(18, 58);
            addstr("    - wall");
            move(19, 57);
            addstr("     - bushes (slow down)");
            move(20, 57);

            addstr("     - wild beast");
            move(21, 57);

            addstr("     - one coin");
            move(22, 57);
            addstr("     - dropped treasure");
            move(23, 57);
            addstr("     - treasure (10 coins)");
            move(24, 57);

            addstr("     - large treasure (50 coins)");
            move(25, 57);

            addstr("     - campsite");
            attroff(COLOR_PAIR(99));
            printBlock('A', 25, 57, 6);
            printBlock('T', 24, 57, 4);
            printBlock('t', 23, 57, 4);
            printBlock('D', 22, 57, 8);
            printBlock('c', 21, 57, 4);
            printBlock(' ', 18, 57, 1);
            printBlock('#', 19, 57, 5);
            printBlock('*', 20, 57, 7);
            move(0,0);
            refresh();
        } else {
            addstr("Cannot start colours\n");
            refresh();
        }
    } else {
        addstr("Not colour capable\n");
        refresh();
    }
}



void setPlayer(char ***board,struct player *player,int pn)
{

    srand(time(0));
    int x = rand() % 25,y=rand() % 51;
    while(*(*(*board+x)+y)!=' ')
    {
        x = rand() % 25;
        y=rand() % 51;
    }
    *(*(*board+x)+y) = (char)(pn+'0');
    player->x = x;
    player->y = y;
    player->brought = 0;
    player->id = pn;
    player->status = NO_BUSH;
    player->carried = 0;
    player->deathes = 0;
}

int movePlayer(char ***board,struct player *player,char step) {

    int x, y;

    x = player->x, y = player->y;

    if (tolower(step) != 'w' && tolower(step) != 'a' && tolower(step) != 's' && tolower(step) != 'd' &&
        tolower(step) != 'q')
        return 1;

    switch (tolower(step)) {
        case 'w':
            x--;
            break;
        case 's':
            x++;
            break;
        case 'a':
            y--;
            break;
        case 'd':
            y++;
            break;
        default:
            return 1;
    }
    if (*(*((*board) + x) + y) == 'w'){
        *(*((*board) + player->x) + player->y) = (char) (player->id + '0');
        return 1;
    }
    else if(*(*((*board) + x) + y) == '1' || *(*((*board) + x) + y) == '2')
    {
        return -1;
    }

    if (player->status == TO_BUSH) {
        player->status = OUT_BUSH;
        return 0;
    } else if (player->status == OUT_BUSH) {
        player->status = NO_BUSH;
        *(*((*board) + player->x) + player->y) = '#';
    } else if (player->status == CAMP) {
        *(*((*board) + player->x) + player->y) = 'A';
        player->status = NO_BUSH;
    } else
        *(*((*board) + player->x) + player->y) = ' ';

    player->y = y;
    player->x = x;

    switch (*(*((*board) + player->x) + player->y)) {
        case '#':
            player->status = TO_BUSH;
            break;
        case 'c':
            player->carried++;
            break;
        case 't':
            player->carried += 10;
            break;
        case 'T':
            player->carried += 50;
            break;
        case 'A':
            player->status = CAMP;
            player->brought += player->carried;
            player->carried = 0;
            break;
        case 'D':player->carried+= getTreasure(treasureList,player->x,player->y);break;
    }
    *(*((*board) + player->x) + player->y) = (char) (player->id + '0');
    return 0;
}

void* waitingForPlayer(void* package)
{
    struct infoForPlayer *info = (struct infoForPlayer *) package;
    mkfifo("check",0777);
    int ftw,i = getpid(),numerOfPlayers = *(info->numberOfPlayers);
    while(1)
    {
        if(numerOfPlayers>2)
            i = -1;
        else
            numerOfPlayers = 3;
        ftw= open("check",O_WRONLY);
        write(ftw,&i,sizeof(int));
        close(ftw);
        if(i!=-1)
        {
            checkPlayer2 = 1;
        }
        else if(res) {
            break;
        }
    }
    return NULL;
}

void* comunicationWithPlayer(void *package) {
    struct infoForPlayer *info = (struct infoForPlayer *) package;
    int ftr,ftw,err,i = getpid(),option;
    char wall = 'w',contener;

    pthread_mutex_init(&(info->player+1)->playerMutex,NULL);
    pthread_mutex_lock(&(info->player+1)->playerMutex);


//    server->playerNumber++;

    mkfifo("toPlayer",0777);
    mkfifo("fromPlayer",0777);
    ftw = open("toPlayer",O_WRONLY);
    ftr = open("fromPlayer",O_RDONLY);
    while (1)
    {
        pthread_mutex_lock(&(info->player+1)->playerMutex);
        write(ftw, &(info->player+1)->id, sizeof(int));
        write(ftw, &(info->player+1)->x, sizeof(int));
        write(ftw, &(info->player+1)->y, sizeof(int));
        write(ftw, &(info->player+1)->deathes, sizeof(int));
        write(ftw, &(info->player+1)->carried, sizeof(int));
        write(ftw, &(info->player+1)->brought, sizeof(int));
        write(ftw, &(info->player+1)->round, sizeof(int));


        for (int j = ((info->player+1)->x)-2; j <=((info->player+1)->x)+2 ; ++j) {
            for (int k = ((info->player+1)->y)-2; k <=((info->player+1)->y)+2 ; ++k) {
                if(j<0||k<0||k>=51||j>=25) {
                    err = write(ftw, &wall, sizeof(char));
                }
                else
                {
                    err = write(ftw,(*((*info->board) + j) + k),sizeof(char));
                }
                if(err == -1)
                {
                    printw("coś poszło nie tak");
                    refresh();
                    break;
                }

            }
        }
        while(1)
        {

            read(ftr,&contener,sizeof(char));

            if(contener == 'q')
                break;
            else if(contener == ' ')
            {
                option = 1;
                write(ftw,&option,sizeof(int));
                break;
            }
            option = movePlayer(info->board,(info->player+1),contener);
            if(option==0)
                option = 1;
            else if(option == 1)
                option = 0;
            else if(option == -1)
            {
                option = 1;
                if((info->player+1)->carried!=0 || (info->player)->carried!=0) {
                    addTreasure(info->board, (info->player)->x, (info->player)->y,(info->player + 1)->carried + (info->player)->carried);
                    *(*((*info->board) + (info->player)->x) + (info->player)->y) = 'D';
                }
                else {
                    *(*((*info->board) + (info->player)->x) + (info->player)->y) = ' ';
                }
                *(*((*info->board) + (info->player + 1)->x) + (info->player + 1)->y) = ' ';

                deleteUser((info->player+1),info->board);
                deleteUser((info->player),info->board);
            }

            write(ftw,&option,sizeof(int));

            if(option)
                break;
        }
        if(contener == 'q') {
            close(ftw);
            close(ftr);
 //           res = 1;
            *(*((*info->board) + (info->player+1)->x) + (info->player+1)->y) = ' ';
            break;
        }
        write(ftw, &(info->player+1)->id, sizeof(int));
        write(ftw, &(info->player+1)->x, sizeof(int));
        write(ftw, &(info->player+1)->y, sizeof(int));
        write(ftw, &(info->player+1)->deathes, sizeof(int));
        write(ftw, &(info->player+1)->carried, sizeof(int));
        write(ftw, &(info->player+1)->brought, sizeof(int));
        write(ftw, &(info->player+1)->round, sizeof(int));


        for (int j = ((info->player+1)->x)-2; j <=((info->player+1)->x)+2 ; ++j) {
            for (int k = ((info->player+1)->y)-2; k <=((info->player+1)->y)+2 ; ++k) {
                if(j<0||k<0||k>=51||j>=25) {
                    err = write(ftw, &wall, sizeof(char));
                }
                else
                {
                    err = write(ftw,(*((*info->board) + j) + k),sizeof(char));
                }
                if(err == -1)
                {
                    printw("coś poszło nie tak");
                    refresh();
                    break;
                }

            }
        }
    }

    return NULL;
}

void* addSomethingToMap(void *serw)
{
    noecho();
    char option;
    int buff;
    struct server_t *server = (struct server_t *)serw;
    while(1)
    {
        pthread_mutex_lock(&addSth);
        flushinp();
        timeout(1000);
        buff = getch();
        timeout(-1);
        if(buff == ERR) {
            continue;
        }
        switch (buff) {
            case 259: option ='w';break;
            case 258: option ='s';break;
            case 260: option ='a';break;
            case 261: option ='d';break;
            default: option =(char)buff;
        }
        if(tolower(option) == 'q')
        {
            res = 1;
            break;
        }
        else if(option == 'w' || option == 's'||option=='a'||option=='d')
        {
            offlineMove = option;
        }
        else if(tolower(option) == 'b')
        {
            if(server->beastNumber<MAX_BEASTS)
            {
                setBeast(server,&(server->beasts[server->beastNumber]));
                server->beastNumber++;
            }
        }
        else if( option == 'c'|| option == 't'|| option == 'T')
        {
            int x = rand() % 25,y=rand() % 51;
            while(*(*(server->mapInfo.board+x)+y)!=' ')
            {
                x = rand() % 25;
                y=rand() % 51;
            }
            *(*(server->mapInfo.board+x)+y) = option;
        }
    }
    return NULL;
}

void* moveBeast(void* beastt)
{
    struct beast *beast = (struct beast *)beastt;

    int where = rand() % 4,i=0,j=0;
    switch(where)
    {
        case 0: beast->direction = UP;break;
        case 1: beast->direction = DOWN;break;
        case 2: beast->direction = LEFT;break;
        case 3: beast->direction = RIGHT;break;
    }

    while(1)
    {
        pthread_mutex_lock(&beast->mutexBeast);
        if(beastt == NULL || res)
            break;

            if (beast->status == TO_BUSH) {
                beast->status = OUT_BUSH;
                continue;
            } else if (beast->status == OUT_BUSH) {
                beast->status = NO_BUSH;
                *(*((*beast->board) + beast->pos.x) + beast->pos.y) = '#';
            } else if (beast->status == IN_OBJECT) {
                *(*((*beast->board) + beast->pos.x) + beast->pos.y) = beast->position;
                beast->status = NO_BUSH;
            } else
                *(*((*beast->board) + beast->pos.x) + beast->pos.y) = ' ';


            while(1)
            {
                i=0;j=0;
                if(followPlayer(beast))
                {
                    switch (followPlayer(beast)) {
                        case 1: i++; break;
                        case 2: i--; break;
                        case 3: j--; break;
                        case 4: j++; break;
                    }
                }
                else
                {
                    switch (beast->direction) {
                        case UP: i++;break;
                        case DOWN: i--;break;
                        case LEFT: j--;break;
                        case RIGHT: j++;break;
                    }
                }

                if(*(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j) != 'w' && *(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j) != '*')
                {
                    if(*(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j) == '#')
                        beast->status = TO_BUSH;
                    else if (*(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j) == '1' || *(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j) == '2')
                    {
                        int p = (int)(*(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j)-'1');
                        if((beast->players+p)->carried!=0){
                            beast->position = 'D';
                            addTreasure(beast->board,beast->pos.x+i,beast->pos.y+j,(beast->players+p)->carried);
                        }
                        else
                            beast->position = ' ';
                        deleteUser((beast->players+p),beast->board);
                        beast->status = IN_OBJECT;
                    }
                    else
                    {
                        beast->status = IN_OBJECT;
                        beast->position = *(*((*beast->board) + beast->pos.x + i) + beast->pos.y + j);
                    }
                    
                    *(*((*beast->board) + beast->pos.x+i) + beast->pos.y+j) = '*';
                    beast->pos.x +=i;
                    beast->pos.y +=j;
                    break;
                }
                else
                {
                    where = rand() % 4;
                    switch(where)
                    {
                        case 0: beast->direction = UP;break;
                        case 1: beast->direction = DOWN;break;
                        case 2: beast->direction = LEFT;break;
                        case 3: beast->direction = RIGHT;break;
                    }
                    continue;
                }
            }
    }
    return NULL;
}

void setBeast(struct server_t *server,struct beast *beast)
{
    int x = rand() % 25,y=rand() % 51;
    while(*(*(server->mapInfo.board+x)+y)!=' ')
    {
        x = rand() % 25;
        y=rand() % 51;
    }
    *(*(server->mapInfo.board+x)+y)='*';
    beast->pos.y = y;
    beast->pos.x = x;
    beast->status = NO_BUSH;
    beast->position = ' ';
    beast->players = server->players;
    pthread_mutex_init(&beast->mutexBeast,NULL);
    pthread_mutex_lock(&beast->mutexBeast);
    pthread_create(&(server->threads[server->beastNumber]), NULL, moveBeast, beast);
}

void addTreasure(char ***map, int x,int y, int amountOfCoins)
{
    if(*(*((*map) + x) + y)=='D')
    {
        struct treasure_node *node = findTreasure(treasureList,x,y);
        node->coinsNmber+=amountOfCoins;
    }
    else if(treasureList.head==NULL)
    {
        treasureList.head = calloc(1,sizeof(struct treasure_node));
        treasureList.head->prev=NULL;
        treasureList.head->next=NULL;
        treasureList.head->pos.x=x;
        treasureList.head->pos.y=y;
        treasureList.head->coinsNmber=amountOfCoins;
        treasureList.head->next=NULL;
        treasureList.tail = treasureList.head;
        *(*((*map) + x) + y)='D';
    }
    else
    {
        struct treasure_node *node = calloc(1,sizeof(struct treasure_node));
        node->pos.x = x;
        node->pos.y = y;
        node->coinsNmber = amountOfCoins;
        node->next = NULL;
        treasureList.tail->next = node;
        node->prev = treasureList.tail;
        treasureList.tail = node;
        *(*((*map) + x) + y)='D';
    }
}

void deleteUser(struct player *player,char ***board)
{
    srand(time(0));
    int x = rand() % 25,y=rand() % 51;
    while(*(*(*board+x)+y)!=' ')
    {
        x = rand() % 25;
        y=rand() % 51;
    }

    player->x = x;
    player->y = y;
    *(*((*board) + x) + y)= player->id+'0';
    player->carried = 0;
    player->status = NO_BUSH;
    player->deathes ++;
}

struct treasure_node* findTreasure(struct treasure_list list, int x,int y)
{
    struct treasure_node *node = list.head;
    while(node!=NULL){
        if(node->pos.x == x && node->pos.y == y)
            return node;
        node = node->next;
    }

    return NULL;
}
int followPlayer(struct beast *beast)
{
    int x = beast->pos.x;
    int y = beast->pos.y;

    
    // Nie jestem dumny z tego kodu ale było już późno a musiałem to skończyć xD
    if((x-2>0 && (*(*((*beast->board) + (x-2)) + y) == '1' || *(*((*beast->board) + (x-2)) + y) == '2') && *(*((*beast->board) + (x-1)) + y) !='w')||(x-1>0 &&(*(*((*beast->board) + (x-1)) + y) =='1'||*(*((*beast->board) + (x-1)) + y) =='2')))
        return 2; //up
    else if((x+2<25 && (*(*((*beast->board) + (x+2)) + y) == '1' || *(*((*beast->board) + (x+2)) + y) == '2') && *(*((*beast->board) + (x+1)) + y) !='w') ||(x+1<25 && (*(*((*beast->board) + (x+1)) + y) =='1'||*(*((*beast->board) + (x+1)) + y) =='2')))
        return 1; //down
    else if((y-2>0 && (*(*((*beast->board) + x) + (y-2)) == '1' || *(*((*beast->board) + x) + (y-2)) == '2') && *(*((*beast->board) + x) + (y-1)) !='w')||(y-1>0 && (*(*((*beast->board) + x) + (y-1)) == '1' || *(*((*beast->board) + x) + (y-1)) == '2')))
        return 3; // left
    else if((y+2<51 && (*(*((*beast->board) + x) + (y+2)) == '1' || *(*((*beast->board) + x) + (y+2)) == '2') && *(*((*beast->board) + x) + (y+1)) !='w')||(y+2<51 && ((*(*((*beast->board) + x) + (y+1)) == '1' || *(*((*beast->board) + x) + (y+1)) == '2'))))
        return 4; // right
    else if((*(*((*beast->board) + (x+1)) + (y+1)) == '1' || *(*((*beast->board) + (x+1)) + (y+1)) == '2')&&*(*((*beast->board) + (x+1)) + (y)) != 'w')
        return 1;
    else if((*(*((*beast->board) + (x+1)) + (y+1)) == '1' || *(*((*beast->board) + (x+1)) + (y+1)) == '2')&&*(*((*beast->board) + (x)) + (y+1)) != 'w')
        return 4;
    else if((*(*((*beast->board) + (x+1)) + (y-1)) == '1' || *(*((*beast->board) + (x+1)) + (y-1)) == '2')&&*(*((*beast->board) + (x+1)) + (y)) != 'w')
        return 1;
    else if((*(*((*beast->board) + (x+1)) + (y-1)) == '1' || *(*((*beast->board) + (x+1)) + (y-1)) == '2')&&*(*((*beast->board) + (x)) + (y-1)) != 'w')
        return 3;
    else if((*(*((*beast->board) + (x-1)) + (y-1)) == '1' || *(*((*beast->board) + (x-1)) + (y-1)) == '2')&&*(*((*beast->board) + (x-1)) + (y)) != 'w')
        return 2;
    else if((*(*((*beast->board) + (x-1)) + (y-1)) == '1' || *(*((*beast->board) + (x-1)) + (y-1)) == '2')&&*(*((*beast->board) + (x)) + (y-1)) != 'w')
        return 3;
    else if((*(*((*beast->board) + (x-1)) + (y+1)) == '1' || *(*((*beast->board) + (x-1)) + (y+1)) == '2')&&*(*((*beast->board) + (x-1)) + (y)) != 'w')
        return 2;
    else if((*(*((*beast->board) + (x-1)) + (y+1)) == '1' || *(*((*beast->board) + (x-1)) + (y+1)) == '2')&&*(*((*beast->board) + (x)) + (y-1)) != 'w')
        return 4;

    else if( (x+2<25) && (*(*((*beast->board) + (x+2)) + (y+1)) == '1' || *(*((*beast->board) + (x+2)) + (y+1)) == '2') && (*(*((*beast->board) + (x+1)) + (y)) != 'w') &&((*(*((*beast->board) + (x+1)) + (y+1)) != 'w')))
        return 1;
    else if( (x+2<25) && (*(*((*beast->board) + (x+2)) + (y-1)) == '1' || *(*((*beast->board) + (x+2)) + (y-1)) == '2') && (*(*((*beast->board) + (x+1)) + (y)) != 'w') &&((*(*((*beast->board) + (x+1)) + (y-1)) != 'w')))
        return 1;

    else if( (x-2>0) && (*(*((*beast->board) + (x-2)) + (y+1)) == '1' || *(*((*beast->board) + (x-2)) + (y+1)) == '2') && (*(*((*beast->board) + (x-1)) + (y)) != 'w') &&((*(*((*beast->board) + (x-1)) + (y+1)) != 'w')))
        return 2;
    else if( (x-2>0) && (*(*((*beast->board) + (x-2)) + (y-1)) == '1' || *(*((*beast->board) + (x-2)) + (y-1)) == '2') && (*(*((*beast->board) + (x-1)) + (y)) != 'w') &&((*(*((*beast->board) + (x-1)) + (y-1)) != 'w')))
        return 2;

    else if( (y+2<51) && (*(*((*beast->board) + (x+1)) + (y+2)) == '1' || *(*((*beast->board) + (x+1)) + (y+2)) == '2') && (*(*((*beast->board) + (x)) + (y+1)) != 'w') &&((*(*((*beast->board) + (x-1)) + (y+1)) != 'w')))
        return 4;
    else if( (y+2<51) && (*(*((*beast->board) + (x-1)) + (y+2)) == '1' || *(*((*beast->board) + (x-1)) + (y+2)) == '2') && (*(*((*beast->board) + (x)) + (y+1)) != 'w') &&((*(*((*beast->board) + (x+1)) + (y+1)) != 'w')))
        return 4;

    else if( (y-2>0) && (*(*((*beast->board) + (x+1)) + (y-2)) == '1' || *(*((*beast->board) + (x+1)) + (y-2)) == '2') && (*(*((*beast->board) + (x)) + (y-1)) != 'w') &&((*(*((*beast->board) + (x-1)) + (y-1)) != 'w')))
        return 3;
    else if( (y-2>0) && (*(*((*beast->board) + (x-1)) + (y-2)) == '1' || *(*((*beast->board) + (x-1)) + (y-2)) == '2') && (*(*((*beast->board) + (x)) + (y-1)) != 'w') &&((*(*((*beast->board) + (x+1)) + (y-1)) != 'w')))
        return 3;

    return 0;

}

int getTreasure(struct treasure_list list,int x,int y)
{

    struct treasure_node *node = list.head;
    while (node!=NULL)
    {
        if(node->pos.x == x && node->pos.y == y)
        {
            node->pos.x = -1;
            node->pos.y = -1;
            return node->coinsNmber;
        }
        node = node->next;
    }
    return 0;
}
void freeTreasure(struct  treasure_list list)
{
    struct treasure_node *node = list.head;
    struct treasure_node *wsk;
    while (node!=NULL)
    {
        wsk = node;
        node = node->next;
        free(wsk);
    }
}

void* offlinePlayerMove(void* package)
{
    struct infoForPlayer *info = (struct infoForPlayer *) package;
    int option;
    while(1)
    {
        pthread_mutex_lock(&(info->player)->playerMutex);
        if(offlineMove!=' ')
        {
            option = movePlayer(info->board,info->player,offlineMove);
           if(option == -1)
            {
                if((info->player+1)->carried!=0 || (info->player)->carried!=0) {
                    addTreasure(info->board, (info->player+1)->x, (info->player+1)->y,(info->player + 1)->carried + (info->player)->carried);
                    *(*((*info->board) + (info->player+1)->x) + (info->player+1)->y) = 'D';
                }
                else {
                    *(*((*info->board) + (info->player+1)->x) + (info->player+1)->y) = ' ';
                }
                *(*((*info->board) + (info->player )->x) + (info->player )->y) = ' ';

                deleteUser((info->player+1),info->board);
                deleteUser((info->player),info->board);
            }
           offlineMove = ' ';
        }

        if(res)
            break;
    }
    return NULL;
}








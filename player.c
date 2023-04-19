//
// Created by root on 10/14/22.
//
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <thread_db.h>
#include "player.h"
#include <ncurses.h>

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
    init_pair(9, COLOR_RED, COLOR_RED);          //TREASURE

    attrset(COLOR_PAIR(option));
    move(i, j);
    addch(x);
    attroff(COLOR_PAIR(option));
}
 void printStat(int pid,int number,int Cx,int Cy,int round,int x,int y,int deaths,int found,int brought)
 {
     if (has_colors()) {
         if (start_color() == OK) {


             init_pair(98,COLOR_WHITE,COLOR_WHITE);
             attrset(COLOR_PAIR(98));

             for (int i = 1; i <=13 ; ++i) {
                 for (int j = 10; j <= 30; ++j) {
                     move(i,j);
                     addch(' ');
                 }
             }
             attroff(COLOR_PAIR(98));


             init_pair(99, COLOR_BLACK, COLOR_WHITE);
             attrset(COLOR_PAIR(99));
             move(1, 10);
             printw("Server's PID: %d", pid);
             move(2, 11);
             if (Cx != -1) {
                 printw("Campsite X/Y: %d/%d", Cy, Cx);
             }
             else
                 printw("Campsite X/Y: unknow");
             move(3, 11);
             printw("Round number: %d", round);
             move(5, 10);
             addstr("Player");
             move(6, 11);
             printw("Number:\t%d", number);
             move(7, 11);
             printw("Type:\tHUMAN");
             move(8, 11);
             printw("Curr X/Y\t%d/%d", x, y);
             move(9, 11);
             printw("Deaths\t%d", deaths);
             move(11, 11);
             printw("Coins found %d", found);
             move(12, 11);
             printw("Coins brought  %d", brought);
             attroff(COLOR_PAIR(99));

             move(16, 10);
             init_pair(99, COLOR_BLACK, COLOR_WHITE);
             attrset(COLOR_PAIR(99));
             addstr("Legend:");
             move(17, 11);
             attroff(COLOR_PAIR(99));
             init_pair(33, COLOR_WHITE, COLOR_CYAN);
             attrset(COLOR_PAIR(33));
             addstr("1234");
             attroff(COLOR_PAIR(33));
             attrset(COLOR_PAIR(99));
             addstr(" - players");

             move(18, 11);
             addstr("     - wall");
             move(19, 11);
             addstr("     - bushes (slow down)");
             move(20, 11);

             addstr("     - wild beast");
             move(21, 11);

             addstr("     - one coin");
             move(22, 11);
             addstr("     - dropped treasure");
             move(23, 11);
             addstr("     - treasure (10 coins)");
             move(24, 11);

             addstr("     - large treasure (50 coins)");
             move(25, 11);

             addstr("     - campsite");
             attroff(COLOR_PAIR(99));
             printBlock('A', 25, 11, 6);
             printBlock('T', 24, 11, 4);
             printBlock('t', 23, 11, 4);
             printBlock('D', 22, 11, 8);
             printBlock('c', 21, 11, 4);
             printBlock(' ', 18, 11, 1);
             printBlock('#', 19, 11, 5);
             printBlock('*', 20, 11, 7);
             move(0,0);
         }
     }
 }


int main()
{
    int pid,number,Cx=-1,Cy=-1,round = 1,x,y,deaths,found,brought;
    int licznik =0 ;
    initscr();
    start_color();
    keypad(stdscr,true);
    noecho();
    refresh();


    int ftr,ftw,option;
    char con;
    addstr("Oczekiwanie na serwer");
    refresh();
    ftr= open("check",O_RDONLY);
    read(ftr,&pid,sizeof(int));
    close(ftr);

    if(pid<0) {
        move(0,0);
        printw("Odmowa dostepu do serwera\n");
        refresh();
        return 0;
    }
    init_pair(98,COLOR_WHITE,COLOR_WHITE);
    attrset(COLOR_PAIR(98));

    for (int w = 0; w < 90; w++) {
        mvhline(w, 0, 98, 200);
    }
    attroff(COLOR_PAIR(98));

    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) {
            printBlock(' ',i,j,1);
        }
    }
    ftr = open("toPlayer",O_RDONLY);
    ftw = open("fromPlayer",O_WRONLY);
    while(1)
    {

        read(ftr,&number,sizeof(int));
        read(ftr,&x,sizeof(int));
        read(ftr,&y,sizeof(int));
        read(ftr,&deaths,sizeof(int));
        read(ftr,&found,sizeof(int));
        read(ftr,&brought,sizeof(int));
        read(ftr,&round,sizeof(int));



        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                read(ftr,&con,sizeof(char));
                if(con == 'A' && Cx == -1)
                {
                    Cx = x+(-2+i);
                    Cy = y+(-2+j);
                }
                switch(con){
                    case 'w':printBlock(' ',i+1,j+1,1);break;
                    case ' ':printBlock(' ',i+1,j+1,2);break;
                    case '1':
                    case '2':printBlock(con,i+1,j+1,3);break;
                    case '#':printBlock(con,i+1,j+1,5);break;
                    case 'A':printBlock(con,i+1,j+1,6);break;
                    case '*':printBlock(con,i+1,j+1,7);break;
                    case 'c':
                    case 't':
                    case 'T':printBlock(con,i+1,j+1,4);break;
                    case 'D':printBlock(con,i+1,j+1,8);break;
                }
            }

        }
        printStat(pid,number,Cx,Cy,round,x,y,deaths,found,brought);
        refresh();


        while (1)
        {
         //   flushinp();
         //   timeout(1000);
            licznik = getch();
         //   timeout(-1);
          //  if(licznik == ERR) {
          //      con = ' ';
           // }
           // else {
                switch (licznik) {
                    case 259: con ='w';break;
                    case 258: con ='s';break;
                    case 260: con ='a';break;
                    case 261: con ='d';break;
                    default: con =(char)licznik;
                }
           // }
            write(ftw,&con,sizeof(char));
            if(con == 'q')
                break;
            read(ftr,&option,sizeof(int));
            if(option)
                break;
        }
        if(con == 'q')
        {
            close(ftr);
            close(ftw);
            break;
        }

        read(ftr,&number,sizeof(int));
        read(ftr,&x,sizeof(int));
        read(ftr,&y,sizeof(int));
        read(ftr,&deaths,sizeof(int));
        read(ftr,&found,sizeof(int));
        read(ftr,&brought,sizeof(int));
        read(ftr,&round,sizeof(int));



        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                read(ftr,&con,sizeof(char));
                if(con == 'A' && Cx == -1)
                {
                    Cx = x+(-2+i);
                    Cy = y+(-2+j);
                }
                switch(con){
                    case 'w':printBlock(' ',i+1,j+1,1);break;
                    case ' ':printBlock(' ',i+1,j+1,2);break;
                    case '1':
                    case '2':printBlock(con,i+1,j+1,3);break;
                    case '#':printBlock(con,i+1,j+1,5);break;
                    case 'A':printBlock(con,i+1,j+1,6);break;
                    case '*':printBlock(con,i+1,j+1,7);break;
                    case 'c':
                    case 't':
                    case 'T':printBlock(con,i+1,j+1,4);break;
                    case 'D':printBlock(con,i+1,j+1,8);break;
                }
            }

        }
        printStat(pid,number,Cx,Cy,round,x,y,deaths,found,brought);
        refresh();


    }
    init_pair(97,COLOR_BLACK,COLOR_BLACK);
    attrset(COLOR_PAIR(97));

    for (int w = 0; w < 90; w++) {
        mvhline(w, 0, 98, 200);
    }
    attroff(COLOR_PAIR(97));
    refresh();
    endwin();
    return 0;
}

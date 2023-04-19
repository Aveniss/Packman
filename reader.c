//
// Created by root on 10/12/22.
//
#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
void readMap(char* txt,char ***board)
{
    FILE *file = fopen(txt,"r");
    if(file!=NULL)
    {
        char x;
        *board = calloc(25,sizeof(char*));
        for (int i = 0; i < 25; ++i) {
            *((*board)+i) = calloc(52,sizeof(char));
            for (int j = 0; j < 51; ++j) {
                fscanf(file,"%c",(*((*board)+i)+j));
            }
            fscanf(file,"%c",&x);
        }


    }
    else
    {printf("File not found");}
}
#include <stdio.h>
#include <stdlib.h>
#define MAX_STAGE 5

char** map;
int map_rows;
int map_cols;

void readMap(int level);
void printMap();

void main(){
    readMap(1);
}

void readMap(int level){
    FILE* map_file;
    int map_level = 0;
    int row=0, col=0;
    char map;
    int temp_col1 = 0;
    int temp_col2 = 0;

    map_file = fopen("map.txt","r");
    while((map = fgetc(map_file)) != EOF){
        temp_col1++;

        if(map == 'm'){
            map_level++;
            if(map_level > level){
                map_level = level;
                break;
            }
        }
        if(map == '\n'){
            row++;
            if(temp_col1 > temp_col2){
                temp_col2 = temp_col1;
                temp_col1 = 0;
            }
        }
    }
    map_rows = row-1;
    map_cols = temp_col2;

    printf("%d\n",map_rows);
    printf("%d\n%d\n",map_cols,map_level);
}

void printMap(){
    for(int i=0; i<map_rows; i++){
       for(int j=0; j<map_cols; j++)
            printf("%c",map[i][j]);
    }
}

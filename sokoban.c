#include <stdio.h>
#include <stdlib.h>
#define MAX_STAGE 5

char** map;
int map_rows;
int map_cols;

void readMap(int level);
void printMap();

void main(){
    readMap(5);
    printf("%d %d\n",map_rows, map_cols);
    printMap();
}

void readMap(int level){
    FILE* map_file;
    int map_level = 0;
    int row=0, col=0;
    int temp_col = 0;
    char temp_char;
    int count = 0;

    // 파일을 읽어서 map배열을 동적할당할 크기를 알아냄
    map_file = fopen("map.txt","r");
    while((temp_char = fgetc(map_file)) != EOF){
        if(temp_char == 'm'|| temp_char == 'e'){
            map_level++;
        }
        if(map_level != level)
            continue;

        temp_col++;

        if(temp_char == '\n'){
            row++;
            if(temp_col > col){
                col = temp_col;
            }
            temp_col=0;
        }
    }

    map_rows = row-1;
    map_cols = col;
    fclose(map_file);

    // map 배열 동적할당하고, 적당히 초기화 한 후,  파일 내용을 읽어옴
    map = (char**)malloc(map_rows*sizeof(char*));
    for(int i=0; i<map_rows; i++)
        *(map + i) = (char*)malloc(map_cols*sizeof(char));

    for(int i=0; i<map_rows; i++)
        for(int j=0; j<map_cols; j++)
            map[i][j] = ' ';

    map_file = fopen("map.txt","r");
    map_level = 0;
    count = 0;
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++){
            map[i][j] = fgetc(map_file);
            if(map[i][j] == 'm'){
                map[i][j] = fgetc(map_file);
                map_level++;
            }
            if(map_level != level){
                i=0; j=-1;
                continue;
            }
            if(map[i][j] == 'a')
                map[i][j] = fgetc(map_file);
            if(map[i][i] == 'p')
                map[i][j] = fgetc(map_file);
            if(map[i][j] == '\n' && count ==0){
                count++;
                map[i][j] = fgetc(map_file);
            }
            if(map[i][j] == '\n'){
                for(; j<map_cols-1; j++)
                    map[i][j] = ' ';
                if(map[i][j] == ' ' || map[i][j] == '\n')
                    map[i][j] = '\n';
            }
        }
    }
    fclose(map_file);
}

void printMap(){
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++)
            putchar(map[i][j]);
    }
    printf("\n");
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termio.h>

#define MAX_STAGE 5
#define MAX_RC 30

char map[MAX_RC][MAX_RC];
int map_rows;
int map_cols;

char user_name[10];
clock_t start, end;   // 스테이지 시작 및 끝 시각
float t1,t2,t3,t4,t5; // 스테이지별 클리어 시간
int Px,Py; // 플레이어 위치
int stage = 1; // 현재 스테이지

void readMap(int level);
void printMap();
void displayHelp();
int getch();
void screen_clear();
void move(char dir);

void main(){
    char cmd;
    while(1){
        printf("input name : ");
        gets(user_name);
        user_name[10] = '\0';
        screen_clear();
        while(1){
            readMap(stage);
            start = clock();
            while(1){
                printMap();
                cmd = getch();
                switch(cmd){
                    case 'h':
                    case 'j':
                    case 'k':
                    case 'l':
                        move(cmd);
                        screen_clear();
                        break;
                    case 'd':
                        displayHelp();
                        cmd = getch();
                        screen_clear();
                        break;
                    case 'n':
                    //현재까지의 시간기록 삭제 후 첫번째 맵부터 다시시작
                        break;
                    case 'e':
                    //현재 상태 파일에 저장하고 종료
                        end = clock();
                        t1 = (float) ((end - start) /  CLOCKS_PER_SEC);
                        exit(0);
                    case 't':
                        break;
            }
        }
                    end = clock();
                    t1 = (float) ((end - start) / CLOCKS_PER_SEC);
    }
    }
}

void move(char dir){

    map[Py][Px] = ' ';

    switch(dir){
        case 'h':
            Px = Px - 1;
            break;
        case 'j':
            Py = Py + 1;
            break;
        case 'k':
            Py = Py - 1;
            break;
        case 'l':
            Px = Px + 1;
            break;
    }
}

void screen_clear(){
    system("clear");
    printf("Hello %s\n\n",user_name);
}

void displayHelp(){
    printf("h(왼쪽), j(아래), k(위), l(오른쪽)\n");
    printf("u(undo)\n");
    printf("r(replay)\n");
    printf("n(new)\n");
    printf("e(exit)\n");
    printf("s(save)\n");
    printf("f(file load)\n");
    printf("d(display help)\n");
    printf("t(top)\n");
}

void readMap(int level){
    FILE* map_file;
    int map_level = 0;
    int row=0, col=0;
    int temp_col;
    char temp_char;
    int count = 0;
    int gold_count = 0;
    int store_count = 0;

    // 파일을 읽어서 stage의 크기를 알아냄
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

    // map 배열 적당히 초기화 한 후,  파일 내용을 읽어옴

    for(int i=0; i<MAX_RC; i++)
        for(int j=0; j<MAX_RC; j++)
            map[i][j] = ' ';

    map_file = fopen("map.txt","r");
    map_level = 0;
    count = 0;
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++){
            map[i][j] = fgetc(map_file);

            if(map[i][j] == '@'){ // 초기 플레이어 위치 세팅
                Px = j;
                Py = i;
            }

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
            if(map[i][j] == '$')
                gold_count++;
            if(map[i][j] == 'O')
                store_count++;
        }
    }
    if(gold_count != store_count){
        printf("$와 0의 개수가 같지않아 종료합니다.\n");
        exit(1);
    }
    fclose(map_file);
}

void printMap(){
    map[Py][Px] = '@';
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++)
            putchar(map[i][j]);
    }
    printf("\n");
}

int getch(void){
    int ch;
    struct termios buf;
    struct termios save;
    tcgetattr(0, &save);
    buf = save;
    buf.c_lflag&=~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}
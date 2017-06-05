#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>
#include<sys/time.h>
#include <termio.h>

#define MAX_STAGE 5            //최대 맵 개수
#define MAX_RC 30              //최대 row 또는 column 개수

char map[MAX_RC][MAX_RC];      // 메모리 상에 올려둘 map 배열
int map_rows;                  // stage의 최대 행 개수
int map_cols;                  // stage의 최대 열 개수

char user_name[10];
struct timeval start, end;   // 스테이지 시작 및 끝 시각
double t[MAX_STAGE]; // 스테이지별 클리어 시간
int Px,Py; // 플레이어 위치
int stage = 1; // 현재 스테이지
int undo_x[5] = {0};
int undo_y[5] = {0};               // undo 실행을 위해 이전 위치 저장
int undo_count=0;                  // undo 실행 횟수
int slot_x[MAX_RC] = {0};
int slot_y[MAX_RC] = {0};          //slot 위치
int slot_count=0;                 //slot 개수
int gold_x[MAX_RC] = {0};
int gold_y[MAX_RC] = {0};          // gold 위치
int gold_count=0;                 // gold 개수
int stage_cleared_flag[5] = {0}; // 스테이지 클리어 표시

void readMap(int level); // 레벨에 따라 맵 로드
void printMap();         // 화면에 맵 출력
void displayHelp();      // 도움말 보여주는 함수
int getch();             // 키보드 입력을 받는 함수
void screen_clear();     // 화면을 지우고 플레이어명 띄우는 함수
void move(char dir);     // 플레이어 이동 구현 함수
int isCleared();         // 스테이지 클리어했으면 1, 못했으면 0 리턴
double time_diff(struct timeval *end, struct timeval *start); // 끝-시작 시간차 리턴

void main(){
    char cmd;
    while(1){
        printf("input name : ");
        gets(user_name);
        user_name[10] = '\0';
        screen_clear();
        while(1){
            for(int i=0; i<MAX_STAGE; i++){ // flag를 조사하여 클리어한 스테이지는 skip
                if(stage_cleared_flag[stage-1] == 1){
                    stage++;
                }
            }
            screen_clear();
            readMap(stage);
            gettimeofday(&start,NULL);
            while(1){
                printMap();
                if(isCleared()){
                    stage_cleared_flag[stage-1] = 1;
                    break;
                }
                cmd = getch();
                switch(cmd){
                    case 'h':
                    case 'j':
                    case 'k':
                    case 'l': //방향 이동 구현
                        move(cmd);
                        screen_clear();
                        break;
                    case 'd': //dispaly help
                        screen_clear();
                        displayHelp();
                        cmd = getch();
                        screen_clear();
                        break;
                    case 'n': //현재까지의 시간기록 삭제 후 첫번째 맵부터 다시시작
                        screen_clear();
                        for(int i=0; i<stage; i++)
                            t[i] = 0;
                        readMap(1);
                        printMap();
                        screen_clear();
                        break;
                    case 'e': //현재 상태 파일에 저장하고 종료
                        printf("%d\n",t[stage-1]);
                        system("clear");
                        printf("\n\n\n\nS E E   Y O U   %s . . . .\n\n\n\n",user_name);
                        exit(0);
                    case 't':
                        break;
                    case 'r': //게임시간 유지하며 현재 맵 재시작
                        screen_clear();
                        readMap(stage);
                        printMap();
                        screen_clear();
                        break;
            }
        }
                    gettimeofday(&end,NULL);
                    t[stage-1] = time_diff(&end, &start);
    }
    }
}
double time_diff(struct timeval *end, struct timeval *start){
    double diff_sec = difftime(end->tv_sec,start->tv_sec); // 초단위 시간차
    double diff_milsec = end->tv_usec - start->tv_usec;    // 나노초 단위 시간차
    diff_milsec /= (double) 1000000;               //나노초를 밀리초 단위로 변경
    diff_sec += diff_milsec;                       // 초단위 + 밀리초 단위
    return diff_sec;                             // 밀리초까지 정확한 시간차
}

int isCleared(){
    int matched_count = 0;

    for(int i=0; i<slot_count; i++){
        for(int j=0; j<gold_count; j++){
            if(slot_x[i] == gold_x[j] && slot_y[i] == gold_y[j])
                matched_count++;  // slot과 gold 위치가 같다면 matched_count 증가
        }
    }

    if(matched_count == slot_count) // 서로 위치가 같은 개수가 slot개수와 일치하면
        return 1; // 참
    return 0;     // 거짓
}

void move(char dir){
    map[Py][Px] = ' '; // 다음 이동을 위해 현재 @를 화면에서 지움
    switch(dir){
        case 'h':
            Px = Px - 1; // @ 이동 구현
            if(map[Py][Px] == '#') //벽을 만나면 이동 취소
                Px = Px + 1;
            if(map[Py][Px] == '$'){ //이동한 곳에 $가 있다면
                for(int i=0; i<gold_count; i++){
                    if((gold_x[i] == Px) && (gold_y[i] == Py)){ // 해당 $의 index를 찾아내서
                        gold_x[i] = gold_x[i] - 1;           // @와 동일한 방향으로 이동
                        if((map[gold_y[i]][gold_x[i]] == '#') || //$가 움직인 방향에 #이나
                           (map[gold_y[i]][gold_x[i]] == '$')){ // $가 있다면
                            gold_x[i] = gold_x[i] + 1;          // $ 및 @ 이동 취소
                            Px = Px + 1;
                        }
                    }
                }
            }
            break;
        case 'j':
            Py = Py + 1;
            if(map[Py][Px] == '#')
                Py = Py - 1;
            if(map[Py][Px] == '$'){
                for(int i=0; i<gold_count; i++){
                    if((gold_x[i] == Px) && (gold_y[i] == Py)){
                        gold_y[i] = gold_y[i] + 1;
                        if((map[gold_y[i]][gold_x[i]] == '#') ||
                           (map[gold_y[i]][gold_x[i]] == '$')){
                            gold_y[i] = gold_y[i] - 1;
                            Py = Py - 1;
                        }
                    }
                }
            }
            break;
        case 'k':
            Py = Py - 1;
            if(map[Py][Px] == '#')
                Py = Py + 1;
             if(map[Py][Px] == '$'){
                for(int i=0; i<gold_count; i++){
                    if((gold_x[i] == Px) && (gold_y[i] == Py)){
                        gold_y[i] = gold_y[i] - 1;
                        if((map[gold_y[i]][gold_x[i]] == '#') ||
                           (map[gold_y[i]][gold_x[i]] == '$')){
                            gold_y[i] = gold_y[i] + 1;
                            Py = Py + 1;
                        }
                    }
                }
            }
            break;
        case 'l':
            Px = Px + 1;
            if(map[Py][Px] == '#')
                Px = Px - 1;
            if(map[Py][Px] == '$'){
                for(int i=0; i<gold_count; i++){
                    if((gold_x[i] == Px) && (gold_y[i] == Py)){
                        gold_x[i] = gold_x[i] + 1;
                        if((map[gold_y[i]][gold_x[i]] == '#') ||
                           (map[gold_y[i]][gold_x[i]] == '$')){
                            gold_x[i] = gold_x[i] - 1;
                            Px = Px - 1;
                        }
                    }
                }
            }
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
    slot_count = 0;
    gold_count = 0;
    undo_count = 0;

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
            if(map[i][j] == 'O'){ // 초기 slot 위치 및 개수 알아내기
                slot_x[slot_count] = j;
                slot_y[slot_count] = i;
                slot_count++;
            }
            if(map[i][j] == '$'){ // 초기 gold 위치 및 개수 알아내기
                gold_x[gold_count] = j;
                gold_y[gold_count] = i;
                gold_count++;
            }


            if(map[i][j] == 'm'){
                map[i][j] = fgetc(map_file);
                map_level++;
            }
            if(map_level != level){
                i=0; j=-1;
                slot_count = 0;
                gold_count = 0;
                continue;
            }
            if(map[i][j] == 'a')
                map[i][j] = fgetc(map_file);
            if(map[i][i] == 'p')
                map[i][j] = fgetc(map_file);
            if(map[i][j] == '\n' && count ==0){ //상단에 map글자 안읽어오도록
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
    if(gold_count != slot_count){
        printf("$와 0의 개수가 같지않아 종료합니다.\n");
        exit(1);
    }
    fclose(map_file);
}

void printMap(){
    map[Py][Px] = '@'; // 플레이어의 이동이 현재 구현된 위치에 @ 출력

    int row, col;
    for(int k=0; k<slot_count; k++){ // @의 이동으로 인해 slot 위치가 공백이 되지않도록
        if(map[slot_y[k]][slot_x[k]] == ' ')
            map[slot_y[k]][slot_x[k]] = 'O';
    }

// @의 이동으로 인해 gold 위치가 공백이 되지않고 slot위에 gold가 올라가게 보일 수 있도록
    for(int k=0; k<gold_count; k++){
        if(map[gold_y[k]][gold_x[k]] == ' '|| map[gold_y[k]][gold_x[k]] == 'O')
            map[gold_y[k]][gold_x[k]] = '$';
    }
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++){
            putchar(map[i][j]);       // 현재 상태의 map 전체 출력
        }
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
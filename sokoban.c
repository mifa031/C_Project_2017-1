#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>
#include<sys/time.h>
#include <termio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_STAGE 5            //최대 맵 개수
#define MAX_RC 30              //최대 row 또는 column 개수

char map[MAX_RC][MAX_RC];      // 메모리 상에 올려둘 map 배열
int map_rows;                  // stage의 최대 행 개수
int map_cols;                  // stage의 최대 열 개수

char user_name[10];
struct timeval start, end;   // 스테이지 시작 및 끝 시각
double t[MAX_STAGE];         // 스테이지별 클리어 시간
int Px,Py;                // 플레이어 위치
int stage = 1;             // 현재 스테이지

int undo_point = -1;        //플레이 상태를 담은 undo배열들 중 꺼내올 index위치
int input_index = 0;       // undo배열들 중 데이터를 담을 index
int undo_Px[5] = {0};
int undo_Py[5] = {0};       // undo를 위한 플레이어 위치 저장
int undo_gold_x[5] = {-1};
int undo_gold_y[5] = {-1};
int undo_gold_index[5] = {-1};  //undo를 위한 gold 위치 저장
int undo_count = 5;        // 남은 undo 실행 횟수

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
int isCleared();         // 스테이지 클리어했으면 TRUE, 못했으면 FALSE 리턴
double time_diff(struct timeval *end, struct timeval *start); // 끝-시작 시간차 리턴
void undo();
void undo_record(int iS_gold_moved, int current_gold_index);
void load_rank(int level);
void save_rank(int level);
void save_game();
void load_game();

void main(){
    char cmd;
    int cmd_typed = FALSE;
    while(1){
        printf("input name : ");
        gets(user_name);
        user_name[10] = '\0';
        screen_clear();
        while(1){
                if(stage_cleared_flag[stage-1] == 1){
                    stage++;
                }
                if(stage > MAX_STAGE){
                    system("clear");
                    printf("\nCongratulations!!!\nAll STAGE CLEARED !!!\n");
                    exit(0);
                }
stage_start:
            screen_clear();
            readMap(stage);
load_point:
            gettimeofday(&start,NULL);
            while(1){
                if(!cmd_typed)
                    printMap();
                if(isCleared()){ // 스테이지 클리어 여부 확인
                    stage_cleared_flag[stage-1] = 1;
                    break;
                }
                if(!cmd_typed)
                    cmd = getch();
                cmd_typed = FALSE;
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
                        cmd_typed = TRUE;
                        screen_clear();
                        break;
                    case 'n': //현재까지의 시간기록 삭제 후 첫번째 맵부터 다시시작
                        screen_clear();
                        for(int i=0; i<stage; i++)
                            t[i] = 0;
                        stage = 1;
                        goto stage_start;
                    case 'e': //현재 상태 파일에 저장하고 종료
                        printf("%d\n",t[stage-1]);
                        system("clear");
                        printf("\n\n\n\nS E E   Y O U   %s . . . .\n\n\n\n",user_name);
                        exit(0);
                        break;
                    case 't':
                        cmd = getch();
                        int cmd_int = cmd - 48;
                        load_rank(cmd_int);
                        cmd = getch();
                        cmd_typed = TRUE;
                        screen_clear();
                        break;
                    case 'r': //게임시간 유지하며 현재 맵 재시작
                        screen_clear();
                        readMap(stage);
                        printMap();
                        screen_clear();
                        break;
                    case 'u':
                        undo();
                        screen_clear();
                        break;
                    case 's':
                        gettimeofday(&end,NULL);
                        t[stage-1] = time_diff(&end, &start);
                        save_game();
                        break;
                    case 'f':
                        load_game();
                        goto load_point;
            }
        }
                    gettimeofday(&end,NULL);
                    t[stage-1] += time_diff(&end, &start);
                    save_rank(stage);
    }
    }
}

void load_game(){
    FILE* file;
    file = fopen("sokoban.txt","r");

    fscanf(file,"%d\n",&stage);
    readMap(stage);          // 파일에 저장된 스테이지 블러옴
    map[Py][Px] = ' ';       // 추후 @및 $의 위치 변경을 위해 초기위치 공백처리
    for(int i=0; i<gold_count; i++){
        map[gold_y[i]][gold_x[i]] = ' ';
    }

    fscanf(file,"%s\n",&user_name);
    for(int i=0; i<MAX_STAGE; i++)
        fscanf(file,"%lf\n",&t[i]);
    fscanf(file,"%d\n",&Px);
    fscanf(file,"%d\n",&Py);
    fscanf(file,"%d\n",&undo_point);
    fscanf(file,"%d\n",&input_index);
    for(int i=0; i<5; i++)
        fscanf(file,"%d\n",&undo_Px[i]);
    for(int i=0; i<5; i++)
        fscanf(file,"%d\n",&undo_Py[i]);
    for(int i=0; i<5; i++)
        fscanf(file,"%d\n",&undo_gold_x[i]);
    for(int i=0; i<5; i++)
        fscanf(file,"%d\n",&undo_gold_y[i]);
    for(int i=0; i<5; i++)
        fscanf(file,"%d\n",&undo_gold_index[i]);
    fscanf(file,"%d\n",&undo_count);
    fscanf(file,"%d\n",&slot_count);
    fscanf(file,"%d\n",&gold_count);
    for(int i=0; i<slot_count; i++)
        fscanf(file,"%d\n",&slot_x[i]);
    for(int i=0; i<slot_count; i++)
        fscanf(file,"%d\n",&slot_y[i]);
    for(int i=0; i<gold_count; i++)
        fscanf(file,"%d\n",&gold_x[i]);
    for(int i=0; i<gold_count; i++)
        fscanf(file,"%d\n",&gold_y[i]);
    for(int i=0; i<MAX_STAGE; i++)
        fscanf(file,"%d\n",&stage_cleared_flag[i]);

    screen_clear();
    fclose(file);
}

void save_game(){
    FILE* file;
    file = fopen("sokoban.txt","w");

    fprintf(file,"%d\n",stage);
    fprintf(file,"%s\n",user_name);
    for(int i=0; i<MAX_STAGE; i++)
        fprintf(file,"%lf\n",t[i]);
    fprintf(file,"%d\n",Px);
    fprintf(file,"%d\n",Py);
    fprintf(file,"%d\n",undo_point);
    fprintf(file,"%d\n",input_index);
    for(int i=0; i<5; i++)
        fprintf(file,"%d\n",undo_Px[i]);
    for(int i=0; i<5; i++)
        fprintf(file,"%d\n",undo_Py[i]);
    for(int i=0; i<5; i++)
        fprintf(file,"%d\n",undo_gold_x[i]);
    for(int i=0; i<5; i++)
        fprintf(file,"%d\n",undo_gold_y[i]);
    for(int i=0; i<5; i++)
        fprintf(file,"%d\n",undo_gold_index[i]);
    fprintf(file,"%d\n",undo_count);
    fprintf(file,"%d\n",slot_count);
    fprintf(file,"%d\n",gold_count);
    for(int i=0; i<slot_count; i++)
        fprintf(file,"%d\n",slot_x[i]);
    for(int i=0; i<slot_count; i++)
        fprintf(file,"%d\n",slot_y[i]);
    for(int i=0; i<gold_count; i++)
        fprintf(file,"%d\n",gold_x[i]);
    for(int i=0; i<gold_count; i++)
        fprintf(file,"%d\n",gold_y[i]);
    for(int i=0; i<MAX_STAGE; i++)
        fprintf(file,"%d\n",stage_cleared_flag[i]);

    screen_clear();
    fclose(file);
}

void load_rank(int level){ // stage 0은 전체 순위
    FILE* file;
    char temp_rank[50][50]; // 랭킹 파일 데이터를 임시로 저장할 변수
    char name[5][10];
    float time[5];
    char current_map[6];
    int user_num = 0;
    char temp_str[50];
    int is_no_user_map[50];

    for(int i=0; i<50; i++){
        is_no_user_map[i] = -1;
    }

    char* temp_char;
    if((file = fopen("ranking.txt","r")) == NULL){
        screen_clear();
        printf("저장된 랭킹 데이터가 없습니다.\n");
        sleep(3);
    }else if(level == 0){
        int i = 0;
        while(!feof(file)){
            fgets(temp_rank[i],sizeof(temp_rank[i]),file);
            if(feof(file))
                break;
            for(char ascii=49; ascii<=53; ascii++){
                if((temp_rank[i][4] == ascii) && (temp_rank[i][5] == '\n')){
                    i++;
                    temp_char = fgets(temp_rank[i],sizeof(temp_rank[i]),file);
                    if(((temp_rank[i][4] == (ascii+1)) && (temp_rank[i][5] == '\n')||
                                temp_char == NULL)){
                        is_no_user_map[i-1] = 1;
                        break;
                    }else{
                        break;
                    }
                }
            }
            i++;
        }
        screen_clear();
        for(int a=0; a<i; a++){
            if(is_no_user_map[a] < 0)
                printf("%s",temp_rank[a]);
        }
    }else{
        int i = 0;
        char map_level = level+48;
        while(!feof(file)){
            fgets(temp_rank[i],sizeof(temp_rank[i]),file);
            if(temp_rank[i][4] == map_level && temp_rank[i][5] == '\n'){
                strcpy(current_map,temp_rank[i]);
                int j = 0;
                while(!feof(file)){
                    fscanf(file,"%s %f%s\n",&name[j],&time[j],&temp_char);
                    user_num++;
                    j++;
                    i++;
                    if(strcmp(temp_char,"sec") != 0){
                        break;
                    }
                }
                break;
            }
            i++;

        }
        printf("%s",current_map);
        for(int a=0; a<user_num-1; a++)
            printf("%s %.1fsec\n",name[a],time[a]);
    }
}
void save_rank(int level){
    FILE* file;
    FILE* file2;
    char temp_rank[50][50]; // 랭킹 파일 데이터를 임시로 저장할 변수
    char name[6][10];
    float time[6];
    char ch_level = level + 48;
    char map_exp[6] = "map  \n";
    map_exp[3] = ' ';
    map_exp[4] = ch_level;
    char temp_char[4];
    int user_num = 0;

    if((file = fopen("ranking.txt","r+")) == NULL){
        file = fopen("ranking.txt","a+");
    }

    char tmp_str[50];
    int map_exp_check[MAX_STAGE] = {0};
    char tmp_map_exp[6] = "map  \n";
    while(!feof(file)){ // map 1 같은 맵 표시가 있는지 체크, 없으면 0
        fgets(tmp_str,sizeof(tmp_str),file);
        for(int a=0; a<MAX_STAGE; a++){
            tmp_map_exp[4] = a+49;
            if(strcmp(tmp_str,tmp_map_exp) == 0){
                map_exp_check[a] = a;
            }
        }
    }
    fclose(file);

    file = fopen("ranking.txt","a"); // 맵 표시가 없으면 추가
    for(int a=0; a<MAX_STAGE; a++){
            if(map_exp_check[a] == 0){
                tmp_map_exp[4] = a+49;
                fputs(tmp_map_exp,file);
            }
        }
    fclose(file);


    if((file = fopen("ranking.txt","r+")) == NULL){
        file = fopen("ranking.txt","a+");
    }


    // 레벨에 맞는 플레이어 데이터를 불러온다.
    int i = 0;
    while(!feof(file)){
        fgets(temp_rank[i],sizeof(temp_rank[i]),file);

        if(temp_rank[i][4] == map_exp[4] && temp_rank[i][5] == '\n'){
            int j = 0;

            while(!feof(file)){
                fscanf(file,"%s %f%s\n",&name[j],&time[j],&temp_char);
                j++;
                i++;
                if(strcmp(temp_char,"sec") != 0){
                    user_num = j-1;
                    break;
                }
            }
            break;
        }
        i++;
    }

    // 현재 플레이어의 데이터를 추가 한다.
    user_num++;
    strcpy(name[user_num-1],user_name);
    time[user_num-1] = t[stage-1];


    // 플레이어 데이터를 시간 기준 오름차순으로 정렬
    char temp_name[10];
    float temp_time;
    for(int i=0; i<user_num; i++){
        for(int j=0; j<user_num; j++){
            if(time[i] < time[j]){
                strcpy(temp_name,name[j]);
                temp_time = time[j];
                strcpy(name[j],name[i]);
                time[j] = time[i];
                strcpy(name[i],temp_name);
                time[i] = temp_time;
            }
        }
    }
    if(user_num > 5)
        user_num = 5; // 최대 5명까지만 보기위한 세팅

    fclose(file);
    // 바뀐 랭킹 데이터를 다시 쓴다. (ranking2에 임시저장)
    file = fopen("ranking.txt","r");
    file2 = fopen("ranking2.txt","w");
    i = 0;
    while(!feof(file)){
        fgets(temp_rank[i],sizeof(temp_rank[i]),file);
        if(feof(file))
            break;
        if(temp_rank[i][4] == map_exp[4] && temp_rank[i][5] == '\n'){
            fputs(temp_rank[i],file2);
            i++;
            for(int j=0; j<user_num; j++){
                fprintf(file2,"%s %.1fsec\n",name[j],time[j]);
                fgets(temp_rank[i],sizeof(temp_rank[i]),file);
            }
            break;
        }
            fputs(temp_rank[i],file2);
            i++;
    }

    for(int i=0; i<50; i++){
        for(int j=0; j<50; j++){
            temp_rank[i][j] = '\0';
        }
    }
    fclose(file);
    fclose(file2);

    file = fopen("ranking.txt","r");
    file2 = fopen("ranking2.txt","a");
    i = 0;
    int j = 1;
    while(!feof(file)){
        fgets(temp_rank[i],sizeof(temp_rank[i]),file);
        if(feof(file))
            break;
        if(stage == 5)
            j = 0;
        if(temp_rank[i][4] == (map_exp[4]+j) && temp_rank[i][5] == '\n'){
            fputs(temp_rank[i],file2);
            i++;
            while(!feof(file)){
                 fgets(temp_rank[i],sizeof(temp_rank[i]),file);
                 if(temp_rank[i][4] == '1' && temp_rank[i][5] == '\n')
                     break;
                 fputs(temp_rank[i],file2);
                 i++;
            }
            break;
        }
    }

    for(int i=0; i<50; i++){
        for(int j=0; j<50; j++){
            temp_rank[i][j] = '\0';
        }
    }
    fclose(file);
    fclose(file2);



    //ranking2의 내용을 ranking파일로 복사한뒤 ranking2 삭제하고 종료
    file = fopen("ranking.txt","w");
    file2 = fopen("ranking2.txt","r");
    while(!feof(file2)){
        fgets(temp_rank[i],sizeof(temp_rank),file2);
        if(feof(file2))
            break;
        fputs(temp_rank[i],file);
        i++;
    }
    fclose(file);
    fclose(file2);
    remove("ranking2.txt");
}

void undo_record(int is_gold_moved, int current_gold_index){
    // 1번째 undo 기록
    if(input_index < undo_count && !is_gold_moved){
        undo_Px[input_index] = Px;
        undo_Py[input_index] = Py;

        undo_gold_x[input_index] = -1;
        undo_gold_y[input_index] = -1;
        undo_gold_index[input_index] = -1;

        undo_point = input_index;
    }else if(input_index >= undo_count && !is_gold_moved){
        for(int i=0; i<undo_count-1; i++){
            undo_Px[i] = undo_Px[i+1];
            undo_Py[i] = undo_Py[i+1];
        }
        undo_Px[undo_count-1] = Px;
        undo_Py[undo_count-1] = Py;

        for(int i=0; i<undo_count-1; i++){
            undo_gold_x[i] = undo_gold_x[i+1];
            undo_gold_y[i] = undo_gold_y[i+1];
            undo_gold_index[i] = undo_gold_index[i+1];
        }
        undo_gold_x[undo_count-1] = -1;
        undo_gold_y[undo_count-1] = -1;
        undo_gold_index[undo_count-1] = -1;

        undo_point = undo_count-1;
    }

    // 만약 gold의 움직임이 있다면 2번째 undo 기록 수행
    if(input_index < undo_count && is_gold_moved){
        undo_gold_x[input_index] = gold_x[current_gold_index];
        undo_gold_y[input_index] = gold_y[current_gold_index];
        undo_gold_index[input_index] = current_gold_index;
    }else if(input_index >= undo_count && is_gold_moved){
        undo_gold_x[undo_count-1] = gold_x[current_gold_index];
        undo_gold_y[undo_count-1] = gold_y[current_gold_index];
        undo_gold_index[undo_count-1] = current_gold_index;
    }

    if(input_index < undo_count)
        input_index++;
}
void undo(){
    if((undo_count > 0) && (undo_point >= 0)){      // undo_count가 남아 있다면
        map[Py][Px] = ' ';
        Px = undo_Px[undo_point];
        Py = undo_Py[undo_point]; // 플레이어 위치 undo
        if(undo_gold_index[undo_point] >= 0){
            map[gold_y[undo_gold_index[undo_point]]][gold_x[undo_gold_index[undo_point]]] = ' ';
            gold_x[undo_gold_index[undo_point]] = undo_gold_x[undo_point];
            gold_y[undo_gold_index[undo_point]] = undo_gold_y[undo_point]; //gold 위치 undo
        }
        undo_point--;
        undo_count--;
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
        return TRUE;
    return FALSE;
}
void move(char dir){
    undo_record(FALSE,FALSE);
    map[Py][Px] = ' '; // 다음 이동을 위해 현재 @를 화면에서 지움
    switch(dir){
        case 'h':
            Px = Px - 1; // @ 이동 구현
            if(map[Py][Px] == '#') //벽을 만나면 이동 취소
                Px = Px + 1;
            if(map[Py][Px] == '$'){ //이동한 곳에 $가 있다면
                for(int i=0; i<gold_count; i++){
                    if((gold_x[i] == Px) && (gold_y[i] == Py)){ // 해당 $의 index를 찾아내서
                        undo_record(TRUE,i);
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
                        undo_record(TRUE,i);
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
                        undo_record(TRUE,i);
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
                        undo_record(TRUE,i);
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
    undo_count = 5;

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

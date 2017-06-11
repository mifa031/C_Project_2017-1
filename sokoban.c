#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <termio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_STAGE 5            //�ִ� �� ����
#define MAX_RC 30              //�ִ� row �Ǵ� column ����

char map[MAX_RC][MAX_RC];      // �޸� �� �÷��� map �迭
int map_rows;                  // stage�� �ִ� �� ����
int map_cols;                  // stage�� �ִ� �� ����

char user_name[10];
struct timeval start, end;   // �������� ���� �� �� �ð�
double t[MAX_STAGE];         // ���������� Ŭ���� �ð�
int Px,Py;                // �÷��̾� ��ġ
int stage = 1;             // ���� ��������

int undo_point = -1;        //�÷��� ���¸� ���� undo�迭�� �� ������ index��ġ
int input_index = 0;       // undo�迭�鿡 �����͸� ���� index
int undo_Px[5] = {0};
int undo_Py[5] = {0};       // undo�� ���� �÷��̾� ��ġ ����
int undo_gold_x[5] = {-1};
int undo_gold_y[5] = {-1};
int undo_gold_index[5] = {-1};  //undo�� ���� gold ��ġ ����
int undo_count = 5;        // ���� undo ���� Ƚ��

int slot_x[MAX_RC] = {0};
int slot_y[MAX_RC] = {0};          //slot ��ġ
int slot_count=0;                 //slot ����
int gold_x[MAX_RC] = {0};
int gold_y[MAX_RC] = {0};          // gold ��ġ
int gold_count=0;                 // gold ����

int stage_cleared_flag[5] = {0}; // �������� Ŭ���� ǥ��

void readMap(int level); // ������ ���� �� �ε�
void printMap();         // ȭ�鿡 �� ���
void displayHelp();      // ���� �����ִ� �Լ�
int getch();             // Ű���� �Է��� �޴� �Լ�
void screen_clear();     // ȭ���� ����� �÷��̾�� ���� �Լ�
void move(char dir);     // �÷��̾� �̵� ���� �Լ�
int isCleared();         // �������� Ŭ���������� TRUE, �������� FALSE ����
double time_diff(struct timeval *end, struct timeval *start); // ��-���� �ð��� ����
void undo();             // �÷��̾� �̵� undo
void undo_record(int iS_gold_moved, int current_gold_index); // undo�ϱ����� �ʿ��� ��ġ���� ���
void load_rank(int level); // ������ ���� ��ŷ �ε�
void save_rank(int level); // ������ ���� ��ŷ ����
void save_game();          // ���� ����
void load_game();          // ���� �ε�

void main(){
    char cmd;       // getch()�� ���� 1���� �޾ƿö� ���� ����
    char cmd_s[4];  // gets()�� ���ڿ� �޾ƿö� ���� ����
    char cmd_t[4];  // gets()�� ���ڿ� �޾ƿö� �ӽ÷� ���� ����
    int cmd_int = 0;  // t��ɿ��� ���ڰ� �����Ҷ� ���� ����
    while(1){
        printf("input name : ");
        gets(user_name);
        user_name[10] = '\0';
        screen_clear();
        while(1){
            for(int i=0; i<MAX_STAGE; i++){ // flag�� �����Ͽ� Ŭ������ ���������� skip
                if(stage_cleared_flag[stage-1] == 1){
                    stage++;
                }
                if(stage > MAX_STAGE){ // ������ ������������ Ŭ�����ߴٸ�,
                    system("clear");   // ���ϸ޽��� ��� �� ����
                    printf("\nCongratulations!!!\nAll STAGE CLEARED !!!\n");
                    sleep(10);
                    exit(0);
                }
            }
stage_start:
            screen_clear();
            readMap(stage); // stage�� �°� ���� �о�´�.
load_point:
            gettimeofday(&start,NULL);  // �ð� ���� ����
            if(cmd != 'f')
                cmd = ' ';
            while(1){
                if(cmd == ' ' || cmd == 'h' || cmd == 'j' || cmd == 'k' || cmd == 'l' || cmd =='r' || cmd=='u'||cmd=='s'||cmd=='f'){
                    // ȭ�鿡 ���� ��µǾ� �������� ����Էºκ�
                    screen_clear();
                    printMap();
                    if(cmd == 'r' || cmd == 'u' || cmd == 's'||cmd == 'f'){
                        printf("\n(Command) %c",cmd);
                    }else{
                        printf("\n(Command) ");
                    }
                    cmd = getch();
                    if(cmd == 't'){
                        screen_clear();
                        printMap();
                        printf("\n(Command) %c",cmd);
                        cmd_s[0] = cmd;
                        gets(cmd_t);
                        cmd_s[1] = cmd_t[0];
                        cmd_s[2] = cmd_t[1];
                        cmd_s[3] = cmd_t[2];
                        cmd = cmd_s[0];
                    }
                }else{ // ȭ�鿡 ���� ��µ��� ���� ��� ����Էºκ�
                    printf("\n(Command) ");
                    gets(cmd_s);
                    cmd = cmd_s[0];
                }

                if(isCleared()){ // �������� Ŭ���� ���� Ȯ��
                    stage_cleared_flag[stage-1] = 1;
                    break;   //Ŭ���� �� flag�� 1�� set�ϰ� break
                }

                switch(cmd){
                    case ' ':
                    case '\n':
                    case 'h':
                    case 'j':
                    case 'k':
                    case 'l': //���� �̵� ����
                        move(cmd);
                        break;
                    case 'd': //dispaly help
                        screen_clear();
                        displayHelp();
                        break;
                    case 'n': //��������� �ð���� ���� �� ù��° �ʺ��� �ٽý���
                        system("clear");
                        for(int i=0; i<stage; i++)
                            t[i] = 0;
                        stage = 1;
                        goto stage_start;
                    case 'e': //���� ���� ���Ͽ� �����ϰ� ����
                        system("clear");
                        printf("\n\n\n\nS E E   Y O U   %s . . . .\n\n\n\n",user_name);
                        printf("(Command) e\n");
                        gettimeofday(&end,NULL);
                        t[stage-1] += time_diff(&end, &start);
                        save_game();
                        sleep(3);
                        exit(0);
                        break;
                    case 't': // �ʺ� ���� ���
                        screen_clear();
                        cmd_int = 0;
                        if(cmd_s[1]-48 > 0 && cmd_s[1]-48 < 6){
                            switch(cmd_s[1]){
                                case '1':
                                    cmd_int = 1;
                                    break;
                                case '2':
                                    cmd_int = 2;
                                    break;
                                case '3':
                                    cmd_int = 3;
                                    break;
                                case '4':
                                    cmd_int = 4;
                                    break;
                                case '5':
                                    cmd_int = 5;
                                    break;
                                default:
                                    cmd_int = 0;
                            }
                        }else if(cmd_s[2]-48 > 0 && cmd_s[2]-48 < 6){
                            switch(cmd_s[2]){
                                case '1':
                                    cmd_int = 1;
                                    break;
                                case '2':
                                    cmd_int = 2;
                                    break;
                                case '3':
                                    cmd_int = 3;
                                    break;
                                case '4':
                                    cmd_int = 4;
                                    break;
                                case '5':
                                    cmd_int = 5;
                                    break;
                                default:
                                    cmd_int = 0;

                        }
                        }
                        load_rank(cmd_int);
                        cmd_int = 0;
                        for(int i=0; i<4; i++)
                            cmd_s[i] = '\0';
                        break;
                    case 'r': //���ӽð� �����ϸ� ���� �� �����
                        screen_clear();
                        readMap(stage);
                        break;
                    case 'u': // ����� �̵� undo
                        undo();
                        break;
                    case 's': // ���� ����
                        gettimeofday(&end,NULL);
                        t[stage-1] += time_diff(&end, &start);
                        save_game();
                        break;
                    case 'f': // ���� �ε�
                        load_game();
                        goto load_point;
                }
            }
            // stage Ŭ���� �� ó�� �����ϴ� �κ�
                    gettimeofday(&end,NULL);
                    t[stage-1] += time_diff(&end, &start);
                    save_rank(stage);
                    //�ɸ� �ð��� �����ϰ�, ��ŷ ���
        }
    }
}

void load_game(){
    FILE* file;
    file = fopen("sokoban.txt","r");

    fscanf(file,"%d\n",&stage);
    readMap(stage);          // ���Ͽ� ����� �������� ����
    map[Py][Px] = ' ';       // ���� @�� $�� ��ġ ������ ���� �ʱ���ġ ����ó��
    for(int i=0; i<gold_count; i++){
        map[gold_y[i]][gold_x[i]] = ' ';
    }

    //���Ͽ� ����� ���������� �ε�
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

    fclose(file);
}

void save_game(){
    FILE* file;
    file = fopen("sokoban.txt","w");

    // ���� save�� �ʿ��� ������������ ���Ͽ� ����
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

    fclose(file);
}

void load_rank(int level){ // level 0�� ��ü ����, 1~5�� �ʺ� ���� ���
    FILE* file;
    char temp_rank[50][50];
    char name[5][10];
    float time[5];
    char current_map[6];
    int user_num = 0;
    char temp_str[50];     // ���ϳ� �����͵��� �ӽ÷� �����ϴ� ������
    int is_no_user_map[50]; // ��ŷ ����ڰ� ���� map�� skip�ϴµ� ���

    for(int i=0; i<50; i++){
        is_no_user_map[i] = -1;
    }

    char* temp_char;
    if((file = fopen("ranking.txt","r")) == NULL){
        screen_clear();
        printf("����� ��ŷ �����Ͱ� �����ϴ�.\n");
        sleep(3);
    }else if(level == 0){ // ��ü �� ��ŷ ���
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
                        is_no_user_map[i] = 1;
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
    }else if(level > 0 && level < 6){ // �ʺ� ��ŷ ���
        int i = 0;
        int map_level = level+48;
        char temp_char2[3];
        while(!feof(file)){
            fgets(temp_rank[i],sizeof(temp_rank[i]),file);
            if(temp_rank[i][4] == map_level && temp_rank[i][5] == '\n'){
                strcpy(current_map,temp_rank[i]);
                int j = 0;
                while(!feof(file)){
                    fscanf(file,"%s %f%s\n",&name[j],&time[j],&temp_char2);
                    user_num++;
                    j++;
                    i++;
                    if(strcmp(temp_char2,"sec") != 0){
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
    char temp_rank[50][50];
    char name[6][10];
    float time[6];
    char ch_level = level + 48;
    char map_exp[7] = "map  \n";
    map_exp[3] = ' ';
    map_exp[4] = ch_level;
    char temp_char[4];
    int user_num = 0; // ��ŷ���� �����͸� �ӽ÷� �����ϴ� ������

    if((file = fopen("ranking.txt","r+")) == NULL){
        file = fopen("ranking.txt","a+");
    }

    char tmp_str[50];
    int map_exp_check[MAX_STAGE] = {0};
    char tmp_map_exp[7] = "map  \n";
    while(!feof(file)){ // 'map 1' ���� �� ǥ�ð� �ִ��� üũ, ������ 0
        fgets(tmp_str,sizeof(tmp_str),file); //(���� ��ǥ�ø� �����Ͽ� ��ŷ����)
        for(int a=0; a<MAX_STAGE; a++){
            tmp_map_exp[4] = a+49;
            if(strcmp(tmp_str,tmp_map_exp) == 0){
                map_exp_check[a] = a;
            }
        }
    }
    fclose(file);

    file = fopen("ranking.txt","a"); // �� ǥ�ð� ������ �߰�
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

    // ������ �´� �÷��̾� �����͸� �ҷ��´�.
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

    // ���� �÷��̾��� �����͸� �߰� �Ѵ�.
    user_num++;
    strcpy(name[user_num-1],user_name);
    time[user_num-1] = t[stage-1];


    // �÷��̾� �����͸� �ð� ���� ������������ ����
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
        user_num = 5; // �ִ� 5������� �������� ����

    fclose(file);

    // �ٲ� ��ŷ �����͸� �ٽ� ����. (ranking2�� �ӽ�����)
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



    //ranking2�� ������ ranking���Ϸ� �����ѵ� ranking2 �����ϰ� ����
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
    // 1��° undo ��� (����ڸ� �������� ��� 1��° undo�� ���)
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

    // ���� gold�� �������� �ִٸ� 2��° undo ��� ����
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
    if((undo_count > 0) && (undo_point >= 0)){      // undo_count�� ���� �ִٸ�
        map[Py][Px] = ' ';
        Px = undo_Px[undo_point];
        Py = undo_Py[undo_point]; // �÷��̾� ��ġ undo
        if(undo_gold_index[undo_point] >= 0){
            map[gold_y[undo_gold_index[undo_point]]][gold_x[undo_gold_index[undo_point]]] = ' ';
            gold_x[undo_gold_index[undo_point]] = undo_gold_x[undo_point];
            gold_y[undo_gold_index[undo_point]] = undo_gold_y[undo_point]; //gold ��ġ undo
        }
        undo_point--;
        undo_count--;
    }
}

double time_diff(struct timeval *end, struct timeval *start){
    double diff_sec = difftime(end->tv_sec,start->tv_sec); // �ʴ��� �ð���
    double diff_milsec = end->tv_usec - start->tv_usec;    // ������ ���� �ð���
    diff_milsec /= (double) 1000000;               //�����ʸ� �и��� ������ ����
    diff_sec += diff_milsec;                       // �ʴ��� + �и��� ����
    return diff_sec;                             // �и��ʱ��� ��Ȯ�� �ð���
}

int isCleared(){
    int matched_count = 0;

    for(int i=0; i<slot_count; i++){
        for(int j=0; j<gold_count; j++){
            if(slot_x[i] == gold_x[j] && slot_y[i] == gold_y[j])
                matched_count++;  // slot�� gold ��ġ�� ���ٸ� matched_count ����
        }
    }

    if(matched_count == slot_count) // ���� ��ġ�� ���� ������ slot������ ��ġ�ϸ�
        return TRUE;
    return FALSE;
}

void move(char dir){
    if(dir == ' '){
    }else{
    undo_record(FALSE,FALSE);
    map[Py][Px] = ' '; // ���� �̵��� ���� ���� @�� ȭ�鿡�� ����
    switch(dir){
        case 'h':
            Px = Px - 1; // @ �̵� ����
            if(map[Py][Px] == '#') //���� ������ @ �̵� ���
                Px = Px + 1;
            if(map[Py][Px] == '$'){ //@�� �̵��� ���� $�� �ִٸ�
                for(int i=0; i<gold_count; i++){
                    if((gold_x[i] == Px) && (gold_y[i] == Py)){ // �ش� $�� index�� ã�Ƴ���
                        undo_record(TRUE,i);
                        gold_x[i] = gold_x[i] - 1;           // @�� ������ �������� �̵�
                        if((map[gold_y[i]][gold_x[i]] == '#') || //$�� ������ ���⿡ #�̳�
                           (map[gold_y[i]][gold_x[i]] == '$')){ // $�� �ִٸ�
                            gold_x[i] = gold_x[i] + 1;          // $ �� @ �̵� ���
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
}
void screen_clear(){
    system("clear");
    printf("Hello %s\n\n",user_name);
}

void displayHelp(){
    printf("h(����), j(�Ʒ�), k(��), l(������)\n");
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
    FILE* map_file = NULL;
    int map_level = 0;
    int row=0, col=0;
    int temp_col = 0;
    char temp_char = 0;
    char temp_char2 = 0;
    int entry_count = 0;

    slot_count = 0;
    gold_count = 0;
    undo_count = 5;

    // ������ �о stage�� ũ�⸦ �˾Ƴ�
    map_file = fopen("map.txt","r");
    while((temp_char = fgetc(map_file)) != EOF){
        if(temp_char == 'm'|| temp_char == 'e'){
            map_level++;
        }
        if(map_level != level)
            continue;

        temp_col++;

        if(temp_char == 0x0D){
            temp_char = fgetc(map_file);
            temp_col++;
            row++;
            if(temp_col > col){
                col = temp_col;
            }
            temp_col=0;
        }
        else if(temp_char == 0x0A){
            row++;
            if(temp_col > col){
                col = temp_col;
            }
            temp_col=0;
        }
    }

    map_rows = row;
    map_cols = col;
    fclose(map_file);

    // map �迭 ������ �ʱ�ȭ �� ��,  ���� ������ �о��
    for(int i=0; i<MAX_RC; i++)
        for(int j=0; j<MAX_RC; j++)
            map[i][j] = ' ';


    map_file = fopen("map.txt","r");
    map_level = 0;
    entry_count = 0;
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++){
            map[i][j] = fgetc(map_file);

            if(map[i][j] == 0x0D){ //������ȯ�� ����(\r\n)�� ����� ��ƾ
                continue;
            }
            if(map[i][j] == 0x0A && map[i][j-1] == 0x0D ){
                map[i][j-1] = ' ';
                map[i][j] = 0x0A;
            }

            if(map[i][j] == '@'){ // �ʱ� �÷��̾� ��ġ ����
                Px = j;
                Py = i;
            }
            if(map[i][j] == 'O'){ // �ʱ� slot ��ġ �� ���� �˾Ƴ���
                slot_x[slot_count] = j;
                slot_y[slot_count] = i;
                slot_count++;
            }
            if(map[i][j] == '$'){ // �ʱ� gold ��ġ �� ���� �˾Ƴ���
                gold_x[gold_count] = j;
                gold_y[gold_count] = i;
                gold_count++;
            }

            if(map[i][j] == 'm'){
                map[i][j] = fgetc(map_file);
                map_level++;
            }
            if(map_level != level){ // ���� level�� �´� map���� Ȯ��
                i=0; j=-1;
                slot_count = 0;
                gold_count = 0;
                entry_count = 0;
                continue;
            }
            if(map[i][j] == 'a')
                map[i][j] = fgetc(map_file);
            if(map[i][i] == 'p')
                map[i][j] = fgetc(map_file);
            if(map[i][j] == '\n' && entry_count ==0){ // ��ܿ� mapǥ�� ���о������
                entry_count++;
                //map[i][j] = fgetc(map_file);
            }


            if(map[i][j] == '\n'){  // map�迭�� ������ ���� �� ����ó��
                for(; j<map_cols-1; j++)
                    map[i][j] = ' ';

                if(map[i][j] == ' ')
                    map[i][j] = '\n';
            }
        }
    }
    if(gold_count != slot_count){ // gold�� slot ������ �ȸ����� ����
        printf("$�� 0�� ������ �����ʾ� �����մϴ�.\n");
        exit(1);
    }
    fclose(map_file);
}

void printMap(){
    map[Py][Px] = '@'; // �÷��̾��� �̵��� ���� ������ ��ġ�� @ ���

    int row, col;
    for(int k=0; k<slot_count; k++){ // @�� �̵����� ���� slot ��ġ�� ������ �����ʵ���
        if(map[slot_y[k]][slot_x[k]] == ' ')
            map[slot_y[k]][slot_x[k]] = 'O';
    }

// @�� �̵����� ���� gold ��ġ�� ������ �����ʰ� slot���� gold�� �ö󰡰� ���� �� �ֵ���
    for(int k=0; k<gold_count; k++){
        if(map[gold_y[k]][gold_x[k]] == ' '|| map[gold_y[k]][gold_x[k]] == 'O')
            map[gold_y[k]][gold_x[k]] = '$';
    }
    for(int i=0; i<map_rows; i++){
        for(int j=0; j<map_cols; j++){
            putchar(map[i][j]);       // ���� ������ map ��ü ���
        }
    }
    printf("\n");
}

int getch(){ // Ű����� �Է¹��� ������ ȭ�鿡 ��������ʰ� ����
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
#include<stdio.h>
#include<windows.h>
#include<string.h>
#define t 116
#define MAX_STAGE 5
void t_key(int key,int level){
	int ranking;
	key=getch();
	switch (key) {
		
		case t:
		
		switch(key)
		{
			Sleep(1000);
			char buf[10000];
			int i;
			char t_buf[1000];
			float t_sec[500];
			char buf_sec[1000];
			FILE* file = fopen("ranking.txt","r");

			case 1 :
			
			level="1";
			load_rank(level);
			getch();
			
			break;


			case 2 :
			
			level="2";
			load_rank(level);
			getch();
			
			break;


			case 3 :
			
			level="3";
			load_rank(level);
			getch();
			
			break;


			case 4 :
			
			level="4";
			load_rank(level);
			getch();
			
			break;


			case 5 :
			
			level="5";
			load_rank(level);
			getch();
			
			break;


			default :
			
			fopen("ranking.txt","r");
			fgets(buf,sizeof(buf),file);
			printf("%s",buf);
			while(!feof(file));
			{
				fgets(buf,sizeof(buf),file);
				printf("%s",buf);
			}
			fclose(file);
			getch();

			break;
		}
	}
}


void load_rank(int level){
				
     FILE* file = fopen("ranking.txt","r");
     char temp_rank[50][50]; // 랭킹 파일 데이터를 저장할 변수
     float time[6];
     char ch_level = level + 48;
     char map_exp[6] = "map  \n";
     map_exp[3] = ' ';
     map_exp[4] = ch_level;
     char current_map[6];
	 char name[6][10];
     char temp_char[4];
     int user_num = 0;
  	 
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
 	
 	 int i = 0;
	 fopen("ranking.txt","r");
	 while(!feof(file)){
		 fgets(temp_rank[i],sizeof(temp_rank[i]),file);
		
		 if(temp_rank[i][4] == map_exp[4] && temp_rank[i][5] == '\n'){
             strcpy(current_map,temp_rank[i]);
             int j = 0;

			 printf("map %s\n",level);
             while(!feof(file)){
                 fscanf(file,"%s %f%s\n",&name[j],&time[j],&temp_char);
                 printf("%s %f%s\n",name[j],time[j],temp_char);
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
	 fclose(file);
}




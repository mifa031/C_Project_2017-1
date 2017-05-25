#include<stdio.h>
 #define L 104
 #define R 105
 #define U 107
 #define D 106
 #define UNDO 117

 //키 아스키코드 정의

void UNDO_KEY()
{
 char stage[18][21];            //크기는 임의
 int u_stage[6][18][21];         //undo용 스테이지 데이터
 //u_stage[0]은 현재 맵 정보 저장 그 이후부터가 이동 이전의 데이터
 int num_of_undo=0;//undo를 시행한 총 횟수(최대 5회)
 int u_x=0;
 int u_y=0;
 int u_num=0;
 int temp_u_stage=0;

 int key;
 int dx = 0;
 int dy = 0;
 int x, y;
 int mapx, mapy;
 int x_1,y_1;
       while((temp_u_stage=fopen("map.txt","r")!=EOF))
       {
           u_stage[0][u_y][u_x]=temp_u_stage;
           if(temp_u_stage=='\n')
               u_y++;
           else
               u_x++;
       }                //시작시의 맵을 u_stage[0]에 저장함

	     for( u_num = 1; u_num < 6; u_num++)
		 {
			 for(y_1 = 0; y_1 < 17 ; y++, y_1++)
			 {
				 for(x_1 = 0; x_1 < 20 ; x_1 ++)
				 {
					 u_stage[u_num][y_1][x_1] = ' ';
				 }
			 }
		 }//시작시 u_stage[0]를 제외한 u_stage를 공백으로 표시

        key = getchar();

       switch (key)
       {
         case L:

         dx = -1;
         dy = 0;
		 for( u_num = 5; u_num >=1 ; u_num--)
	{
		 for(y_1 = 0; y_1 < 18 ; y_1++)
		 {
			 for(x_1 = 0; x_1 < 21 ; x_1 ++)
			 {
				 u_stage[u_num + 1][y_1][x_1] = u_stage[u_num][y_1][x_1];
			 }
		 }
	}//u_stage[4 ->5번,3 -> 4번,2 -> 3번,1 -> 2번,0 -> 1번]에 저장
		
		for(y_1 = 0; y_1 <= 17 ; y++, y_1++)
		{
			for( x_1 = 0; x_1 <= 20 ; x_1 ++)
			{
				u_stage[0][y_1][x_1] = stage[y_1][x_1];
			}
		} // 현재의 맵 데이터를 u_stage[0]에 저장함
		
		break;
		
		case D:
		
		dx = 0;
		dy = -1;
		
		for( u_num = 5; u_num >=1 ; u_num--)
		{
		for(y_1 = 0; y_1 < 18 ; y_1++)
		{
			for(x_1 = 0; x_1 < 21 ; x_1 ++)
			{
				u_stage[u_num + 1][y_1][x_1] = u_stage[u_num][y_1][x_1];
			}
		}
	}//u_stage[4 ->5번,3 -> 4번,2 -> 3번,1 -> 2번,0 -> 1번]에 저장되게 함

for(y_1 = 0; y_1 <= 17 ; y++, y_1++)
{
	for( x_1 = 0; x_1 <= 20 ; x_1 ++)
	{
		u_stage[0][y_1][x_1] = stage[y_1][x_1];
	}
} // 현재의 맵 데이터를 u_stage[0]에 저장함
         
		 break;

         case U:
         
		 dx = 0;
         dy = +1;
         
    for( u_num = 5; u_num >=1 ; u_num--)
	{
		for(y_1 = 0; y_1 < 18 ; y_1++)
		{
			for(x_1 = 0; x_1 < 21 ; x_1 ++)
			{
				u_stage[u_num + 1][y_1][x_1] = u_stage[u_num][y_1][x_1];
			}
		}
	}//u_stage[4 ->5번,3 -> 4번,2 -> 3번,1 -> 2번,0 -> 1번]에 저장되게 함

for(y_1 = 0; y_1 <= 17 ; y++, y_1++)
{
	for( x_1 = 0; x_1 <= 20 ; x_1 ++)
	{
		u_stage[0][y_1][x_1] = stage[y_1][x_1];
	}
} // 현재의 맵 데이터를 u_stage[0]에 저장함
		 
		 break;

         case R:
         
		 dx = +1;
         dy = 0;
         
    for( u_num = 5; u_num >=1 ; u_num--)
	{
		for(y_1 = 0; y_1 < 18 ; y_1++)
		{
			for(x_1 = 0; x_1 < 21 ; x_1 ++)
			{
				u_stage[u_num + 1][y_1][x_1] = u_stage[u_num][y_1][x_1];
			}
		}
	}//u_stage[4 ->5번,3 -> 4번,2 -> 3번,1 -> 2번,0 -> 1번]에 저장되게 함

for(y_1 = 0; y_1 <= 17 ; y++, y_1++)
{
	for( x_1 = 0; x_1 <= 20 ; x_1 ++)
	{
		u_stage[0][y_1][x_1] = stage[y_1][x_1];
	}
} // 현재의 맵 데이터를 u_stage[0]에 저장함
		 
		 break;

		 case UNDO:
		 
		 if(num_of_undo<5)
{
	for( u_num = 5; u_num >=1 ; u_num--)
	{
		for(y_1 = 0; y_1 < 18 ; y_1++)
		{
			for(x_1 = 0; x_1 < 21 ; x_1 ++)
			{
				u_stage[u_num - 1][y_1][x_1] = u_stage[u_num][y_1][x_1];
			}
		}
	}//u_stage[5 ->4번,4 -> 3번,3 -> 2번,2 -> 1번,1 -> 0번]에 저장되게 함
	num_of_undo++;
}
}
}

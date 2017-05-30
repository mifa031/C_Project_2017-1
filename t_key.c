#include<stdio.h>
#include<windows.h>
#define t 116
int t_key(int key)
{
	int ranking;
	key= getch();
	switch (key)
	{
		case t:
		Sleep(1000);
		switch(key)
		{
			char buf[10000];
			char buf_sec[2000];
			float sec;
			int map_num;
					
			case 1:
			fopen("ranking.txt","r");
			while(fscanf(fopen("ranking.txt","r"),"%s  %f%s",buf,sec,buf_sec )!="map 2")
             {
				 printf("%s  %f%s",buf,sec,buf_sec);
             }
			fclose(fopen("ranking.txt","r"));
			break;

			case 2:
			fopen("ranking.txt","r");
			while(fscanf(fopen("ranking.txt","r"),"%s  %f%s",buf,sec,buf_sec )!="map 3")
             {
				 printf("%s  %f%s",buf,sec,buf_sec);
             }
			fclose(fopen("ranking.txt","r"));
			break;
			
			case 3:
			fopen("ranking.txt","r");
			while(fscanf(fopen("ranking.txt","r"),"%s  %f%s",buf,sec,buf_sec )!="map 2")
             {
				 printf("%s  %f%s",buf,sec,buf_sec);
             }
			fclose(fopen("ranking.txt","r"));
			break;
			
			case 4:
			fopen("ranking.txt","r");
			while(fscanf(fopen("ranking.txt","r"),"%s  %f%s",buf,sec,buf_sec )!="map 2")
             {
				 printf("%s  %f%s",buf,sec,buf_sec);
             }
			fclose(fopen("ranking.txt","r"));
			break;

			case 5:
			fopen("ranking.txt","r");
			while(fscanf(fopen("ranking.txt","r"),"%s  %f%s",buf,sec,buf_sec )!="map 2")
             {
				 printf("%s  %f%s",buf,sec,buf_sec);
             }
			fclose(fopen("ranking.txt","r"));
			break;
			
			default :
			fopen("ranking.txt","r");
			while(ranking = fgetc(fopen("ranking.txt","r"))!=EOF)
			{
				putc(ranking,stdout);
			}
			fclose(fopen("ranking.txt","r"));
		}
	}
}

 #define L 104
 #define R 105
 #define U 107
 #define D 106
 
 //방향키 아스키코드 정의
 
 void Key_point()//함수 설정
 {
        char stage[18][21]; //stage 배열 이름은 임의로 설정한 것입니다.
                            //배열 크기도 임시로 정했습니다. 
        int key;
        int dx = 0;
        int dy = 0; 
        int x,y;
        int mapx, mapy;
        
        key = getchar();
        
        switch (key)
        {
        case L:
        dx = -1;
        dy = 0;
        break;
        
        case D:
        dx = 0;
        dy = -1;
        break;
        
        case U:
        dx = 0;
        dy = +1;
        break;
        
        case R:
        dx = +1;
        dy = 0;
        break;
        }
        
        //stsge [][] : 스테이지 2차원 배열
        if (stage[y+dy][x+dx] == '#')
        {
        mapx = x;// 플레이어 좌표 지정
        mapy = y;
        return; // 창고지기가 벽을 만나면 그대로 리턴한다.
        }
        
                 if (stage[y+dy][x+dx] == '$' || stage [y+dy][x+dx] == ' '){
                      mapx = x;
                      mapy = y;
                      stage[y][x] == ' ';
                      stage[y+dy][x+dx] == '@';
                      stage[y+2*dy][x+2*dy] == '$';
                      if (stage[y+2*dy][x+2*dx] == 'O'){
                            stage[y+2*dy][x+2*dx] == ' ';
                            stage[y+2*dy][x+2*dx] == '$';
                      }
                 }
                 
                 // 창고지기가 빈공간이나 박스를 만나면 원래있던 자리는 공백으로 채우고 그다음 공간을 창고지기로 채운다.
                 // 박스는 그다음 자리로 이동한다.
                 return;
                 }
 void main()
{
          while(1)
          {
           Key_point();
           }
 }

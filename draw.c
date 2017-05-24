void draw()
{
      char stage[18][21];
      // 18이랑 21은 임의로 정한 맵의 크기입니다.
      int mapx, mapy;
      int x,y;
      
      for (y = 0; y < 18; y++){
            for (x = 0; x < 21; x++){
                  if (stage[y][x] == '@'){
                        mapx = x; //플레이어의 x좌표
                        mapy = y; //플레이어의 y좌표
                        stage[y][x] == ' '; // 플레이어가 있던 자리를 공백으로 채운다.
                   }
             }
       }
       return;
}

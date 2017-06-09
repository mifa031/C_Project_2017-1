void file_load()  
{

    FILE *load; // 저장한 게임을 불러오는 파일을 지정
    char p_map[MAX_RC][MAX_RC];
    load = fopen("sokoban.txt", "r"); // 파일 오픈
    
    
    if (load == NULL)
        return 1;
        // 예외처리
        
    while (feof(load)==0){// 파일에 있는 배열 읽어서 출력
        fgets(p_map, sizeof p_map[MAX_RC][MAX_RC], load);
        printf("%s", map);
        }
        
        
        fclose(load);
}

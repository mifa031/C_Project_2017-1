void save()
{

         FILE *save_point;
         save_point = fopen("sokoban.txt","w");
         
         
         fprintf(save_point, "%s", username[]);
         
         for (int i = 0; i <map_rows; i++){
              for (int j = 0; j < map_cols; j++){
                    fprintf(save_point, "%c", map[i][j]);
              }
         }
         
         fclose(save_point);
}

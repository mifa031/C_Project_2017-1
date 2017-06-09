void save()
{

         FILE *save_point;
         float temp_t;
         save_point = fopen("sokoban.txt","w");
         
         
         fprintf(save_point, "%s\n", user_name);
         
         for (int i = 0; i <map_rows; i++){
              for (int j = 0; j < map_cols; j++){
                    fprintf(save_point, "%c", map[i][j]);
              }
         }
         fprintf(save_point, "\n");
         
         gettimeifday(&end,NULL);
         temp_t = time_diff(&end, &start);
         fprintf(save_point, "play time : %.1f\n", temp_t);
         fprintf(save_point, "remain undo : %d\n", undo_count);
         
         fclose(save_point);
}

#define MAX_STAGE 5

char map_data[MAX_STAGE][][];
char map[][];

void readMap(int level){
    FILE* map_file;
    int map_level = 0;
    char object;

    map_file = fopen("map.txt","r");
    while(object = fgets(map_file) != EOF){
        if(object == 'm'){
            map_level++;
            if(map_level > level)
                break;
        }
        if(object == '\n'){
            map
        }

        printf("%c",object);
        fscanf(map_file,"%c",&object)

    }

}

void main(){
    while(1){
        readMap();
    }
}

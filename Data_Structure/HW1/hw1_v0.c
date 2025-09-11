#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define f printf(".line: %d.\n", __LINE__)
// void map_print(int size_maze, int maze[size_maze][size_maze]);

//adjust up, right, down, left's coordinate in maze
int adj[4][2]={{-1,0}, {0,1}, {1,0}, {0,-1}};
//save bfs path
char save_path[4][3000]={"\0", "\0", "\0", "\0"};
int save_len[4];

typedef struct{ //branch point
    int point[2], count_path;
    char *path;
}node;

node* push_queue(node *q, node new, int *size){
    (*size)++;
    q=realloc(q, sizeof(node)*(*size));
    *(q+*size-1)=new;
    return q;
}

node* pop_queue(node *q, int *size){
    for(int i=0; i<*size-1; i++){
        *(q+i)=*(q+i+1);
    }
    (*size)--;
    q=realloc(q, sizeof(node)*(*size));
}

//find walk to next point's path
char* find_path(int *src, int size_maze, int maze[size_maze][size_maze], char *path, int ball_num){
    char direction; //5=none, [0,1,2,3]=[up,right,down,left]
    while(1){
        direction='5';
        //search up, right, down, left (in each time)
        for(int i=0; i<4; i++){
            if(maze[src[0]+adj[i][0]][src[1]+adj[i][1]]!=1 && maze[src[0]+adj[i][0]][src[1]+adj[i][1]]!=ball_num){
                if(direction=='5') direction='0'+i;
                else return path;
            }
        }

        //check is dst or not
        if(maze[src[0]][src[1]]==9) return path;
        maze[src[0]][src[1]]=ball_num;
        
        //dead path
        if(direction=='5'){
            *path='\0';
            return path;
        }

        //modify
        int num=direction-'0';
        src[0]+=adj[num][0], src[1]+=adj[num][1]; //adjustment
        char ctmp[2]={direction, '\0'};
        strcat(path, ctmp);
    }
}

//search src to dst's path
char* search(int src[], int dst1[], int dst2[], int ball_num, int size_maze, int maze[size_maze][size_maze]){
    //init
    int size_queue=0, find_dst_ct=0;
    char *tmp_path = malloc(500);
    node *queue = NULL;
    queue=push_queue(queue, (node){src[0], src[1], 0, "\0"}, &size_queue);

    while(find_dst_ct<2){
        //search up, right, down, left (in each time)
        // printf("from pt: %d %d.\n", queue->point[0], queue->point[1]);
        for(int i=0; i<4; i++){
            //init
            src[0] = queue->point[0];
            src[1] = queue->point[1];
            *tmp_path = '\0';

            //start search
            if(maze[src[0]+adj[i][0]][src[1]+adj[i][1]]!=1 && maze[src[0]+adj[i][0]][src[1]+adj[i][1]]!=ball_num){
                char ctmp[2]={'0'+i, '\0'};
                strcat(tmp_path, ctmp);
                src[0]+=adj[i][0], src[1]+=adj[i][1]; //setting
                maze[src[0]][src[1]] = ball_num;
                find_path(src, size_maze, maze, tmp_path, ball_num);

                //if it's not dead path
                if(*tmp_path!='\0'){
                    //merge path
                    node new={src[0], src[1], 0, malloc(1000)};
                    strcpy(new.path, queue->path);
                    strcat(new.path, tmp_path);
                    queue=push_queue(queue, new, &size_queue);
                    // printf("push) pt: %d %d. path: %s.\n", (queue+size_queue-1)->point[0],  (queue+size_queue-1)->point[1],  (queue+size_queue-1)->path);

                    if(maze[src[0]][src[1]]==9){//find dst
                        find_dst_ct++;
                        if(src[0]==dst1[0] && src[1]==dst1[1]){
                            strcpy(save_path[(ball_num-7)*2], new.path);
                            save_len[(ball_num-7)*2]=strlen(new.path);
                        }
                        else{
                            strcpy(save_path[(ball_num-7)*2+1], new.path);
                            save_len[(ball_num-7)*2+1]=strlen(new.path);
                        }
                    }
                    maze[src[0]][src[1]]=ball_num;
                }
            }
        }
        if(queue) pop_queue(queue, &size_queue);
    }
}

int main(){
    //initialize
    int size, src1[2], src2[2], dst1[2], dst2[2]; // [0]:x [1]:y
    scanf("%d", &size);
    int maze[size][size];
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++)
            scanf("%d", &maze[i][j]);
    }

    //input and modify points' position
    scanf("%d %d %d %d", &src1[1], &src1[0], &src2[1], &src2[0]);
    src1[0]=size-1-src1[0], src2[0]=size-1-src2[0];
    scanf("%d %d %d %d", &dst1[1], &dst1[0], &dst2[1], &dst2[0]);
    dst1[0]=size-1-dst1[0], dst2[0]=size-1-dst2[0];
    maze[dst1[0]][dst1[1]]=9, maze[dst2[0]][dst2[1]]=9; //set dst is 9
    //start searching
    maze[src1[0]][src1[1]]=7; //set ball_1 is 7
    maze[dst1[0]][dst1[1]]=9, maze[dst2[0]][dst2[1]]=9; //set dst is 9
    search(src1, dst1, dst2, 7, size, maze); // #1
    maze[src2[0]][src2[1]]=8; //set ball_2 is 8
    maze[dst1[0]][dst1[1]]=9, maze[dst2[0]][dst2[1]]=9; //reset dst is 9
    search(src2, dst1, dst2, 8, size, maze); // #2
    //compare
    int first, second, complement_direct[4]={2,3,0,1}, second_src[2];
    if(save_len[0]+save_len[3] < save_len[1]+save_len[2]){
        if(save_len[0]<save_len[3]) first=0, second=3;
        else first=3, second=0;
    }
    else{
        if(save_len[1]<save_len[2]) first=1, second=2;
        else first=2, second=1;
    }

    //move one first(save another modify path)

    if(first<2) second_src[0]=src2[0], second_src[1]=src2[1];
    else        second_src[0]=src1[0], second_src[1]=src1[1];

    //move_ptr is another's original path's moving pointer
    int adding_len=0, move_ptr=0;
    char path_adding[save_len[first]];

    //start move shortest one
    for(int i=0; i<save_len[first]; i++){
        int direc = save_path[first][i]-'0';
        if(adding_len==0 && save_path[first][i]==save_path[second][move_ptr]) move_ptr++;
        else if(adding_len!=0 && complement_direct[direc]==path_adding[adding_len-1]-'0') adding_len--;
        else if(maze[second_src[0]+adj[direc][0]][second_src[1]+adj[direc][1]]!=1){
            second_src[0]+=adj[direc][0];
            second_src[1]+=adj[direc][1];
            path_adding[adding_len++]=complement_direct[direc]+'0';
        }
    }
    path_adding[adding_len]='\0';
    //opposite
    char opposite_path[adding_len+1];
    for(int i=0; i<adding_len; i++) opposite_path[i]=path_adding[adding_len-i-1];
    opposite_path[adding_len]='\0';

    strcat(strcat(save_path[first], opposite_path), save_path[second]+move_ptr);
    printf("%s", save_path[first]);

}

// void map_print(int size_maze, int maze[size_maze][size_maze]){
//     printf("--------------------map---------------------\n");
//     for(int i=0; i<size_maze; i++){
//         for(int j=0; j<size_maze; j++){
//             printf("%d ", maze[i][j]);
//         }
//         printf("\n");
//     }
//     printf("\n!!------------------map-------------------!!\n");
// }

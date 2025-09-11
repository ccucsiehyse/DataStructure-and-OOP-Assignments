#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define f printf(".line: %d.\n", __LINE__)
//direction -> 0:up, 1:right, 2:down, 3:left

//branch point
typedef struct{
    int pt[2]; //node coordinate
    int len_path; //path length
    char aval_road[4]; //available road direction
    char *path; //path record
}node;

//set global variables

node src[2], dst[2][2], *branch_point, **queue;
int size_branch=0, size_queue=0;


//adjust up, right, down, left's coordinate in maze
int adj[4][2]={{-1,0}, {0,1}, {1,0}, {0,-1}};

//the opposite direction of each
int opp_direct[4]={2,3,0,1};

//set node (init)
void set_node(node *n, int xy[2], int len, char road[4], char *pth){
    n->pt[0] = xy[0];
    n->pt[1] = xy[1];
    n->len_path = len;
    for(int i=0; i<4; i++)
        n->aval_road[i] = road[i];
    n->path = malloc(len+1);
    n->path[0] = '\0';
    strcpy(n->path, pth);
}

// void print_queue(){
//     printf("queue-> ");
//     for(int i=0; i<size_queue; i++)
//         printf("%d %d. ", queue[i]->pt[0], queue[i]->pt[1]);
//     printf("\n");
// }

void push_queue(node* new){
    // print_queue();
    // printf("push) %d %d.\n", new->pt[0], new->pt[1]);

    queue=realloc(queue, sizeof(node*)*(++size_queue));
    *(queue+size_queue-1)=new;

    // print_queue();
}

void pop_queue(){
    // print_queue();
    // printf("(pop) %d %d.\n", queue[0]->pt[0], queue[0]->pt[1]);

    size_queue--;
    for(int i=0; i<size_queue; i++)
        *(queue+i)=*(queue+i+1);
    queue=realloc(queue, sizeof(node*)*size_queue);

    // print_queue();
}

//compare point. if two is the same: 1, otherwise: 0.
int pt_cmp(int a[2], int b[2]){
    if(a[0]==b[0] && a[1]==b[1]) return 1;
    return 0;
}

//find branch point's location in *branch_point
node* find_branch(int pt[2], int size_maze){
    for(int i=0; i<size_branch; i++){
        if(pt[0]==branch_point[i].pt[0] && pt[1]==branch_point[i].pt[1])
            return &branch_point[i];
    }
    return NULL;
}

//find path from (point) to (point, dead path, dst)
char* find_path(int pt0[2], int size_maze, int maze[size_maze][size_maze], int direct, int ct_dst){
    int pt[2]={pt0[0], pt0[1]};
    char *tmp_path=malloc(1);
    int size_path=1;
    do{
        pt[0]+=adj[direct][0];
        pt[1]+=adj[direct][1];
        tmp_path=realloc(tmp_path, ++size_path);
        tmp_path[size_path-2]='0'+direct;
        for(int i=0; i<4; i++){
            if(i==opp_direct[direct]) continue;
            if(maze[pt[0]+adj[i][0]][pt[1]+adj[i][1]]!=1){
                direct=i;
                break;
            }
        }
    }while(maze[pt[0]][pt[1]]<2);
    if(maze[pt[0]][pt[1]]==4) return NULL;
    tmp_path[size_path-1]='\0';

    //set next finding point
    node *next;
    if(maze[pt[0]][pt[1]]>7) //if it's dst
        next = dst[ct_dst]+(maze[pt[0]][pt[1]]-8);
    else next = find_branch(pt, size_maze);
    next->aval_road[opp_direct[tmp_path[size_path-2]-'0']]=0;
    push_queue(next);
    
    return tmp_path;
}

// void map_print(int size_maze, int maze[size_maze][size_maze]){
//     printf("--------------------map---------------------\n");
//     for(int i=0; i<size_maze; i++){
//         for(int j=0; j<size_maze; j++){
//             if(pt_cmp((int[]){i,j}, src[0].pt)) printf("%d ", 6);
//             else if(pt_cmp((int[]){i,j}, src[1].pt)) printf("%d ", 7);
//             else printf("%d ", maze[i][j]);
//         }
//         printf("\n");
//     }
//     printf("!!------------------map-------------------!!\n");
// }

int main(){
    //initialize
    int size_maze;
    scanf("%d", &size_maze);
    int maze[size_maze][size_maze];
    for(int i=0; i<size_maze; i++){
        for(int j=0; j<size_maze; j++)
            scanf("%d", &maze[i][j]);
    }

    //input and modify points to array's position
    scanf("%d %d %d %d", &(src[0].pt[1]), &(src[0].pt[0]), &(src[1].pt[1]), &(src[1].pt[0]));
    src[0].pt[0]=size_maze-1-src[0].pt[0];
    src[1].pt[0]=size_maze-1-src[1].pt[0];
    // maze[src[0].pt[0]][src[0].pt[1]]=6; //set ball_1 is 6
    // maze[src[1].pt[0]][src[1].pt[1]]=7; //set ball_2 is 7

    scanf("%d %d %d %d",  &(dst[0][0].pt[1]), &(dst[0][0].pt[0]), &(dst[0][1].pt[1]), &(dst[0][1].pt[0]));
    dst[0][0].pt[0]=size_maze-1-dst[0][0].pt[0];
    dst[0][1].pt[0]=size_maze-1-dst[0][1].pt[0];
    maze[dst[0][0].pt[0]][dst[0][0].pt[1]]=8; //set dst1 is 8
    maze[dst[0][1].pt[0]][dst[0][1].pt[1]]=9; //set dst1 is 9

    //check if it's a node, dead path, src, dst. And initialize.

    //set src first
    for(int i=0; i<2; i++){
        for(int j=0; j<4; j++){
            if(maze[src[i].pt[0]+adj[j][0]][src[i].pt[1]+adj[j][1]]!=1)
                src[i].aval_road[j]=1;
            else src[i].aval_road[j]=0;
        }
        src[i].path="\0";
        src[i].len_path=0;
        // printf("src[%d]) %d%d%d%d.\n", i, src[i].aval_road[0], src[i].aval_road[1], src[i].aval_road[2], src[i].aval_road[3]);
    }
    //set dst
    for(int i=0; i<2; i++){
        for(int j=0; j<4; j++){
            if(maze[dst[0][i].pt[0]+adj[j][0]][dst[0][i].pt[1]+adj[j][1]]!=1)
                dst[0][i].aval_road[j]=1;
            else dst[0][i].aval_road[j]=0;
        }
        dst[0][i].path=malloc(10);
        dst[0][i].path[0]='\0';
        dst[0][i].len_path=99999;
        dst[1][i]=dst[0][i];
        dst[1][i].path=malloc(10);
        dst[1][i].path[0]='\0';
        // printf("dst[1][%d]) %d%d%d%d.\n", i, dst[1][i].aval_road[0], dst[1][i].aval_road[1], dst[1][i].aval_road[2], dst[1][i].aval_road[3]);
    }

    //set node & dead path
    for(int i=1; i<size_maze-1; i++){
        for(int j=1; j<size_maze-1; j++){
            //if src or dst or wall
            if(maze[i][j]>0) continue;

            //check 4 direction
            int ct_tmp=0;
            for(int k=0; k<4; k++){
                if(maze[i+adj[k][0]][j+adj[k][1]]!=1) ct_tmp++;
            }
            if(ct_tmp==1) maze[i][j]=4; //set dead path is 4
            else if(ct_tmp>2){
                maze[i][j]=5; //set branch point is 5

                // printf("branch pt: %d %d.\n", i, j);

                //save in branch_point
                branch_point = realloc(branch_point, sizeof(node)*(++size_branch));
                char tmp_direc[4]={0};
                for(int k=0; k<4; k++)
                    if(maze[i+adj[k][0]][j+adj[k][1]]!=1) tmp_direc[k]=1;
                set_node(&branch_point[size_branch-1], (int[]){i, j}, 99999, tmp_direc, "\0");
            }
        }
    }

    //start search
    
    for(int ct=0; ct<2; ct++){
        //push src twice
        push_queue(&src[ct]);
        // printf("--------src %d--------\n", ct);
        // map_print(size_maze, maze);

        //search path from src to dst
        while(queue){
            char *tmp_path;
            // printf("from %d %d.\n", queue[0]->pt[0], queue[0]->pt[1]);

            //find 4 direction's path
            for(int i=0; i<4; i++){
                if(!queue[0]->aval_road[i]) continue;
                queue[0]->aval_road[i]=0;

                //find next point in this direction & setting
                tmp_path = find_path(queue[0]->pt, size_maze, maze, i, ct);

                if(tmp_path==NULL) continue; //dead path
                int tmp_len=strlen(tmp_path);

                //if path shorter than original, changing to new one
                if(queue[0]->len_path + tmp_len < queue[size_queue-1]->len_path){
                    queue[size_queue-1]->len_path = queue[0]->len_path + tmp_len;
                    queue[size_queue-1]->path = realloc(queue[size_queue-1]->path, queue[size_queue-1]->len_path+1);
                    strcpy(queue[size_queue-1]->path, queue[0]->path);
                    strcat(queue[size_queue-1]->path, tmp_path); 
                }
                // printf("path: %s.\n", queue[size_queue-1]->path);
                // printf("------------------------------------\n");
            }
            pop_queue();
        }

        //reset
        if(ct<1){
            for(int i=0; i<size_branch; i++){
                branch_point[i].len_path=99999;
                for(int k=0; k<4; k++)
                    if(maze[branch_point[i].pt[0]+adj[k][0]][branch_point[i].pt[1]+adj[k][1]]!=1)
                        branch_point[i].aval_road[k]=1;
            }
        }
    }
    
    // for(int i=0; i<2; i++){
    //     for(int j=0; j<2; j++)
    //         printf("dst[%d][%d]-> len: %d, path: %s.\n", i, j, dst[i][j].len_path, dst[i][j].path);
    // }
    
    //combine two path and try to get the shorter path.

    //select the shortest combination path
    int first[2], second[2]; // (short short, short long) position in dst array
    if(dst[0][0].len_path + dst[1][1].len_path < dst[0][1].len_path + dst[1][0].len_path){
        if(dst[0][0].len_path < dst[1][1].len_path)
            first[0] = 0, first[1] = 0, second[0] = 1, second[1] = 1;
        else
            first[0] = 1, first[1] = 1, second[0] = 0, second[1] = 0;
    }
    else{
        if(dst[0][1].len_path < dst[1][0].len_path)
            first[0] = 0, first[1] = 1, second[0] = 1, second[1] = 0;
        else
            first[0] = 1, first[1] = 0, second[0] = 0, second[1] = 1;
    }

    //combine path

    int final_len=0;
    int path_len[2]={dst[first[0]][first[1]].len_path, dst[second[0]][second[1]].len_path};
    int ptr[2]={path_len[0]-1, path_len[1]-1}; //ptr is each original path's moving pointer

    char path[2][path_len[0] + path_len[1] + 1]; //save selected path by reverse (init at following)
    char final_path[ path_len[0] * 2 + path_len[1] + 1 ];

    //init
    for(int i=ptr[0]; i>=0; i--)
        path[0][ptr[0]-i]=dst[first[0]][first[1]].path[i];
    for(int i=ptr[1]; i>=0; i--)
        path[1][ptr[1]-i]=dst[second[0]][second[1]].path[i];

    //test
    // path[0][ptr[0]+1]='\0';
    // path[1][ptr[1]+1]='\0';
    // printf("%s.\n%s..\n", dst[first[0]][first[1]].path, path[0]);
    // printf("%s.\n%s..\n", dst[second[0]][second[1]].path, path[1]);

    //combine path
    while(ptr[0]>=0 || ptr[1]>=0){
        if(ptr[0]>=0){
            if(ptr[1]>=0){ //both have left
                //if path same
                if(path[0][ptr[0]]==path[1][ptr[1]]){
                    final_path[final_len++]=path[0][ptr[0]];
                    src[first[0]].pt[0]+=adj[path[0][ptr[0]]-'0'][0], src[first[0]].pt[1]+=adj[path[0][ptr[0]]-'0'][1]; //modify src[first] position
                    src[second[0]].pt[0]+=adj[path[1][ptr[1]]-'0'][0], src[second[0]].pt[1]+=adj[path[1][ptr[1]]-'0'][1]; //modify src[second] position
                    ptr[0]--, ptr[1]--; //ptr move foward
                }

                //if only src[first] can move
                else if(maze[src[second[0]].pt[0]+adj[path[0][ptr[0]]-'0'][0]][src[second[0]].pt[1]+adj[path[0][ptr[0]]-'0'][1]]==1){
                    final_path[final_len++]=path[0][ptr[0]];
                    src[first[0]].pt[0]+=adj[path[0][ptr[0]]-'0'][0], src[first[0]].pt[1]+=adj[path[0][ptr[0]]-'0'][1]; //modify src[first] position
                    ptr[0]--;
                }

                //if only src[second] can move
                else if(maze[src[first[0]].pt[0]+adj[path[1][ptr[1]]-'0'][0]][src[first[0]].pt[1]+adj[path[1][ptr[1]]-'0'][1]]==1){
                    final_path[final_len++]=path[1][ptr[1]];
                    src[second[0]].pt[0]+=adj[path[1][ptr[1]]-'0'][0], src[second[0]].pt[1]+=adj[path[1][ptr[1]]-'0'][1]; //modify src[second] position
                    ptr[1]--;
                }

                else{ //move src[first]
                    final_path[final_len++]=path[0][ptr[0]];
                    src[first[0]].pt[0]+=adj[path[0][ptr[0]]-'0'][0], src[first[0]].pt[1]+=adj[path[0][ptr[0]]-'0'][1]; //modify src[first] position
                    src[second[0]].pt[0]+=adj[path[0][ptr[0]]-'0'][0], src[second[0]].pt[1]+=adj[path[0][ptr[0]]-'0'][1]; //modify src[second] position
                    path[1][++ptr[1]]=opp_direct[path[0][ptr[0]--]-'0']+'0';
                }
            }
            else{ //only src[first] have left
                for(; ptr[0]>=0; ptr[0]--)
                    final_path[final_len++]=path[0][ptr[0]];
            }
        }
        else{ //only src[second] have left
            for(; ptr[1]>=0; ptr[1]--)
                final_path[final_len++]=path[1][ptr[1]];
        }
        // printf("move direction: %c.\n", final_path[final_len-1]);
        // map_print(size_maze, maze);
    }

    //print final path
    final_path[final_len]='\0';
    printf("%s", final_path);

}
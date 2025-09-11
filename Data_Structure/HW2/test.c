#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define P printf("line: %d.\n", __LINE__)

int main(){

    srand(time(NULL));
    printf("RAND_MAX = %d\n", RAND_MAX);

    int ct_over=0, ct_lower=0, ct_equal=0, ct_time[20]={0};

    int n, count_output=2, update_count=0, ct_op_np[2]={0};
    // scanf("%d", &n); // n points

    n = 10000;

    int path[3][n]; // [0]:old path, [1]:new path, [2]:cp old path
    //cp: common point, op: old point, np: new point
    char point[n]; // cp=0, op=1, np=2 // if >= 3, complete update
    char check[2][n]; // check random yet

    // test t times
    for(int t=0; t<1000; t++)
    {
    
    count_output=2, update_count=0, ct_op_np[0]=0, ct_op_np[1]=0;

    int av_pt[2], tmp1=0, tmp2=0, r1, r2; // set random number
    av_pt[0]=n*3/4+rand()%(n/4);
    av_pt[1]=75+rand()%(n/4);
    
    for(int i=0; i<2; i++){ //init
        for(int j=0; j<n; j++){
            check[i][j]=0;
            path[i][j]=-1; 
        }
    }
    //set path [0] & [1]
    for(int i=0; i<av_pt[0]-1; i++){
        check[0][tmp1]=1;
        r1=rand()%(n-1);
        while(check[0][r1]==1){
            r1++;
            r1%=(n-1);
        }
        path[0][tmp1]=r1;
        tmp1=r1;
    }
    path[0][tmp1]=n-1;

    for(int i=0; i<av_pt[1]-1; i++){
        check[1][tmp2]=1;
        r2=rand()%(n-1);
        while(check[1][r2]==1){
            r2++;
            r2%=(n-1);
        }
        path[1][tmp2]=r2;
        tmp2=r2;
    }
    path[1][tmp2]=n-1;

    //input old, new path
    for(int j=0; j<n; j++){
        // scanf("%d", &path[0][j]);
        if(path[0][j]!=-1 || j==n-1) point[j]=0; // cp
        else{
            point[j]=2; // np
            ct_op_np[1]++; // np count
        }
    }
    for(int j=0; j<n; j++){
        // scanf("%d", &path[1][j]);
        if(j!=n-1 && path[1][j]==-1){ // op
            point[j]=1;
            update_count++;
        }
    }
    ct_op_np[0]=update_count; // op count

    //array (save each step except old/new path)
    int **step = malloc(sizeof(int*));
    
    //if have new point
    if(ct_op_np[1]>0){
        step[0] = malloc(n * sizeof(int));
        
        //first step: update new point (np)
        for(int j=0; j<n; j++){
            if(point[j]==2){ // np
                step[0][j]=path[1][j];
                point[j]+=3; // mark updated
                update_count++;
            }
            else step[0][j]=path[0][j];
        }
        count_output++;
    }

    //init "cp old path"
    for(int j=0; j<n; j++) path[2][j]=-9;
    int count_cp_old=0, tmp_pt=0;

    // if it's cp, save point's sequence number
    while(tmp_pt > -1){
        if(point[tmp_pt]==0) path[2][tmp_pt]=count_cp_old++;
        tmp_pt=path[0][tmp_pt]; // tmp = next point
    }
    
    //update path until finish
    while(update_count<n-1){

        //update cp that jump forward in "cp old path"
        count_output++;
        step = realloc(step, (count_output-2) * sizeof(int*));
        step[count_output-3] = malloc(n * sizeof(int));
        for(int j=0; j<n; j++){
            if(path[2][j]>=0 && j<n-1 && point[j]==0){ // if it's on "cp old path" & not dst & no update
                int end=path[1][j]; // new next point

                // end = next point (until end is on "cp old path")
                if(count_output>3) while(path[2][end]<0) end=step[count_output-4][end];
                else while(path[2][end]<0) end=path[0][end];

                if(path[2][end]>path[2][j]){ // if jump forward
                    step[count_output-3][j]=path[1][j];
                    point[j]+=3; // mark updated
                    update_count++;
                }
                else{
                    if(count_output>3) step[count_output-3][j]=step[count_output-4][j]; // no update
                    else step[count_output-3][j]=path[0][j];
                }
            }
            else{
                if(count_output>3) step[count_output-3][j]=step[count_output-4][j]; // no update
                else step[count_output-3][j]=path[0][j];
            }
        }

        if(update_count>=n-1) continue; // finish
        
        //update "cp old path"
        int count_leave=0; // see if anyone leave "cp old path"
        for(int j=0; j<n; j++){
            if(path[2][j]>-1){
                path[2][j]=-1;
                count_leave++;
            }
        }
        count_cp_old=0;
        tmp_pt=0;
        while(tmp_pt > -1){
            if(point[tmp_pt]%3==0){
                path[2][tmp_pt]=count_cp_old++;
                count_leave--;
            }
            tmp_pt=step[count_output-3][tmp_pt]; // tmp = next point
        }
        
        if(count_leave==0) continue; // no one in "cp old path"

        //update cp that isn't in "cp old path"
        count_output++;
        step = realloc(step, (count_output-2) * sizeof(int*));
        step[count_output-3] = malloc(n * sizeof(int));
        for(int j=0; j<n; j++){
            if(path[2][j]==-1 && point[j]==0){ // cp (not in "cp old path" && no update)
                step[count_output-3][j]=path[1][j];
                point[j]+=3; // mark updated
                update_count++;
            }
            else{
                if(count_output>3) step[count_output-3][j]=step[count_output-4][j]; // no update
                else step[count_output-3][j]=path[0][j];
            }
        }
    }

    for(int j=0; j<count_output-2; j++) free(step[j]);

    //print
    if(ct_op_np[0]==0) count_output--; // if no op
    // printf("%d\n", count_output);
    // for(int j=0; j<n; j++) printf("%d ", path[0][j]); // old path
    // printf("\n");
    // for(int i=0; i<count_output-2; i++){ //each step in middle
    //     for(int j=0; j<n; j++) printf("%d ", step[i][j]);
    //     printf("\n");
    // }
    // for(int j=0; j<n; j++) printf("%d ", path[1][j]); // new path

    int limit=14;
    if(count_output>limit) ct_over++;
    else if(count_output<limit) ct_lower++;
    else ct_equal++;

    // if(count_output<20) ct_time[count_output]++;

    printf("%2d over: %d, lower: %d, equal: %d.\n", t, ct_over, ct_lower, ct_equal);
    // printf("%2d: ", t);
    // for(int j=0; j<20; j++) printf("%2d:%2d. ", j, ct_time[j]);
    // printf("\n");
    free(step);


    // clock_t start_time = clock();  
    // while (clock() < start_time + 10);

    }
    printf("over: %d, lower: %d, equal: %d.\n", ct_over, ct_lower, ct_equal);
    
}
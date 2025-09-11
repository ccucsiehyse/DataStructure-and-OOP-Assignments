#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define P printf("line: %d.\n", __LINE__)
#define UnVisit -9
#define MIN(a, b) a>b ? b : a
#define CT_MAX 30

//tree's node
struct node{
    int a, b, t; //left_num, right_num, time_slot (in tree)
    struct node *Lchild, *Rchild;
};
typedef struct node node;

//request's path information
typedef struct{
    int req_id;
    int len, *path;
    int t_linear, t_complete; // linear/complete tree's height
    node *t_root; //tree root
    int **time_bar, t_i; //time_memory array, time_revise
}path_info;

//4 input variables
int nodes, links, time, requests;

//store graph's link by 2D malloc array
int **graph;
int *link_len, *space_len;

//store queue by rotate malloc array
//use in bfs
int *queue;
int q_front = 0, q_rear = 0, q_size = 8;

//store heap by malloc pointer array
//use in sorting request's priority
path_info **heap;
int h_size = 0; //heap_size, heap[0] will not be use when sort

//store accepted_requests by malloc pointer array
path_info **acc_req;
int acc_size = 0, acc_space = 2; //acc_req_size, acc_req malloc size

//functions
//please check details below main() function
int floor_log(int n);
int big2(int n);
node* set_node(int a, int b, int t, node* L, node* R);
node* left_complete(int a, int b, int t);
node* right_complete(int a, int b, int t);
node* SD_linear(int a, int b, int t);
node* DS_linear(int a, int b, int t);
node* make_tree(int n, int t, int type);
void push_link(int id, int num);
void push_queue(int num);
int pop_queue();
void bfs(path_info *req_path, int src, int dst, int id);
void Swap(path_info **a, path_info **b);
void push_heap(path_info *p);
path_info* pop_heap();
int corner_compare(int memory[time][nodes], path_info* p, int now_t);
void make_bar(path_info *p, node *now);
int set_memory(int memory[time][nodes], path_info *p, int t_i);
void print_tree(path_info *p, node *n);

int main(){
    //input 4 variables
    scanf("%d %d %d %d", &nodes, &links, &time, &requests);
    
    //input each node's memory
    int memory[time][nodes], id_tmp, mem_tmp;
    for(int i=0; i<nodes; i++){
        scanf("%d %d", &id_tmp, &mem_tmp);
        for(int j=0; j<time; j++) memory[j][id_tmp] = mem_tmp;
    }

    //init graph
    graph = malloc(sizeof(int*) * nodes + 1);
    link_len = malloc(sizeof(int) * nodes + 1);
    space_len = malloc(sizeof(int) * nodes + 1);
    for(int i=0; i<nodes; i++){
        link_len[i] = 0;
        space_len[i] = 2;
        graph[i] = malloc(sizeof(int) * 3);
    }

    //input links + make grath links
    int link1, link2; //tmp
    for(int i=0; i<links; i++){
        scanf("%d %d %d", &id_tmp, &link1, &link2);
        push_link(link1, link2);
        push_link(link2, link1);
    }

    //set request_need variable
    int req[requests][2]; // [0]: reqSrc, [1]: reqDst
    path_info req_path[requests][CT_MAX];
    queue = malloc(sizeof(int) * 9);
    heap = malloc(sizeof(path_info*) * (requests + 2));
    heap[0] = NULL;
    
    //input request
    for(int i=0; i<requests; i++){
        scanf("%d %d %d", &id_tmp, &req[i][0], &req[i][1]);
        
        //BFS
        bfs(req_path[i], req[i][0], req[i][1], i);
        
        //heap sort (push)
        push_heap(&req_path[i][0]);
    }
    
    //init accepted_requests & counter
    acc_req = malloc(sizeof(path_info*) * 3);
    int ct_reqPath[requests]; //counter of each request's path number
    for(int i=0; i<requests; i++) ct_reqPath[i]=0; //init
    
    //start make tree & maintain memory & choose acc_req
    while(h_size > 0){
        int tree_type;
        path_info *now_req = pop_heap();
        
        //try to store memory, move up one time (t+1), until success or fail
        for(int i=0; i <= time - now_req->t_complete; i++){
            //compare 4 corner's 2*2's smallest memory
            tree_type = corner_compare(memory, now_req, i);
            
            //if tree cannot be build, continue
            if(tree_type == 0) continue;
            
            //get tree_height & make tree
            int tree_height;
            (tree_type > 2) ? (tree_height = now_req->t_linear) : (tree_height = now_req->t_complete);
            now_req->t_root = make_tree(now_req->len, tree_height, tree_type);
            
            //init time_bar
            now_req->time_bar = malloc(sizeof(int*) * tree_height);
            for(int j=0; j<tree_height; j++) now_req->time_bar[j] = calloc(now_req->len, sizeof(int));
            
            //make personal time_bar
            make_bar(now_req, now_req->t_root);
            
            //if memory is enough to time_bar, update memory
            int enough = set_memory(memory, now_req, i);
            
            //if memory is enough, save now_req in acc_req
            if(enough){
                //if full, realloc
                if(acc_size >= acc_space){
                    acc_space *= 2;
                    acc_req = realloc(acc_req, sizeof(path_info*) * acc_space + 1);
                }
                //push
                acc_req[acc_size++] = now_req;
                
                break; //move to next accepted_request
            }
        }
        
        //if request is not accepted 
        if(acc_size == 0 || acc_req[acc_size-1] != now_req){
            //change path
            ct_reqPath[now_req->req_id]++;
            //if path is exist & not NULL, push request back to heap
            if(ct_reqPath[now_req->req_id] < CT_MAX && req_path[now_req->req_id][ct_reqPath[now_req->req_id]].len != 0)
                push_heap(&req_path[now_req->req_id][ct_reqPath[now_req->req_id]]);
        }
    }
    
    //print
    printf("%d\n", acc_size); //AccReq
    for(int i=0; i<acc_size; i++){
        printf("%d ", acc_req[i]->req_id); //ReqID
        for(int j=acc_req[i]->len-1; j>=0; j--)
            printf("%d ", acc_req[i]->path[j]); //ReqSrc -> ReqDst
        printf("\n");

        //tree stucture
        print_tree(acc_req[i], acc_req[i]->t_root);
    }
    
    //free malloc space
    free(graph);
    free(queue);
    free(heap);
    free(acc_req);
}

//floor( log(n)/log(2) )
int floor_log(int n){
    if(n == 0) return -1;
    int j = 0;
    for(int i=n; i>1; i/=2) j++;
    return j;
}

//the biggest 2^x less than n
int big2(int n){
    int i = 0;
    for(i=1; i*2<n; i*=2);
    return i;
}

//create and setting new node
node* set_node(int a, int b, int t, node* L, node* R){
    node* temp = malloc(sizeof(node));
    temp->a = a;
    temp->b = b;
    temp->t = t;
    temp->Lchild = L;
    temp->Rchild = R;
    return temp;
}

//make left complete tree, using recursive (VLR)
node* left_complete(int a, int b, int t){
    node* n = set_node(a, b, t, NULL, NULL);
    if(b-a == 1) return n;
    int middle = big2(b-a) +a;
    n->Lchild = left_complete(a, middle, t-1);
    n->Rchild = left_complete(middle, b, t-1);
    return n;
}

//make right complete tree, using recursive (VLR)
node* right_complete(int a, int b, int t){
    node* n = set_node(a, b, t, NULL, NULL);
    if(b-a == 1) return n;
    int middle = b - big2(b-a);
    n->Lchild = right_complete(a, middle, t-1);
    n->Rchild = right_complete(middle, b, t-1);
    return n;
}

//make S>D linear tree, using recursive [V(R)L]
node* SD_linear(int a, int b, int t){
    node* n = set_node(a, b, t, NULL, NULL);
    if(b-a == 1) return n;
    n->Rchild = set_node(b-1, b, t-1, NULL, NULL);
    n->Lchild = SD_linear(a, b-1, t-1);
    return n;
}

//make D>S linear tree, using recursive [V(L)R]
node* DS_linear(int a, int b, int t){
    node* n = set_node(a, b, t, NULL, NULL);
    if(b-a == 1) return n;
    n->Lchild = set_node(a, a+1, t-1, NULL, NULL);
    n->Rchild = DS_linear(a+1, b, t-1);
    return n;
}

//strategy tree
//n: S>D nodes count; t: root's time slot
//type: 1. left complete, 2. right complete, 3. S>D linear, 4. D>S linear
node* make_tree(int n, int t, int type){
    switch(type){
        case 1: //left complete
            return left_complete(1, n, t);
            break;
        case 2: //right complete
            return right_complete(1, n, t);
            break;
        case 3: //S>D linear (left-down right-up)
            return SD_linear(1, n, t);
            break;
        case 4: //D>S linear (right-down left-up)
            return DS_linear(1, n, t);
            break;
    }
    return 0;
}

//push link to graph
void push_link(int id, int num){
    //if space is full, realloc
    if(link_len[id] >= space_len[id]){
        space_len[id] *= 2;
        graph[id] = realloc(graph[id], sizeof(int) * space_len[id] + 1);
    }
    //save num
    graph[id][link_len[id]++] = num;
}

//push num in queue
void push_queue(int num){
    //if queue is full, realloc
    if((q_rear + 1) % q_size == q_front){
        int move_end = q_size; //original size
        q_size *= 2;
        queue = realloc(queue, sizeof(int) * q_size + 1);
        //if front isn't 0, than move
        if((q_front) != 0){
            for(int i=q_front; i<move_end; i++) queue[i+move_end]=queue[i];
            q_front += move_end; //reset front
        }
    }
    //set rear & push num
    queue[q_rear] = num;
    q_rear = (q_rear + 1) % q_size;
}

//pop num in queue
int pop_queue(){
    //if empty
    if(q_front == q_rear) return -1;
    //return num & set front
    int num = queue[q_front];
    if(q_front == q_size-1) q_front -= q_size;
    q_front++;
    return num;
}

//use bfs to get paths (from src to dst)
void bfs(path_info *req_path, int src, int dst, int id){
    //init
    int recorder[nodes]; //record path and visit yet
    for(int i=0; i<nodes; i++) recorder[i]=UnVisit;
    recorder[src] = -1;
    push_queue(src);

    //bfs start
    int path_count=0, vertex;
    //while queue is not empty
    while((vertex = pop_queue()) != -1){
        
        //push vertex's neighbor in queue
        for(int i=0; i<link_len[vertex]; i++){
            
            //if neighbor is unvisit
            if(recorder[graph[vertex][i]] == UnVisit){
                
                //find path to dst
                if(graph[vertex][i] == dst){
                    
                    //set path_info
                    req_path[path_count].req_id = id;
                    req_path[path_count].len = 1;
                    req_path[path_count].path = malloc(sizeof(int) * nodes + 1);
                    req_path[path_count].path[0] = dst;
                    
                    //save path in reverse (dst -> src)
                    int now = vertex;
                    while(recorder[now] >= 0){
                        //save now vertex in path
                        req_path[path_count].path[req_path[path_count].len++] = now;
                        now = recorder[now];
                    }
                    //store src
                    req_path[path_count].path[req_path[path_count].len++] = src;
                    
                    //calculate two type's tree height
                    int complete_height = floor_log(req_path[path_count].len - 2) +3;
                    if(complete_height > time) break; //if complete_tree_height > time_slot, break & set NULL
                    req_path[path_count].t_complete = complete_height;
                    req_path[path_count].t_linear = req_path[path_count].len;
                    
                    path_count++;
                    //if save CT_MAX req_path, than break
                    if(path_count == CT_MAX) break;
                    continue;
                }
                
                recorder[graph[vertex][i]] = vertex;
                push_queue(graph[vertex][i]);
            }
        }
        
        if(path_count == CT_MAX) break;
    }
    
    //if path less than CT_MAX, set NULL to the rest path
    if(path_count < CT_MAX) req_path[path_count].len = 0;
}

void Swap(path_info **a, path_info **b){
    path_info *tmp = *a;
    *a = *b;
    *b = tmp;
}

//push p (req_id) in heap
void push_heap(path_info *p){
    //push num
    heap[++h_size] = p;
    
    //modify heap array
    int pos = h_size; //tmp_num position
    while(pos != 1){
        int parent = pos/2;
        if(heap[pos]->len < heap[parent]->len){
            Swap(&heap[pos], &heap[parent]);
            pos = parent;
        }
        else break;
    }
}

//pop num (top) from heap
path_info* pop_heap(){
    //if empty
    if(h_size == 0) return NULL;
    
    //get num (top)
    path_info *num = heap[1];

    //modify heap array
    heap[1] = heap[h_size--];
    int pos = 1; //tmp_modify_num position
    while(pos*2 <= h_size){
        int Lkid = pos*2, Rkid = pos*2 + 1; //Left, Right
        
        //if only left kid
        if(Lkid == h_size){
            if(heap[Lkid]->len < heap[pos]->len) Swap(&heap[Lkid], &heap[pos]);
            break;
        }

        //compare two kid & swap if fit
        int small_kid;
        (heap[Lkid]->len < heap[Rkid]->len) ? (small_kid = Lkid) : (small_kid = Rkid);
        if(heap[small_kid]->len < heap[pos]->len){
            Swap(&heap[small_kid], &heap[pos]);
            pos = small_kid;
        }
        else break;
    }

    return num;
}

//compare memory's 4 corner 2*2 block's smallest_memory_left
int corner_compare(int memory[time][nodes], path_info* p, int now_t){
    int id[4] = {p->path[p->len - 1], p->path[p->len - 2], p->path[1], p->path[0]};
    //[0]~[3]: left_up, right_up, left_down, right_down
    int mem_left[4] = {9999, 9999, 9999, 9999}; //smallest_memory_left in each corner's 2*2 block
    
    //calculate left_down, right_down
    for(int i=0; i<2; i++){
        if(memory[0][id[0+i*2]] < mem_left[i+2]) mem_left[i+2] = memory[0][id[0+i*2]];
        if(memory[0][id[1+i*2]] < mem_left[i+2]) mem_left[i+2] = memory[0][id[1+i*2]];
        if(memory[1][id[0+i*2]] < mem_left[i+2]) mem_left[i+2] = memory[1][id[0+i*2]];
        if(memory[1][id[1+i*2]] < mem_left[i+2]) mem_left[i+2] = memory[1][id[1+i*2]];
    }
    
    //if linear_tree has enough space height to build, calculate left_up, right_up
    if(time - now_t >= p->t_linear && p->t_linear > 4){
        int y = now_t + p->t_linear - 3;
        for(int i=0; i<2; i++){
            if(memory[y][id[0+i*2]] < mem_left[i]) mem_left[i] = memory[y][id[0+i*2]];
            if(memory[y][id[1+i*2]] < mem_left[i]) mem_left[i] = memory[y][id[1+i*2]];
            if(memory[y+1][id[0+i*2]] < mem_left[i]) mem_left[i] = memory[y+1][id[0+i*2]];
            if(memory[y+1][id[1+i*2]] < mem_left[i]) mem_left[i] = memory[y+1][id[1+i*2]];
        }
    }
    else mem_left[0] = 0, mem_left[1] = 0;
    
    //1. left complete, 2. right complete, 3. S>D linear, 4. D>S linear
    int tree_type = 0; // 0: the rest memory cannot build 4 types tree
    //[1]: complete, [2]: SD linear, [3]: DS linear
    int type_ct[4] = {0, MIN(mem_left[2], mem_left[3]), MIN(mem_left[1], mem_left[2]), MIN(mem_left[0], mem_left[3])};
    
    //compare and choose type
    for(int i=2; i<5; i++){
        if(type_ct[i-1] != 0 && type_ct[i-1] > type_ct[0]){
            tree_type = i;
            type_ct[0] = type_ct[i-1];
        }
    }
    
    //if(type = complete), than choose direction
    if(tree_type == 2 && type_ct[1] == mem_left[3]) tree_type = 1;
    return tree_type;
}

//make personal time_memory_bar (using recursive VLR)
void make_bar(path_info *p, node *now){
    //V
    p->time_bar[now->t - 1][now->a - 1]++;
    p->time_bar[now->t - 1][now->b - 1]++;
    
    //if now is leaf (no child)
    if(now->Lchild == NULL){
        p->time_bar[now->t - 2][now->a - 1]++;
        p->time_bar[now->t - 2][now->b - 1]++;
        return;
    }

    //LR
    if(now->Lchild) make_bar(p, now->Lchild);
    if(now->Rchild) make_bar(p, now->Rchild);
}

//if(memory is enough to time_bar) update & return 1; otherwise, return 0;
//memory[time][nodes], p->time_bar[p->t_root->t][p->len]
int set_memory(int memory[time][nodes], path_info *p, int t_i){
    //check two array
    for(int i=0; i<(p->t_root->t); i++){
        for(int j=0; j<(p->len); j++){
            //if memory not enough
            if(memory[i + t_i][p->path[p->len-j-1]] < p->time_bar[i][j])
                return 0;
        }
    }

    //update memory array
    for(int i=0; i<(p->t_root->t); i++){
        for(int j=0; j<(p->len); j++)
            memory[i + t_i][p->path[p->len-j-1]] -= p->time_bar[i][j];
    }
    p->t_i = t_i;

    return 1;
}

//print by postorder (LRV)
void print_tree(path_info *p, node *n){
    //LR
    if(n->Lchild != NULL) print_tree(p, n->Lchild);
    if(n->Rchild != NULL) print_tree(p, n->Rchild);
    //V
    if(n->Lchild == NULL) // if no child (leaf)
        printf("%d %d %d\n", p->path[p->len - n->a], p->path[p->len - n->b], n->t + p->t_i);
    else
        printf("%d %d %d %d %d %d %d\n", p->path[p->len - n->a], p->path[p->len - n->b],
                p->path[p->len - n->Lchild->a], p->path[p->len - n->Lchild->b],
                p->path[p->len - n->Rchild->a], p->path[p->len - n->Rchild->b], n->t + p->t_i);
}

// // ***test*** print
// printf("memory:\n");
// for(int i=0; i<time; i++){
//     for(int j=0; j<nodes; j++)
//         printf("%d ", memory[i][j]);
//     printf("\n");
// }printf("\n");

// // ***test*** print
// printf("graph:\n");
// for(int i=0; i<nodes; i++){
//     printf("(%d) ", i);
//     for(int j=0; j<link_len[i]; j++)
//         printf("%d ", graph[i][j]);
//     printf("\n");
// }printf("\n");

// // *** test *** print
// printf("id: %d, len: %d, path: ", req_path[path_count]->req_id, req_path[path_count]->len);
// for(int j=req_path[path_count]->len-1; j>=0; j--) printf("%d ", req_path[path_count]->path[j]);
// printf("\n"); 
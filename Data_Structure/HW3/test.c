#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define P printf("line: %d.\n", __LINE__)
#define MIN(a, b) a>b ? b : a

//tree's node
struct node{
    int a, b, t; //left_num, right_num, time_slot
    struct node *Lchild, *Rchild;
};
typedef struct node node;

//request's path information
typedef struct{
    int req_id;
    int len, *path;
    int t_linear, t_complete; // linear/complete tree's height
    node *t_root; //tree root
    int **time_bar; //time_memory array
}path_info;



int *queue;
int q_front = 0, q_rear = 0, q_size = 2;

//push num in queue
void push_queue(int num){
    //if queue is full, realloc
    if((q_rear + 1) % q_size == q_front){
        int move_end = q_size; //original size
        q_size *= 2;
        queue = realloc(queue, sizeof(int) * q_size);
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

int main(){
    // queue = malloc(sizeof(int) * 2);
    
    int n = 0;

    while(n != -1){
        scanf("%d", &n);
        printf("cal: %d.\n", (int)floor(log2(n+1+n))+1);
    }
    

    // 2 5 7 16 19 10 4 8 13 17 15 1 0 18 11 6 9 12 14 3
    // 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
}



// //floor( log(n)/log(2) )
// int floor_log(int n){
//     int j=0;
//     for(int i=n; i>1; i/=2) j++;
//     return j;
// }

// //the biggest 2^x less than n
// int big2(int n){
//     int i=0;
//     for(i=1; i*2<n; i*=2);
//     return i;
// }

// //tree's node
// struct node{
//     int a, b, t; //left_num, right_num, time_slot
//     struct node *Lchild, *Rchild;
// };
// typedef struct node node;

// //create and setting new node
// node* set_node(int a, int b, int t, node* L, node* R){
//     node* temp = malloc(sizeof(node));
//     temp->a = a;
//     temp->b = b;
//     temp->t = t;
//     temp->Lchild = L;
//     temp->Rchild = R;
//     return temp;
// }

// //make left complete tree, using recursive (VLR)
// node* left_complete(int a, int b, int t){
//     node* n = set_node(a, b, t, NULL, NULL);
//     if(b-a == 1) return n;
//     int middle = big2(b-a) +a;
//     n->Lchild = left_complete(a, middle, t-1);
//     n->Rchild = left_complete(middle, b, t-1);
//     return n;
// }

// //make right complete tree, using recursive (VLR)
// node* right_complete(int a, int b, int t){
//     node* n = set_node(a, b, t, NULL, NULL);
//     if(b-a == 1) return n;
//     int middle = b - big2(b-a);
//     n->Lchild = right_complete(a, middle, t-1);
//     n->Rchild = right_complete(middle, b, t-1);
//     return n;
// }

// //make S>D linear tree, using recursive [V(R)L]
// node* SD_linear(int a, int b, int t){
//     node* n = set_node(a, b, t, NULL, NULL);
//     if(b-a == 1) return n;
//     n->Rchild = set_node(b-1, b, t-1, NULL, NULL);
//     n->Lchild = SD_linear(a, b-1, t-1);
//     return n;
// }

// //make D>S linear tree, using recursive [V(L)R]
// node* DS_linear(int a, int b, int t){
//     node* n = set_node(a, b, t, NULL, NULL);
//     if(b-a == 1) return n;
//     n->Lchild = set_node(a, a+1, t-1, NULL, NULL);
//     n->Rchild = DS_linear(a+1, b, t-1);
//     return n;
// }

// //strategy tree
// //n: S>D nodes count; t: root's time slot
// //type: 1. left complete, 2. right complete, 3. S>D linear, 4. D>S linear
// node* make_tree(int n, int t, int type){
//     switch(type){
//         case 1: //left complete
//             return left_complete(1, n, t);
//             break;
//         case 2: //right complete
//             return right_complete(1, n, t);
//             break;
//         case 3: //S>D linear (left-down right-up)
//             return SD_linear(1, n, t);
//             break;
//         case 4: //D>S linear (right-down left-up)
//             return DS_linear(1, n, t);
//             break;
//     }
// }

path_info **heap;
int h_size = 0; //heap_size, heap[0] will not be use when sort

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
    while(pos > 1){
        int parent = pos/2;
        if(heap[pos]->len < heap[parent]->len){
            printf("before: %2d %2d.  ", heap[pos]->len, heap[parent]->len);
            Swap(&heap[pos], &heap[parent]);
            printf("after: %2d %2d.\n", heap[pos]->len, heap[parent]->len);
            pos = parent;
        }
        else break;
    }

    // for(int i=1; i<=h_size; i++) printf("%d ", heap[i]->len);
    // printf("\n");
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
            if(heap[Lkid]->len < heap[pos]->len) Swap(&heap[Lkid] ,&heap[pos]);
            break;
        }

        //compare two kid & swap if fit
        int small_kid;
        (heap[Lkid]->len < heap[Rkid]->len) ? (small_kid = Lkid) : (small_kid = Rkid);
        if(heap[small_kid]->len < heap[pos]->len){
            Swap(&heap[small_kid] ,&heap[pos]);
            pos = small_kid;
        }
        else break;
    }

    return num;
}
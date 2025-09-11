#include<iostream>
#include<vector>
#include<queue>
#include<utility>
#include<algorithm>

using namespace std;

class LQB;

//physical qubit class
class PQB{
  public:
    int id; //ID number
    int neighbor; //neighbor count
    int n_n_num=0; //total of neighbor's neighbor count
    vector<int> p2p_length; //this pqb to others' length
    vector<class PQB*> *p2p_path; //this pqb to others' shortest path in BFS
    class LQB* map_q; //this p map q in initialize
};

//logical qubit class
class LQB{
  public:
    int id; //ID number
    int ex_num=0; //count lqb exist time before (p-link pair) gateID
    vector<int> ex_id; //lqb exist in which gate ID
    class PQB* map_p; //this lqb mapping with which pqb now
};

//do phyQubits time BFS to find all p2p length & shortest path
void BFS(int p_count, PQB *p, vector<PQB*> *graph){
  queue<PQB*> que;

  //BFS p_count time
  for(int i=1; i<=p_count; i++){
    char visited[p_count+1] = {0}; //check if p has visited
    visited[i] = 1;
    p[i].p2p_length[i] = 0;
    que.push(&p[i]); //push root

    //BFS
    while(!que.empty()){
      //set up
      PQB* now = que.front();
      que.pop();
      int now_id = now->id;
      int now_size = graph[now_id].size();

      //push queue & set up p2p length/path
      for(int j=0; j<now_size; j++){
        PQB* get_p = graph[now_id][j];
        
        //see unvisited
        if(!visited[get_p->id]){
          visited[get_p->id] = 1; //set visited
          que.push(get_p); //push que
          get_p->p2p_length[i] = now->p2p_length[i] + 1; //set length
          
          //set path (p2p from back to front)
          get_p->p2p_path[i] = now->p2p_path[i];
          get_p->p2p_path[i].push_back(now);
        }

        //set p's total neighbor's neighbor count
        // if(i==1) now->n_n_num += get_p->neighbor;
      }
    }
  }
}

//swap two qubit and cout
void swap_gate(LQB* q1, PQB* p2){
  //swap
  PQB *p1 = q1->map_p;
  LQB *q2 = p2->map_q;
  q1->map_p = p2;
  q2->map_p = p1;
  p1->map_q = q2;
  p2->map_q = q1;

  //output
  cout<< "SWAP q"<< q1->id<< " q"<< q2->id<< endl;
}

//execute CNOT or SWAP to finish the mission
void gate_execute(PQB* p, LQB* q, int gates, pair<int, int> *gateID){
  //do gates pair time
  for(int i=1; i<=gates; i++){
    int p2p_len = q[gateID[i].first].map_p->p2p_length[q[gateID[i].second].map_p->id];
    if(p2p_len > 1){
      //reduce name
      LQB* qfst = &q[gateID[i].first];
      LQB* qsec = &q[gateID[i].second];

      //check route (choose farest gateID's pair)
      int check_count = p2p_len-2;
      // int farest = 0, farestID = 0;
      // for(int j=1; j<=check_count; j++){
      //   //compare two side x<-O->x
      //   if(j!=check_count){
      //     //set up
      //     j++;
      //     LQB* now_q = qfst->map_p->p2p_path[qsec->map_p->id][j]->map_q;
      //     int size = now_q->ex_id.size();
      //     int index = get_index(now_q->ex_id, i, size);
      //     int targetID[2] = {q[qfst->ex_id[j-1]].id, q[qfst->ex_id[j+1]].id};
      //     int found_ct = 0;

      //     // compare the farest
      //     for(int k=index+1; k<size; k++){
      //       if(found_ct != 1 && (gateID[qfst->ex_id[k]].first == targetID[0] || gateID[qfst->ex_id[k]].second == targetID[0])){
      //         if(qfst->ex_id[k] > farest){
      //           farest = qfst->ex_id[k];
      //           farestID = j-1;
      //         }
      //         if(found_ct == 2) break;
      //         else{
      //           found_ct++;
      //           continue;
      //         }
      //       }
      //       else if(found_ct != 2 && (gateID[qfst->ex_id[k]].first == targetID[1] || gateID[qfst->ex_id[k]].second == targetID[1])){
      //         if(qfst->ex_id[k] > farest){
      //           farest = qfst->ex_id[k];
      //           farestID = j;
      //         }
      //         if(found_ct == 1) break;
      //         else{
      //           found_ct+=2;
      //           continue;
      //         }
      //       }
      //       else if(k==size-1){ // not found at the rest
      //         farest = 999999;
      //         farestID = j;
      //       }
      //     }
      //   }

      //   //compare one (the rest) side x<-O
      //   else{
      //     LQB* now_q = qfst->map_p->p2p_path[qsec->map_p->id][j]->map_q;
      //     int size = now_q->ex_id.size();
      //     int index = get_index(now_q->ex_id, i, size);
      //     int targetID = q[qfst->ex_id[j+1]].id;
      //     for(int k=index+1; k<size; k++){ // compare the farest
      //       if(gateID[qfst->ex_id[k]].first == targetID || gateID[qfst->ex_id[k]].second == targetID){
      //         if(qfst->ex_id[k] > farest){
      //           farest = qfst->ex_id[k];
      //           farestID = j;
      //         }
      //         break;
      //       }
      //       else if(k==size-1){ // not found at the rest
      //         farest = 999999;
      //         farestID = j;
      //       }
      //     }
      //   }
      //   if(farest==999999) break;
      // }

      //swap
      int path_size = qfst->map_p->p2p_path[qsec->map_p->id].size();
      int farestID = path_size/2; //tmp set in middle
      PQB* start_pf = qfst->map_p, *start_ps = qsec->map_p;
      for(int j=path_size-1; j>farestID; j--)
        swap_gate(qfst, start_pf->p2p_path[start_ps->id][j]);
      for(int j=1; j<=farestID; j++)
        swap_gate(qsec, start_pf->p2p_path[start_ps->id][j]);
    }

    //output cnot gate
    cout<< "CNOT q"<< gateID[i].first<< " q"<< gateID[i].second<< endl;
  }

}

int main(){
  //input info
  int logQubits, gates, precedences, phyQubits, phyLinks;
  cin>> logQubits>> gates>> precedences>> phyQubits>> phyLinks;

  //set class
  PQB p[phyQubits+1]; //set all PQB members
  LQB q[logQubits+1]; //set all LQB members
  for(int i=1; i<=phyQubits; i++){
    p[i].id = i; //set pqb id number
    q[i].id = i; //set lqb id number
    p[i].p2p_length.reserve(phyQubits+1); //malloc p2p_length
    p[i].p2p_path = new vector<PQB*>[phyQubits+1]; //malloc p2p_path
  }

  //input detail
  pair<int, int> gateID[gates+1];
  pair<int, int> precedenceID[precedences+1];
  vector<PQB*> graph[phyQubits+1]; //store neighbors
  int tmp, ID1_tmp, ID2_tmp;
  //cin >> gateID
  for(int i=1; i<=gates; i++){
    cin>> tmp>> gateID[i].first>> gateID[i].second;
    // q[gateID[i].first].ex_id.push_back(i); //set q's exist id number
    // q[gateID[i].second].ex_id.push_back(i);

    //calculate exist_count before p-link pair
    // if(i<=phyLinks){
    //   q[gateID[i].first].ex_num++;
    //   q[gateID[i].second].ex_num++;
    // }
  }
  //cin>> precedenceID
  for(int i=1; i<=precedences; i++){
    cin>> tmp>> precedenceID[i].first>> precedenceID[i].second;
  }
  //cin>> phyLinkID
  for(int i=1; i<=phyLinks; i++){
    cin>> tmp>> ID1_tmp>> ID2_tmp;
    graph[ID1_tmp].push_back(&p[ID2_tmp]);
    graph[ID2_tmp].push_back(&p[ID1_tmp]);
  }

  //set pqb's neighbor number
  // for(int i=1; i<=phyQubits; i++) p[i].neighbor = graph[i].size();

  //use BFS to find p2p length & shortest path
  BFS(phyQubits, p, graph);

  //mapping <LQB, PQB>---------------------------------------------vvv

  //sorting LQB with ex_num
  // LQB* sorted_q[logQubits+1];
  // for(int i=1; i<=logQubits; i++) sorted_q[i] = &q[i];
  // sort(sorted_q+1, sorted_q+logQubits+1, q_cmp);
  // // //change in pair
  // // pair<int, LQB*> sort_q[logQubits+1];
  // // for(int i=1; i<=logQubits; i++)
  // //   sort_q[i] = make_pair(i, sorted_q[i]);

  // //sorting PQB with n_n_num
  // PQB* sorted_p[phyQubits+1];
  // for(int i=1; i<=phyQubits; i++) sorted_p[i] = &p[i];
  // sort(sorted_p+1, sorted_p+phyQubits+1, p_cmp);
  // // //change in pair
  // // pair<int, PQB*> sort_p[phyQubits+1];
  // // for(int i=1; i<=phyQubits; i++)
  // //   sort_p[i] = make_pair(i, sorted_p[i]);

  // //mapping <lqb, pqb>
  // mapping_pq(phyQubits, min(gates, phyLinks), sorted_p, sorted_q, graph, gateID, q);

  //mapping in baseline
  for(int i=1; i<=phyQubits; i++){
    p[i].map_q = &q[i];
    q[i].map_p = &p[i];
  }

  // cout << "lqb&pqb_mapping:\n";
  for(int i=1; i<=logQubits; i++)
    cout<< i<< " "<< q[i].map_p->id<< endl;
  
  //execute two-bit gate (CNOT & SWAP)
  gate_execute(p, q, gates, gateID);
}




  //test p2p_length/ path(from back to front)
  // for(int i=1; i<=phyQubits; i++){
  //   cout<< "p["<< i<< "]:\n  p2p_length: ";
  //   for(int j=1; j<=phyQubits; j++) cout<< p[i].p2p_length[j]<< " ";
  //   cout<< "\n  p2p_path:\n";
  //   for(int j=1; j<=phyQubits; j++){
  //     cout<< "    "<< j<< ": ";
  //     for(int k=p[i].p2p_path[j].size()-1; k>=0; k--) cout<< p[i].p2p_path[j][k]->id<< " ";
  //     cout<< "\n";
  //   }
  // }

  //test lqb's ex_id
  // for(int i=1; i<=logQubits; i++){
  //   cout<< "q["<< i<< "]: ";
  //   for(int j=0; j<q[i].ex_id.size(); j++)
  //     cout<< q[i].ex_id[j]<< " ";
  //   cout<< endl;
  // }

  //test sorted_q
  // cout<< "sorted_q:\n  ex_num: ";
  // for(int i=1; i<=logQubits; i++)
  //   cout<< sorted_q[i]->ex_num<< " ";
  // cout<< endl;

  //test sorted_p
  // cout<< "sorted_p:\n  id,n_n_num:";
  // for(int i=1; i<=logQubits; i++)
  //   cout<< " "<< sorted_p[i]->id<< ","<< sorted_p[i]->n_n_num;
  // cout<< endl;

  //test (graph &) p's n_n_num
  // cout<< "graph:\n";
  // for(int i=1; i<=phyQubits; i++){
  //   cout<< "  i="<< i<< "|n="<< p[i].neighbor<< ":";
  //   for(int j=0; j<graph[i].size(); j++)
  //     cout<< " "<< graph[i][j]->id;
  //   cout<< endl;
  // }
  // cout<< "n_n_num:\n  ";
  // for(int i=1; i<=phyQubits; i++)
  //   cout<< p[i].n_n_num<< " ";
  // cout<< endl;



//  --------------------------------------------------
//sorting q's ex_num comparison
// class comp_q{
//   public:
//     bool operator() (LQB* q1, LQB* q2){return q1->ex_num > q2->ex_num;}
// }q_cmp;
// //sorting p's n_n_num comparison
// class comp_p{
//   public:
//     bool operator() (PQB* p1, PQB* p2){return p1->n_n_num > p2->n_n_num;}
// }p_cmp;

// //priority sorting q's ex_num comparison
// class priority_q{
//   public:
//     bool operator() (LQB* q1, LQB* q2){return q1->ex_num < q2->ex_num;}
// };
// //priority sorting p's n_n_num comparison
// class priority_p{
//   public:
//     bool operator() (PQB* p1, PQB* p2){return p1->n_n_num < p2->n_n_num;}
// };

// //mapping LQB & PQB after sorting
// void mapping_pq(int p_count, int pair_num, PQB** sort_p, LQB** sort_q, vector<PQB*> *graph, pair<int, int> *gateID, LQB* q){
//   queue<PQB*> que;
//   PQB** sp_ptr = &sort_p[1]; //sorted_p pointer
//   LQB** sq_ptr = &sort_q[1]; //sorted_q pointer

//   char p_visited[p_count+1] = {0}; //check if p has visited
//   char q_matched[p_count+1] = {0}; //check if p has visited
//   int match_count = p_count; //count matching pair 
  
//   //similar to BFS
//   while(match_count){
//     //if queue is empty
//     if(que.empty()){
//       //set two ptr
//       while(p_visited[(*sp_ptr)->id]) sp_ptr++;
//       while(q_matched[(*sq_ptr)->id]) sq_ptr++;

//       //match the rest's first one in two sorted_array
//       que.push(*sp_ptr);
//       p_visited[(*sp_ptr)->id] = 1;
//       q_matched[(*sq_ptr)->id] = 1;
//       match_count--;
//       (*sq_ptr)->map_p = (*sp_ptr);
//       (*sp_ptr)->map_q = (*sq_ptr);
      
//       //set p's neighbor & n_n_num
//       int g_size = graph[(*sp_ptr)->id].size();
//       for(int i=0; i<g_size; i++){
//         //if un-visited -> modify
//         if(!p_visited[graph[(*sp_ptr)->id][i]->id]){
//           graph[(*sp_ptr)->id][i]->neighbor--;
//           graph[(*sp_ptr)->id][i]->n_n_num -= (*sp_ptr)->neighbor;
//         }
//       }
//     }
    
//     //set queue
//     PQB* now = que.front();
//     LQB* now_q = now->map_q;
//     que.pop();
    
//     //sort prepared p for matching
//     priority_queue<PQB*, vector<PQB*>, priority_p> p_neibr;
//     int g_size = graph[now->id].size();
//     for(int i=0; i<g_size; i++)
//       if(!p_visited[graph[now->id][i]->id]) p_neibr.push(graph[now->id][i]);
    
//     //sort prepared q for matching
//     priority_queue<LQB*, vector<LQB*>, priority_q> q_exnum;
//     int pair_ct = 0;
//     int exID_size = now_q->ex_id.size();
//     while(pair_ct < exID_size && now_q->ex_id[pair_ct] <= pair_num){
//       if(gateID[now_q->ex_id[pair_ct]].first == now_q->id && !q_matched[gateID[now_q->ex_id[pair_ct]].second])
//         q_exnum.push(&q[gateID[now_q->ex_id[pair_ct]].second]);
//       else if(gateID[now_q->ex_id[pair_ct]].second == now_q->id && !q_matched[gateID[now_q->ex_id[pair_ct]].first])
//         q_exnum.push(&q[gateID[now_q->ex_id[pair_ct]].first]);
//       else now_q->ex_num--;
      
//       pair_ct++; //count++
//     }
    
//     //matching
//     int count = min(now->neighbor, now_q->ex_num);
//     for(int i=0; i<count; i++){
//       //set priority_queue
//       if(p_neibr.empty() || q_exnum.empty()) break;
//       PQB* p_nbr = p_neibr.top();
//       LQB* q_nbr = q_exnum.top();
//       p_neibr.pop();
//       q_exnum.pop();
      
//       //set queue
//       que.push(p_nbr);
      
//       //match setting
//       p_visited[p_nbr->id] = 1;
//       q_matched[q_nbr->id] = 1;
//       match_count--;
//       q_nbr->map_p = p_nbr;
//       p_nbr->map_q = q_nbr;

//       //set p's neighbor & n_n_num
//       p_nbr->neighbor--;
//       p_nbr->n_n_num -= now->neighbor;
//     }
//   }
// }

// //get ex_ID index using binary search
// int get_index(vector<int> &v, int target, int size){
//   int left = 0, right = size-1, middle;
//   while(left<right){
//     middle = (left+right)/2;
//     if(v[middle] == target) return middle;
//     else if(v[middle] > target) right = middle - 1;
//     else left = middle + 1;
//   }
//   return -1;
// }
//--------------------------------------------------------------
#include <iostream>
#include <queue>
#include <vector>
#include <set>

using namespace std;

typedef pair<pair<unsigned, unsigned>, unsigned> puuu;

int main(){
    set<puuu> pq;

    pq.insert(make_pair(make_pair(20, 100), 20));
    pq.insert(make_pair(make_pair(10, 200), 100));
    pq.insert(make_pair(make_pair(20, 20), 200));
    pq.insert(make_pair(make_pair(10, 50), 150));
    pq.insert(make_pair(make_pair(20, 150), 50));

    for(auto it = pq.rbegin(); it != pq.rend(); it++){
        // puu top = pq.top();
        // pq.pop();

        cout << it->first.first << " " << it->first.second << " " << it->second << "\n";
    }

    // puu a1 = make_pair(10, 12), a2;
    // a2 = a1;

    // cout << a2.first << " " << a2.second << "\n";
}
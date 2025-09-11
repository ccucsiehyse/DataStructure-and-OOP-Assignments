#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class PQB {
public:
    int ID;
    vector<int> PQBIDs;
    unordered_map<int, int> sp; // Store shortest path
    int *parent;
};


void bfs(map<int, PQB> &PQBcount, int end) {
    queue<int> q;  // Queue of PQB IDs
    q.push(end);

    int nowID;
    while (!q.empty()) {
        nowID = q.front();
        q.pop();

        for (const auto &id : PQBcount[nowID].PQBIDs) {
            if (PQBcount[id].parent[end] == 0) {
                q.push(id);
                PQBcount[id].parent[end] = nowID;
            }
        }
    }
}


int main() {
    int logQubits, gates, precedences, phyQubits, phyLinks;

    cin >> logQubits >> gates >> precedences >> phyQubits >> phyLinks;
    
    vector<int> gatecount_first(gates+1), gatecount_second(gates+1);
    map<int, PQB> PQBcount;
    int PQB_LQB[phyQubits + 1];
    
    int gateID, logQubitID1, logQubitID2;
    for (int gate_num = 1; gate_num <= gates; gate_num++) {
        // cin >> gateID >> logQubitID1 >> logQubitID2;
        cin >> gateID >> gatecount_first[gate_num] >> gatecount_second[gate_num];
    }
    
    int precedenceID, gateID1, gateID2;
    for (int prece_num = 0; prece_num < precedences; prece_num++) {
        cin >> precedenceID >> gateID1 >> gateID2;
    }

    int phyLinkID, phyQubitID1, phyQubitID2;
    for (int link_num = 0; link_num < phyLinks; link_num++) {
        cin >> phyLinkID >> phyQubitID1 >> phyQubitID2;

        PQBcount[phyQubitID1].PQBIDs.push_back(phyQubitID2);
        PQBcount[phyQubitID2].PQBIDs.push_back(phyQubitID1);
    }

    for (int id = 1; id <= phyQubits; id++){
        PQBcount[id].ID = id;
        PQB_LQB[id] = id;
        PQBcount[id].parent = new int[phyQubits + 1]{0};
    }

    // Calculate shortest paths
    for (int j = 1; j <= phyQubits; j++) {
        bfs(PQBcount, j);
    }

    // Output gates
    for (int i = 1; i <= logQubits; i++) {
        cout << i << " " << i << "\n";
    }

    // Simulate the gate operations and SWAPs
    int num_2, nextgate, now_node, sp_next, temp;
    for (int gate_num = 1; gate_num <= gates; gate_num++) {
        num_2 = PQBcount[gatecount_second[gate_num]].ID;

        nextgate = 0;
        now_node = gatecount_first[gate_num];
        while (nextgate == 0) {
            sp_next = PQBcount[now_node].parent[num_2];
            
            if (sp_next == num_2) {
                cout << "CNOT q" << gatecount_first[gate_num] << " q" << gatecount_second[gate_num] << "\n";
                nextgate = 1;
            } else {
                cout << "SWAP q" << now_node << " q" << PQB_LQB[sp_next] << "\n";
                temp = PQBcount[now_node].ID;
                swap(PQBcount[now_node], PQBcount[PQB_LQB[sp_next]]);
                swap(PQB_LQB[temp], PQB_LQB[sp_next]);
            }
        }
    }

    return 0;
}
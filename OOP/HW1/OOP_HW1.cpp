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
    
    // PQB() : parent(nullptr) {}
    
    // PQB(int phyQubits) {
    //     parent = new int[phyQubits + 1];
    //     fill(parent, parent + phyQubits, -1);
    // }
    
    // ~PQB() {
    //     delete[] parent;
    // }

};

class Gate {
public:
    int precedence;
    int LQBID1;
    int LQBID2;
};

void bfs(map<int, PQB> &PQBcount, int end) {
    // cout << "bfs start.\n";

    queue<int> q;  // Queue of PQB IDs
    q.push(end);

    while (!q.empty()) {
        int nowID = q.front();
        q.pop();

        // cout << "in bfs.\n";

        for (const auto &id : PQBcount[nowID].PQBIDs) {
            if (PQBcount[id].parent[end] == -1) {
                q.push(id);
                PQBcount[id].parent[end] = nowID;
            }
        }
    }

    // cout << "bfs end.\n";
}


int main() {
    int logQubits, gates, precedences, phyQubits, phyLinks;

    cin >> logQubits >> gates >> precedences >> phyQubits >> phyLinks;
    
    vector<int> gatecount_first(gates+1), gatecount_second(gates+1);
    map<int, PQB> PQBcount;

    for (int gate_num = 1; gate_num <= gates; gate_num++) {
        int gateID, logQubitID1, logQubitID2;
        cin >> gateID >> logQubitID1 >> logQubitID2;
        gatecount_first[gate_num] = logQubitID1;
        gatecount_second[gate_num] = logQubitID2;
    }

    for (int prece_num = 0; prece_num < precedences; prece_num++) {
        int precedenceID, gateID1, gateID2;
        cin >> precedenceID >> gateID1 >> gateID2;
    }

    for (int link_num = 0; link_num < phyLinks; link_num++) {
        int phyLinkID, phyQubitID1, phyQubitID2;
        cin >> phyLinkID >> phyQubitID1 >> phyQubitID2;
        
        // if (PQBcount.find(phyQubitID1) == PQBcount.end()) {
        //     PQBcount.emplace(phyQubitID1, PQB(phyQubits));
        //     PQBcount[phyQubitID1].ID = phyQubitID1;
        // }
        // if (PQBcount.find(phyQubitID2) == PQBcount.end()) {
        //     PQBcount.emplace(phyQubitID2, PQB(phyQubits));
        //     PQBcount[phyQubitID2].ID = phyQubitID2;
        // }
        // cout << ".";

        PQBcount[phyQubitID1].ID = phyQubitID1;
        PQBcount[phyQubitID1].PQBIDs.push_back(phyQubitID2);
        PQBcount[phyQubitID2].ID = phyQubitID2;
        PQBcount[phyQubitID2].PQBIDs.push_back(phyQubitID1);
    }

    for (int id = 1; id <= phyQubits; id++){
        // PQBcount[id].ID = id;
        PQBcount[id].parent = new int[phyQubits + 1];
        for(int j = 0; j <= phyQubits; j++){
            PQBcount[id].parent[j] = -1;

            // cout << id << ". parent[" << j << "]: " << PQBcount[id].parent[j] << "\n";
        }
    }

    // cout << "TMP_HERE\n";

    // Calculate shortest paths
    // for (int i = 1; i <= phyQubits; i++) {
        for (int j = 1; j <= phyQubits; j++) {
            // unordered_set<int> neighbors(PQBcount[i].PQBIDs.begin(), PQBcount[i].PQBIDs.end());
            // if (neighbors.find(j) != neighbors.end() || i == j) {
            //     PQBcount[i].sp[j] = j;
            // } else {
            // cout << "bfs_count: " << j << ".\n";
            bfs(PQBcount, j);
            
                // PQBcount[i].parent[j] = shortest_path[shortest_path.size() - 2];
            // }
        }
    // }

    // Output gates
    for (int i = 1; i <= logQubits; i++) {
        cout << i << " " << i << endl;
    }

    // Simulate the gate operations and SWAPs
    for (int gate_num = 1; gate_num <= gates; gate_num++) {
        int num_1 = PQBcount[gatecount_first[gate_num]].ID;
        int num_2 = PQBcount[gatecount_second[gate_num]].ID;

        // cout << gatecount_first[gate_num] << " " << num_1 << "\n";
        // cout << gatecount_second[gate_num] << " " << num_2 << "\n";

        int nextgate = 0;
        int now_node = gatecount_first[gate_num];
        // for(int i=0; i<=phyQubits; i++) cout << num_1 << ". parent[" << i << "]: " << PQBcount[num_1].parent[i] << "\n";
        while (nextgate == 0) {
            // cout << "A" << endl;
            int sp_next = PQBcount[now_node].parent[num_2];
            // cout << sp_next << endl;
            // cout << "B" << endl;
            if (sp_next == num_2) {
                cout << "CNOT q" << gatecount_first[gate_num] << " q" << gatecount_second[gate_num] << endl;
                nextgate = 1;
            } else {
                // for (auto it = PQBcount.begin(); it != PQBcount.end(); ++it) {
                //     if (it->second.ID == sp_next) {
                        cout << "SWAP q" << now_node << " q" << sp_next << endl;
                        swap(PQBcount[now_node], PQBcount[sp_next]);
                //         break;
                //     }
                // }
            }
        }
    }

    return 0;
}
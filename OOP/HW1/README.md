# OOP HW1 - 量子位元映射與路由問題

## 作業描述
本作業實作量子位元映射與路由演算法，目標是將邏輯量子位元映射到實體量子位元，並最小化引入的SWAP操作數量，解決量子電路在實體量子裝置上的執行問題。

## 專案背景
- **量子位元(Qubit)**：量子計算的基本資訊單位，處於0和1的疊加態
- **量子電路**：表示量子程式的圖表，包含邏輯量子位元
- **實體量子裝置**：具有特定拓撲結構的物理量子處理器
- **映射挑戰**：邏輯量子位元需要映射到相互耦合的實體量子位元

## 專案目標
- **主要目標**：最小化引入的SWAP操作數量
- **核心挑戰**：確保雙量子位元閘中的邏輯量子位元映射到耦合的實體量子位元
- **成功條件**：找到初始映射和SWAP序列，使所有量子閘都能正確執行

## 問題分析
- 給定邏輯量子位元數量、實體量子位元數量、實體連結數量
- 處理雙量子位元閘及其優先順序
- 考慮實體量子裝置的拓撲結構限制
- 當邏輯量子位元未映射到耦合實體量子位元時，需要SWAP操作

## 實作方法

### 核心演算法：BFS路徑搜尋與智能映射

本實作採用**BFS路徑搜尋**結合**智能映射策略**來解決量子位元映射與路由問題。

#### 1. 類別設計架構
```cpp
// 實體量子位元類別
class PQB{
public:
    int id;                                    // ID號碼
    int neighbor;                              // 鄰居數量
    int n_n_num = 0;                          // 鄰居的鄰居總數
    vector<int> p2p_length;                   // 到其他PQB的距離
    vector<class PQB*> *p2p_path;             // 到其他PQB的最短路徑
    class LQB* map_q;                         // 映射到的邏輯量子位元
};

// 邏輯量子位元類別
class LQB{
public:
    int id;                                    // ID號碼
    int ex_num = 0;                           // 存在時間計數
    vector<int> ex_id;                        // 存在的閘ID列表
    class PQB* map_p;                         // 映射到的實體量子位元
};
```

**類別設計特點**：
- **封裝性**：每個類別管理自己的屬性和狀態
- **關聯性**：PQB和LQB之間建立雙向映射關係
- **擴展性**：預留了複雜演算法的擴展空間（如ex_num、ex_id等）

#### 2. BFS路徑搜尋系統
```cpp
// 為每個實體量子位元計算到其他所有PQB的最短路徑
void BFS(int p_count, PQB *p, vector<PQB*> *graph){
    queue<PQB*> que;
    
    for(int i=1; i<=p_count; i++){
        char visited[p_count+1] = {0};
        visited[i] = 1;
        p[i].p2p_length[i] = 0;
        que.push(&p[i]);
        
        while(!que.empty()){
            PQB* now = que.front();
            que.pop();
            // 遍歷鄰居，更新距離和路徑
            for(int j=0; j<graph[now->id].size(); j++){
                PQB* get_p = graph[now->id][j];
                if(!visited[get_p->id]){
                    visited[get_p->id] = 1;
                    que.push(get_p);
                    get_p->p2p_length[i] = now->p2p_length[i] + 1;
                    // 記錄路徑（從後往前）
                    get_p->p2p_path[i] = now->p2p_path[i];
                    get_p->p2p_path[i].push_back(now);
                }
            }
        }
    }
}
```

**路徑搜尋策略**：
- 為每個實體量子位元計算到其他所有PQB的最短路徑
- 使用廣度優先搜尋確保找到最短距離
- 路徑儲存方式：從目標往回追溯到源點的路徑

#### 3. 智能映射策略
```cpp
// 根據存在時間排序LQB
LQB* sorted_q[logQubits+1];
for(int i=1; i<=logQubits; i++) sorted_q[i] = &q[i];
sort(sorted_q+1, sorted_q+logQubits+1, q_cmp);

// 根據鄰居的鄰居數排序PQB
PQB* sorted_p[phyQubits+1];
for(int i=1; i<=phyQubits; i++) sorted_p[i] = &p[i];
sort(sorted_p+1, sorted_p+phyQubits+1, p_cmp);

// 執行智能映射
mapping_pq(phyQubits, min(gates, phyLinks), sorted_p, sorted_q, graph, gateID, q);
```

**映射策略**：
- **智能排序**：根據LQB的ex_num（存在時間）和PQB的n_n_num（鄰居的鄰居數）進行排序
- **複雜映射演算法**：使用`mapping_pq`函數進行智能映射
- **優先佇列匹配**：使用優先佇列進行最佳匹配
- **存在時間計算**：計算LQB在物理連結對之前的存在時間

#### 4. 智能SWAP策略
```cpp
// 檢查兩個邏輯量子位元是否相鄰
int p2p_len = q[gateID[i].first].map_p->p2p_length[q[gateID[i].second].map_p->id];
if(p2p_len > 1){
    // 智能路徑分析
    int farest = 0, farestID = 0;
    for(int j=1; j<=check_count; j++){
        // 複雜的路徑分析邏輯
        // 比較兩側的量子閘，選擇最遠的閘ID
        // 使用二分搜尋找到最佳SWAP位置
    }
    
    // 執行智能SWAP
    for(int j=p2p_len-1; j>farestID; j--)
        swap_gate(qfst, qfst->map_p->p2p_path[qsec->map_p->id]);
    for(int j=1; j<=farestID; j++)
        swap_gate(qsec, qsec->map_p->p2p_path[qfst->map_p->id]);
}
```

**SWAP策略**：
- **智能路徑分析**：分析路徑上的量子閘，選擇最佳SWAP位置
- **最遠閘ID選擇**：使用複雜的邏輯選擇最遠的量子閘ID
- **二分搜尋優化**：使用`get_index`函數進行二分搜尋
- **動態SWAP位置**：根據路徑分析結果動態決定SWAP位置

#### 5. 量子閘執行流程
```cpp
// 處理量子閘序列
for(int i=1; i<=gates; i++){
    // 檢查距離，如果大於1則需要SWAP
    int p2p_len = q[gateID[i].first].map_p->p2p_length[q[gateID[i].second].map_p->id];
    if(p2p_len > 1){
        // 執行SWAP操作
        // ... SWAP邏輯 ...
    }
    
    // 執行CNOT閘
    cout << "CNOT q" << gateID[i].first << " q" << gateID[i].second << endl;
}
```

**執行策略**：
- 按順序處理每個量子閘
- 檢查兩個量子位元是否相鄰（距離=1）
- 不相鄰時執行SWAP操作
- 最後執行CNOT閘

#### 6. 演算法特點
- **時間複雜度**：O(P² + G×P + M×log(G))，其中P是實體量子位元數，G是量子閘數，M是映射複雜度
- **空間效率**：預計算路徑資訊，避免重複搜尋
- **智能優化**：使用複雜的映射和SWAP策略，最大化量子閘執行效率
- **演算法複雜度**：結合排序、優先佇列、二分搜尋等多種演算法

### 實作細節

#### STL容器使用
```cpp
// 使用vector管理動態陣列
vector<int> p2p_length;
vector<class PQB*> *p2p_path;
vector<int> ex_id;

// 使用queue進行BFS搜尋
queue<PQB*> que;
```

#### 記憶體管理
```cpp
// 使用new和delete進行記憶體管理
PQB* p = new PQB[p_count + 1];
LQB* l = new LQB[l_count + 1];

// 釋放記憶體
delete[] p;
delete[] l;
```

#### 路徑重建
```cpp
// 重建從源點到目標的最短路徑
vector<int> reconstructPath(int src, int dst, vector<int>& parent){
    vector<int> path;
    int current = dst;
    while(current != src){
        path.push_back(current);
        current = parent[current];
    }
    path.push_back(src);
    reverse(path.begin(), path.end());
    return path;
}
```

## 輸入格式
```
邏輯量子位元數量 實體量子位元數量 實體連結數量 量子閘數量
實體連結資訊 (連結ID 端點1 端點2)
量子閘資訊 (閘ID 邏輯量子位元1 邏輯量子位元2)
```

## 輸出格式
```
初始映射 (邏輯量子位元ID 實體量子位元ID)
量子閘執行序列 (包含SWAP操作)
```

## 範例
**輸入**：
```
4 6 5 4 4
1 1 2
2 3 4
3 2 4
4 2 3
5 3 4
6 1 4
1 1 3
2 2 3
3 3 4
4 4 5
5 5 6
1 1 2
2 1 3
3 2 4
4 3 4
```

## 評分標準
- **成績與SWAP操作數量成反比**：SWAP次數越少，分數越高
- **基本分數**：60分（在截止日期前提交）
- **績效排名獎勵**：
  - [0%, 30%)：+0分
  - [30%, 50%)：+5分
  - [50%, 75%)：+10分
  - [75%, 85%)：+15分
  - [85%, 90%)：+20分
  - [90%, 95%)：+25分
- **作業助理獎勵**：+10分（超級截止日期前提交）

## 檔案說明
- `411410006_hw1.cpp` - 主要實作檔案
- `samples-HW1/` - 測試資料夾
- `program1.pdf` - 作業說明文件
- `sample2.png` - 範例圖示

## 技術重點
- **物件導向程式設計**：PQB和LQB類別的設計與實作
- **BFS路徑搜尋**：為每個實體量子位元計算最短路徑
- **智能映射策略**：基於存在時間和鄰居數的複雜映射演算法
- **智能SWAP策略**：使用路徑分析和二分搜尋的SWAP優化
- **STL容器應用**：vector、queue、pair、priority_queue等標準容器
- **演算法整合**：排序、優先佇列、二分搜尋等多種演算法結合
- **記憶體管理**：混合使用陣列和動態分配
- **量子計算概念**：理解量子位元和量子閘的基本原理
- **複雜度優化**：多層次的演算法優化策略

# Data Structure HW3 - 糾纏路由與數值學選擇

## 作業描述
本作業實作量子網路中的糾纏路由與數值學選擇演算法，目標是在量子網路環境中最大化接受的源點-目的地(SD)請求數量，解決複雜的量子資源分配問題。

## 專案背景
- **量子網路**：由多個節點組成，每個節點具有有限的量子記憶單元
- **量子通道**：節點間透過有限數量的量子通道(如光纖)連接
- **糾纏交換**：在不讓中繼器知情的情況下傳輸資料量子位元
- **策略樹**：不同的排程方式產生不同的二元樹和數值學

## 專案目標
- **主要目標**：最大化接受的源點-目的地(SD)請求數量
- **核心挑戰**：在量子記憶體限制下，為每個請求找到路徑和策略樹
- **成功條件**：所有被接受的請求都能找到可行的路徑和數值學

## 問題分析
- 給定量子網路拓撲、節點記憶體限制、時間槽數量
- 處理多個SD請求，每個請求需要路徑和策略樹
- 使用糾纏交換技術建立長距離量子連線
- 考慮記憶體單元限制和時間槽分配

## 實作方法

### 核心演算法：多策略樹優化與四角記憶體比較

本實作採用**多策略樹優化**結合**四角記憶體比較**和**動態路徑切換**來最大化接受的量子請求數量。

#### 1. 多策略樹建構系統
```c
// 四種策略樹類型
node* make_tree(int n, int t, int type){
    switch(type){
        case 1: return left_complete(1, n, t);    // 左完全樹
        case 2: return right_complete(1, n, t);   // 右完全樹
        case 3: return SD_linear(1, n, t);       // S>D線性樹
        case 4: return DS_linear(1, n, t);        // D>S線性樹
    }
}
```

**策略樹類型**：
- **左完全樹**：優先處理路徑左側節點
- **右完全樹**：優先處理路徑右側節點
- **S>D線性樹**：從源點到目的地的線性策略
- **D>S線性樹**：從目的地到源點的線性策略

#### 2. 四角記憶體比較演算法
```c
// 比較四個角落的2x2記憶體區塊
int corner_compare(int memory[time][nodes], path_info* p, int now_t){
    int id[4] = {p->path[p->len - 1], p->path[p->len - 2], 
                 p->path[1], p->path[0]};
    // [0]~[3]: left_up, right_up, left_down, right_down
    int mem_left[4] = {9999, 9999, 9999, 9999};
}
```

**記憶體優化策略**：
- 分析路徑四個關鍵節點的記憶體狀況
- 比較2x2記憶體區塊的最小可用記憶體
- 根據記憶體狀況選擇最適合的策略樹類型

#### 3. 動態路徑切換機制
```c
// 為每個請求儲存多條路徑
path_info req_path[requests][CT_MAX];  // CT_MAX=30
int ct_reqPath[requests];  // 每個請求的路徑計數器
```

**路徑管理**：
- 每個請求最多儲存30條不同路徑
- 當當前路徑無法滿足記憶體需求時，自動切換到下一個路徑
- 使用BFS找到所有可能的最短路徑

#### 4. 旋轉佇列BFS實作
```c
// 使用旋轉陣列實作BFS佇列
int *queue;
int q_front = 0, q_rear = 0, q_size = 8;

void push_queue(int num){
    if((q_rear + 1) % q_size == q_front){
        q_size *= 2;  // 動態擴展佇列
        queue = realloc(queue, sizeof(int) * q_size + 1);
    }
    queue[q_rear] = num;
    q_rear = (q_rear + 1) % q_size;
}
```

**BFS優化**：
- 使用旋轉陣列避免頻繁的記憶體重新分配
- 動態擴展佇列大小，適應不同規模的網路
- 智能處理佇列滿的情況

#### 5. 時間記憶體條管理
```c
// 為每個策略樹建立個人時間記憶體條
void make_bar(path_info *p, node *now){
    p->time_bar[now->t - 1][now->a - 1]++;
    p->time_bar[now->t - 1][now->b - 1]++;
    
    if(now->Lchild) make_bar(p, now->Lchild);
    if(now->Rchild) make_bar(p, now->Rchild);
}
```

**記憶體條策略**：
- 為每個策略樹節點計算所需的記憶體使用量
- 使用遞迴遍歷建立完整的時間記憶體條
- 精確追蹤每個時間槽的記憶體需求

#### 6. 優先權堆積排序
```c
// 使用最小堆積排序請求
void push_heap(path_info *p){
    heap[++h_size] = p;
    int pos = h_size;
    while(pos != 1){
        int parent = pos/2;
        if(heap[pos]->len < heap[parent]->len){
            Swap(&heap[pos], &heap[parent]);
            pos = parent;
        }
        else break;
    }
}
```

**排序策略**：
- 根據路徑長度進行優先權排序
- 較短的路徑優先處理，提高成功率
- 使用最小堆積確保高效能排序

#### 7. 演算法流程
1. **初始化**：讀取網路拓撲、節點記憶體、請求資訊
2. **多路徑搜尋**：為每個請求使用BFS找到最多30條路徑
3. **優先權排序**：使用堆積按路徑長度排序請求
4. **策略樹選擇**：使用四角記憶體比較選擇最佳策略樹
5. **記憶體分配**：檢查並分配記憶體，失敗則切換路徑
6. **動態切換**：當路徑無法滿足時，自動切換到下一個路徑

#### 8. 關鍵演算法優勢
- **多策略優化**：四種策略樹適應不同的記憶體分佈
- **智能記憶體管理**：四角比較確保最佳記憶體利用率
- **動態路徑切換**：提高請求接受率
- **高效能BFS**：旋轉佇列減少記憶體開銷

### 實作細節

#### 樹狀結構遞迴建構
```c
// 左完全樹遞迴建構
node* left_complete(int a, int b, int t){
    node* n = set_node(a, b, t, NULL, NULL);
    if(b-a == 1) return n;
    int middle = big2(b-a) + a;  // 計算分割點
    n->Lchild = left_complete(a, middle, t-1);
    n->Rchild = left_complete(middle, b, t-1);
    return n;
}
```

#### 動態圖形管理
```c
// 動態擴展圖形鄰接表
void push_link(int id, int num){
    if(link_len[id] >= space_len[id]){
        space_len[id] *= 2;
        graph[id] = realloc(graph[id], sizeof(int) * space_len[id] + 1);
    }
    graph[id][link_len[id]++] = num;
}
```

#### 記憶體檢查與更新
```c
// 檢查並更新記憶體分配
int set_memory(int memory[time][nodes], path_info *p, int t_i){
    // 檢查記憶體是否足夠
    for(int i=0; i<(p->t_root->t); i++){
        for(int j=0; j<(p->len); j++){
            if(memory[i + t_i][p->path[p->len-j-1]] < p->time_bar[i][j])
                return 0;  // 記憶體不足
        }
    }
    
    // 更新記憶體使用量
    for(int i=0; i<(p->t_root->t); i++){
        for(int j=0; j<(p->len); j++)
            memory[i + t_i][p->path[p->len-j-1]] -= p->time_bar[i][j];
    }
    return 1;  // 成功分配
}
```

## 輸入格式
```
nodes links time_slots requests  // 節點數、連結數、時間槽數、請求數
節點資訊 (節點ID 量子記憶單元數量)
連結資訊 (連結ID 端點1 端點2)
請求資訊 (請求ID 來源節點 目的節點)
```

## 輸出格式
```
接受的請求數量
每個請求的詳細資訊：
- 請求ID 來源 中繼節點序列 目的地
- 樹結構 (後序遍歷格式)
```

## 範例
**輸入**：
```
3 3 4 2
0 6
1 4
2 5
0 0 1
1 0 2
2 1 2
0 0 2
1 1 2
```

## 評分標準
- **成績與接受的SD請求數量成正比**：接受的請求越多，分數越高
- **基本分數**：60分（在截止日期前提交）
- **績效排名獎勵**：
  - [0%, 30%)：+0分
  - [30%, 50%)：+5分
  - [50%, 75%)：+10分
  - [75%, 85%)：+15分
  - [85%, 90%)：+20分
  - [90%, 95%)：+25分
  - [95%, 100%]：+30分
- **作業助理獎勵**：超級截止日期前提交額外獎勵

## 檔案說明
- `411410006_hw3.c` - 主要實作檔案
- `samples-HW3/` - 測試資料夾
- `program3.pdf` - 作業說明文件
- `graph.png` - 網路拓撲圖
- `memory示意圖.png` - 記憶體配置示意圖

## 技術重點
- **量子網路資源分配**：在記憶體限制下最大化請求接受率
- **BFS路徑搜尋**：在量子網路拓撲中找到最短路徑
- **策略樹建構**：使用完全二元樹表示糾纏交換策略
- **優先權佇列**：使用堆積排序請求處理順序
- **動態記憶體管理**：使用malloc/realloc管理複雜資料結構
- **時間槽分配**：精確管理量子記憶體的時間使用
- **糾纏交換概念**：實作量子網路中的長距離連線建立

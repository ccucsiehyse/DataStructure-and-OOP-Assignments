# OOP HW2+3 - 物聯網聚合樹建構

## 作業描述
本作業在教授提供的OOP架構基礎上，實作物聯網(IoT)或無線感測器網路中的資料收集系統，透過建構聚合樹(Aggregation Tree)來有效地將所有節點的資料收集到匯集節點(sink node)。

## 專案背景
- **物聯網環境**：大型物聯網或無線感測器網路，每個節點監測環境數據
- **聚合樹概念**：將所有資料收集到匯集節點的樹狀結構
- **網路內聚合**：在網路內部進行資料聚合，節省傳輸能量
- **有限資源**：節點具有有限的儲存和計算能力

## 專案目標
- **HW2**：建構一個以節點0為根的聚合樹
- **HW3**：建構進階聚合樹，最小化總成本
- **核心挑戰**：有效收集所有節點資料到匯集節點

## 問題分析
- 給定帶有節點權重的網路G = (V, E)
- 使用分散式廣度優先搜尋(Distributed BFS)建構樹
- 考慮跳數(hop count)和節點ID的優先順序
- HW3額外考慮封包大小、資料大小和成本優化

## 實作方法

### 核心演算法：分散式BFS與聚合樹建構

本實作採用**分散式廣度優先搜尋**結合**智能聚合策略**來建構物聯網聚合樹。

#### 1. 類別設計架構
```cpp
// IoT裝置類別
class IoT_device: public node {
public:
    unsigned int parent = PARENT_INIT;        // 父節點ID
    unsigned int counter = UNINT_NEG1;        // 計數器
    unsigned int child_ct = 0;                // 子節點數量
    vector<unsigned int> children;           // 子節點列表
    vector<unsigned int> unchild_buf;         // 未確認子節點緩衝區
    set<unsigned int> neighbor;               // 鄰居集合
    unsigned int data_size;                   // 資料大小
    unsigned int* reverse_path;               // 反向路徑
};

// IoT匯集節點類別
class IoT_sink: public node {
public:
    unsigned int parent = PARENT_INIT;        // 父節點ID
    unsigned int counter = UNINT_NEG1;        // 計數器
    unsigned int child_ct = 0;                // 子節點數量
    vector<unsigned int> children;            // 子節點列表
    set<unsigned int> neighbor;               // 鄰居集合
    unsigned int Nodes;                       // 總節點數
    unsigned int *reverse_path;               // 反向路徑
    unsigned int *origin_parent_list;         // 原始父節點列表
    unsigned int *new_parent_list;            // 新父節點列表
};
```

**類別設計特點**：
- **教授預設計架構**：IoT_device和IoT_sink都繼承自教授提供的node類別
- **封裝性**：每個類別管理自己的樹狀結構資訊
- **擴展性**：HW3在現有架構上擴展了更多屬性來支援進階功能

#### 2. 分散式BFS建構
```cpp
// 控制封包處理
if (p->type() == "IoT_ctrl_packet") {
    IoT_ctrl_packet *p3 = dynamic_cast<IoT_ctrl_packet*>(p);
    IoT_ctrl_payload *l3 = dynamic_cast<IoT_ctrl_payload*>(p3->getPayload());
    
    unsigned int pre_id = p3->getHeader()->getPreID();
    unsigned int pkt_ct = l3->getCounter();
    
    // 設定父節點：優先選擇ID較小且計數器較小的節點
    if(parent == PARENT_INIT || ((pre_id < parent) && (pkt_ct <= this->counter))){
        this->parent = pre_id;
        this->counter = pkt_ct;
        need_send = true;
    }
}
```

**BFS建構策略**：
- **分散式搜尋**：使用IoT_ctrl_packet進行分散式BFS
- **父節點選擇**：優先選擇ID較小且計數器較小的節點
- **動態更新**：當收到更好的父節點候選時會更新父節點
- **子節點管理**：透過封包訊息確認父子關係，使用緩衝區避免重複

#### 3. 資料傳輸機制
```cpp
// 資料封包處理
if (p->type() == "IoT_data_packet") {
    IoT_data_packet *p2 = dynamic_cast<IoT_data_packet*>(p);
    IoT_data_payload *l2 = dynamic_cast<IoT_data_payload*>(p2->getPayload());
    
    // 葉子節點：立即發送給父節點
    if(child_ct == 0) {
        send_to_parent(p2);
    }
    // 非葉子節點：等待所有子節點資料後再發送
    else {
        wait_for_all_children(p2);
    }
}
```

**傳輸策略**：
- **葉子節點**：獲得資料封包後立即發送給父節點
- **非葉子節點**：收到所有子節點封包後才發送自己的資料
- **匯集節點**：接收所有資料並進行聚合處理

#### 4. HW3智能聚合樹建構演算法
```cpp
// 四階段聚合樹建構
// 階段1：分散式BFS建構原始樹
// 階段2：資訊收集 (AGG_ctrl_packet)
// 階段3：決策與更新 (DIS_ctrl_packet)  
// 階段4：資料傳輸

// 核心演算法：智能父節點選擇
for(auto elder = elder_list[nodeID].begin(); elder != elder_list[nodeID].end(); elder++){
    // 選擇能產生最滿額外封包的父節點
    unsigned int extra_pktSize_elder = (new_data_size[*elder] + new_data_size[nodeID]) % packet_size;
    if (fuller_extraPkt == 0) continue;
    if (fuller_extraPkt < extra_pktSize_elder || extra_pktSize_elder == 0) {
        parent_choose = *elder;
        fuller_extraPkt = extra_pktSize_elder;
    }
}

// 優先佇列驅動的聚合
priority_queue< pair<unsigned int, unsigned int> > dataSize_ID; // pair<new_data_size, nodeID>, max_heap
dataSize_ID.push( make_pair(new_data_size[nodeID] % packet_size, nodeID) );
```

**智能聚合策略**：
- **四階段建構**：BFS建構 → 資訊收集 → 決策更新 → 資料傳輸
- **封包利用率優化**：選擇能產生最滿額外封包的父節點
- **貪心演算法**：使用優先佇列進行貪心選擇
- **成本最小化**：透過封包聚合減少總傳輸成本
- **層級處理**：按計數器層級處理節點
- **動態更新**：即時更新父節點的資料大小和成本

#### 5. 封包處理系統
```cpp
// 封包類型處理
virtual void recv_handler(packet *p) {
    if (p->type() == "IoT_ctrl_packet") {
        handle_ctrl_packet(p);
    }
    else if (p->type() == "IoT_data_packet") {
        handle_data_packet(p);
    }
    else if (p->type() == "AGG_ctrl_packet") {
        handle_agg_ctrl_packet(p);
    }
    else if (p->type() == "DIS_ctrl_packet") {
        handle_dis_ctrl_packet(p);
    }
}
```

**封包處理**：
- **控制封包**：用於建構聚合樹和傳遞控制資訊
- **資料封包**：用於傳輸實際的感測器資料
- **聚合控制封包**：HW3中用於資訊收集
- **分發控制封包**：HW3中用於分發新父節點資訊

#### 6. 演算法特點
- **HW2時間複雜度**：O(V + E)，其中V是節點數，E是邊數
- **HW3時間複雜度**：O(V log V + E)，包含優先佇列操作
- **空間效率**：使用STL容器管理動態資料結構
- **分散式設計**：每個節點獨立處理封包
- **事件驅動**：基於封包到達事件進行處理
- **智能優化**：HW3使用貪心演算法優化封包利用率

### 實作細節

#### STL容器使用
```cpp
// 使用vector管理動態陣列
vector<unsigned int> children;
vector<unsigned int> unchild_buf;

// 使用set管理鄰居集合
set<unsigned int> neighbor;
set<unsigned int> elder;

// 使用map管理反向路徑
map<unsigned int, unsigned int> reverse_path;
```

#### 記憶體管理
```cpp
// 使用new和delete進行記憶體管理
unsigned int* reverse_path = new unsigned int[Nodes];
unsigned int* new_parent_list = new unsigned int[Nodes];

// 釋放記憶體
delete[] reverse_path;
delete[] new_parent_list;
```

#### 封包處理
```cpp
// 使用dynamic_cast進行類型轉換
IoT_ctrl_packet *p3 = dynamic_cast<IoT_ctrl_packet*>(p);
IoT_ctrl_payload *l3 = dynamic_cast<IoT_ctrl_payload*>(p3->getPayload());

// 更新封包標頭
p3->getHeader()->setPreID(current_id);
p3->getHeader()->setNexID(next_id);
```

## 輸入格式
```
節點數量 邊數 封包大小 資料大小
模擬時間 BFS開始時間 聚合開始時間 分發開始時間 資料傳輸時間
邊資訊 (端點1 端點2)
節點權重資訊
```

## 輸出格式
```
封包交換記錄
每個節點的父節點資訊
聚合樹結構
總成本統計
```

## 範例
**輸入**：
```
5 6 100 50
1000 100 200 300 400
0 1
1 2
2 3
3 4
4 0
1 3
10 20 30 40 50
```

## 評分標準
- **HW2**：正確建構聚合樹，輸出每個節點的父節點
- **HW3**：總成本越低，分數越高
- **基本分數**：60分（在截止日期前提交）
- **績效排名獎勵**：0-25分（根據總成本排名）
- **作業助理獎勵**：+10分（超級截止日期前提交）

## 檔案說明
- `OOP_HW2_411410006.cpp` - HW2主要實作檔案
- `OOP_HW3_411410006.cpp` - HW3主要實作檔案
- `samples-HW2/` - HW2測試資料夾
- `samples-HW3/` - HW3測試資料夾
- `program2+3 v2.pdf` - 作業說明文件

## 技術重點
- **物件導向程式設計**：類別繼承、多型、封裝
- **分散式演算法**：分散式BFS、聚合樹建構
- **事件驅動程式設計**：基於封包事件的處理機制
- **網路協定實作**：IoT封包處理、路由協定
- **STL容器應用**：vector、set、map、priority_queue等標準容器
- **記憶體管理**：使用new/delete進行動態記憶體管理
- **複雜系統模擬**：物聯網環境的完整模擬
- **智能優化演算法**：HW3中的貪心聚合策略
- **封包利用率優化**：透過智能父節點選擇最大化封包利用率
- **四階段建構流程**：BFS建構 → 資訊收集 → 決策更新 → 資料傳輸
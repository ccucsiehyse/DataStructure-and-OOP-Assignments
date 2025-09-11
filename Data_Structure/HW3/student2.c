#include <stdio.h>
#include <stdlib.h>

typedef struct QuantumNode {
    int id;
    int quantumMemory;
    struct QuantumNode* next;
} QuantumNode;

typedef struct QuantumChannel {
    int sourceNodeId;
    int destinationNodeId1;
    int destinationNodeId2;
    struct QuantumChannel* next;
} QuantumChannel;

typedef struct SwapRequest {
    int reqId;
    int reqSrc;
    int reqDst;
    struct SwapRequest* next;
} SwapRequest;

typedef struct TreeNode {
    int id;
    struct TreeNode* rep1;
    struct TreeNode* rep2;
    struct TreeNode* next;
} TreeNode;

typedef struct QuantumNetwork {
    QuantumNode* nodes;
    QuantumChannel* channels;
    SwapRequest* swapRequests;
} QuantumNetwork;

int currentReqId;

void createEntangledPair(QuantumNode* node1, QuantumNode* node2, QuantumChannel* channel, TreeNode** treeRoot) {
    // Simulate entanglement process
    printf("%d %d %d %d  // ReqID ReqSrc Rep1 Rep2\n", currentReqId, channel->sourceNodeId, node1->id, node1->id);
    printf("%d %d %d %d  // ReqID ReqSrc Rep1 Rep2\n", currentReqId, channel->destinationNodeId1, node2->id, node2->id);
    currentReqId++;

    // Update quantum memory or perform other necessary actions
    node1->quantumMemory--;
    node2->quantumMemory--;

    // Add nodes to the tree structure
    TreeNode* newNode1 = (TreeNode*)malloc(sizeof(TreeNode));
    newNode1->id = node1->id;
    newNode1->rep1 = NULL;
    newNode1->rep2 = NULL;
    newNode1->next = NULL;

    TreeNode* newNode2 = (TreeNode*)malloc(sizeof(TreeNode));
    newNode2->id = node2->id;
    newNode2->rep1 = NULL;
    newNode2->rep2 = NULL;
    newNode2->next = NULL;

    if (*treeRoot == NULL) {
        *treeRoot = newNode1;
    }

    TreeNode* current = *treeRoot;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode2;
}

void processSwapRequests(QuantumNetwork* network, TreeNode** treeRoot) {
    SwapRequest* currentRequest = network->swapRequests;
    while (currentRequest != NULL) {
        QuantumNode* node1 = network->nodes;
        QuantumNode* node2 = network->nodes;
        while (node1 != NULL && node1->id != currentRequest->reqSrc) {
            node1 = node1->next;
        }

        while (node2 != NULL && node2->id != currentRequest->reqDst) {
            node2 = node2->next;
        }

        QuantumChannel* currentChannel = network->channels;
        while (currentChannel != NULL) {
            if ((currentChannel->sourceNodeId == currentRequest->reqSrc && currentChannel->destinationNodeId1 == currentRequest->reqDst) ||
                (currentChannel->sourceNodeId == currentRequest->reqDst && currentChannel->destinationNodeId1 == currentRequest->reqSrc)) {
                createEntangledPair(node1, node2, currentChannel, treeRoot);
                break;
            }
            currentChannel = currentChannel->next;
        }

        currentRequest = currentRequest->next;
    }
}

void postOrderTraversal(TreeNode* root, int startTime) {
    if (root == NULL) {
        return;
    }

    postOrderTraversal(root->rep1, startTime);
    postOrderTraversal(root->rep2, startTime);

    printf("%d %d %d %d  //// ReqID ReqSrc Rep1 Rep2\n", startTime, root->id, root->rep1->id, root->rep2->id);
}

void insertNode(QuantumNode** head, QuantumNode* newNode) {
    newNode->next = *head;
    *head = newNode;
}

void insertChannel(QuantumChannel** head, QuantumChannel* newChannel) {
    newChannel->next = *head;
    *head = newChannel;
}

void insertSwapRequest(SwapRequest** head, SwapRequest* newRequest) {
    newRequest->next = *head;
    *head = newRequest;
}

QuantumNode* createNode(int id, int quantumMemory) {
    QuantumNode* newNode = (QuantumNode*)malloc(sizeof(QuantumNode));
    newNode->id = id;
    newNode->quantumMemory = quantumMemory;
    newNode->next = NULL;
    return newNode;
}

QuantumChannel* createChannel(int sourceNodeId, int destinationNodeId1, int destinationNodeId2) {
    QuantumChannel* newChannel = (QuantumChannel*)malloc(sizeof(QuantumChannel));
    newChannel->sourceNodeId = sourceNodeId;
    newChannel->destinationNodeId1 = destinationNodeId1;
    newChannel->destinationNodeId2 = destinationNodeId2;
    newChannel->next = NULL;
    return newChannel;
}

SwapRequest* createSwapRequest(int reqId, int reqSrc, int reqDst) {
    SwapRequest* newRequest = (SwapRequest*)malloc(sizeof(SwapRequest));
    newRequest->reqId = reqId;
    newRequest->reqSrc = reqSrc;
    newRequest->reqDst = reqDst;
    newRequest->next = NULL;
    return newRequest;
}

void freeChannels(QuantumChannel* head) {
    QuantumChannel* currentChannel = head;
    while (currentChannel != NULL) {
        QuantumChannel* temp = currentChannel;
        currentChannel = currentChannel->next;
        free(temp);
    }
}

void freeQuantumNetwork(QuantumNetwork* network) {
    QuantumNode* currentNode = network->nodes;
    while (currentNode != NULL) {
        QuantumNode* tempNode = currentNode;
        currentNode = currentNode->next;
        free(tempNode);
    }

    freeChannels(network->channels);

    SwapRequest* currentRequest = network->swapRequests;
    while (currentRequest != NULL) {
        SwapRequest* tempRequest = currentRequest;
        currentRequest = currentRequest->next;
        free(tempRequest);
    }
}

int main() {
    QuantumNetwork network;
    TreeNode* treeRoot = NULL;
    int i;
    int numNodes, numChannels, numTimeSlots, numRequests;

    // Read network details
    scanf("%d %d %d %d", &numNodes, &numChannels, &numTimeSlots, &numRequests);

    // Initialize linked lists for nodes, channels, and swap requests
    network.nodes = NULL;
    network.channels = NULL;
    network.swapRequests = NULL;

    // Read node details
    for (i = 0; i < numNodes; i++) {
        int id, quantumMemory;
        scanf("%d %d", &id, &quantumMemory);
        QuantumNode* newNode = createNode(id, quantumMemory);
        insertNode(&network.nodes, newNode);
    }

    // Read channel details
    for (i = 0; i < numChannels; i++) {
        int sourceNodeId, destinationNodeId1, destinationNodeId2;
        scanf("%d %d %d", &sourceNodeId, &destinationNodeId1, &destinationNodeId2);
        QuantumChannel* newChannel = createChannel(sourceNodeId, destinationNodeId1, destinationNodeId2);
        insertChannel(&network.channels, newChannel);
    }

    // Read and process requests
    for (i = 0; i < numRequests; i++) {
        int reqId, reqSrc, reqDst;
        scanf("%d %d %d", &reqId, &reqSrc, &reqDst);
        SwapRequest* newRequest = createSwapRequest(reqId, reqSrc, reqDst);
        insertSwapRequest(&network.swapRequests, newRequest);
    }

    printf("%d\n", numRequests);

    // Process swap requests and simulate the swapping process
    currentReqId = 1; // Reset the request ID counter
    processSwapRequests(&network, &treeRoot);

    // Print Tree Structure using postorder traversal
    postOrderTraversal(treeRoot, 1);

    // Free allocated memory
    freeQuantumNetwork(&network);

    return 0;
}

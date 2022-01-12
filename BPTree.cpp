// reference: https://www.geeksforgeeks.org/insertion-in-a-b-tree/
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

#define MAX_VALUE_CNT 1000

using namespace std;

// set MAX node cnt (degree - 1)
int MAX = 3;

void instr() {
    cout << "PRESS 1 TO INSERT ONE BY ONE" << endl;
    cout << "PRESS 2 TO BUILD TREE BY BOTTOM UP" << endl;
}

class Node {
private:

public:
    bool isLeaf;
    // ptr to key value array
    int *key;
    // size, height
    int size = 0, height = 0;
    // ptr to child node array if inner node, else sibling node
    Node **ptr;

    // constructor
    Node();
};

Node::Node() {
    key = new int[MAX];
    ptr = new Node *[MAX + 1];
}

class BPTree {
private:
    Node *root;
    Node* insertInternal(int, Node *, Node *);
    Node *findParent(Node *, Node *);

public:
    BPTree();
    void search(int);
    void insert(int);
    
    // display by BFS method
    void display(Node *);
    void displayByBFS(Node *cursor);
    Node *getRoot() { return root; };

    // build by bottom up
    void bottomUpBuilding(int* array, int cnt);
};

BPTree::BPTree() {
    root = NULL;
}

// Insert Operation
void BPTree::insert(int x) {
    if (root == NULL) {
        root = new Node;
        root->key[0] = x;
        root->isLeaf = true;
        root->size = 1;
    } else {
        Node *cursor = root;
        Node *parent;
        // find the leaf belong to x
        while (cursor->isLeaf == false) {
            parent = cursor;
            for (int i = 0; i < cursor->size; i++) {
                if (x < cursor->key[i]) {
                    cursor = cursor->ptr[i];
                    break;
                }
                if (i == cursor->size - 1) {
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        } // end of finding leaf

        if (cursor->size < MAX) { // only need to insert to this leaf
            int i = 0;
            while (x > cursor->key[i] && i < cursor->size)
                i++;
            for (int j = cursor->size; j > i; j--) {
                cursor->key[j] = cursor->key[j - 1];
            }
            cursor->key[i] = x;
            cursor->size++;
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
            cursor->ptr[cursor->size - 1] = NULL;
        } else { // need to split and insert value to inner node
            Node *newLeaf = new Node;
            int virtualNode[MAX + 1] = {0};
            // copy of current full node value 
            for (int i = 0; i < MAX; i++) {
                virtualNode[i] = cursor->key[i];
            }

            int i = 0, j = 0;
            // which index to insert x
            while (x > virtualNode[i] && i < MAX)
                i++;
            // insert x
            for (int j = MAX + 1; j > i; j--) {
                virtualNode[j] = virtualNode[j - 1];
            }
            virtualNode[i] = x;

            // set two leaves values
            newLeaf->isLeaf = true;
            cursor->size = (MAX + 1) / 2;
            newLeaf->size = MAX + 1 - (MAX + 1) / 2;
            cursor->ptr[cursor->size] = newLeaf;
            newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
            cursor->ptr[MAX] = NULL;
            for (i = 0; i < cursor->size; i++) {
                cursor->key[i] = virtualNode[i];
            }
            for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
                newLeaf->key[i] = virtualNode[j];
            } // end of setting two leaves values

            // debug
            // cout << "cursor ------------\n";
            // for (int i = 0; i < cursor->size; i++) {
            //   cout << cursor->key[i] << " ";
            // }
            // cout << "\n";

            // // debug
            // cout << "newLeaf ------------\n";
            // for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
            //   cout << newLeaf->key[i] << " ";
            // }
            // cout << "\n";

            if (cursor == root) { // build a new inner node
                Node *newRoot = new Node;
                newRoot->key[0] = newLeaf->key[0];
                newRoot->ptr[0] = cursor;
                newRoot->ptr[1] = newLeaf;
                newRoot->isLeaf = false;
                newRoot->size = 1;
                root = newRoot;
            } else {
                insertInternal(newLeaf->key[0], parent, newLeaf); // (key, ptr to parent, newleaf)
            }
        }
    }
}

// used for insert in inner node
Node* BPTree::insertInternal(int x, Node *cursor, Node *child) {
    if (cursor->size < MAX) { // insert to inner node and finish insertion
        int i = 0;
        while (x >= cursor->key[i] && i < cursor->size)
            i++;
        for (int j = cursor->size; j > i; j--) {
            cursor->key[j] = cursor->key[j - 1];
        }
        for (int j = cursor->size + 1; j > i + 1; j--) {
            cursor->ptr[j] = cursor->ptr[j - 1];
        }
        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[i + 1] = child;

        return cursor;
    } 

    // inner node is full     
    int virtualKey[MAX + 1] = {0};
    Node* virtualPtr[MAX + 2] = {0};

    // Copy of current node ptr and keys
    for (int i = 0; i < MAX; i++) {
        virtualKey[i] = cursor->key[i];
    }
    for (int i = 0; i < MAX + 1; i++) {
        virtualPtr[i] = cursor->ptr[i];
    }

    // insert new key value to array
    int i = 0, j = 0;
    // find which index to insert x, note that ptr also need to be updated
    while (x >= virtualKey[i] && i < MAX)
        i++;
    for (int j = MAX + 1; j > i; j--) {
        virtualKey[j] = virtualKey[j - 1];
    }
    virtualKey[i] = x;

    for (int j = MAX + 2; j > i + 1; j--) {
        virtualPtr[j] = virtualPtr[j - 1];
    }
    virtualPtr[i + 1] = child;
    // end of insert new key and ptr

    // start to split inner node
    Node *newInternal = new Node;
    newInternal->isLeaf = false;
    cursor->size = (MAX + 1) / 2;
    newInternal->size = MAX - (MAX + 1) / 2;
    for (i = 0; i < cursor->size; i++) {
        cursor->key[i] = virtualKey[i];
        cursor->ptr[i] = virtualPtr[i];
    }
    cursor->ptr[cursor->size] = virtualPtr[cursor->size];
    for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
        newInternal->key[i] = virtualKey[j];
        newInternal->ptr[i] = virtualPtr[j];
    }
    newInternal->ptr[newInternal->size] = virtualPtr[MAX+1];
    // end of splitting two inner node

    // start to insert  newInterval
    if (cursor == root) {
        Node *newRoot = new Node;
        newRoot->key[0] = virtualKey[cursor->size];
        newRoot->ptr[0] = cursor;
        newRoot->ptr[1] = newInternal;
        newRoot->isLeaf = false;
        newRoot->size = 1;
        root = newRoot;
    } else {
        insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
    }
    return newInternal;
}

// Find the node by node ptr
Node *BPTree::findParent(Node *cursor, Node *child) {
    Node *parent;
    if (cursor->isLeaf || (cursor->ptr[0])->isLeaf) {
        return NULL;
    }
    for (int i = 0; i < cursor->size + 1; i++) {
        if (cursor->ptr[i] == child) {
            parent = cursor;
            return parent;
        } else {
            parent = findParent(cursor->ptr[i], child);
            if (parent != NULL)
                return parent;
        }
    }
    return parent;
}

// Print the tree
void BPTree::display(Node *cursor) {
    if (cursor != NULL) {
        cout << "---";
        for (int i = 0; i < cursor->size; i++) {
            cout << cursor->key[i] << " ";
        }
        cout << "\n";
        if (cursor->isLeaf != true) {
            for (int i = 0; i < cursor->size + 1; i++) {
                cout << "---";
                display(cursor->ptr[i]);
            }
        }
    }
}

void BPTree::displayByBFS(Node *cursor) {
    queue<Node*> q;
    cursor -> height = 0;
    q.push(cursor);
    int current_h = 0;
    
    // cout << "current -> key [i]: " << cursor -> key [0] << "\n";
    while(!q.empty()){
        Node* current = q.front();
        if(current -> height > current_h){
        current_h++;
        cout << "\n";
        }
        cout << "|" ;
        for (int i = 0; i < current->size; i++) {
        cout << " " << current -> key [i] << " ";
        }
        cout << "|" ;
        
        q.pop();
        if (current->isLeaf != true) {
            for (int i = 0; i < current->size+1; i++) {
                current -> ptr[i] -> height = current -> height+1;
                q.push(current -> ptr[i]);
            }
        }
    }
}

void BPTree::bottomUpBuilding(int* array, int cnt) {
    if (root != NULL) {
        return;
    }

    int leafNodeCnt = cnt % MAX == 0 ? cnt/MAX : cnt/MAX + 1;
    cout << "leaf node cnt" << leafNodeCnt << endl;
    // START TO DISTRIBUTE VALUE TO LEAF NODES
    Node **leafNodeArray = new Node*[leafNodeCnt];
    
    for (int i = 0; i < leafNodeCnt-1; i++){
        leafNodeArray[i] = new Node;
        leafNodeArray[i]->isLeaf = true;
        leafNodeArray[i]->size = MAX;
        for (int j = 0; j < MAX; j++){
            leafNodeArray[i]->key[j] = array[i*MAX+j];
        }
    }
    if (cnt % MAX == 0){
        leafNodeArray[leafNodeCnt-1] = new Node;
        leafNodeArray[leafNodeCnt-1]->isLeaf = true;
        leafNodeArray[leafNodeCnt-1]->size = MAX;
        for (int j = 0; j < MAX; j++){
            leafNodeArray[leafNodeCnt-1]->key[j] = array[(leafNodeCnt-1)*MAX+j];
        }        
    } else {
        leafNodeArray[leafNodeCnt-1] = new Node;
        leafNodeArray[leafNodeCnt-1]->isLeaf = true;
        leafNodeArray[leafNodeCnt-1]->size = cnt % MAX;
        for (int j = 0; j < cnt % MAX; j++){
            leafNodeArray[leafNodeCnt-1]->key[j] = array[(leafNodeCnt-1)*MAX+j];
        }            
    }
    // END OF DISTRIBUTING

    // DEBUG TO SHOW ALL LEAF NODE
    // for (int i = 0; i < leafNodeCnt; i++){
    //     for (int j = 0; j < leafNodeArray[i]->size; j++)
    //         cout << leafNodeArray[i]->key[j] << " ";
    //     cout << endl;
    // }

    // build tree root
    if (leafNodeCnt == 1){
        root = leafNodeArray[0];
        return;
    } 

    // initialize
    root = new Node;
    root->key[0] = leafNodeArray[1]->key[0];
    root->ptr[0] = leafNodeArray[0];
    root->ptr[1] = leafNodeArray[1];
    root->isLeaf = false;
    root->size = 1;

    // displayByBFS(root);
    // insert leaf node one by one
    Node* cursor = root;
    for (int i = 2; i < leafNodeCnt; i++) {
        cursor = insertInternal(leafNodeArray[i]->key[0], cursor, leafNodeArray[i]);
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition (int arr[], int low, int high) {
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element
 
    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
 
/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSort(int arr[], int low, int high) {
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main(){
    cout << "Type the maximum degree: ";
    cin >> MAX;
    MAX -= 1;

    int method = 1;
    instr();
    cin >> method;
    while(method != 1 && method != 2) {
        cout << "Please enter 1 or 2." << endl;
        cin >> method;
    }

    // read file
    string filename("input.txt");
    int number;

    int *array = new int [MAX_VALUE_CNT];
    for (int i = 0; i < MAX_VALUE_CNT; i++)
        array[i] = 0;
    
    int valueCnt = 0;
    ifstream input_file(filename);

    if (!input_file.is_open()) {
            cerr << "Could not open the file - '"
                << filename << "'" << endl;
            return EXIT_FAILURE;
        }

        while (input_file >> number) {
            array[valueCnt] = number;
            valueCnt += 1;
    }
    //end reading file
    cout << "value cnt: "<< valueCnt << endl;
    BPTree node;

    for(int i = 0 ; i < valueCnt; i ++ ){
        	cout << array[i] << " ";
    }
    cout << "\n";
    if (method == 1){
        for(int i = 0 ; i < valueCnt; i++ ){
            node.insert(array[i]);
        }
        cout << "\n";
        
        node.displayByBFS(node.getRoot());
    } else if (method == 2){
        // build by bottom up
        quickSort(array, 0, valueCnt-1);
        node.bottomUpBuilding(array, valueCnt);
        node.displayByBFS(node.getRoot());
    }

    // print sorted array
    // for(int i = 0 ; i < valueCnt; i++ ){
    //     	cout << array[i] << " ";
    // }
    // cout << "\n";

	// node.display(node.getRoot());
}
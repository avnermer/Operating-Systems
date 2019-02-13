//Avner Merhavi
//203090436

#ifndef UNTITLED5_LINKEDLIST_H
#define UNTITLED5_LINKEDLIST_H


/**
 * The Node struct,
 * contains item and the pointers that point to previous node/next node.
 */
typedef struct Node {
    int pid;
    char name[1024];
    // previous node
    struct Node* prev;
    // next node
    struct Node* next;
} Node;


/**
 * The LinkedList struct, contains the pointers that
 * point to first node and last node, the size of the LinkedList,
 * and the function pointers.
 */
typedef struct LinkedList {
    Node* head;
    Node* tail;
    // size of this LinkedList
    int size;

    // add item to any position
    void (*add) (struct LinkedList*, int, char*, int);
    // add item after tail
    Node* (*addLast) (struct LinkedList*, int, char* name);
    // add item before head
    void (*addFirst) (struct LinkedList*, int, char* name);

    // insert node
    void (*insertBefore) (struct LinkedList*, Node*, Node*);
    // get item from any position
    int (*get) (struct LinkedList*, int);
    // get last item
    int (*getLast) (struct LinkedList*);
    // get first item
    int (*getFirst) (struct LinkedList*);

    // remove item from any position
    int (*remove) (struct LinkedList*, int);
    // remove last item
    int (*removeLast) (struct LinkedList*);
    // remove first item
    int (*removeFirst) (struct LinkedList*);
    //remove given a reference
    void (*removeNode)(struct LinkedList*, Node*);

    // display all element in the LinkedList
    void (*display) (struct LinkedList*);
    //delete list
    void (*deleteList)(struct LinkedList**);
    // create a node with item
    Node* (*createNode) (int, char*);

} LinkedList;


/** add item to any position
 */
void add (LinkedList* _this, int item, char* name, int position);
/** add item to head
 */
void addFirst (LinkedList* _this, int pid, char* name);
/** add item to tail
 */
Node* addLast (LinkedList* _this, int pid, char *name);
/** insert one node before another,
 * newNdoe, node and node->prev should not be null.
 */
void insertBefore (LinkedList* _this, Node* node, Node* newNode);
/** get item from specific position
 */
int get (LinkedList* _this, int position);
/** get item from head
 */
int getFirst (LinkedList* _this);
/** get item from tail
 */
int getLast (LinkedList* _this);
/** get item and remove it from any position
 */
int _remove (LinkedList* _this, int position);
/** get and remove item from head
 */
int _removeFirst (LinkedList* _this);
/** get and remove item from tail
 */
int _removeLast (LinkedList* _this);

void removeNode(LinkedList* _this, Node* node);
/** display the items in the list
 */
void display (LinkedList* _this);
/** create a LinkedList
 */
LinkedList* createLinkedList ();
/** create a Node
 */
Node* createNode (int pid, char*name);

void deleteList(LinkedList** linkedList);

#endif //UNTITLED5_LINKEDLIST_H

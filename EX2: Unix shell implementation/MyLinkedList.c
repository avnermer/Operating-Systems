//Avner Merhavi
//203090436

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "MyLinkedList.h"
/**
 * based on public domain materials from the internet, modified&added functions
 * Type of item is int, string
 * Add item to head, tail or any position
 * Get item from head, tail or any position
 * Get and remove item from head, tail or any position
 * Can get the size
 * Can display all item
 */


/** add item to any position
 */
void add (LinkedList* _this, int pid, char* name, int position) {
    // index out of list size
    if (position > _this->size) {
        printf("LinkedList#add: Index out of bound");
        system("PAUSE");
        exit(0);
    }
    // add to head
    if (position == 0) {
        _this->addFirst(_this, pid, name);
    } else if (position == _this->size) {
        // add to tail
        _this->addLast(_this, pid, name);
    } else {
        // insert between head and tail

        Node* node = _this->head;
        int i = 0;
        // loop until the position
        while (i < position) {
            node = node->next;
            i++;
        }
        // insert new node to position
        Node* newNode = _this->createNode(pid, name);
        _this->insertBefore(_this, node, newNode);
        _this->size++;
    }
}
/** add item to head
 */
void addFirst (LinkedList* _this, int pid, char* name) {
    Node* newNode = _this->createNode(pid, name);
    Node* head = _this->head;
    // list is empty
    if (head == NULL)
        _this->head = newNode;
    else { // has item(s)
        Node* last = _this->tail;
        if (last == NULL) // only head node
            last = head;
        newNode->next = head;
        head->prev = newNode;
        _this->head = newNode;
        _this->tail = last;
    }

    _this->size++;
}
/** add item to tail
 */
Node* addLast (LinkedList* _this, int pid, char* name) {
    Node* newNode = _this->createNode(pid, name);
    Node* head = _this->head;
    Node* tail = _this->tail;
    // list is empty
    if (head == NULL)
        _this->head = newNode;
    else { // has item(s)
        Node* lastNode = tail;
        if (tail == NULL) // only head node
            lastNode = head;
        lastNode->next = newNode;
        newNode->prev = lastNode;
        _this->tail = newNode;
    }
    _this->size++;
    return newNode;
}

/** insert one node before another,
 * newNdoe, node and node->prev should not be null.
 */
void insertBefore (LinkedList* _this, Node* node, Node* newNode) {
    Node* prev = node->prev;

    node->prev = newNode;
    newNode->next = node;
    prev->next = newNode;
    newNode->prev = prev;
}
/** get item from specific position
 */
int get (LinkedList* _this, int position) {
    // list is empty
    if (_this->size == 0) {
        printf("LinkedList#get: The list is empty.");
        system("PAUSE");
        exit(0);
    } else if (position >= _this->size) {
        // out of bound
        printf("LinkedList#get: Index out of bound");
        system("PAUSE");
        exit(0);
    }
    // get head item
    if (position == 0) {
        return _this->getFirst(_this);
    } else if (position+1 == _this->size) {
        // get tail item
        return _this->getLast(_this);
    } else {
        Node* node = _this->head;
        int i = 0;
        // loop until position
        while (i < position) {
            node = node->next;
            i++;
        }
        return node->pid;
    }
}
/** get item from head
 */
int getFirst (LinkedList* _this) {
    // list is empty
    if (_this->size == 0) {
        printf("LinkedList#getFirst: The list is empty.");
        system("PAUSE");
        exit(0);
    }
    return _this->head->pid;
}
/** get item from tail
 */
int getLast (LinkedList* _this) {
    // list is empty
    if (_this->size == 0) {
        printf("LinkedList#getLast: The list is empty.");
        system("PAUSE");
        exit(0);
    }
    // only head node
    if (_this->size == 1) {
        return getFirst(_this);
    }
    return _this->tail->pid;
}
/** get item and remove it from any position
 */
int _remove (LinkedList* _this, int position) {
    // list is empty
    if (_this->size == 0) {
        printf("LinkedList#_remove: The list is empty.");
        system("PAUSE");
        exit(0);
    } else if (position >= _this->size) {
        // out of bound
        printf("LinkedList#_remove: Index out of bound");
        system("PAUSE");
        exit(0);
    }

    // remove from head
    if (position == 0) {
        return _this->removeFirst(_this);
    } else if (position+1 == _this->size) {
        // remove from tail
        return _this->removeLast(_this);
    } else {
        Node* node = _this->head;
        Node* prev;
        Node* next;
        int i = 0, item;
        // loop until position
        while (i < position) {
            node = node->next;
            i++;
        }
        item = node->pid;
        // remove node from list
        prev = node->prev;
        next = node->next;
        prev->next = next;
        next->prev = prev;
        free(node);
        _this->size--;
        return node->pid;
    }
}
/** get and remove item from head
 */
int _removeFirst (LinkedList* _this) {
    Node* head = _this->head;
    Node* next;
    int item;
    // list is empty
    if (head == NULL) {
        printf("LinkedList#_removeFirst: The list is empty.");
        system("PAUSE");
        exit(0);
    }
    item = head->pid;
    next = head->next;
    _this->head = next;
    if (next != NULL) // has next item
        next->prev = NULL;
    free(head);
    _this->size--;
    if (_this->size <= 1) // empty or only head node
        _this->tail = NULL;
    return item;
}
/** get and remove item from tail
 */
int _removeLast (LinkedList* _this) {
    // list is empty
    if (_this->size == 0) {
        printf("LinkedList#_removeLast: The list is empty.");
        system("PAUSE");
        exit(0);
    }
    if (_this->size == 1) { // only head node
        return _this->removeFirst(_this);
    } else {
        Node* tail = _this->tail;
        Node* prev = tail->prev;
        int item = tail->pid;
        prev->next = NULL;
        if (_this->size > 1)
            _this->tail = prev;
        _this->size--;
        if (_this->size <= 1) // empty or only head node
            _this->tail = NULL;
        return item;
    }
}

/**
 * remove node given a reference to it
 */
void removeNode(LinkedList* _this, Node* node) {
    if(node == _this->head)
        _this->removeFirst(_this);
    else if (node == _this->tail)
        _this->removeLast(_this);
    else {
        node->prev->next = node->next;
        free(node);
    }
}

/** display the items in the list
 */
void display (LinkedList* _this) {
    int i, size = _this->size;
    if (size == 0)
        ;
    else {
        Node* node = _this->head;
        for (i = 0; i < size; i++) {
            if (i > 0)
            printf("%d ", node->pid);
            printf("%s\n", node->name);
            node = node->next;
        }
    }
}
/** create a LinkedList
 */
LinkedList* createLinkedList () {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->add = &add;
    list->addFirst = &addFirst;
    list->addLast = &addLast;
    list->insertBefore = &insertBefore;
    list->get = &get;
    list->getFirst = &getFirst;
    list->getLast = &getLast;
    list->remove = &_remove;
    list->removeFirst = &_removeFirst;
    list->removeLast = &_removeLast;
    list->display = &display;
    list->createNode = &createNode;
    return list;
}
/** create a Node
 */
Node* createNode (int item, char* name) {
    Node* node = (Node*) malloc (sizeof(Node));
    node->pid = item;
    strcpy (node->name, name);
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void deleteList(LinkedList** linkedList)
{
    Node* current = (*linkedList)->head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(*linkedList);
    *linkedList = NULL;
}

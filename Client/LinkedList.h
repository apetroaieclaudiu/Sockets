#include <pthread.h>

typedef struct Node {
    int value;
    void (*callback)(void *data);
    struct Node *next;
} Node;

extern Node *head;
extern pthread_mutex_t lock_head;
extern pthread_mutex_t lock_next;
//This function will add a new node at the end of the list with the value equal to "value"
void add_node(int value);

//This function will delete the node with the specified value
void delete_node(int value);

//A function which will print all the elements of the current list
void print_list();

//A functin that will sort the current list in an ascending order
void sort_list();

//A function that will erase all nodes from the list
void flush_list();

//A function that will print the value of the current nodes
void print_node(void *data);

void sortedInsert(Node **head, Node* new_node);

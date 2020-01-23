#include <pthread.h>

typedef struct Node {
    char file_requested[256];
    int descriptor_id;
    struct Node *next;
    int offset;
} Node;

extern Node *head;
extern pthread_mutex_t lock_head;
extern pthread_mutex_t lock_next;
//This function will add a new node at the end of the list with the value equal to "value"
Node* add_node(char file_req[], int desc_id);

//This function will delete the node with the specified value
void delete_node(int value);

//A function which will print all the elements of the current list
void print_list();

//A function that will erase all nodes from the list
void flush_list();

Node* getNode(int desc_id);

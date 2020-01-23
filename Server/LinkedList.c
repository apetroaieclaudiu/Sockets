#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

Node* head = NULL;
pthread_mutex_t lock_head = PTHREAD_MUTEX_INITIALIZER;   
pthread_mutex_t lock_next = PTHREAD_MUTEX_INITIALIZER;   

void add_node(char file_req[], int desc_id) {
	Node* new_node  = (Node*) malloc(sizeof(Node));
	Node* last;

	new_node->descriptor_id = desc_id;
	strcpy(new_node->file_requested, file_req);	
	new_node->offset = 0;
	new_node->next = NULL;
	if (head == NULL) {
		head = new_node;
		return ;
	}
	last = head;
	while (last->next != NULL) {
		if (last->descriptor_id == desc_id) {
			return ;
		}
		last = last->next;
	}

	last->next = new_node;
}


void delete_node(int value) {
	Node* temp = head;
	Node* prev;

	if (temp != NULL && temp->descriptor_id != value) {
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL) return;

	prev->next = temp->next;

	free (temp);
}

void print_list() {
	Node* temp = head;

	while (temp != NULL) {
		printf("\nFile Requested is: %s\n", temp->file_requested);
		temp = temp->next;
	}
}

void flush_list() {
	struct Node* current = head;
	struct Node* next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	head = NULL;
}

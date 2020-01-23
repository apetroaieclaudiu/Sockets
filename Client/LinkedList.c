#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

Node* head = NULL;
pthread_mutex_t lock_head = PTHREAD_MUTEX_INITIALIZER;   
pthread_mutex_t lock_next = PTHREAD_MUTEX_INITIALIZER;   


void print_node(void *data) {
	int *temp = data;
	printf("%d\n", *temp);
}

void add_node(int value) {
	Node* new_node  = (Node*) malloc(sizeof(Node));
	Node* last;

	new_node->value = value;
	new_node->callback = &print_node;
	new_node->next = NULL;
	if (head == NULL) {
		head = new_node;
		return ;
	}
	last = head;
	while (last->next != NULL) {
		last = last->next;
	}

	last->next = new_node;
}


void delete_node(int value) {
	Node* temp = head;
	Node* prev;

	if (temp != NULL && temp->value != value) {
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
		temp->callback(&temp->value);
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

void sort_list() {
	Node *sorted = NULL;

	Node *current = head;
	while (current != NULL)
	{
		Node *next = current->next;

		sortedInsert(&sorted, current);

		current = next;
	}

	head = sorted;
}


void sortedInsert(Node** head, Node* new_node)
{
	struct Node* current;

	if (*head == NULL || (*head)->value >= new_node->value)
	{
		new_node->next = *head;
		*head = new_node;
	}
	else
	{
		current = *head;
		while (current->next!=NULL &&
				current->next->value < new_node->value)
		{
			current = current->next;
		}
		new_node->next = current->next;
		current->next = new_node;
	}
}

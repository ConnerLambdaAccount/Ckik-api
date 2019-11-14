typedef struct node {
	char* chat;
	struct node* next;
} node;
typedef node chat_list;

// Prints list given head
void print_list(node* head) {
	while(head != NULL) {
		printf("node @ %p\n", head);
		printf("\tchat: %s\n", head->chat);
		printf("\tnext: %p\n", head->next);
		head = head->next;
	}
}

// Used in add_node
node* ll_newnode(node* newnode, char* chat) {
	newnode->chat = malloc((strlen(chat)+1));
	// Error checking malloc
	if(newnode == NULL || newnode->chat == NULL) {
		Ckik_error("malloc", __FILE__, __LINE__);
		return NULL;
	}

	strcpy(newnode->chat, chat);
	newnode->next = NULL;
	return newnode;
}

// set head param to NULL for a new head
// returns address of node on success, NULL on failure
node* add_chat(char* chat, node* head) {
	
	if(head == NULL) {
		// If NULL, then create list head
		head = malloc(sizeof(node));
		return ll_newnode(head, chat);
	}

	// Otherwise, go to last node
	while(head->next != NULL) {
		head = head->next;
	}
	
	// Create new node at end of list
	head->next = malloc(sizeof(node));

	node* new_node = head->next;
	return ll_newnode(new_node, chat);
}

void free_list(chat_list* head) {
	if(head == NULL) { return; }

	node* next;
	node* ph = head;
	while(ph != NULL) {
		next = ph->next;
		free(ph->chat);
		free(ph);
		ph = next;
	}
}

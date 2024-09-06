#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#define DIE(assertion, call_description)				\
	do {												\
		if (assertion) {								\
			fprintf(stderr, "(%s, %d): ",				\
					__FILE__, __LINE__);				\
			perror(call_description);					\
			exit(errno);								\
		}												\
	} while (0)

//function that creates the structure that holds the allocated blocks
occupied_mem_block
*create_occupied_mem_block(heap_t *heap, int bytes_per_list)
{
	occupied_mem_block *new_block = malloc(sizeof(*new_block));
	if (!new_block) {
		perror("Allocation failed");
		exit(EXIT_FAILURE);
	}

	new_block->head = NULL;
	new_block->tail = NULL;
	new_block->size = 0;
	new_block->data_size = sizeof(mem_block);// I retain the addresses as ints
	new_block->adresses =
	malloc(heap->num_lists * bytes_per_list);
	return new_block;
}

//function in which I allocate a doubly linked list
list_t*
dll_create(unsigned int block_size)
{
	list_t *ll;

	ll = malloc(sizeof(*ll));

	if(!ll) {
		perror("Malloc failed");
		exit(EXIT_FAILURE);
	}
	ll->head = NULL;
	ll->block_size = block_size;
	ll->data_size = sizeof(dll_node_t);
	ll->size = 0;

	return ll;
}

//function that returns the nth node from the dll
dll_node_t*
dll_get_nth_node(list_t *list, unsigned int n)
{
	dll_node_t *curr = list->head;
	if (!list || !list->head) {
		printf("Not created or empty list!\n");
		return NULL;
    }
	for (int i = 0; i < n; i++)
		curr = curr->next;
	return curr;
}

//function that removes the nth node from the dll and returns a pointer
//to the removed node
dll_node_t*
dll_remove_nth_node(list_t *list, unsigned int n)
{
	if (!list || !list->head)
		return NULL;
	if (n >= list->size)
		n = list->size - 1;
	if (list->size == 1) {
		dll_node_t *node = list->head;
		list->head = NULL;
		list->tail = NULL;
		list->size = 0;
		return node;
	}
	dll_node_t *curr = dll_get_nth_node(list, n);
	if (n == 0)
		list->head = list->head->next;
	if (n == list->size - 1)
		list->tail = list->tail->prev;
	if (curr->prev)
		curr->prev->next = curr->next;
	if (curr->next)
		curr->next->prev = curr->prev;
	list->size--;
	return curr;
}

//function that adds a node in a doubly linked list at position n
void
dll_add_nth_node(list_t *list, unsigned int n, const void *data)
{
	if (!list) {
		printf("Not created list.\n");
		return;
	}
	dll_node_t *new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "malloc() failed!\n");
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "malloc() failed!\n");
	memcpy(new_node->data, data, list->data_size);
	new_node->prev = NULL;
	new_node->next = NULL;
    //checking if the list has no elements
	if (list->size == 0) {
		list->head = new_node;
		list->tail = new_node;
		new_node->next = NULL;
		new_node->prev = NULL;
	} else {
		dll_node_t *curr, *prev;
		if (n >= list->size) {
			prev = dll_get_nth_node(list, list->size - 1);
			curr = NULL;
	    } else {
		    curr = dll_get_nth_node(list, n);
		    prev = curr->prev;
	    }
	    new_node->prev = prev;
	    new_node->next = curr;
	    if (prev)
		    prev->next = new_node;
	    if (curr)
		    curr->prev = new_node;
	    if (n == 0)
		    list->head = new_node;
	    if (n >= list->size)
		    list->tail = new_node;
	}
	list->size++;
}

unsigned int
dll_get_size(list_t *list)
{
	if (!list)
		return 0;
	return list->size;
}

//function by which I free the memory allocated to each node in the list
//and the list.
void dll_free(list_t **pp_list)
{
	dll_node_t *curr;
	while ((*pp_list)->size != 0) {
		curr = dll_remove_nth_node(*pp_list, 0);
		free(curr->data);
		free(curr);
	}
	free(*pp_list);
	(*pp_list) = NULL;
}

//function with which I display the starting addresses of the blocks
//in hexadecimal form
void
dll_print_addresses(list_t *list)
{
    // checking if the list is null
	if (!list || !list->head)
		return;
	dll_node_t *temp = list->head;
	while (temp) {
		printf(" 0x%x", *((int *)temp->data));
		temp = temp->next;
	}

	printf("\n");
}

//function, in which i iniatialise the heap and create an arraylist
//with lists that have blocks with size of 2 to the power of 3 plus i
//with i from 0 to num_lists
heap_t *init_heap(int heap_base, int num_lists, int bytes_per_list)
{
	heap_t *heap = malloc(sizeof(*heap));
	if (!heap) {
		perror("Malloc failed");
		exit(EXIT_FAILURE);
	}
	heap->num_lists = num_lists;
	heap->heap_base = heap_base;
	heap->size = bytes_per_list;
	heap->segregated_lists = (list_t **)malloc(num_lists * sizeof(list_t *));
	if (!heap->segregated_lists) {
		perror("Malloc failed");
		free(heap);
		exit(EXIT_FAILURE);
	}

	int x = heap_base;
	int y = 1;
	for (int i = 0; i < num_lists; ++i) {
		heap->segregated_lists[i] = dll_create(y * 8);
		if (!heap->segregated_lists[i]) { //defensive programming
			perror("Malloc failed");
			for (int j = 0; j < i; ++j)
				dll_free(&heap->segregated_lists[j]);
			free(heap->segregated_lists);
			free(heap);
			exit(EXIT_FAILURE);
		}
		for (int j = 0; j < bytes_per_list / (y * 8); j++) {
			dll_add_nth_node(heap->segregated_lists[i], j, &x);
			x = x + y * 8;//I update the start address for the next block
		}
		y = y * 2;
	}

	return heap;
}

//function to add a new block allocated to position n
void
add_nth_mem_block(occupied_mem_block *list,
				  const void *address, int bytes, int n)
{
	if (!list || n < 0)
		return;

	mem_block *new_block = (mem_block *)malloc(sizeof(mem_block));
	if (!new_block) {
		perror("Malloc failed");
		return;
	}
	new_block->start_address = malloc(sizeof(int));
	if(!new_block->start_address) {
		free(new_block);
		free(new_block->start_address);
		return;
	}
    //adding to memory the address of the allocated block
	memcpy(new_block->start_address, address, sizeof(int));
	new_block->size = bytes;
	new_block->prev = NULL;
	new_block->next = NULL;
    //checking if the data_structure doesn't have any elements
	if (!list->head || n == 0) {
		new_block->next = list->head;
		new_block->prev = NULL;

		if (list->head)
			list->head->prev = new_block;
		else
			list->tail = new_block;

		list->head = new_block;
	} else {
		struct mem_block *current = list->head;
		int count = 0;
		while (current && count < n - 1) {
			current = current->next;
			count++;
		}

		if (!current) {
			free(new_block);
			return;
		}
		new_block->prev = current;
		new_block->next = current->next;
		if (current->next)
			current->next->prev = new_block;
		else
			list->tail = new_block;
		current->next = new_block;
	}

	list->size++;
	list->data_size += new_block->size;
}

//function to find a list that has blocks of size == bytes
int find(heap_t *heap, int bytes)
{
	int poz = -1;
	for (int i = 0; i < heap->num_lists; i++)
		if (heap->segregated_lists[i]->block_size == bytes) {
			poz = i;
			break;
		}

	return poz;
}

//I go through the list and see which one has size == 0, if there is one I
//remove it with shift and I free it
void delete_null_lists(heap_t *heap)
{
	for (int i = 0; i < heap->num_lists; i++) {
		if (heap->segregated_lists[i]->size == 0) {
			dll_free(&heap->segregated_lists[i]);
			for (int j = i; j < heap->num_lists - 1; j++)
				heap->segregated_lists[j] = heap->segregated_lists[j + 1];
			heap->num_lists--;
			heap->segregated_lists =
			(list_t **)realloc(heap->segregated_lists,
								heap->num_lists * sizeof(list_t *));
		}
	}
}

//function to insert a newly allocated block in the second structure
void
insert_address_in_allocated_blocks(list_t *list, int bytes,
								   occupied_mem_block *blocks)
{
	int pos = 0;
	int address = *((int *)list->head->data);
	mem_block *temp = blocks->head;
	if (!temp) {
		add_nth_mem_block(blocks, &address, bytes, 0);
		pos = 0;
	} else {
		while (temp && temp->next) {
			if (address > *((int *)temp->start_address) &&
				address < *((int *)temp->next->start_address)) {
				pos  = pos + 1;
				break;
			}
			temp = temp->next;
			pos++;
		}
		if (*((int *)blocks->tail->start_address) < address)
			pos = blocks->size;
		if (*((int *)blocks->head->start_address) > address)
			pos = 0;
		add_nth_mem_block(blocks, &address, bytes, pos);
		}
	dll_node_t *curr = dll_remove_nth_node(list, 0);
	free(curr->data);
	free(curr);
}

//function to insert a newly freed block in the list
void add_in_list(list_t *list, int address)
{
	int k = 0;
	dll_node_t *temp = list->head;
	if (*(int *)temp->data > address) {
		k = 0;
		dll_add_nth_node(list, 0, &address);
	} else {
		k = 1;
		while (temp->next && *((int *)temp->next->data) < address) {
			k++;
			temp = temp->next;
		}
		dll_add_nth_node(list, k, &address);
	}
}

//checking if an address is between the datas of two nodes in a list
int condition(int address, mem_block *temp)
{
	return address > *((int *)temp->start_address) &&
			address < *((int *)temp->next->start_address);
}

//checking if an address is between the starting addresses of two allocated
//blocks
int condition_list(int address, dll_node_t *temp)
{
	return address > *((int *)temp->data) &&
			address < *((int *)temp->next->data);
}

//returns on which position I should insert a newly allocated block
int return_position(int address, mem_block *temp)
{
	int pos = 0;
	while (temp && temp->next) {
		if (condition(address, temp)) {
			pos  = pos + 1;
			break;
		}
		temp = temp->next;
		pos++;
	}

	return pos;
}

//I'm looking for which position I should put the block with
//starting_address == address in the list
int return_position_list(int address, dll_node_t *temp)
{
	int pos = 0;
	while (temp && temp->next) {
		if (condition_list(address, temp)) {
			pos  = pos + 1;
			break;
		}
		temp = temp->next;
		pos++;
	}

	return pos;
}

//In this function I'm verifying if I can apply malloc for the size I read from
//stdin. If I can t I display a message. If I can I am looking for a block with
//block_size == num_bytes i want to allocate. If I find free block, I remove it
//from the list. If I don't find any I'm looking for a block that has the block
//size > bytes. I remove the first node and the part that I didn t allocate,the
//part I insert it in a new list if in the arraylist there isn t a list with
//blocks of its size
void
my_malloc(heap_t *heap,  int bytes, occupied_mem_block *blocks,
		  int *malloc_calls, int *frag)
{
	if (bytes > heap->segregated_lists[heap->num_lists - 1]->block_size) {
		printf("Out of memory\n");
		return;
	}
	(*malloc_calls)++;
	int bytes_left;
	int address;
	int pos = 0;
	int ok = 0;
	for (int i = 0; i < heap->num_lists; i++)
		if (heap->segregated_lists[i]->block_size == bytes &&
			heap->segregated_lists[i]->size) {
			ok = 1;
			insert_address_in_allocated_blocks(heap->segregated_lists[i],
											   bytes, blocks);
			break;
		}

	if (ok == 0) {
		pos = 0;
		for (int i = 0; i < heap->num_lists; i++)
			if (heap->segregated_lists[i]->block_size > bytes &&
				heap->segregated_lists[i]->size) {
				address = *((int *)heap->segregated_lists[i]->head->data);
				mem_block *temp = blocks->head;
				if (!temp) {
					add_nth_mem_block(blocks, &address, bytes, 0);
					pos = 0;
				} else {
					pos = return_position(address, temp);
					if (*((int *)blocks->tail->start_address) < address)
						pos = blocks->size;
					if (*((int *)blocks->head->start_address) > address)
						pos = 0;
					add_nth_mem_block(blocks, &address, bytes, pos);
				}
				dll_node_t *curr =
				dll_remove_nth_node(heap->segregated_lists[i], 0);
				free(curr->data);
				free(curr);
				bytes_left = heap->segregated_lists[i]->block_size - bytes;
				break;
				}

		address = address + bytes;
		if (find(heap, bytes_left) != -1) {
			pos = find(heap, bytes_left);
			add_in_list(heap->segregated_lists[pos], address);
		} else {
			heap->num_lists++;
			heap->segregated_lists =
			(list_t **)realloc(heap->segregated_lists,
							   heap->num_lists * sizeof(list_t *));
			for (int j = 0; j < heap->num_lists; j++)
				if (heap->segregated_lists[j]->block_size >
					bytes_left) {
					pos = j;
					break;
				}
			for (int j = heap->num_lists - 1; j > pos; j--)
				heap->segregated_lists[j] =
				heap->segregated_lists[j - 1];
				heap->segregated_lists[pos] = dll_create(bytes_left);
				dll_add_nth_node(heap->segregated_lists[pos], 0, &address);
			}
			(*frag)++;
	}
	delete_null_lists(heap);
}

//removal function of the nth allocated block
void remove_nth_mem_block(occupied_mem_block *list, int n)
{
	if (!list || !list->head)
		return;

	mem_block *current = list->head;
	int count = 0;

	if (n == 0) {
		list->head = current->next;
		if (list->head)
			list->head->prev = NULL;
		free(current->start_address);
		free(current);
		list->size--;
		return;
	}

	while (current && count < n) {
		current = current->next;
		count++;
	}

	if (!current)
		return;

	if (current->prev)
		current->prev->next = current->next;
	if (current->next)
		current->next->prev = current->prev;
	if (current == list->tail)
		list->tail = current->prev;

	free(current->start_address);
	free(current);

	list->size--;
}

void freetype0(heap_t *heap, occupied_mem_block *blocks, int address)
{
	int pos = 0, stop = -1, found = 0;
	unsigned int bytes;
	struct mem_block *temp = blocks->head;
	for (int i = 0; i < blocks->size; i++) {
		if (*((int *)temp->start_address) == address) {
			stop = i;
		for (int j = 0; j < heap->num_lists; j++)
			if (heap->segregated_lists[j]->block_size == temp->size) {
				dll_node_t *temp1 = heap->segregated_lists[j]->head;
				found = 1;
				if ((*(int *)temp1->data) > address) {
					pos = 0;
					found = 1;
				} else {
					pos = return_position_list(address, temp1);
					if ((*(int *)temp1->data) < address)
						pos = heap->segregated_lists[j]->size;
				}
				dll_add_nth_node(heap->segregated_lists[j], pos, &address);
				break;
			}
			break;
		}
		temp = temp->next;
	}

	if (found == 0) {
		mem_block *temp = blocks->head;
		if (!temp) {
			printf("blocks->head is NULL.\n");
			return;
		}

		for (int i = 0; i < stop; i++)
			temp = temp->next;
		int pos = -1;
		if (temp->size < heap->segregated_lists[0]->block_size)
			pos = 0;
		for (int i = 1; i < heap->num_lists ; i++) {
			if (temp->size > heap->segregated_lists[i - 1]->block_size &&
				temp->size < heap->segregated_lists[i]->block_size) {
				pos = i;
				break;
			}
		}

		if (temp->size >
			heap->segregated_lists[heap->num_lists - 1]->block_size)
			pos = heap->num_lists;
		heap->num_lists++;
		heap->segregated_lists =
		(list_t **)realloc(heap->segregated_lists,
		heap->num_lists * sizeof(list_t *));
		for (int i = heap->num_lists - 1; i > pos; i--)
			heap->segregated_lists[i] = heap->segregated_lists[i - 1];
		heap->segregated_lists[pos] = dll_create(temp->size);
		dll_add_nth_node(heap->segregated_lists[pos], 0, &address);
	}
	remove_nth_mem_block(blocks, stop);
}

//function in which I add a block to the list with block_size == size
void add_list(heap_t *heap, int bytes, int address)
{
	int pos = 0;
	if (heap->num_lists) {
		for (int j = 0; j < heap->num_lists; j++)
			if (heap->segregated_lists[j]->block_size > bytes) {
				pos = j;
				break;
			}

		if (heap->segregated_lists[heap->num_lists - 1]->block_size
			< bytes)
			pos = heap->num_lists;
		}
		heap->num_lists++;
		heap->segregated_lists =
		(list_t **)realloc(heap->segregated_lists,
							heap->num_lists * sizeof(list_t *));
		for (int i = heap->num_lists - 1; i > pos; i--)
			heap->segregated_lists[i] = heap->segregated_lists[i - 1];
		heap->segregated_lists[pos] = dll_create(bytes);
		dll_add_nth_node(heap->segregated_lists[pos], 0, &address);
}

// checking if an address was the start of a block from the initial heap
int is_start(int address, int bytes_per_list, int num_lists)
{
	int level;
	for (int i = 0; i < num_lists; i++)
		if (address >= bytes_per_list * i &&
			address < bytes_per_list * (i + 1)) {
			level = i;
			break;
		}

	int x = 8;
	for(int i = 0; i < level; i++)
		x = x * 2;
	return (address % x == 0);
}

//in this function I am looking to refragment, in this sense I am looking to
//see if I can concatenate to the left or to the right but at the same time I
//check unless the block I want to concatenate with doesn't have the start address
//of a block from the initial heap.After I can't find any more blocks to
//concatenate, I'm looking to insert it in the list of proper size
void freetype1(int address, heap_t *heap, occupied_mem_block *blocks,
			   int *frag, int bytes_per_list, int num_lists)
{
	mem_block *temp = blocks->head;
	int k = 0, bytes, pos = 0, found;
	while (temp) {
		if (*((int *)temp->start_address) == address) {
			bytes = temp->size;
			break;
		}

		temp = temp->next;
		k++;
	}

	remove_nth_mem_block(blocks, k);
	for (int i = 0; i < heap->num_lists; i++) {
		if (heap->segregated_lists[i]->size) {
			dll_node_t *temp = heap->segregated_lists[i]->head;
			k = 0;
			int block_size = heap->segregated_lists[i]->block_size;
			found = 0;
			while (temp) {
				if (*(int *)temp->data +
					heap->segregated_lists[i]->block_size ==
					address &&
					is_start(address - heap->heap_base, bytes_per_list,
							 num_lists) == 0) {
					bytes = bytes + heap->segregated_lists[i]->block_size;
					address = (*(int *)temp->data);
					found = 1;
				} else if ((*(int *)temp->data) == address + bytes &&
							is_start((*(int *)temp->data) - heap->heap_base,
									 bytes_per_list, num_lists) == 0) {
					bytes = bytes + heap->segregated_lists[i]->block_size;
					found = 1;
					}
				temp = temp->next;
				if (found == 1) {
					dll_node_t *curr =
					dll_remove_nth_node(heap->segregated_lists[i], k);
					if (curr) {
						free(curr->data);
						free(curr);
					}
					temp = heap->segregated_lists[i]->head;
					k = 0;
					found = 0;
				} else
					k++;
			}
		}
	}

	int positive = 0;
	pos = 0;
	for (int j = 0; j < heap->num_lists; j++) {
		if (heap->segregated_lists[j]->block_size == bytes &&
			heap->segregated_lists[j]->size) {
			positive = 1;
			dll_node_t *temp = heap->segregated_lists[j]->head;
			if (*(int *)temp->data > address) {
				k = 0;
				dll_add_nth_node(heap->segregated_lists[j],
								 0, &address);
				} else {
					k = 0;
					while (temp && *((int *)temp->data)
							< address) {
						k++;
						temp = temp->next;
					}

			dll_add_nth_node(heap->segregated_lists[j],
							 k, &address);
			}
			break;
		}
	}

	if (!positive)
		add_list(heap, bytes, address);
}

void
my_free(heap_t **heap, int address, occupied_mem_block *blocks, int type,
		int *nr_free, int *frag, int bytes_per_list, int num_lists)
{
	int ok = 0;
	struct mem_block *temp = blocks->head;
	//checking if it is a valid free
	for (int i = 0; i < blocks->size; i++)  {
		if (address == *((int *)temp->start_address)) {
			ok = 1;
			break;
			}

		temp = temp->next;
	}
	if (ok == 0) {
		printf("Invalid free");
		printf("\n");
	} else {
		(*nr_free)++;
		if (type == 0)
			freetype0(*heap, blocks, address);
		else
			freetype1(address, *heap, blocks, frag, bytes_per_list, num_lists);
	}
}

void
dump_memory(heap_t *heap, occupied_mem_block *blocks, int malloc_calls,
			int free_calls, int frag, int total_memory)
{
	unsigned int num_free_blocks = 0;
	unsigned int total_allocated_memory = 0;
	unsigned int num_allocated_blocks = 0;
	mem_block *node = blocks->head;
    //determining how many bytes were allocated
	for (int i = 0; i < blocks->size; i++) {
		total_allocated_memory += node->size;
		node = node->next;
	}

	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", total_memory);
	printf("Total allocated memory: %u bytes\n", total_allocated_memory);
	printf("Total free memory: %u bytes\n", total_memory -
			total_allocated_memory);
	for (unsigned int i = 0; i < heap->num_lists; ++i)
		num_free_blocks += dll_get_size(heap->segregated_lists[i]);

	printf("Free blocks: %u\n", num_free_blocks);
	printf("Number of allocated blocks: %u\n", blocks->size);
	printf("Number of malloc calls: %u\n", malloc_calls);
	printf("Number of fragmentations: %d\n", frag);

	printf("Number of free calls: %d\n", free_calls);

	for (int i = 0; i < heap->num_lists; i++) {
		if (heap->segregated_lists[i] && heap->segregated_lists[i]->size) {
			printf("Blocks with %u bytes - %u free block(s) :",
				   heap->segregated_lists[i]->block_size,
				   heap->segregated_lists[i]->size);
			dll_node_t *temp = heap->segregated_lists[i]->head;
			while (temp) {
				printf(" 0x%x", *((int *)temp->data));
				temp = temp->next;
			}
			printf("\n");
		}
	}

	printf("Allocated blocks :");
	if (blocks && blocks->size != 0) {
		mem_block *temp = blocks->head;
		while (temp) {
			printf(" (0x%x - %d)", *((int *)temp->start_address), temp->size);
			temp = temp->next;
		}
	}

	printf("\n");
	printf("-----DUMP-----\n");
}

//in this function I free the second data_structure
void free_occupied_mem_blocks(occupied_mem_block **blocks)
{
	if (!(*blocks))
		return;

	mem_block *current = (*blocks)->head;
	mem_block *next;
	while (current) {
		next = current->next;
		free(current->start_address);
		free(current);
		current = next;
	}
	free((*blocks)->adresses);
	free(*blocks);
}

//in this function I free everything I used
void
destroy_heap(heap_t **heap, occupied_mem_block **blocks)
{
	for (unsigned int i = 0; i < (*heap)->num_lists; ++i)
		dll_free(&((*heap)->segregated_lists[i]));
	free((*heap)->segregated_lists);
	free(*heap);
	free_occupied_mem_blocks(blocks);
}

void
write(heap_t *heap, int address, char *data, int nr_bytes,
	  occupied_mem_block *mem_blocks, int malloc_calls, int free_calls,
	  int frag, int total_memory)
{
	int ok = 0, place = address;
	if (nr_bytes > strlen(data))
		nr_bytes = strlen(data);
	int size = nr_bytes;
	mem_block *curr = mem_blocks->head;
    //I'm looking to see if I have allocated enough blocks to be able to write
    //nr_bytes of characters
	while (curr && ok == 0) {
		if (*((int *)curr->start_address) <= address && address <
					*((int *)curr->start_address) + curr->size) {
			ok = 1;
			break;
		}
		curr = curr->next;
	}
	//I do one last check for the last node
	while (nr_bytes > 0 && curr && ok) {
		nr_bytes = nr_bytes -
				(*((int *)curr->start_address) + curr->size - address);
		address = *((int *)curr->start_address) + curr->size;
		curr = curr->next;
		if (curr && nr_bytes > 0) {
			if (*((int *)curr->start_address) != address) {
				ok = 0;
				break;
			}
		}
	}
    //if I did not find a continuous area, I display a message and stop the
    //program
	if (ok == 0 && nr_bytes > 0) {
		printf("Segmentation fault (core dumped)");
		printf("\n");
		dump_memory(heap, mem_blocks, malloc_calls, free_calls,
					frag, total_memory);
		destroy_heap(&heap, &mem_blocks);
		exit(0);
	} else {
		for (int i = place; i < place + size; i++)
			((char *)mem_blocks->adresses)[i - heap->heap_base] = data[i - place];
	}
}

void
read(heap_t *heap, int address, int nr_bytes, occupied_mem_block *mem_blocks,
	 int malloc_calls, int free_calls, int frag, int total_memory)
{
	int ok = 0, size = nr_bytes, place = address;

	mem_block *curr = mem_blocks->head;
    //I'm looking to see if I have allocated enough blocks to be able to read
    //nr_bytes of characters
	while (curr && ok == 0) {
		if (*((int *)curr->start_address) <= address && address <
						*((int *)curr->start_address) + curr->size) {
			ok = 1;
			break;
		}
		curr = curr->next;
	}
    //I do one last check for the last node
	while (nr_bytes > 0 && curr && ok) {
		nr_bytes = nr_bytes - (*((int *)curr->start_address) +
				   curr->size - address);
		address = *((int *)curr->start_address) + curr->size;
		curr = curr->next;
		if (curr && nr_bytes > 0) {
			if (*((int *)curr->start_address) != address) {
				ok = 0;
				break;
			}
			break;
		}
	}
	if (ok == 0 && nr_bytes > 0) {
		printf("Segmentation fault (core dumped)");
		printf("\n");
		dump_memory(heap, mem_blocks, malloc_calls, free_calls, frag,
					total_memory);
		destroy_heap(&heap, &mem_blocks);
		exit(0);//I display a message, and I stop the program
	} else {
		for (int i = place; i < place + size; i++) {
            char c = ((char *)mem_blocks->adresses)[i - heap->heap_base];
			printf("%c", c);
        }
		printf("\n");
	}
}

//function that extracts a text delimited by quotes from a string of characters
//and an integer
void det_parameters(char *text, char *data, int *size)
{
    char number_buffer[600];
	int index_number = 0, pos;
	int length = strlen(text);
	for (int i = length - 1; i > 0; i--)
		if (text[i] == '"') {
			pos = i;
			break;
		} //I looked for the last quote

	for (int i = 1; i < pos; i++)
		data[i - 1] = text[i];
	pos++;
	for (int i = pos; i < length; i++)
		number_buffer[index_number++] = text[i];//determining the number
	number_buffer[index_number] = '\0';
	*size = atoi(number_buffer);//retaining the number in a int variable
}

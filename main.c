//Copyright 312CAb Popescu Mihnea-Gabriel
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

typedef struct dll_node_t {
	void *data;
	struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct list_t {
	dll_node_t *head, *tail;
	unsigned int block_size;
	unsigned int data_size;
	unsigned int size;
} list_t;

typedef struct {
	int heap_base;
	size_t size;
	list_t **segregated_lists;
	int num_lists;
} heap_t;

typedef struct mem_block {
	void *start_address;
	int size;
	struct mem_block *prev, *next;
} mem_block;

typedef struct occupied_mem_block {
	struct mem_block *head, *tail;
	unsigned int size;
	unsigned int data_size;
	void *adresses;
} occupied_mem_block;

occupied_mem_block *create_occupied_mem_block(heap_t *heap, int bytes_per_list)
{
	occupied_mem_block *new_block = malloc(sizeof(*new_block));
	DIE(!new_block, "malloc failed");
	new_block->head = NULL;
	new_block->tail = NULL;
	new_block->size = 0;
	new_block->data_size = sizeof(mem_block);
	new_block->adresses =
	malloc(heap->num_lists * bytes_per_list);
	return new_block;
}

list_t *dll_create(unsigned int block_size)
{
	list_t *ll;
	ll = malloc(sizeof(*ll));
	DIE(!ll, "malloc failed\n");
	ll->head = NULL;
	ll->block_size = block_size;
	ll->data_size = sizeof(dll_node_t);
	ll->size = 0;
	return ll;
}

dll_node_t *dll_get_nth_node(list_t *list, unsigned int n)
{
	if (!list->head || !list)
		return NULL;
	dll_node_t *current = list->head;
	for (unsigned int i = 0; i < n; i++) {
		if (!current->next)
			current = list->head;
		else
			current = current->next;
	}
	return current;
}

dll_node_t *dll_remove_nth_node(list_t *list, unsigned int n)
{
	if (n == 0) {
		dll_node_t *ans = list->head;
		if (list->head) {
			list->head = list->head->next;
			if (list->head)
				list->head->prev = NULL;
		}
		list->size--;
		return ans;
	}
	dll_node_t *node = list->head;
	for (unsigned int i = 0; node && i < n - 1; ++i)
		node = node->next;
	if (!node)
		return NULL;
	if (node->prev)
		node->prev->next = node->next;
	else
		list->head = node->next;
	if (node->next)
		node->next->prev = node->prev;
	list->size--;
	return node;
}

void dll_add_nth_node(list_t *list, unsigned int n, const void *data)
{
	dll_node_t *node = list->head;
	dll_node_t *new_node = malloc(sizeof(dll_node_t));
	DIE(!new_node, "malloc failed");
	new_node->data = malloc(sizeof(unsigned long));
	DIE(!new_node->data, "malloc failed");
	memcpy(new_node->data, data, sizeof(unsigned long));
	new_node->next = NULL;
	new_node->prev = NULL;
	if (n == 0) {
		new_node->next = list->head;
		if (list->head)
			list->head->prev = new_node;
		list->head = new_node;
		list->size++;
		return;
	}
	if (!node) {
		list->head = new_node;
		list->size++;
		return;
	}
	for (unsigned int i = 0; i < n - 1; ++i) {
		if (!node->next) {
			node->next = new_node;
			new_node->prev = node;
			list->size++;
			return;
		}
		node = node->next;
	}
	if (!node->next) {
		node->next = new_node;
		new_node->prev = node;
		list->size++;
		return;
	}
	new_node->prev = node;
	new_node->next = node->next;
	node->next = new_node;
	new_node->next->prev = new_node;
	list->size++;
}

unsigned int dll_get_size(list_t *list)
{
	dll_node_t *curr = list->head;
	unsigned int count = 0;
	while (curr) {
		count++,
		curr = curr->next;
	}
	return count;
}

void dll_free(list_t **pp_list)
{
	dll_node_t *currnode;
	if (!pp_list || !(*pp_list))
		return;
	while (dll_get_size(*pp_list) > 0) {
		currnode = dll_remove_nth_node(*pp_list, 0);
		free(currnode->data);
		currnode->data = NULL;
		free(currnode);
		currnode = NULL;
	}
	free(*pp_list);
	*pp_list = NULL;
}

heap_t *init_heap(int heap_base, int num_lists, int bytes_per_list)
{
	heap_t *heap = malloc(sizeof(*heap));
	DIE(!heap, "malloc failed");
	heap->num_lists = num_lists;
	heap->heap_base = heap_base;
	heap->size = bytes_per_list;
	heap->segregated_lists = malloc(num_lists * sizeof(list_t *));
	DIE(!heap->segregated_lists, "malloc failed");
	int x = heap_base;
	for (int i = 0; i < num_lists; ++i) {
		heap->segregated_lists[i] = dll_create((1 << i) * 8);
		for (int j = 0; j < bytes_per_list / ((1 << i) * 8); j++) {
			dll_add_nth_node(heap->segregated_lists[i], j, &x);
			x += (1 << i) * 8;
		}
	}
	return heap;
}

void add_nth_mem_block(occupied_mem_block *list,
					   const void *address, int bytes, int n)
{
	if (!list || n < 0)
		return;
	mem_block *new_block = malloc(sizeof(mem_block));
	if (!new_block) {
		perror("malloc failed");
		return;
	}
	new_block->start_address = malloc(sizeof(int));
	if (!new_block->start_address) {
		free(new_block);
		perror("malloc failed");
		return;
	}
	memcpy(new_block->start_address, address, sizeof(int));
	new_block->size = bytes;
	new_block->prev = NULL;
	new_block->next = NULL;
	if (!list->head || n == 0) {
		new_block->next = list->head;
		if (list->head)
			list->head->prev = new_block;
		else
			list->tail = new_block;
		list->head = new_block;
	} else {
		mem_block *current = list->head;
		int count = 0;
		while (current && count < n - 1) {
			count++;
			current = current->next;
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

int find(heap_t *heap, int bytes)
{
	int poz = -1;
	int i = 0;
	while (i < heap->num_lists) {
		if (heap->segregated_lists[i]->block_size == (unsigned int)bytes) {
			poz = i;
			break;
		}
		i++;
	}
	return poz;
}

void delete_null_lists(heap_t *heap)
{
	for (int i = 0; i < heap->num_lists;) {
		if (heap->segregated_lists[i]->size == 0) {
			dll_free(&heap->segregated_lists[i]);
			for (int j = i; j < heap->num_lists - 1; j++)
				heap->segregated_lists[j] = heap->segregated_lists[j + 1];
			heap->num_lists--;
			heap->segregated_lists = (list_t **)realloc(heap->segregated_lists,
														heap->num_lists *
														sizeof(list_t *));
		} else {
			i++;
		}
	}
}

void insert_address_in_allocated_blocks_list(list_t *list, int bytes,
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
				pos++;
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
			temp = temp->next;
			k++;
		}
		dll_add_nth_node(list, k, &address);
	}
}

int is_address_between_blocks(int address, mem_block *temp)
{
	return address > *((int *)temp->start_address) &&
		address < *((int *)temp->next->start_address);
}

int is_address_between_start_addresses(int address, dll_node_t *temp)
{
	return address > *((int *)temp->data) &&
		address < *((int *)temp->next->data);
}

int return_position_address(int address, mem_block *temp)
{
	int pos = 0;
	while (temp && temp->next) {
		if (is_address_between_blocks(address, temp)) {
			pos++;
			break;
		}
		temp = temp->next;
		pos++;
	}
	return pos;
}

int return_position_address_list(int address, dll_node_t *temp)
{
	int pos = 0;
	while (temp && temp->next) {
		if (is_address_between_start_addresses(address, temp)) {
			pos++;
			break;
		}
		temp = temp->next;
		pos++;
	}
	return pos;
}

//realoc numarul de liste si fac una corespunzatoare pentru adresa address
void upgrade_list(heap_t *heap, int bytes_left, int pos, int j, int address)
{
	heap->num_lists++;
	heap->segregated_lists =
		(list_t **)realloc(heap->segregated_lists,
						heap->num_lists * sizeof(list_t *));
	j = 0;
	while (j < heap->num_lists) {
		if (heap->segregated_lists[j]->block_size >
			(unsigned int)bytes_left) {
			pos = j;
			break;
		}
		j++;
	}
	int k = heap->num_lists - 1;
	while (k > pos) {
		heap->segregated_lists[k] = heap->segregated_lists[k - 1];
		k--;
	}
	heap->segregated_lists[pos] = dll_create(bytes_left);
	dll_add_nth_node(heap->segregated_lists[pos], 0, &address);
}

void my_malloc(heap_t *heap,  int bytes, occupied_mem_block *blocks,
			   int *malloc_calls, int *frag)
{
	if ((unsigned int)bytes >
		heap->segregated_lists[heap->num_lists - 1]->block_size) {
		printf("Out of memory\n");
		return;
	}
	int bytes_left, address, pos = 0, ok = 0, i = 0;
	while (i < heap->num_lists && ok == 0) {
		if (heap->segregated_lists[i]->block_size == (unsigned int)bytes &&
			heap->segregated_lists[i]->size) {
			ok = 1;
			insert_address_in_allocated_blocks_list(heap->segregated_lists[i],
													bytes, blocks);
			break;
		}
		i++;
	}
	if (ok == 0) {
		(*frag)++;
		pos = 0, i = 0;
		while (i < heap->num_lists) {
			if (heap->segregated_lists[i]->block_size > (unsigned int)bytes &&
				heap->segregated_lists[i]->size) {
				address = *((int *)heap->segregated_lists[i]->head->data);
				bytes_left = heap->segregated_lists[i]->block_size - bytes;
				mem_block *temp = blocks->head;
				if (!temp) {
					add_nth_mem_block(blocks, &address, bytes, 0);
					pos = 0;
				} else {
					pos = return_position_address(address, temp);
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
				break;
			}
			i++;
		}
		address += bytes;
		int j = find(heap, bytes_left);
		if (j != -1) {
			pos = j;
			add_in_list(heap->segregated_lists[pos], address);
		} else {
			upgrade_list(heap, bytes_left, pos, j, address);
		}
	}
	(*malloc_calls)++;
	delete_null_lists(heap);
}

void remove_nth_mem_block(occupied_mem_block *list, int n)
{
	if (!list || !list->head || n < 0)
		return;
	mem_block *current = list->head;
	int count = 0;
	while (current && count < n) {
		current = current->next;
		count++;
	}
	if (!current)
		return;
	if (current == list->head) {
		list->head = current->next;
		if (list->head)
			list->head->prev = NULL;
	} else {
		if (current->prev)
			current->prev->next = current->next;
	}
	if (current == list->tail)
		list->tail = current->prev;
	else
		current->next->prev = current->prev;
	free(current->start_address);
	free(current);
	list->size--;
}

void find_pos(int j, heap_t *heap, int *found, int address, mem_block *temp)
{
	int pos;
	while (j < heap->num_lists) {
		if (heap->segregated_lists[j]->block_size == (unsigned int)temp->size) {
			dll_node_t *temp1 = heap->segregated_lists[j]->head;
			*found = 1;
			if ((*(int *)temp1->data) > address) {
				pos = 0;
				*found = 1;
			} else {
				pos = return_position_address_list(address, temp1);
				if ((*(int *)temp1->data) < address)
					pos = heap->segregated_lists[j]->size;
			}
			dll_add_nth_node(heap->segregated_lists[j], pos, &address);
			break;
		}
		j++;
	}
}

//free-ul de tip 0
void freetype0(heap_t *heap, occupied_mem_block *blocks, int address)
{
	int stop = -1, found = 0;
	struct mem_block *temp = blocks->head;
	unsigned int i = 0;
	while (i < blocks->size) {
		if (*((int *)temp->start_address) == address) {
			stop = i;
			int j = 0;
			find_pos(j, heap, &found, address, temp);
			break;
		}
		temp = temp->next;
		i++;
	}
	if (found == 0) {
		mem_block *temp = blocks->head;
		DIE(!temp, "malloc failed");
		int i = 0;
		while (i < stop) {
			temp = temp->next;
			i++;
		}
		int pos = -1;
		if ((unsigned int)temp->size < heap->segregated_lists[0]->block_size)
			pos = 0;
		i = 1;
		while (i < heap->num_lists) {
			if ((unsigned int)temp->size >
				heap->segregated_lists[i - 1]->block_size &&
				(unsigned int)temp->size <
				heap->segregated_lists[i]->block_size) {
				pos = i;
				break;
			}
			i++;
		}
		if ((unsigned int)temp->size >
			heap->segregated_lists[heap->num_lists - 1]->block_size)
			pos = heap->num_lists;
		heap->num_lists++;
		heap->segregated_lists = (list_t **)realloc(heap->segregated_lists,
													heap->num_lists *
													sizeof(list_t *));
		i = heap->num_lists - 1;
		while (i > pos) {
			heap->segregated_lists[i] = heap->segregated_lists[i - 1];
			i--;
		}
		heap->segregated_lists[pos] = dll_create(temp->size);
		dll_add_nth_node(heap->segregated_lists[pos], 0, &address);
	}
	remove_nth_mem_block(blocks, stop);
}

void my_free(heap_t **heap, int address, occupied_mem_block *blocks, int type,
			 int *nr_free)
{
	int ok = 0;
	struct mem_block *temp = blocks->head;
	unsigned int i = 0;
	while (i < blocks->size) {
		if (address == *((int *)temp->start_address)) {
			ok = 1;
			break;
		}
		temp = temp->next;
		i++;
	}
	if (ok == 0) {
		printf("Invalid free");
		printf("\n");
	} else {
		(*nr_free)++;
		if (type == 0)
			freetype0(*heap, blocks, address);
	}
}

void print_list_addresses(list_t *list)
{
	dll_node_t *temp = list->head;
	while (temp) {
		printf(" 0x%x", *((int *)temp->data));
		temp = temp->next;
	}
}

void print_sfl(heap_t *heap)
{
	for (int i = 0; i < heap->num_lists; i++) {
		list_t *list = heap->segregated_lists[i];
		if (list && list->size > 0) {
			printf("Blocks with %u bytes - %u free block(s) :",
				   list->block_size, list->size);
			print_list_addresses(list);
			printf("\n");
		}
	}
}

void print_blocks_addresses(mem_block *block)
{
	mem_block *temp = block;
	while (temp) {
		printf(" (0x%x - %d)", *((int *)temp->start_address), temp->size);
		temp = temp->next;
	}
}

void print_memory_stats(occupied_mem_block *blocks, int total_memory,
						unsigned int total_allocated_memory,
						unsigned int num_free_blocks, int malloc_calls,
						int free_calls, int frag)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", total_memory);
	printf("Total allocated memory: %u bytes\n", total_allocated_memory);
	printf("Total free memory: %u bytes\n",
		   total_memory - total_allocated_memory);
	printf("Free blocks: %u\n", num_free_blocks);
	printf("Number of allocated blocks: %u\n", blocks->size);
	printf("Number of malloc calls: %u\n", malloc_calls);
	printf("Number of fragmentations: %d\n", frag);
	printf("Number of free calls: %d\n", free_calls);
}

int count_free_blocks(heap_t *heap)
{
	int num_free_blocks = 0;
	for (int i = 0; i < heap->num_lists; ++i)
		num_free_blocks += dll_get_size(heap->segregated_lists[i]);
	return num_free_blocks;
}

void print_allocated_blocks(occupied_mem_block *blocks)
{
	printf("Allocated blocks :");
	if (blocks && blocks->size != 0) {
		mem_block *temp = blocks->head;
		while (temp) {
			printf(" (0x%x - %d)", *((int *)temp->start_address), temp->size);
			temp = temp->next;
		}
	}
}

void dump_memory(heap_t *heap, occupied_mem_block *blocks, int malloc_calls,
				 int free_calls, int frag, int total_memory)
{
	unsigned int total_allocated_memory = 0;
	mem_block *node = blocks->head;
	while (node) {
		total_allocated_memory += node->size;
		node = node->next;
	}
	int num_free_blocks = count_free_blocks(heap);
	print_memory_stats(blocks, total_memory, total_allocated_memory,
					   num_free_blocks, malloc_calls, free_calls, frag);
	print_sfl(heap);
	print_allocated_blocks(blocks);
	printf("\n-----DUMP-----\n");
}

void free_occupied_mem_blocks(occupied_mem_block **blocks)
{
	if (!(*blocks))
		return;
	mem_block *current = (*blocks)->head;
	mem_block *next;
	for (; current; current = next) {
		next = current->next;
		free(current->start_address);
		free(current);
	}
	free((*blocks)->adresses);
	free(*blocks);
	*blocks = NULL;
}

void destroy_heap(heap_t **heap, occupied_mem_block **blocks)
{
	int i = 0;
	while (i < (*heap)->num_lists) {
		dll_free(&((*heap)->segregated_lists[i]));
		i++;
	}
	free((*heap)->segregated_lists);
	free(*heap);
	*heap = NULL;
	free_occupied_mem_blocks(blocks);
	*blocks = NULL;
}

int check_valid_access(heap_t *heap, int address, int nr_bytes,
					   occupied_mem_block *mem_blocks, int total_memory)
{
	mem_block *curr = mem_blocks->head;
	while (curr) {
		if (address >= *((int *)curr->start_address) &&
			address < (*((int *)curr->start_address) + curr->size)) {
			if ((address + nr_bytes) <= (heap->heap_base + total_memory))
				return 1;
			break;
		}
		curr = curr->next;
	}
	return 0;
}

void dump_and_destroy(heap_t *heap, occupied_mem_block *mem_blocks,
					  int malloc_calls, int free_calls, int frag,
					  int total_memory)
{
	dump_memory(heap, mem_blocks, malloc_calls, free_calls, frag,
				total_memory);
	destroy_heap(&heap, &mem_blocks);
}

void write(heap_t *heap, int address, char *data, int nr_bytes,
		   occupied_mem_block *mem_blocks, int malloc_calls, int free_calls,
		   int frag, int total_memory)
{
	int ok = check_valid_access(heap, address, nr_bytes, mem_blocks,
								total_memory);
	if (!ok) {
		printf("Segmentation fault (core dumped)\n");
		dump_and_destroy(heap, mem_blocks, malloc_calls, free_calls,
						 frag, total_memory);
		exit(0);
	}
	memcpy((char *)mem_blocks->adresses + (address - heap->heap_base),
		   data, nr_bytes);
}

void read(heap_t *heap, int address, int nr_bytes,
		  occupied_mem_block *mem_blocks,
		  int malloc_calls, int free_calls, int frag, int total_memory)
{
	int ok = check_valid_access(heap, address, nr_bytes,
								mem_blocks, total_memory);
	if (!ok) {
		printf("Segmentation fault (core dumped)\n");
		dump_and_destroy(heap, mem_blocks, malloc_calls, free_calls,
						 frag, total_memory);
		exit(0);
	}
	for (int i = 0; i < nr_bytes; i++) {
		char c = *((char *)mem_blocks->adresses + (address - heap->heap_base)
				   + i);
		printf("%c", c);
	}
	printf("\n");
}

void det_parameters(char *text, char *data, int *size)
{
int length = strlen(text);
	int pos = -1;
	for (int i = length - 1; i > 0; i--) {
		if (text[i] == '"') {
			pos = i;
			break;
		}
	}
	if (pos > 0 && pos < length - 1) {
		strncpy(data, &text[1], pos - 1);
		data[pos - 1] = '\0';
	}
	if (pos >= 0 && pos < length - 1) {
		char number_buffer[600];
		strncpy(number_buffer, &text[pos + 1], length - pos);
		*size = atoi(number_buffer);
	}
}

int main(void)
{
	heap_t *heap = NULL;
	occupied_mem_block *blocks = NULL;
	char *command;
	int type, free_calls = 0, malloc_calls = 0, frag = 0, total_memory;
	int address;
	int num_lists, bytes_per_list;
	int size;
	char data[600] = "", buffer[600] = "", c;
	command = (char *)malloc(600 * sizeof(char));
	if (!command) {
		free(command);
		exit(0);
	}
	scanf("%s", command);
	while (strcmp(command, "DESTROY_HEAP") != 0) {
		if (strcmp(command, "INIT_HEAP") == 0) {
			scanf("%x %d %d %d", &address, &num_lists, &bytes_per_list, &type);
			heap = init_heap(address, num_lists, bytes_per_list);
			blocks = create_occupied_mem_block(heap, bytes_per_list);
			total_memory = num_lists * bytes_per_list;
		} else if (strcmp(command, "MALLOC") == 0) {
			scanf("%u", &size);
			my_malloc(heap, size, blocks, &malloc_calls, &frag);
		} else if (strcmp(command, "FREE") == 0) {
			scanf("%x", &address);
			my_free(&heap, address, blocks, type, &free_calls);
		} else if (strcmp(command, "READ") == 0) {
			scanf("%x %d", &address, &size);
			read(heap, address, size, blocks, malloc_calls,
				 free_calls, frag, total_memory);
		} else if (strcmp(command, "WRITE") == 0) {
			scanf("%x", &address);
			scanf("%c", &c);//citesc spatiul
			fgets(buffer, 600, stdin);
			buffer[strlen(buffer)] = '\0';
			det_parameters(buffer, data, &size);//determin ce trebuie sa scriu
			write(heap, address, data, size, blocks, malloc_calls,
				  free_calls, frag, total_memory);
		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump_memory(heap, blocks, malloc_calls, free_calls,
						frag, total_memory);
		}
		scanf("%s", command);
	}
	destroy_heap(&heap, &blocks);
	free(command);
	return 0;
}


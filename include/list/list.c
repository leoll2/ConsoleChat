#include "list.h"

/*DEFINITIONS*/
/*Compare size bytes from start and pattern. Returns 0 if matches, 1 otherwise*/
int ldatacmp(void* start, unsigned int size, void* pattern, bool indirect) {
    char *p = indirect ? *(char**)start : (char *)start;
	for (unsigned int i = 0; i < size; ++i) {
		if (*(p + i) != *((char *)pattern + i))
			return 1;
	}
	return 0;
}

void ldatacpyval(list_node* elem, void* data, unsigned int size) {
	elem->data = malloc(size);
	for (unsigned int i=0; i < size; ++i)
		*(char *)(elem->data + i) = *(char *)(data + i);
}

void ldatacpyptr(list_node* elem, void* data) {
	elem->data = data;
}

list list_new() {
	return NULL;
}

void list_print(list l, void (*f)(void *)) {
	while (l != NULL){
		(*f)(l->data);
		l = l->next;
	}
}

/*Creates a copy of the input data in the memory heap.
 * Check list_add_head_by_pointer to add without copying*/
void list_add_head_by_value(list* l, void* data, unsigned int size) {
	list_node* newelem;
	newelem = malloc(sizeof(list_node));
	ldatacpyval(newelem, data, size);
	newelem->next = *l;
	*l = newelem;
}

/*Doesn't create a copy of the input data, hence the
 * caller shouldn't directly deallocate it.
 * Check list_add_head_by_value to add with safely copying.*/
void list_add_head_by_pointer(list* l, void* data) {
	list_node* newelem;
	newelem = malloc(sizeof(list_node));
	ldatacpyptr(newelem, data);
	newelem->next = *l;
	*l = newelem;
}

/*Creates a copy of the input data in the memory heap.
 * Check list_add_tail_by_pointer to add without copying*/
void list_add_tail_by_value(list* l, void* data, unsigned int size) {
	list_node* newelem;
	list_node* p;
	for (newelem = *l; newelem != NULL; newelem = newelem->next)
		p = newelem;
	newelem = malloc(sizeof(list_node));
	ldatacpyval(newelem, data, size);
	newelem->next = NULL;
	if (*l == NULL)
		*l = newelem;
	else p->next = newelem;
}

/*Doesn't create a copy of the input data, hence the
 * caller shouldn't directly deallocate it.
 * Check list_add_tail_by_value to add with safely copying.*/
void list_add_tail_by_pointer(list* l, void* data) {
	list_node* newelem;
	list_node* p;
	for (newelem = *l; newelem != NULL; newelem = newelem->next)
		p = newelem;
	newelem = malloc(sizeof(list_node));
	ldatacpyptr(newelem, data);
	newelem->next = NULL;
	if (*l == NULL)
		*l = newelem;
	else p->next = newelem;
}

/*Removes from the list the first element whose data field is the
 * same of pattern for 'size' bytes, starting from 'elem->data + offset'.
 * Returns the removed element, NULL if not found.*/
list_node* list_extract(list* l, unsigned int offset, unsigned int size, void* pattern, bool ind) {
	list_node *p = NULL;
	list_node *q;
	for (q = *l; q != NULL && ldatacmp((q->data + offset), size, pattern, ind); q = q->next)
		p = q;
	if (q == NULL) return NULL;	//not found
	if (q == *l)
		*l = q->next;
	else
		p->next = q->next;
	return q;
}

list_node* list_extract_head(list* l) {
	list_node* q = *l;
	if (q == NULL) return NULL;		//empty
	*l = q->next;
	return q;
}

list_node* list_extract_tail(list* l) {
	list_node *p = NULL;
	list_node *q;
	if (*l == NULL) return NULL;	//empty
	for (q = *l; q->next != NULL; q = q->next)
		p = q;
	if (q == *l)
		*l = NULL;
	else
		p->next = NULL;
	return q;
}

bool list_remove(list* l, unsigned int offset, unsigned int size, void* pattern, bool ind) {
	list_node *elem = list_extract(l, offset, size, pattern, ind);
	if (!elem) return false;	//if not found
	free(elem->data);
	free(elem);
	return true;
}

list_node* list_find(list l, unsigned int offset, unsigned int size, void* pattern, bool ind) {
	list_node *p = l;
	while (p != NULL && ldatacmp((p->data + offset), size, pattern, ind))
		p = p->next;
	return p;
}

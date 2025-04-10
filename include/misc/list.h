#ifndef LIST_H_KERNEL
#define LIST_H_KERNEL

/*
 * An intrusive linked list for use in the kernel
 */

#include <stddef.h>

struct KList {
    struct KList *next;
    struct KList *prev;
};

typedef struct KList KNode;

struct KListIter {
    struct KList *list;
    KNode* cursor;
};


/* 
 * Does pointer arithmetic to get the pointer of to the struct that contains
 * the attribute.
 *
 * Arguments:
 *      (KNode*) entry_ptr: A pointer to a KNode attribute in a struct.
 *      struct: The name of the struct that holds the attribute 
 *          which entry_pointer points to.
 *      attr: The name of the attribute that entry_ptr points to.
 *
 *  THIS ASSUMES YOU ARE RIGHT ABOUT THE ATTRIBUTE AND CLASS BE CAREFUL!!!!
 */
#define knode_data(entry_ptr, struc, attr) \
    (struc*)((char*) entry_ptr - offsetof(struc, attr));


/*
 * Initializes a list as empty
 */
static inline void list_init(struct KList *list) {
    list->next = list;
    list->prev = list;
}


/*
 * Returns 1 if list is empty else 0
 */
static inline int list_empty(struct KList *list) {
    return list->next == list;
}


/*
 * Adds a node to list at the head of the list.
 *
 * Args:
 *      (struct KList*) list: the list that is being added to.
 *      (KNode) node: The node to be added (the node needs to be
 *          an attribute of a class)
 */
static inline void list_add(struct KList *list, KNode *node) {
    node->next = list->next;
    node->prev = list;
    list->next->prev = node;
    list->next = node;
}


/*
 * Adds a node to list at the tail of the list.
 *
 * Args:
 *      (struct KList*) list: the list that is being added to.
 *      (KNode) node: The node to be added (the node needs to be
 *          an attribute of a class)
 */
static inline void list_add_tail(struct KList *list, KNode *node) {
    node->prev = list->prev;
    node->next = list;
    list->prev->next = node;
    list->prev = node;
}


/*
 * Pops a node out of the list it is in.
 *
 * ASSUMPTION: assumnes the know is actually in a list.
 *
 * Args:
 *      (KNode*) node: A pointer to the node being popped.
 *
 * Returns a pointer to the node that was popped (same as the inputted node).
 */
static inline KNode* knode_pop(KNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    return node;
}


/*
 * Pops a node out of a list from the head.
 *
 * Args:
 *      (struct KList*) list: the list to pop a node out of.
 *
 * Returns a pointer to the popped knode.
 */
static inline KNode* list_pop(struct KList *list) {
    return knode_pop(list->next);
}


/*
 * Pops a node out of a list from the tail.
 *
 * Args:
 *      (struct KList*) list: the list to pop a node out of.
 *
 * Returns a pointer to the popped knode.
 */
static inline KNode* list_pop_tail(struct KList *list) {
    return knode_pop(list->prev);
}


/*
 * Returns a pointer to the first knode in the list.
 */
static inline KNode* list_first(struct KList *list) {
    return list->next;
}


/*
 * Returns a pointer to the last knode in the list.
 */
static inline KNode* list_last(struct KList *list) {
    return list->prev;
}


/* All functions below were not used in the current OS but will still be included for future use */


static inline void li_go_first(struct KListIter *iter) {
    iter->cursor = iter->list->next;
}

static inline void li_go_last(struct KListIter *iter) {
    iter->cursor = iter->list->prev;
}

static inline void li_init(struct KListIter *iter, struct KList *list) {
    iter->list = list;
    li_go_first(iter);
}

static inline void li_go_forth(struct KListIter *iter) {
    iter->cursor = iter->cursor->next;
}

static inline void li_go_back(struct KListIter *iter) {
    iter->cursor = iter->cursor->prev;
}

static inline int li_is_entry(struct KListIter *iter) {
    return !(iter->cursor == iter->list);
}

static inline KNode* li_get_entry(struct KListIter *iter) {
    return iter->cursor;
}

static inline KNode* li_pop_entry(struct KListIter *iter) {
    KNode* entry;
    entry = knode_pop(iter->cursor);
    li_go_forth(iter);
    return entry;
}

#endif


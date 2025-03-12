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
 * Does pointer arithmetic to get the pointer of the struct containing
 * the attribute pointer to by ptr
 *
 * example:
 *  struct tclass {
 *      int attr1;
 *      int attr2;
 *  }
 *
 *  struct tclass t;
 *
 *  x = &t.attr2;
 *
 *  Then get_container(x, struct tclass, attr2) will equal &t
 *
 *  THIS ASSUMES YOU ARE RIGHT ABOUT THE ATTRIBUTE AND CLASS BE CAREFUL!!!!
 */
#define knode_data(entry_ptr, struc, attr) \
    (struc*)((char*) entry_ptr - offsetof(struc, attr));

static inline void list_init(struct KList *list) {
    list->next = list;
    list->prev = list;
}

static inline int list_empty(struct KList *list) {
    return list->next == list;
}

static inline void list_add(struct KList *list, KNode *node) {
    node->next = list->next;
    node->prev = list;
    list->next->prev = node;
    list->next = node;
}

static inline void list_add_tail(struct KList *list, KNode *node) {
    node->prev = list->prev;
    node->next = list;
    list->prev->next = node;
    list->prev = node;
}

static inline KNode* knode_pop(KNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    return node;
}

static inline KNode* list_pop(struct KList *list) {
    return knode_pop(list->next);
}

static inline KNode* list_pop_tail(struct KList *list) {
    return knode_pop(list->prev);
}

static inline KNode* list_first(struct KList *list) {
    return list->next;
}

static inline KNode* list_last(struct KList *list) {
    return list->prev;
}

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

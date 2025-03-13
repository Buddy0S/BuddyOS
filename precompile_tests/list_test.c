#include <list.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_TESTS 7

struct TestCase {
    int (*test)(void);
    const char* test_string;
};

struct DummyData {
    int id;
    KNode node;
};

int test0(void) {
    struct KList list;
    list_init(&list);
    if (list_empty(&list)) {
        return 0;
    } else {
        return -1;
    }
}

int test1(void) {
    struct KList list;
    struct DummyData d, *retrieved;
    KNode *n;

    d.id = 0;

    list_init(&list);
    list_add(&list, &d.node);

    if (list_empty(&list)) {
        return -1;
    }
    n = list_pop(&list);
    retrieved = knode_data(n, struct DummyData, node);

    if (retrieved == &d) {
        return 0;
    } else {
        return -1;
    }
}

int test2(void) {
    struct KList list;
    struct DummyData d, *retrieved;
    KNode *n;

    d.id = 0;

    list_init(&list);
    list_add_tail(&list, &d.node);

    if (list_empty(&list)) {
        return -1;
    }
    n = list_pop_tail(&list);
    retrieved = knode_data(n, struct DummyData, node);

    if (retrieved == &d) {
        return 0;
    } else {
        return -1;
    }
}

int test3(void) {
    struct KList list;
    struct DummyData *d, *retrieved;
    KNode *n;
    int num_nodes, err_occured;
    
    err_occured = 0;
    num_nodes = 10;
   
    list_init(&list);
    
    for (int i = 0; i < num_nodes; i++) {
        d = malloc(sizeof(struct DummyData));
        d->id = i;
        list_add(&list, &d->node);
 
        if (list_empty(&list)) {
            err_occured = 1;
        }
    }

    for (int i = num_nodes-1; i >= 0; i--) {
        n = list_pop(&list);
        retrieved = knode_data(n, struct DummyData, node);
        if (retrieved->id != i) {
            err_occured = 1;
        }

        free(retrieved);
    }

    if (list_empty(&list) && !err_occured) {
        return 0;
    } else {
        return -1;
    }
}

int test4(void) {
    struct KList list;
    struct DummyData *d, *retrieved;
    KNode *n;
    int num_nodes, err_occured;
    
    err_occured = 0;
    num_nodes = 10;
   
    list_init(&list);
    
    for (int i = 0; i < num_nodes; i++) {
        d = malloc(sizeof(struct DummyData));
        d->id = i;
        list_add_tail(&list, &d->node);
 
        if (list_empty(&list)) {
            err_occured = 1;
        }
    }

    for (int i = num_nodes-1; i >= 0; i--) {
        n = list_pop_tail(&list);
        retrieved = knode_data(n, struct DummyData, node);
        if (retrieved->id != i) {
            err_occured = 1;
        }

        free(retrieved);
    }

    if (list_empty(&list) && !err_occured) {
        return 0;
    } else {
        return -1;
    }
}

int test5(void) {
    struct KList list;
    struct DummyData *d, *retrieved;
    KNode *n;
    int num_nodes, err_occured;
    
    err_occured = 0;
    num_nodes = 10;
   
    list_init(&list);
    
    for (int i = 0; i < num_nodes; i++) {
        d = malloc(sizeof(struct DummyData));
        d->id = i;
        list_add_tail(&list, &d->node);
 
        if (list_empty(&list)) {
            err_occured = 1;
        }
    }

    for (int i = 0; i < num_nodes; i++) {
        n = list_pop(&list);
        retrieved = knode_data(n, struct DummyData, node);
        if (retrieved->id != i) {
            err_occured = 1;
        }

        free(retrieved);
    }

    if (list_empty(&list) && !err_occured) {
        return 0;
    } else {
        return -1;
    }
}

int test6(void) {
    struct KList list;
    struct DummyData d1, d2, *retrieved;
    KNode *n;
    int err_occured;

    list_init(&list);
    err_occured = 0;

    d1.id = 1;
    list_add(&list, &d1.node);
    
    d2.id = 2;
    list_add_tail(&list, &d2.node);
    
    if (list_empty(&list)) {
        err_occured = 1;
    } else {
        return 0;
    }

    n = list_first(&list);
    retrieved = knode_data(n, struct DummyData, node);
    if (retrieved->id != 1) {
        err_occured = 1;
    }
    
    n = list_last(&list);
    retrieved = knode_data(n, struct DummyData, node);
    if (retrieved->id != 2) {
        err_occured = 1;
    }

    return err_occured;
}

struct TestCase testcases[NUM_TESTS] = {
    {test0, "LIST: Checking if a list is empty"},
    {test1, "LIST: Adding and removing a single item"},
    {test2, "LIST: Adding and removing a single item from the tail"},
    {test3, "LIST: Adding and removing multiple items"},
    {test4, "LIST: Adding and removing multiple items from the tail"},
    {test5, "LIST: Adding to tail and removing from head"},
    {test6, "LIST: Retrieving the first and last item from a list"}
};


int main(void) {
   for (int i = 0; i < NUM_TESTS; i++) {
       printf("%s: ", testcases[i].test_string);
       if (testcases[i].test()) {
           printf("\033[31;1m✘\033[0m\n");
       } else {
           printf("\033[32;1m✔\033[0m\n");
       }
   }
}


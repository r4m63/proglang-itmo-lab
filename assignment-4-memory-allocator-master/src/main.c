#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "mem_internals.h"

enum test_result_status {
    TEST_OK = 0,
    TEST_INIT_FAIL,
    TEST_ALLOCATE_FAIL,
    TEST_FAILED,
};

char *const test_result_status_messages[] = {
        [TEST_OK] = "ok",
        [TEST_INIT_FAIL] = "error during heap initialization",
        [TEST_ALLOCATE_FAIL] = "error during memory allocation",
        [TEST_FAILED] = "error during test",
};

typedef enum test_result_status (*test)(void);

struct list_test {
    test func;
    struct list_test *next;
};

struct list_test *list_test_node_create(test value) {
    struct list_test *l = malloc(sizeof(struct list_test));
    l->func = value;
    l->next = NULL;
    return l;
}

struct list_test *list_test_last(struct list_test *list) {
    if (!list) return NULL;
    while (list->next) {
        list = list->next;
    }

    return list;
}

void list_test_push(struct list_test **list, test func) {
    if (!(*list)) {
        *list = list_test_node_create(func);
        return;
    }

    list_test_last(*list)->next = list_test_node_create(func);
}

enum test_result_status test_normal_memory_allocation() {
    void *heap = heap_init(REGION_MIN_SIZE);
    if (heap == NULL) return TEST_INIT_FAIL;
    debug_heap(stdout, heap);

    void *b1 = _malloc(1);
    debug_heap(stdout, heap);
    if (b1 == NULL) return TEST_ALLOCATE_FAIL;

    void *b2 = _malloc(32);
    debug_heap(stdout, heap);
    if (b2 == NULL) return TEST_ALLOCATE_FAIL;
    heap_term();

    heap = heap_init(REGION_MIN_SIZE);
    if (heap == NULL) return TEST_INIT_FAIL;
    debug_heap(stdout, heap);

    void *b3 = _malloc(REGION_MIN_SIZE);
    debug_heap(stdout, heap);
    if (b3 == NULL) return TEST_ALLOCATE_FAIL;
    heap_term();

    return TEST_OK;
}

enum test_result_status test_freeing_blocks() {
    void *heap = heap_init(0);
    if (heap == NULL) return TEST_INIT_FAIL;
    debug_heap(stdout, heap);

    void *b1 = _malloc(1);
    debug_heap(stdout, heap);
    if (b1 == NULL) return TEST_ALLOCATE_FAIL;

    void *b2 = _malloc(32);
    debug_heap(stdout, heap);
    if (b2 == NULL) return TEST_ALLOCATE_FAIL;

    void *b3 = _malloc(32);
    debug_heap(stdout, heap);
    if (b2 == NULL) return TEST_ALLOCATE_FAIL;

    _free(b1);
    debug_heap(stdout, heap);
    _free(b2);
    debug_heap(stdout, heap);
    b1 = _malloc(56);
    debug_heap(stdout, heap);
    if (b1 == NULL) return TEST_ALLOCATE_FAIL;
    _free(b3);
    debug_heap(stdout, heap);
    heap_term();

    return TEST_OK;
}

enum test_result_status test_expanding_memory_region() {
    void *heap = heap_init(0);

    if (heap == NULL) return TEST_INIT_FAIL;
    debug_heap(stdout, heap);

    void *b1 = _malloc(REGION_MIN_SIZE + 1);
    debug_heap(stdout, heap);
    if (b1 == NULL) return TEST_ALLOCATE_FAIL;

    void *b2 = _malloc(REGION_MIN_SIZE + 1);
    debug_heap(stdout, heap);
    if (b2 == NULL) return TEST_ALLOCATE_FAIL;
    _free(b2);
    debug_heap(stdout, heap);
    heap_term();

    return TEST_OK;
}

enum test_result_status test_selecting_region_without_expanding() {
    void *addr = HEAP_START + REGION_MIN_SIZE;
    void *trash = mmap((void *) addr, 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (!trash)
        return TEST_FAILED;

    void *heap = heap_init(0);
    if (heap == NULL) return TEST_INIT_FAIL;
    debug_heap(stdout, heap);

    void *b1 = _malloc(8174);
    debug_heap(stdout, heap);
    if (b1 == NULL) return TEST_ALLOCATE_FAIL;

    void *b2 = _malloc(REGION_MIN_SIZE + 1);
    debug_heap(stdout, heap);
    if (b2 == NULL) return TEST_ALLOCATE_FAIL;
    _free(b2);
    debug_heap(stdout, heap);
    heap_term();

    munmap(trash, 1);

    return TEST_OK;
}

void print_separating_line() {
    printf("--------------------------------------------------------------------------\n");
}

void print_separating_line2() {
    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
}

bool do_tests(struct list_test const *list) {
    printf("Starting tests\n");
    size_t counter = 1;
    bool passed = true;

    while (list) {
        print_separating_line();
        printf("Running test %" PRIu64 ":\n", counter++);
        print_separating_line2();
        enum test_result_status res = (list->func)();
        printf("Test result: ");
        if (res != TEST_OK) {
            printf("FAILED: ");
            passed = false;
        }

        printf("%s\n", test_result_status_messages[res]);

        list = list->next;
        print_separating_line2();
    }

    return passed;
}

int main() {
    struct list_test *tests = NULL;
    list_test_push(&tests, test_normal_memory_allocation);
    list_test_push(&tests, test_freeing_blocks);
    list_test_push(&tests, test_expanding_memory_region);
    list_test_push(&tests, test_selecting_region_without_expanding);

    return !do_tests(tests);
}

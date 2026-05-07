#include "unity.h"
#include "stack.h"
#include "unity_internals.h"
#include <stdlib.h>

void setUp() {

}

void tearDown() {

}

void test_normal_init() {
    Stack* s = init(5);
    TEST_ASSERT_NOT_NULL(s);
    int result = 0;
    check_canaries(s, &result);
    TEST_ASSERT_EQUAL_INT(1, result);
    check_hash(s, &result);
    TEST_ASSERT_EQUAL_INT(1, result);
    clear(s);
}

void test_zero_capacity_init() {
    Stack* s = init(0);
    TEST_ASSERT_NULL(s);
    clear(s);
}

void test_push() {
    Stack* s = init(2);
    push(s, 42);
    type value;
    top(s, &value);
    TEST_ASSERT_EQUAL_INT(42, value);
    clear(s);
}

void test_resize() {
    Stack* s = init(2);
    push(s, 42);
    push(s, 67);
    push(s, 52);
    push(s, 55);
    int result = 0;
    check_canaries(s, &result);
    TEST_ASSERT_EQUAL_INT(1, result);
    check_hash(s, &result);
    TEST_ASSERT_EQUAL_INT(1, result);
    clear(s);
}

void test_normal_pop() {
    Stack* s = init(2);
    push(s, 42);
    push(s, 67);
    push(s, 52);
    push(s, 55);
    pop(s);
    int result = 0;
    top(s, &result);
    TEST_ASSERT_EQUAL_INT(52, result);
    clear(s);
}

void test_empty_pop() {
    Stack* s = init(1);
    push(s, 42);
    pop(s);
    int result = pop(s);
    TEST_ASSERT_EQUAL_INT(EXIT_FAILURE, result);
    clear(s);
}

void test_empty() {
    Stack* s = init(42);
    int out = 0;
    int res = empty(s, &out);
    TEST_ASSERT_EQUAL_INT(1, res);
    clear(s);
}

void test_hash_after_push_pop() {
    Stack* s = init(8);
    push(s, 1);
    push(s, 2);
    pop(s);
    push(s, 3);
    pop(s);
    pop(s);
    int res = 0;
    check_hash(s, &res);
    TEST_ASSERT_EQUAL_INT(1, res);
    clear(s);
}


int main() {
    UNITY_BEGIN();

    RUN_TEST(test_normal_init);
    RUN_TEST(test_zero_capacity_init);
    RUN_TEST(test_push);
    RUN_TEST(test_resize);
    RUN_TEST(test_normal_pop);
    RUN_TEST(test_empty_pop);
    RUN_TEST(test_empty);
    RUN_TEST(test_hash_after_push_pop);

    UNITY_END();
}
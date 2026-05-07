#include "stack.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

const double RESIZE_ON_PUSH_MULTIPLIER = 2;
const double RESIZE_ON_POP_MULTIPLIER = 0.5;

const char* NULL_STACK_POINTER_ERROR = "Pointer to stack is null";
const char* VALUE_POINTER_NULL_ERROR = "Pointer to argumet value is null";
const char* STACK_ALLOCATION_ERROR = "Failed to allocate stack memory";
const char* INCORRECT_START_CAPACITY_ERROR = "Incorrect stack start size!";

const char* RESIZE_ERROR = "An error occured while stack resize.\n Last capacity = %zu.\n Capacity resize attempt = %zu";
const char* PUSH_STACK_OVERFLOW_ERROR = "Stack overflow while push attempt";
const char* EMPTY_STACK_POP_ERROR = "Error: Attempt to pop from empty stack";
const char* EMPTY_STACK_TOP_ERROR = "Error: Attempt to top from empty stack";

struct Stack {
    uint64_t canary_begin;

    size_t capacity;
    size_t size;
    type* data;

    uint32_t error;

    uint64_t canary;
    uint64_t hash;
    uint64_t base;
    uint64_t inv_base;

    uint64_t canary_end;
};

StackStatus print_dump(Stack* stack) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    if(stack->data == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    fprintf(stderr, "Capacity: %zu\n", stack->capacity);
    fprintf(stderr, "Size: %zu\n", stack->size);
    fprintf(stderr, "Error: %u\n", stack->error);
    fprintf(stderr, "Canary: %llu\n", stack->canary);
    fprintf(stderr, "Hash: %llu\n", stack->hash);
    fprintf(stderr, "Base: %llu\n", stack->base);
    fprintf(stderr, "Inverted base: %llu\n", stack->inv_base);
    fprintf(stderr, "Stack:\n");
    for(size_t i = 0; i < stack->size; i++) {
        fprintf(stderr, "%d\n", stack->data[i]);
    }
    return STACK_SUCCESS;
}

StackStatus stack_get_error(Stack* stack, uint32_t* error) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    *error = stack->error;
    return STACK_SUCCESS;
}

static uint64_t generate_base() {
    uint64_t base = 0;
    do {
        base = ((uint64_t)rand() << 33) ^ ((uint64_t)rand() << 1) ^ 1ULL;
    } while(base < 1024);
    return base;
}

static uint64_t compute_inv_base(uint64_t base) {
    uint64_t inv_base = 1;
    for(size_t i = 0; i < 6; i++) {
        inv_base *= 2-base*inv_base;
    }
    return inv_base;
}

static void push_update_hash(Stack* stack, type value) {
    stack->hash = stack->hash * stack->base + (uint64_t)value;
}

static void pop_update_hash(Stack* stack, type value) {
    stack->hash = (stack->hash - (uint64_t)value) * stack->inv_base;
}

StackStatus check_canaries(Stack* stack, int* out) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    if(stack->canary_begin != stack->canary) {
        *out = 0;
        print_dump(stack);
        return STACK_SUCCESS;
    }
    if(stack->canary_end != stack->canary) {
        *out = 0;
        print_dump(stack);
        return STACK_SUCCESS;
    }
    if(stack->data[stack->capacity] != (type)stack->canary) {
        *out = 0;
        print_dump(stack);
        return STACK_SUCCESS;
    }
    *out = 1;
    return STACK_SUCCESS;
}

StackStatus check_hash(Stack* stack, int* out) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    uint64_t hash = 0;
    for(size_t i = 0; i < stack->size; i++) {
        hash = hash * stack->base + (uint64_t)stack->data[i];
    }
    if(hash != stack->hash) {
        *out = 0;
        print_dump(stack);
    }
    else {
        *out = 1;
    }
    return STACK_SUCCESS;
}

Stack* init(size_t start_capacity) {
    if(start_capacity < 1) {
        fprintf(stderr, "%s", INCORRECT_START_CAPACITY_ERROR);
        return NULL;
    }
    Stack* stack = calloc(1, sizeof(Stack));
    if(stack == NULL) {
        fprintf(stderr, "%s", STACK_ALLOCATION_ERROR);
        return NULL;
    }
    stack->capacity = start_capacity;
    stack->data = calloc(start_capacity+1, sizeof(type));
    if(stack->data == NULL) {
        fprintf(stderr, "%s", STACK_ALLOCATION_ERROR);
        free(stack);
        return NULL;
    }
    stack->size = 0;
    stack->error = 0;
    stack->hash = 0;
    stack->base = generate_base();
    stack->inv_base = compute_inv_base(stack->base);
    arc4random_buf(&stack->canary, sizeof(stack->canary_begin));
    stack->canary_begin = stack->canary;
    stack->canary_end = stack->canary;
    stack->data[start_capacity] = (type)stack->canary;
    return stack;
}

static StackStatus resize(Stack* stack, double multiplier) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    size_t past_capacity = stack->capacity;
    size_t new_capacity = ceil(past_capacity*multiplier);
    void* ptr = realloc(stack->data, (new_capacity+1)*sizeof(type));
    if(ptr == NULL) {
        fprintf(stderr, RESIZE_ERROR, past_capacity, new_capacity);
        stack->error |= STACK_RESIZE_ERR;
        return STACK_FAILURE;
    }
    stack->data = ptr;
    stack->capacity = new_capacity;
    stack->data[new_capacity] = stack->canary;
    return STACK_SUCCESS;
}

StackStatus push(Stack* stack, type value) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    if(stack->size == stack->capacity) {
        int code = resize(stack, RESIZE_ON_PUSH_MULTIPLIER);
        if(code != STACK_SUCCESS) {
            fprintf(stderr, "%s", PUSH_STACK_OVERFLOW_ERROR);
            stack->error |= STACK_OVERFLOW_ERR;
            return STACK_FAILURE;
        }
    }
    stack->data[stack->size] = value;
    push_update_hash(stack, value);
    stack->size++;
    return STACK_SUCCESS;
}

StackStatus pop(Stack* stack) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    int out = 0;
    StackStatus res = empty(stack, &out);
    if(res != STACK_SUCCESS) {
        fprintf(stderr, "%s", "Error in pop()");
        return STACK_FAILURE;
    }
    if(out) {
        fprintf(stderr, "%s", EMPTY_STACK_POP_ERROR);
        stack->error |= STACK_EMPTY_ERR;
        return STACK_FAILURE;
    }
    type value = stack->data[stack->size-1];
    pop_update_hash(stack, value);
    stack->size--;
    if(stack->size < stack->capacity/4) {
        StackStatus result = resize(stack, RESIZE_ON_POP_MULTIPLIER);
        if(result != STACK_SUCCESS) {
            fprintf(stderr, "%s", "Error in pop()");
            return STACK_FAILURE;
        }
    }
    return STACK_SUCCESS;
}

StackStatus top(Stack* stack, type* value) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    if(value == NULL) {
        fprintf(stderr, "%s", VALUE_POINTER_NULL_ERROR);
        return STACK_ARG_NULL_POINTER_ERR;
    }
    int out = 0;
    StackStatus result = empty(stack, &out);
    if(result != STACK_SUCCESS) {
        fprintf(stderr, "%s", "Error in top()");
        return STACK_FAILURE;
    }
    if(out) {
        fprintf(stderr, "%s", EMPTY_STACK_TOP_ERROR);
        stack->error |= STACK_EMPTY_ERR;
        return STACK_FAILURE;
    }
    *value = stack->data[stack->size-1];
    return STACK_SUCCESS;
}

StackStatus size(Stack* stack, size_t* size) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    if(size == NULL) {
        fprintf(stderr, "%s", VALUE_POINTER_NULL_ERROR);
        return STACK_ARG_NULL_POINTER_ERR;
    }
    *size = stack->size;
    return STACK_SUCCESS;
}

StackStatus clear(Stack* stack) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    free(stack->data);
    free(stack);
    return STACK_SUCCESS;
}

StackStatus empty(Stack* stack, int* out) {
    if(stack == NULL) {
        fprintf(stderr, "%s", NULL_STACK_POINTER_ERROR);
        return STACK_NULL_POINTER_ERR;
    }
    *out = (stack->size == 0);
    return STACK_SUCCESS;
}
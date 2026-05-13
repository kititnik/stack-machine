#include "vm.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

const char* NULL_VM_POINTER_ERROR = "Pointer to VM is null";
const char* VALUE_POINTER_NULL_ERROR = "Pointer to argumet value is null";
const char* VM_ALLOCATION_ERROR = "Failed to allocate VM memory";
const char* VM_STACK_ERROR_MSG = "Stack error in %s";

struct VM {
    Stack* stack;
    size_t memory_size;
};

VMStatus get_two_top_vals(VM* vm, float* top_val, float* below_val) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    StackStatus status = top(vm->stack, top_val);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "getting top value");
        return VM_STACK_ERROR;
    }
    status = pop(vm->stack);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "pop attempt");
        return VM_STACK_ERROR;
    }
    status = top(vm->stack, below_val);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "getting top value");
        return VM_STACK_ERROR;
    }
    status = pop(vm->stack);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "pop attempt");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}

VM* vm_init(size_t memory_size) {
    VM* vm = calloc(1, sizeof(VM));
    if(vm == NULL) {
        fprintf(stderr, "%s", VM_ALLOCATION_ERROR);
        return NULL;
    }
    Stack* stack = init(memory_size);
    if(stack == NULL) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_init()");
        free(vm);
        return NULL;
    }
    vm->stack = stack;
    vm->memory_size = memory_size;
    return vm;
}

VMStatus vm_push(VM* vm, float value) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    StackStatus status = push(vm->stack, value);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_push()");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}

VMStatus vm_pop(VM* vm) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    StackStatus status = pop(vm->stack);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_pop()");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}

VMStatus vm_add(VM* vm) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    float lhs = 0;
    float rhs = 0;
    VMStatus vm_status = get_two_top_vals(vm, &lhs, &rhs);
    if(vm_status != VM_SUCCESS) {
        fprintf(stderr, "Error in vm_add()");
        return VM_STACK_ERROR;
    }
    float ans = lhs + rhs;
    StackStatus status = push(vm->stack, ans);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_add()");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}

VMStatus vm_sub(VM* vm) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    float lhs = 0;
    float rhs = 0;
    VMStatus vm_status = get_two_top_vals(vm, &lhs, &rhs);
    if(vm_status != VM_SUCCESS) {
        fprintf(stderr, "Error in vm_sub()");
        return VM_STACK_ERROR;
    }
    float ans = lhs - rhs;
    StackStatus status = push(vm->stack, ans);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_sub()");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}

VMStatus vm_mul(VM* vm) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    float lhs = 0;
    float rhs = 0;
    VMStatus vm_status = get_two_top_vals(vm, &lhs, &rhs);
    if(vm_status != VM_SUCCESS) {
        fprintf(stderr, "Error in vm_mul()");
        return VM_STACK_ERROR;
    }
    float ans = lhs * rhs;
    StackStatus status = push(vm->stack, ans);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_mul()");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}

VMStatus vm_div(VM* vm) {
    if(vm == NULL) {
        fprintf(stderr, "%s", NULL_VM_POINTER_ERROR);
        return VM_NULL_POINTER;
    }
    float lhs = 0;
    float rhs = 0;
    VMStatus vm_status = get_two_top_vals(vm, &lhs, &rhs);
    if(vm_status != VM_SUCCESS) {
        fprintf(stderr, "Error in vm_div()");
        return VM_STACK_ERROR;
    }
    if(rhs == 0.0f) {
        fprintf(stderr, "Division by zero");
        return VM_INCORRECT_ARGS;
    }
    float ans = lhs / rhs;
    StackStatus status = push(vm->stack, ans);
    if(status != STACK_SUCCESS) {
        fprintf(stderr, VM_STACK_ERROR_MSG, "vm_div()");
        return VM_STACK_ERROR;
    }
    return VM_SUCCESS;
}
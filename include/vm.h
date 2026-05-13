#ifndef VM_H
#define VM_H

#include <stddef.h>

typedef struct VM VM;

typedef enum VMStatus {
    VM_SUCCESS,
    VM_STACK_ERROR,
    VM_NULL_POINTER,
    VM_INCORRECT_ARGS,
    VM_ALLOC_ERROR
} VMStatus;

VM* vm_init(size_t memory_size);

VMStatus vm_push(VM* vm, float value);

VMStatus vm_pop(VM* vm);

VMStatus vm_add(VM* vm);

VMStatus vm_sub(VM* vm);

VMStatus vm_mul(VM* vm);

VMStatus vm_div(VM* vm);

#endif
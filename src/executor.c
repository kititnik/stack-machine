#include "executor.h"
#include "parser.h"
#include "vm.h"
#include <stdlib.h>

const char* NULL_ARG_POINTER_ERROR = "Pointer to %s is NULL";

static ExecutorStatus execute_push(VM* vm, const Command* command);

ExecutorStatus execute(VM* vm, ParseResult* parse_result) {
    for (size_t i = 0; i < parse_result->count; i++) {
        ExecutorStatus status = execute_command(vm, &parse_result->commands[i]);
        if (status != EXECUTOR_SUCCESS) return status;
    }
    return EXECUTOR_SUCCESS;
}

ExecutorStatus execute_command(VM* vm, const Command* command) {
    if(vm == NULL) {
        fprintf(stderr, NULL_ARG_POINTER_ERROR, "vm");
        return EXECUTOR_NULL_ARG_ERROR;
    }
    switch (command->type) {
        case CMD_PUSH:
            return execute_push(vm, command);
        case CMD_POP:
            return vm_pop(vm) == VM_SUCCESS ? EXECUTOR_SUCCESS : EXECUTOR_VM_ERROR;
            break;
        case CMD_ADD:
            return vm_add(vm) == VM_SUCCESS ? EXECUTOR_SUCCESS : EXECUTOR_VM_ERROR;
            break;
        case CMD_SUB:
            return vm_sub(vm) == VM_SUCCESS ? EXECUTOR_SUCCESS : EXECUTOR_VM_ERROR;
            break;
        case CMD_MUL:
            return vm_mul(vm) == VM_SUCCESS ? EXECUTOR_SUCCESS : EXECUTOR_VM_ERROR;
            break;
        case CMD_DIV:
            return vm_div(vm) == VM_SUCCESS ? EXECUTOR_SUCCESS : EXECUTOR_VM_ERROR;
            break;
        case CMD_UNKNOWN:
            return EXECUTOR_SUCCESS;
    }
    return EXECUTOR_SUCCESS;
}

static ExecutorStatus execute_push(VM* vm, const Command* command) {
    char *end = NULL;
    float value = strtof(command->args[0], &end);

    if (end == command->args[0] || *end != '\0') {
        return EXECUTOR_INVALID_ARGUMENT;
    }

    if (vm_push(vm, value) != VM_SUCCESS) {
        return EXECUTOR_VM_ERROR;
    }

    return EXECUTOR_SUCCESS;
}


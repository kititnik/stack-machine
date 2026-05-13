#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "vm.h"

typedef enum ExecutorStatus {
    EXECUTOR_SUCCESS,
    EXECUTOR_ALLOC_ERROR,
    EXECUTOR_NULL_ARG_ERROR,
    EXECUTOR_VM_ERROR,
    EXECUTOR_INVALID_ARGUMENT
} ExecutorStatus;

ExecutorStatus execute_command(VM* vm, const Command* command);
ExecutorStatus execute(VM* vm, ParseResult* parse_result);

#endif
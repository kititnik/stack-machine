#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef enum ParseStatus {
    PARSER_SUCCESS,
    PARSER_ALLOC_ERROR,
    PARSER_FILE_READ_ERROR,
    PARSER_FILE_OPEN_ERROR,
    PARSER_NULL_ARG_ERROR,
    PARSER_COMMAND_STRING_PARSE_ERROR
} ParseStatus;

typedef enum {
    CMD_UNKNOWN = 0,
    CMD_PUSH = 1,
    CMD_POP = 2,
    CMD_ADD = 3,
    CMD_SUB = 4,
    CMD_MUL = 5,
    CMD_DIV = 6
} CommandType;

typedef struct Command {
    CommandType type;
    int argc;
    char** args;
} Command;

typedef struct {
    Command* commands;
    size_t count;
} ParseResult;

ParseStatus parse(const char* file_path, ParseResult* parse_result);
void parse_free(ParseResult* parse_result);

#endif
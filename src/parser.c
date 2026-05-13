#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* NULL_ARG_POINTER_ERROR = "Pointer to %s is NULL";
const char* FILE_OPEN_ERROR = "File opening error";
const char* PARSE_ALLOCATION_ERROR = "Failed to allocate %s memory";
const char* FILE_READ_ERROR = "File reading error";
const char* COMMAND_STRING_PARSE_ERROR = "Command string parse error. Line: %s. Arg number: %d";

int command_argument_count[] = {
    [CMD_UNKNOWN] = 0, 
    [CMD_PUSH] = 1,
    [CMD_POP] = 0,
    [CMD_ADD] = 0,
    [CMD_SUB] = 0,
    [CMD_MUL] = 0,
    [CMD_DIV] = 0
};

typedef struct {
    char* name;
    CommandType type;
} CommandMapping;

CommandMapping command_table[] = {
    {"push", CMD_PUSH},
    {"pop", CMD_POP},
    {"add", CMD_ADD},
    {"sub", CMD_SUB},
    {"mul", CMD_MUL},
    {"div", CMD_DIV}
};

const size_t COMMAND_TABLE_SIZE = sizeof(command_table) / sizeof(command_table[0]);

static ParseStatus read_file(const char* file_path, char** out_buffer);
static ParseStatus split_lines(char* buffer, size_t* out_count, char*** out_lines);
static ParseStatus parse_command(char* line, Command* out_command);
static void strip_comment(char* line);
static ParseStatus parse_args(Command* command, const char* line);

ParseStatus parse(const char* file_path, ParseResult* parse_result) {
    ParseStatus status;
    if(file_path == NULL) {
        fprintf(stderr, NULL_ARG_POINTER_ERROR, "file_path");
        return PARSER_NULL_ARG_ERROR;
    }
    if(parse_result == NULL) {
        fprintf(stderr, NULL_ARG_POINTER_ERROR, "parse_result");
        return PARSER_NULL_ARG_ERROR;
    }

    char* buffer = NULL;
    status = read_file(file_path, &buffer);
    if(status != PARSER_SUCCESS) {
        return status;
    }
    
    size_t lines_count = 0;
    char** command_lines = NULL;
    status = split_lines(buffer, &lines_count, &command_lines);
    if(status != PARSER_SUCCESS) {
        free(buffer);
        free(command_lines);
        return status;
    }

    Command* commands = calloc(lines_count, sizeof(Command));
    if(commands == NULL) {
        fprintf(stderr, PARSE_ALLOCATION_ERROR, "commands");
        free(command_lines);
        free(buffer);
        return PARSER_ALLOC_ERROR;
    }
    int commands_count = 0;
    for(int i = 0; i < lines_count; i++) {
        Command command;
        status = parse_command(command_lines[i], &command);
        if(status != PARSER_SUCCESS) {
            for(int j = 0; j < commands_count; j++) {
                if (commands[j].args != NULL) {
                    for (int k = 0; k < commands[j].argc; k++) {
                        free(commands[j].args[k]);
                    }
                    free(commands[j].args);
                }
            }
            free(command_lines);
            free(commands);
            free(buffer);
            return status;
        }
        if(command.type == CMD_UNKNOWN) continue;
        commands[commands_count] = command;
        commands_count++;
    }
    
    free(buffer);
    free(command_lines);
    parse_result->commands = commands;
    parse_result->count = commands_count;
    return PARSER_SUCCESS;
}

void parse_free(ParseResult* parse_result) {
    if(parse_result == NULL) {
        return;
    }
    if (parse_result->commands != NULL) {
        for (int i = 0; i < parse_result->count; i++) {
            if (parse_result->commands[i].args != NULL) {
                for (int j = 0; j < parse_result->commands[i].argc; j++) {
                    free(parse_result->commands[i].args[j]);
                }
                free(parse_result->commands[i].args);
            }
        }
        free(parse_result->commands);
    }
    parse_result->commands = NULL;
    parse_result->count = 0;
}

static ParseStatus read_file(const char* file_path, char** out_buffer) {
    FILE* file = fopen(file_path, "r");
    if(file == NULL) {
        fprintf(stderr, "%s", FILE_OPEN_ERROR);
        return PARSER_FILE_OPEN_ERROR;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = calloc(size+1, sizeof(char));
    if(buffer == NULL) {
        fprintf(stderr, PARSE_ALLOCATION_ERROR, "buffer");
        fclose(file);
        return PARSER_ALLOC_ERROR;
    }
    long res = fread(buffer, 1, size, file);
    if(res != size) {
        fprintf(stderr, "%s", FILE_READ_ERROR);
        fclose(file);
        free(buffer);
        return PARSER_FILE_READ_ERROR;
    }
    buffer[size] = '\0';

    fclose(file);
    *out_buffer = buffer;
    return PARSER_SUCCESS;
}

static ParseStatus split_lines(char* buffer, size_t* out_count, char*** out_lines) {
    size_t count = 0;
    size_t idx = 0;
    while(buffer[idx] != '\0') {
        if(buffer[idx] == '\n') {
            count++;
        }
        idx++;
    }
    count += 1;
    char** lines = calloc(count, sizeof(char*));
    if(lines == NULL) {
        fprintf(stderr, PARSE_ALLOCATION_ERROR, "command lines");
        return PARSER_ALLOC_ERROR;
    }
    idx = 0;
    char* line = strtok(buffer, "\n");
    while(line != NULL) {
        strip_comment(line);
        lines[idx] = line;
        line = strtok(NULL, "\n");
        idx++;
    }
    *out_count = idx;
    *out_lines = lines;
    return PARSER_SUCCESS;
}

static ParseStatus parse_command(char* line, Command* out_command) {
    Command command = { CMD_UNKNOWN, 0, NULL };
    const char* token = strtok(line, " ");
    if(token == NULL) {
        *out_command = command;
        return PARSER_SUCCESS;
    }
    for(int i = 0; i < COMMAND_TABLE_SIZE; i++) {
        if(strcmp(token, command_table[i].name) != 0) continue;

        command.type = command_table[i].type;
        command.argc = command_argument_count[command.type];

        if(command.argc <= 0) break;

        command.args = calloc(command.argc, sizeof(char*));
        if(command.args == NULL) {
            fprintf(stderr, PARSE_ALLOCATION_ERROR, "command args");
            return PARSER_ALLOC_ERROR;
        }
        ParseStatus status = parse_args(&command, line);
        if(status != PARSER_SUCCESS) {
            return status;
        }
        
        break;
    }
    *out_command = command;
    return PARSER_SUCCESS;
}

static void strip_comment(char* line) {
    char* comment_ptr = strchr(line, ';');
    if (comment_ptr != NULL) {
        *comment_ptr = '\0';
    }
}

static ParseStatus parse_args(Command* command, const char* line) {
    for(int arg = 0; arg < command->argc; arg++) {
        const char* token = strtok(NULL, " ");
        if(token == NULL) {
            fprintf(stderr, COMMAND_STRING_PARSE_ERROR, line, arg+1);
            for(int j = 0; j < arg; j++) {
                free(command->args[j]);
                command->args = NULL;
            }
            free(command->args);
            command->args = NULL;
            return PARSER_COMMAND_STRING_PARSE_ERROR;
        }
        command->args[arg] = strdup(token);
        if(command->args[arg] == NULL) {
            fprintf(stderr, PARSE_ALLOCATION_ERROR, "command arg strdup");
            fprintf(stderr, COMMAND_STRING_PARSE_ERROR, line, arg+1);
            for(int j = 0; j < arg; j++) {
                free(command->args[j]);
                command->args = NULL;
            }
            free(command->args);
            command->args = NULL;
            return PARSER_ALLOC_ERROR;
        }
    }
    return PARSER_SUCCESS;
}
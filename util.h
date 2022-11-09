#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;
// 所谓meta command 其实就是以'.'开始的，sqlite自带的command
// 而不是对数据库操作的command

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef struct {
  StatementType type;
} Statement;

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

void print_prompt() { printf("db > "); }

InputBuffer* new_input_buffer();

MetaCommandResult do_meta_command(InputBuffer*);

void read_input(InputBuffer* );

void close_input_buffer(InputBuffer* );

PrepareResult prepare_statement(InputBuffer* ,Statement* );

void execute_statement(Statement* );
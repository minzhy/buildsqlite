#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

extern const uint32_t PAGE_SIZE;
extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;
extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;

typedef enum { PREPARE_SUCCESS, PREPARE_NEGATIVE_ID, PREPARE_STRING_TOO_LONG, PREPARE_SYNTAX_ERROR, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef enum{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;
// 所谓meta command 其实就是以'.'开始的，sqlite自带的command
// 而不是对数据库操作的command

typedef struct {
  uint32_t num_rows;
  void* pages[TABLE_MAX_PAGES];
} Table;

typedef struct{
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE+1];
  char email[COLUMN_EMAIL_SIZE+1];
  // 这里需要+1，因为string的最后一位是null，如果要存COLUMN_EMAIL_SIZE位的char的话，要留一位下来存null
} Row;

typedef struct {
  StatementType type;
  Row row_to_insert;
} Statement;

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

// util.c
void print_prompt();

// main.c
InputBuffer* new_input_buffer();
MetaCommandResult do_meta_command(InputBuffer*);
void read_input(InputBuffer* );
void close_input_buffer(InputBuffer* );

// statment.c
PrepareResult prepare_statement(InputBuffer* ,Statement* );
ExecuteResult execute_statement(Statement* ,Table*);
ExecuteResult execute_select(Statement* statement, Table* table);
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);
ExecuteResult execute_insert(Statement* statement, Table* table);

// row.c
void print_row(Row* row);
void* row_slot(Table* table, uint32_t row_num);
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);

// table.c
Table* new_table();
void free_table(Table* table);

#endif
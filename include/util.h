#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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

/*
 * Common Node Header Layout
 */
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint8_t COMMON_NODE_HEADER_SIZE;

/*
 * Leaf Node Header Layout
 */
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET ;
extern const uint32_t LEAF_NODE_NEXT_LEAF_SIZE;
extern const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE ;

/*
 * Leaf Node Body Layout
 */
extern const uint32_t LEAF_NODE_KEY_SIZE ;
extern const uint32_t LEAF_NODE_KEY_OFFSET ;
extern const uint32_t LEAF_NODE_VALUE_SIZE ;
extern const uint32_t LEAF_NODE_VALUE_OFFSET ;
extern const uint32_t LEAF_NODE_CELL_SIZE ;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS ;
extern const uint32_t LEAF_NODE_MAX_CELLS;

// leaf node size
extern const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT;
extern const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT;

/*
 * Internal Node Header Layout
 */
extern const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE;

/*
 * Internal Node Body Layout
 */
extern const uint32_t INTERNAL_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_CELL_SIZE;

/* Keep this small for testing */
extern const uint32_t INTERNAL_NODE_MAX_CELLS;

// typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;
typedef enum {
  EXECUTE_SUCCESS,
  EXECUTE_DUPLICATE_KEY,
  EXECUTE_TABLE_FULL
}ExecuteResult;

typedef enum { PREPARE_SUCCESS, PREPARE_NEGATIVE_ID, PREPARE_STRING_TOO_LONG, PREPARE_SYNTAX_ERROR, PREPARE_UNRECOGNIZED_STATEMENT } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;

typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

typedef enum{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;
// ??????meta command ???????????????'.'????????????sqlite?????????command
// ??????????????????????????????command

typedef struct {
  int file_descriptor;          // ????????????????????????page
  uint32_t file_length;
  void* pages[TABLE_MAX_PAGES]; // ?????????????????????page
  uint32_t num_pages; //????????????????????????page
} Pager;

typedef struct {
  // uint32_t num_rows; // ???????????????rows
  // void* pages[TABLE_MAX_PAGES];
  Pager* pager;
  uint32_t root_page_num;
} Table;

typedef struct{
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE+1];
  char email[COLUMN_EMAIL_SIZE+1];
  // ????????????+1?????????string??????????????????null???????????????COLUMN_EMAIL_SIZE??????char??????????????????????????????null
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

typedef struct {
  Table* table;
  // uint32_t row_num; ???page???????????????????????????row_num???
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;  // Indicates a position one past the last element
} Cursor;

// util.c
void print_prompt();
void print_constants();

// main.c
InputBuffer* new_input_buffer();
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table);
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
// Table* new_table();
Table* db_open(const char* filename);
// void free_table(Table* table);
void db_close(Table* table);

// page.c
void* get_page(Pager* pager, uint32_t page_num);
Pager* pager_open(const char* filename);
void pager_flush(Pager* pager, uint32_t page_num);
uint32_t get_unused_page_num(Pager* pager);

// cursor.c
Cursor* table_start(Table* table);
// Cursor* table_end(Table* table);
Cursor* table_find(Table* table, uint32_t key);
void* cursor_value(Cursor* cursor);
void cursor_advance(Cursor* cursor);

// node.c
uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);
// void print_leaf_node(void* node);
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);
NodeType get_node_type(void* node);
void set_node_type(void* node, NodeType type) ;
void create_new_root(Table* table, uint32_t right_child_page_num);

void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);
uint32_t* internal_node_num_keys(void* node);
uint32_t* internal_node_right_child(void* node);
uint32_t* internal_node_cell(void* node, uint32_t cell_num);
uint32_t* internal_node_child(void* node, uint32_t child_num);
uint32_t* internal_node_key(void* node, uint32_t key_num);
uint32_t get_node_max_key(void* node);

void initialize_internal_node(void* node);
bool is_node_root(void* node);
void set_node_root(void* node, bool is_root);

void indent(uint32_t level);
void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level);
Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key);
uint32_t* leaf_node_next_leaf(void* node);
uint32_t* node_parent(void* node);
void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key);
uint32_t* node_parent(void* node);
uint32_t internal_node_find_child(void* node, uint32_t key);
void internal_node_insert(Table* table, uint32_t parent_page_num,
                          uint32_t child_page_num);

#endif
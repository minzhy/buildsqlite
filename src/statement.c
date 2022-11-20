#include "util.h"

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    // int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),statement->row_to_insert.username, statement->row_to_insert.email);
    // if(args_assigned < 3) return PREPARE_SYNTAX_ERROR;
    // return PREPARE_SUCCESS;
    // 上面这样写是会有问题的，因为sscanf的逻辑是，如果buffer的大小超出了预定的大小，那么buffer就会overflow，然后写进一个未知的区域内。所以会有问题

    return prepare_insert(input_buffer, statement); // 这种方法解决了溢出的问题
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
  // if (table->num_rows >= TABLE_MAX_ROWS) {
  void* node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = (*leaf_node_num_cells(node));
  // if (num_cells >= LEAF_NODE_MAX_CELLS) {
  //   return EXECUTE_TABLE_FULL;
  // }

  Row* row_to_insert = &(statement->row_to_insert);

  // serialize_row(row_to_insert, row_slot(table, table->num_rows)); 换成cursor的形式
  // Cursor* cursor = table_end(table);
  uint32_t key_to_insert = row_to_insert->id;
  Cursor* cursor = table_find(table, key_to_insert);

  if (cursor->cell_num < num_cells) {
    uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
    if (key_at_index == key_to_insert) {
      return EXECUTE_DUPLICATE_KEY;
    }
  }

  // serialize_row(row_to_insert,cursor_value(cursor));
  // table->num_rows += 1;
  leaf_node_insert(cursor, row_to_insert->id, row_to_insert);

  free(cursor);

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
  // Cursor* cursor = table_end(table);
  void *node = get_page(table->pager,table->root_page_num);
  Cursor *cursor = malloc(sizeof(Cursor));
  cursor->cell_num = leaf_node_num_cells(node);
  cursor->table = table;
  cursor->page_num = table->root_page_num;

  Row row;
  // for (uint32_t i = 0; i < table->num_rows; i++) {
  //   deserialize_row(row_slot(table, i), &row);
  //   print_row(&row);
  // }
  while(!cursor->end_of_table){
    deserialize_row(cursor_value(cursor), &row);
    print_row(&row);
    cursor_advance(cursor);
  }

  free(cursor);

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
   switch (statement->type) {
     case (STATEMENT_INSERT):
      return execute_insert(statement, table);
     case (STATEMENT_SELECT):
      return execute_select(statement, table);
   }
}

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
  statement->type = STATEMENT_INSERT;

  char* keyword = strtok(input_buffer->buffer, " ");
  char* id_string = strtok(NULL, " ");
  char* username = strtok(NULL, " ");
  char* email = strtok(NULL, " ");
  // strtok 用于对字符串进行分割

  if (id_string == NULL || username == NULL || email == NULL) {
    return PREPARE_SYNTAX_ERROR;
  }

  int id = atoi(id_string);
  if(id < 0){
    return PREPARE_NEGATIVE_ID;
  }
  if (strlen(username) > COLUMN_USERNAME_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }
  if (strlen(email) > COLUMN_EMAIL_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }

  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username, username);
  strcpy(statement->row_to_insert.email, email);
  // c 语言中，char[]的赋值方式

  return PREPARE_SUCCESS;
}

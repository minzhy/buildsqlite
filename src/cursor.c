#include "util.h"

Cursor* table_start(Table* table) {
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;
  // cursor->end_of_table = (table->num_rows == 0);

  void* root_node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells(root_node);
  cursor->end_of_table = (num_cells == 0); // 一个cell都没有的时候，就是空

  return cursor;
}

Cursor* table_end(Table* table) {
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  // cursor->row_num = table->num_rows;
  cursor->page_num = table->root_page_num;

  void* root_node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells(root_node);
  cursor->cell_num = num_cells;
  cursor->end_of_table = true;

  return cursor;
}

void cursor_advance(Cursor* cursor) {
  // cursor->row_num += 1;
  // if (cursor->row_num >= cursor->table->num_rows) {
  uint32_t page_num = cursor->page_num;
  void* node = get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
    cursor->end_of_table = true;
  }
}

void* cursor_value(Cursor* cursor) {
    // uint32_t row_num = cursor->row_num;
    // uint32_t page_num = row_num / ROWS_PER_PAGE;
    uint32_t page_num = cursor->page_num;
    void* page = get_page(cursor->table->pager, page_num);
    // uint32_t row_offset = row_num % ROWS_PER_PAGE;
    // uint32_t byte_offset = row_offset * ROW_SIZE;
    return leaf_node_value(page, cursor->cell_num);
 }


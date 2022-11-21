#include "util.h"

// Cursor* table_start(Table* table) {
//   Cursor* cursor = malloc(sizeof(Cursor));
//   cursor->table = table;
//   cursor->page_num = table->root_page_num;
//   cursor->cell_num = 0;
//   // cursor->end_of_table = (table->num_rows == 0);

//   void* root_node = get_page(table->pager, table->root_page_num);
//   uint32_t num_cells = *leaf_node_num_cells(root_node);
//   cursor->end_of_table = (num_cells == 0); // 一个cell都没有的时候，就是空

//   return cursor;
// }

Cursor* table_start(Table* table) {
  Cursor* cursor =  table_find(table, 0);

  void* node = get_page(table->pager, cursor->page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}


// Cursor* table_end(Table* table) {
//   Cursor* cursor = malloc(sizeof(Cursor));
//   cursor->table = table;
//   // cursor->row_num = table->num_rows;
//   cursor->page_num = table->root_page_num;

//   void* root_node = get_page(table->pager, table->root_page_num);
//   uint32_t num_cells = *leaf_node_num_cells(root_node);
//   cursor->cell_num = num_cells;
//   cursor->end_of_table = true;

//   return cursor;
// }

void cursor_advance(Cursor* cursor) {
  // cursor->row_num += 1;
  // if (cursor->row_num >= cursor->table->num_rows) {
  uint32_t page_num = cursor->page_num;
  void* node = get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
    // cursor->end_of_table = true;
    /* Advance to next leaf node */
    uint32_t next_page_num = *leaf_node_next_leaf(node);
    if (next_page_num == 0) {
      /* This was rightmost leaf */
      cursor->end_of_table = true;
    } else {
      cursor->page_num = next_page_num;
      cursor->cell_num = 0;
    }
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

/*
Return the position of the given key.
If the key is not present, return the position
where it should be inserted
*/
Cursor* table_find(Table* table, uint32_t key) {
  uint32_t root_page_num = table->root_page_num;
  void* root_node = get_page(table->pager, root_page_num);

  if (get_node_type(root_node) == NODE_LEAF) {
    return leaf_node_find(table, root_page_num, key);
  } else {
    printf("Need to implement searching an internal node\n");
    exit(EXIT_FAILURE);
  }
}
#include "util.h"

// Table* new_table() {
//   Table* table = (Table*)malloc(sizeof(Table));
//   table->num_rows = 0;
//   for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
//      table->pages[i] = NULL;
//   }
//   return table;
// }

Table* db_open(const char* filename) {
  Pager* pager = pager_open(filename);
  // uint32_t num_rows = pager->file_length / ROW_SIZE;

  Table* table = malloc(sizeof(Table));
  table->pager = pager;
  // table->num_rows = num_rows;

  table->root_page_num = 0;

  if (pager->num_pages == 0) {
    // New database file. Initialize page 0 as leaf node.
    void* root_node = get_page(pager, 0);
    initialize_leaf_node(root_node);
  }

  return table;
}

// void free_table(Table* table) {
//   for (int i = 0; table->pages[i]; i++) {
// 	  free(table->pages[i]);
//   }
//   free(table);
// }

void db_close(Table* table) {
  Pager* pager = table->pager;
  // uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

  // 分开处理，先处理所有page full的内存！然后处理所有未满的page
  for (uint32_t i = 0; i < pager->num_pages; i++) {
    if (pager->pages[i] == NULL) {
      continue;
    }
    pager_flush(pager, i);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  // 以page为单位调用，不存在这种情况
  // // There may be a partial page to write to the end of the file
  // // This should not be needed after we switch to a B-tree
  // uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  // if (num_additional_rows > 0) {
  //   uint32_t page_num = num_full_pages;
  //   if (pager->pages[page_num] != NULL) {
  //     pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
  //     free(pager->pages[page_num]);
  //     pager->pages[page_num] = NULL;
  //   }
  // }

  // 查看是否正常关闭file
  int result = close(pager->file_descriptor);
  if (result == -1) {
    printf("Error closing db file.\n");
    exit(EXIT_FAILURE);
  }
  // 释放内存
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    void* page = pager->pages[i];
    if (page) {
      free(page);
      pager->pages[i] = NULL;
    }
  }
  free(pager);
  free(table);
}
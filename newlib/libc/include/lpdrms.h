// by la prise de risque
#ifndef _LPDRMS_H
#include <stdlib.h>
#define STACK_START 0Xffffefff
#define MDS_TABLE_SIZE 256
#define AVL_INSERTION
// #define DEBUG_AVL_INSERT
// #define DEBUG_BST_INSERT
typedef struct bst_node_s
{
	__ptr_t address;
	size_t size;
	struct bst_node_s *left;
	struct bst_node_s *right;
} bst_node_t;

typedef bst_node_t mds_entry;
// typedef mds_entry *mds_entry_ptr;
mds_entry *mds_insert(mds_entry *entry, mds_entry *root);
int is_allowed(__ptr_t address, size_t size);
#ifdef AVL_INSERTION
mds_entry *mds_avl_insert(mds_entry *entry, mds_entry *root);
#endif

mds_entry *mds_register(__ptr_t address, size_t size);

void mds_remove(mds_entry *entry);

int mds_is_allowed(__ptr_t address, size_t size);
mds_entry *mds_get_inf(__ptr_t address);

mds_entry *mds_root();
int mds_count(mds_entry *entry);
int mds_check(mds_entry *entry);
void mds_unregister(__ptr_t address);
void mds_dump(char *buffer, const char *graph_name);
mds_entry *mds_get(__ptr_t address);
mds_entry *mds_get_rec(__ptr_t address);
#endif
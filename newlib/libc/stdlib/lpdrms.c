#include <lpdrms.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/config.h>


mds_entry _mds_table[MDS_TABLE_SIZE];
mds_entry *_ptr_mds_root = NULL;
unsigned int _mds_table_index = 0;
void * smlad = NULL;
// void* lpdrmstest(){
//     return (void*)54321;
// }
#ifdef AVL_INSERTION
int mds_height(mds_entry *entry)
{
    if (entry == NULL)
        return 0;

    int left_height = mds_height(entry->left);
    int right_height = mds_height(entry->right);

    return left_height > right_height ? left_height + 1 : right_height + 1;
}

mds_entry *mds_right_rotation(mds_entry *root)
{
#ifdef DEBUG_AVL_INSERT
    fprintf(stderr, "\tRight Rotating %ld of size %ld : \n", (long int)root->address, (long int)root->size);
    if (root->left == NULL)
        fprintf(stderr, "\t\tleft child is NULL\n");
    else
        fprintf(stderr, "\t\tleft child is %ld of size %ld\n", (long int)root->left->address, (long int)root->left->size);
#endif
    mds_entry *new_root = root->left;
    if (root == _ptr_mds_root)
        _ptr_mds_root = new_root;
    mds_entry *new_root_right = new_root->right;
    new_root->right = root;
    root->left = new_root_right;
#ifdef DEBUG_AVL_INSERT
    fprintf(stderr, "\tswapped %ld of size %ld with %ld of size %ld\n", (long int)new_root->address, (long int)new_root->size, (long int)root->address, (long int)root->size);
#endif
    // Return new root
    return new_root;
}

mds_entry *mds_left_rotation(mds_entry *root)
{
#ifdef DEBUG_AVL_INSERT
    fprintf(stderr, "\tLeft Rotating %ld of size %ld : \n", (long int)root->address, (long int)root->size);
    if (root->right == NULL)
        fprintf(stderr, "\t\tright child is NULL\n");
    else
        fprintf(stderr, "\t\tright child is %ld of size %ld\n", (long int)root->right->address, (long int)root->right->size);
#endif

    mds_entry *new_root = root->right;
    if (root == _ptr_mds_root)
        _ptr_mds_root = new_root;
    mds_entry *new_root_left = new_root->left;
    new_root->left = root;
    root->right = new_root_left;

#ifdef DEBUG_AVL_INSERT
    fprintf(stderr, "\tswapped %ld of size %ld with %ld of size %ld\n", (long int)root->address, (long int)root->size, (long int)new_root->address, (long int)new_root->size);
#endif
    return new_root;
}

// https://www.geeksforgeeks.org/insertion-in-an-avl-tree/
mds_entry *mds_avl_insert(mds_entry *entry, mds_entry *current)
{

    if (current == NULL)
        return entry;

    if (entry->address < current->address)
        current->left = mds_avl_insert(entry, current->left);
    else if (entry->address > current->address)
        current->right = mds_avl_insert(entry, current->right);
    else
    {
#ifdef DEBUG_AVL_INSERT
        fprintf(stderr, "\tAVL Insertion of %ld failed, already exists\n", (long int)entry->address);
#endif
        return current;
    }
    // balancing
    int balance = mds_height(current->left) - mds_height(current->right);
// #ifdef DEBUG_AVL_INSERT
//     fprintf(stderr, "\tBalance: %d\n", balance);
// #endif
    if (balance > 1)
    {
#ifdef DEBUG_AVL_INSERT
        fprintf(stderr, "\tLeft heavy\n");
        if (current->left->address == NULL)
        {
            fprintf(stderr, "\tERR: LA NULL\n");
        }
#endif
        if (entry->address < current->left->address)
            current = mds_right_rotation(current);

        else if (entry->address > current->left->address)
        {
            current->left = mds_left_rotation(current->left);
            current = mds_right_rotation(current);
        }
    }

    else if (balance < -1)
    {
#ifdef DEBUG_AVL_INSERT
        fprintf(stderr, "\tRight heavy\n");
        if (current->right->address == NULL)
        {
            fprintf(stderr, "\tERR: RA NULL\n");
        }
#endif
        if (entry->address > current->right->address)
            current = mds_left_rotation(current);

        else if (entry->address < current->right->address)
        {
            current->right = mds_right_rotation(current->right);
            current = mds_left_rotation(current);
        }
    }

    return current;
}
#endif

/// @brief Insert an entry in the tree and return the inserted entry
/// @param entry
/// @return parrent of the inserted entry
mds_entry *mds_insert(mds_entry *entry, mds_entry *current)
{
#ifdef DEBUG_BST_INSERT
    fprintf(stderr, "Inserting %ld of size %ld : ", (long int)entry->address, (long int)entry->size);
#endif

    // recursive insertion
    if (entry->address < current->address)
    {
        if (current->left == NULL)
        {
            current->left = entry;
            return entry;
        }
        return mds_insert(entry, current->left);
    }
    else
    {
        if (current->right == NULL)
        {
            current->right = entry;
            return entry;
        }
        return mds_insert(entry, current->right);
    }

#ifdef DEBUG_BST_INSERT
    fprintf(stderr, "insertion failure !\n");
#endif

    return NULL;
}
// extern unsigned int z_interrupt_stacks;
// #define STACK_MIN (&z_interrupt_stacks)
// extern unsigned int z_main_stack_end;
// #define STACK_MAX (&z_main_stack_end)
// extern char *__malloc_sbrk_start;


int mds_is_allowed(__ptr_t address, size_t size){
    mds_entry *entry = mds_get_inf(address);
    if (entry == NULL || address < smlad)

        return 1;
    // if (entry->size < size)
    //     return 0;
    
    if (entry->address <= address && address + size <= entry->address + entry->size)
        return 1;
        
    return 0;
}

mds_entry *mds_register(__ptr_t address, size_t size)
{
    mds_entry *entry = &_mds_table[_mds_table_index];
#ifdef DEBUG_BST_INSERT
    fprintf(stderr, "Registering %ld of size %ld using index %d\n", (long int)address, size, _mds_table_index);
#endif
    _mds_table_index++;
    entry->address = address;
    entry->size = size;
    entry->left = NULL;
    entry->right = NULL;
    


    if (_ptr_mds_root == NULL)
    {
        smlad = address;
    
#if defined(DEBUG_BST_INSERT) || defined(DEBUG_AVL_INSERT)
        fprintf(stderr, "Root is NULL, setting it to %ld of size %ld\n", (long int)entry->address, (long int)entry->size);
#endif
        _ptr_mds_root = entry;

        return _ptr_mds_root;
    }

    // else if (smlad > entry->address){
    //     smlad = size;
    // }        

#ifdef AVL_INSERTION
#ifdef DEBUG_AVL_INSERT
    fprintf(stderr, "AVL Inserting %ld of size %ld : \n", (long int)entry->address, (long int)entry->size);
#endif
    mds_avl_insert(entry, _ptr_mds_root);
#ifdef DEBUG_AVL_INSERT
    fprintf(stderr, "\tAVL Insertion of %ld complete\n", (long int)entry->address);
#endif
#else
    mds_insert(entry, _ptr_mds_root);
#endif

    return entry;
}

void _mds_dump_rec(mds_entry *entry, char *buffer)
{
    if (entry == NULL)
        return;

    if (entry->left != NULL)
    {
        sprintf(buffer, "%s    \"%p\" -> \"%p\" [color=red];\n", buffer, entry->address,entry->left->address);
        _mds_dump_rec(entry->left, buffer);
    }
    if (entry->right != NULL)
    {
        sprintf(buffer, "%s    \"%p\" -> \"%p\" [color=blue];\n", buffer, entry->address, entry->right->address);
        _mds_dump_rec(entry->right, buffer);
    }
}

void mds_dump(char *buffer, const char *graph_name)
{
    // char *buffer[4096];
    mds_entry *current = _ptr_mds_root;
    sprintf(buffer, "digraph %s {\n", graph_name);
    _mds_dump_rec(current, buffer);
    sprintf(buffer, "%s}\n", buffer);
}

mds_entry *_mds_get_rec(mds_entry *entry, __ptr_t address)
{
    if (entry == NULL)
        return NULL;

    if (address == entry->address)
        return entry;
    else if (address < entry->address)
        return _mds_get_rec(entry->left, address);
    else
        return _mds_get_rec(entry->right, address);
}

mds_entry *mds_get_rec(__ptr_t address)
{
    return _mds_get_rec(_ptr_mds_root, address);
}

mds_entry *mds_get(__ptr_t address)
{
    mds_entry *current = _ptr_mds_root;
    while (current != NULL)
    {
        if (address == current->address)
            return current;
        else if (address < current->address)
            current = current->left;
        else
            current = current->right;
    }
    return current;
}

mds_entry *mds_get_inf(__ptr_t address){
    mds_entry *current = _ptr_mds_root;
    mds_entry *inf = NULL;
    while (current != NULL)
    {
        if (address == current->address)
            return current;
        else if (address < current->address){
            inf = current;
            current = current->left;
        }
        else
            current = current->right;
    }
    return inf;
}

int mds_count(mds_entry *entry)
{
    if (entry == NULL)
        return 0;
    return 1 + mds_count(entry->left) + mds_count(entry->right);
}

mds_entry *mds_root()
{
    return _ptr_mds_root;
}

int mds_check(mds_entry *entry)
{
    if (entry == NULL)
        return 0;

    if (entry->left != NULL)
    {
        if (entry->left->address >= entry->address)
        {
            fprintf(stderr, "left address %ld is not less than %ld", (long int)entry->left->address, (long int)entry->address);
            return 0;
        }
        if (mds_check(entry->left) == 0)
            return 0;
    }

    if (entry->right != NULL)
    {
        if (entry->right->address <= entry->address)
        {
            fprintf(stderr, "right address %ld is not greater than %ld", (long int)entry->right->address, (long int)entry->address);
            return 0;
        }

        if (mds_check(entry->right) == 0)
            return 0;
    }

    return 1;
}

void mds_remove(mds_entry *entry)
{
    // todo
}

void mds_unregister(__ptr_t address)
{
    // todo
}
//
// Created by james on 4/8/2019.
//

#pragma once
#include <string>

typedef struct val_container{
    double value;
} val_container;
struct node{
    void** children;
    int* keys;
    struct node* parent;
    bool leaf;
    int keyCount;
    struct node* next;
};
class B_plus_tree{

private:
    int order;
public:
    struct node* root;

    // setter for order
    void setOrder(int size);
    // initialize
    static B_plus_tree* initialize(int size);


    // insert
    void* locate(int key, node **target, bool isRecord);
    val_container* create_container(double value);
    struct node* create_node(bool leaf);
    int index_left_sibling(struct node* parent, struct node* left);
    void leaf_insert(struct node *leaf, int key, val_container* pointer);
    void split_leaf_insert(struct node *leaf, int key, val_container *pointer);
    void node_insert(struct node *parent, int left_index, int key, struct node *right);
    void split_node_insert(struct node *old_node, int left_index, int key, struct node *right);
    void parent_insert(struct node *left, int key, struct node *right);
    void new_root_insert(struct node *left, int key, struct node *right);
    void new_root(int key, val_container *pointer);
    void insert(int key, double value);

    // delete
    int get_sib_index(struct node *n);
    node* delete_from_node(node *n, int key, void *pointer);
    void fix_root();
    void node_merge(struct node *n, struct node *neighbor, int neighbor_index, int key_of_index);
    void get_node_from_sibling(struct node *n, struct node *neighbor, int neighbor_index, int index_of_key, int k_prime);
    void delete_entry(struct node * n, int key, void * pointer);
    void main_delete(int key);


    // search
    int get_range(int key_start, int key_end, int *returned_keys, void **returned_pointers);
    void search(int key1, int key2);
};




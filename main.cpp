#include <iostream>
#include <fstream>
#include <string>
#include "bplus.h"
#include <regex>
#include <vector>
#include <algorithm>

node * queue = nullptr;
/*
 * Used for extracting doubles from the input file
 */
double static getDouble(std::string s)
{
    std::string temp = "";
    for(int i = 0; i < s.length(); i ++)
    {
        if(isdigit(s[i]) or s[i] == '.' or s[i] == '-')
        {
            temp += s[i];
        }
    }
    return std::stod(temp);
}
/*
 * Used for extracting ints from the input file
 */
int static getInt(std::string s)
{
    std::string temp = "";
    for(int i = 0; i < s.length(); i ++)
    {
        if(isdigit(s[i]))
        {
            temp += s[i];
        }
    }
    return std::stoi(temp);
}

/*
 * The following function definition is to set the order of the tree
 */
void B_plus_tree::setOrder(int size) {
    this->order = size;
}

/*
 * The following function is utilized to initialize the tree of order m
 */
B_plus_tree* B_plus_tree::initialize(int size){
    B_plus_tree* tree = new B_plus_tree;
    tree->root = nullptr;
    tree->setOrder(size);
    return tree;
}


/*
 * The following groups of function definitions are involved in the insert process
 * The following function returns either a val_container or a leaf node depending on what
 * youre searching for. It will find the node in which your key is stored or should
 * stored and stores the node in leaf. if youre looking for a val_container, it proceeds to
 * run through that entire node's keys checking if the keys matchup and returns the
 * corresponding val_container. Otherwise it just returns the leaf youre searching for
 */
void* B_plus_tree::locate(int key, node **target, bool isRecord) {
    if (this->root == nullptr){
        return nullptr;
    }


    node* leaf = this->root;
    while (!leaf->leaf){
        int count = 0;
        while (count < leaf->keyCount){
            if (key >= leaf->keys[count]){
                count++;
            }
            else break;
        }
        leaf = (node*)leaf->children[count];

    }
    if (isRecord) {
        int index;
        int loopIndex = 0;
        for(loopIndex; loopIndex < leaf->keyCount; loopIndex++){
            if (leaf->keys[loopIndex] == key){
                index = loopIndex;
                break;
            }
        }

        /*
         * This following if statement is used in delete implementation
         */
        if (target != nullptr) {
            *target = leaf;
        }
        if (loopIndex == leaf->keyCount) {
            return nullptr;
        } else {
            return (val_container *) leaf->children[loopIndex];
        }
    }
    else{
        return leaf;
    }
}

int split_node(int order) {
    if (order % 2 == 0){
        return order/2;
    }
    else{
        return order/2 + 1;
    }
}
/*
 * the following function creates the container with the
 * value to be inserted into the tree
 */
val_container * B_plus_tree::create_container(double value){
    struct val_container* pVal_container = new val_container;
    pVal_container->value = value;
    return pVal_container;
}
/*
 * the following function creates a node
 * to be inserted into the tree
 */
node* B_plus_tree::create_node(bool leaf){
    struct node* new_node = new struct node;
    new_node->keys = new int[(this->order - 1) * sizeof(int)];
    new_node->children = new void*[(this->order * sizeof(void *))];
    new_node->leaf = leaf;
    new_node->keyCount = 0;
    new_node->parent = nullptr;
    new_node->next = nullptr;
    return new_node;
}
/*
 * the following function finds the index of the left sibling of a
 * newly created node from a split
 */
int B_plus_tree::index_left_sibling(struct node *parent, struct node *left){
    int left_index = 0;
    while (left_index <= parent->keyCount and parent->children[left_index] != left){
        left_index++;
    }
    return left_index;
}
/*
 * the following function inserts a container into a leaf
 * it looks through the keys, and when it finds the index where
 * it should be inserted into, it adjusts the array of keys to make space
 * for it. it inserts the key and the container into the array of keys
 * and containers, respectively
 */
void B_plus_tree::leaf_insert(struct node *leaf, int key, struct val_container *pointer){
    int insertion_point = 0;
    while (insertion_point < leaf->keyCount and leaf->keys[insertion_point] < key){
        insertion_point++;
    }
    // adjusts array of keys and children to make space for
    // new key and container to be inserted
    for(int i = leaf->keyCount; i > insertion_point; i--){
        leaf->keys[i] = leaf->keys[i-1];
        leaf->children[i] = leaf->children[i - 1];
    }
    /*
     * the following code actually inserts the key and value into their
     * corresponding arrays
     */
    leaf->keys[insertion_point] = key;
    leaf->children[insertion_point] = pointer;
    leaf->keyCount++;
}
/*
 * the following block of code handles inserts which result in a leaf split
 */
void B_plus_tree::split_leaf_insert(struct node *leaf, int key, val_container *pointer){
    node* new_leaf; // new leaf
    int* key_holder; // holder to keep keys in place to handle segment
    void** child_holder; // holder to keep children in place
    int insertion_index;
    int segment; // holds the index at which nodes are being split
    int key_to_insert;
    int i;
    int j;
    new_leaf = create_node(true); // true indicates that the node is a leaf

    key_holder = new int[this->order * sizeof(int)];
    child_holder = new void*[this->order * sizeof(void*)];

    insertion_index = 0;
    // find the index at which the new key should be inserted
    while(insertion_index < order - 1 && leaf->keys[insertion_index] < key){
        insertion_index++;
    }
    // order all of the keys and values that are to be split including
    //the one being inserted
    for (i = 0, j = 0; i < leaf->keyCount; i++, j++){
        if (j == insertion_index){
            j++;
        }
        key_holder[j] = leaf->keys[i];
        child_holder[j] = leaf->children[i];
    }

    key_holder[insertion_index] = key;
    child_holder[insertion_index] = pointer;

    leaf->keyCount = 0;

    segment = split_node(this->order - 1);

    /*
     * split the node where its suppsoed to be split at
     * half stay in the old, half stay in the new
     */
    for (i = 0; i < segment; i++){
        leaf->children[i] = child_holder[i];
        leaf->keys[i] = key_holder[i];
        leaf->keyCount++;
    }


    for(i = segment, j = 0; i < order; i++, j++){
        new_leaf->children[j] = child_holder[i];
        new_leaf->keys[j] = key_holder[i];
        new_leaf->keyCount++;
    }
    // delete holding variables
    delete(child_holder);
    delete(key_holder);

    new_leaf->children[order - 1] = leaf->children[order - 1];
    leaf->children[order - 1] = new_leaf;

    for (i = leaf->keyCount; i < order - 1; i++){
        leaf->children[i] = nullptr;
    }
    for (i = new_leaf->keyCount; i < order - 1; i++){
        new_leaf->children[i] = nullptr;
    }

    new_leaf->parent = leaf->parent;
    key_to_insert = new_leaf->keys[0];
    // reinsert the newly split leaf into the parent
    return this->parent_insert(leaf, key_to_insert, new_leaf);
}
// insert into a node with space
// shift all keys and children pointers over to accomodate
// for where the new key value pair is to be inserted
void B_plus_tree::node_insert(struct node *parent, int left_index, int key, struct node *right){
    int i;
    bool safety = true;
    if(safety) {
        for (i = parent->keyCount; i > left_index; i--) {
            parent->children[i + 1] = parent->children[i];
            parent->keys[i] = parent->keys[i - 1];
        }
    }
    parent->children[left_index + 1] = right;
    parent->keys[left_index] = key;
    parent->keyCount++;
}
/*
 * the following function inserts into a node that is full
 * this results in a split
 */
void B_plus_tree::split_node_insert(struct node *old_node, int left_index, int key, struct node *right){
    int i;
    int j;
    int segment;
    int key_index;
    node* new_node;
    node* child;
    int* key_holder;
    node ** child_holder;

    /*
     * Create holders to keep the children and keys in order
     */
    child_holder = new node*[(order + 1) * sizeof(node*)];
    key_holder = new int[(order + 1) * sizeof(int)];
    // order the keys in order of how they're going to be split
    // leave an empty space for the new key
    for (i = 0, j = 0; i < old_node->keyCount + 1; i++, j++){
        if (j == left_index  + 1){
            j++;
        }
        child_holder[j] = (node *)old_node->children[i];
    }
    // order the children in order of how they're going to be split
    // leave an empty space for the new child
    for (i = 0, j = 0; i < old_node->keyCount; i++, j++){
        if (j == left_index){
            j++;
        }
        key_holder[j] = old_node->keys[i];
    }

    child_holder[left_index + 1] = right;
    key_holder[left_index] = key;

    /*
     * Create the new node and split the keys and children between
     * the old and new node along the index stored in segment
     */
    segment = split_node(this->order);
    new_node = create_node(false);
    old_node->keyCount = 0;
    for(i = 0; i < segment - 1; i++){
        old_node->children[i] = child_holder[i];
        old_node->keys[i] = key_holder[i];
        old_node->keyCount++;
    }
    old_node->children[i] = child_holder[i];
    key_index = key_holder[segment - 1];
    for (++i, j = 0; i < order; i++, j++) {
        new_node->children[j] = child_holder[i];
        new_node->keys[j] = key_holder[i];
        new_node->keyCount++;
    }
    new_node->children[j] = child_holder[i];
    delete(child_holder); // delete holder
    delete(key_holder); // delete holder
    // make sure that the parent of the new node is same as parent of old_node
    new_node->parent = old_node->parent;
    // ensure children in new_node know that the new_node is now its parent
    for (i = 0; i <= new_node->keyCount; i++){
        child = (node *) new_node->children[i];
        child->parent = new_node;
    }

    /*
     * insert new key into the parent
     * of the two nodes resulting from split,
     * old node to left, new node to right
     */

    return parent_insert(old_node, key_index, new_node);
}
/*
 * The following function inserts a node into the parent
 */
void B_plus_tree::parent_insert(struct node *left, int key, struct node *right){
    int left_index;
    node* parent;

    parent = left->parent;

    /*
     * In the first scenario you need a new root since the parent is null
     */
    if (parent == nullptr){
        return this->new_root_insert(left, key, right);
    }

    /*
     * Find parents pointer to left node
     */
    left_index = index_left_sibling(parent, left);

    /*
     * if the parent has space for an extra node, simply insert into the parent
     */
    if (parent->keyCount < order - 1){
        return this->node_insert(parent, left_index, key, right);
    }
    // otherwise
    return this->split_node_insert(parent, left_index, key, right);


}
void B_plus_tree::new_root_insert(struct node *left, int key, struct node *right){
    node* root = create_node(false);
    root->keys[0] = key;
    root->children[0] = left;
    root->children[1] = right;
    root->keyCount++;
    root->parent = nullptr;
    left->parent = root;
    right->parent = root;
    this->root = root;
}
void B_plus_tree::new_root(int key, val_container *pointer){
    this->root = create_node(true);
    root->keys[0] = key;
    root->children[0] = pointer;
    root->children[order-1] = nullptr;
    root->parent = nullptr;
    root->keyCount += 1;
}
void B_plus_tree::insert(int key, double value){
    val_container* record_pointer = nullptr;
    struct node* leaf = nullptr;

    record_pointer = (val_container*) this->locate(key, nullptr, true);
    /*
     * if the value already exists, update the value
     */
    if (record_pointer != nullptr){
        record_pointer->value = value;
        return;
    }
    /*
     * otherwise create a new val_container for the value
     */
    record_pointer = create_container(value);

    /*
     * if the tree doesnt exist yet, you need to make a tree
     */
    if (this->root == nullptr){
        this->new_root(key, record_pointer);
        return;
    }
    /*
     * The tree already exists
     */

    leaf = (node*) this->locate(key, nullptr, false);

    /*
     * leaf has space for the key and the pointer to the val_container
     * TODO: Pick up where you left off right here
     */
    if (leaf->keyCount < order - 1){
        this->leaf_insert(leaf, key, record_pointer);
        return;
    }

    /*
     * leaf does not have space and must be split
     */
    return this->split_leaf_insert(leaf, key, record_pointer);

}


/*
 * The following groups of function definitions are involved in the remove process
 */
int B_plus_tree::get_sib_index(struct node *n){
    int i;
    /*
     * Return the index of the key to the left
     * of the pointer in the parent
     * pointing to n
     */
    for (i = 0; i <= n->parent->keyCount; i++){
        if (n->parent->children[i] == n){
            return i - 1;
        }
    }
}
/*
 * the following function deletes from a particular node given the key
 * and the child
 */
node* B_plus_tree::delete_from_node(node *n, int key, void *pointer) {
    int i, num_pointers;

    //
    i = 0;
    while(n->keys[i] != key){
        if(n->keys[i] != key) {
            i++;
        }
    }
    for (++i; i < n->keyCount; i++){
        n->keys[i - 1] = n->keys[i];
    }

    // remove the pointer and shift other children accordingly

    if(n->leaf){
        num_pointers = n->keyCount;
    }
    else{
        num_pointers = n->keyCount + 1;
    }
    i = 0;
    while (n->children[i] != pointer){
        i++;
    }
    for(++i; i < num_pointers; i++){
        n->children[i - 1] = n->children[i];
    }

    // reduce the keys by 1
    n->keyCount--;

    // set other children equal to nullptr
    if (n->leaf){
        for (i = n->keyCount; i < order - 1; i++){
            n->children[i] = nullptr;
        }
    }
    else{
        for (i = n->keyCount; i < order - 1; i++){
            n->children[i] = nullptr;
        }
    }
    return n;
}
void B_plus_tree::fix_root(){
    node* new_root;

    /*
     * if the root is not empty, nothing needs to be done, youre good
     */
    if (this->root->keyCount > 0){
        return;
    }

    /*
     * if the root is empty and has a child
     * make the child the new root
     */
    if (!this->root->leaf) {
        this->root = (node*)this->root->children[0]; // check for errors
        this->root->parent = nullptr;
        return;
    }

        /*
         * If its a leaf (has no children)
         * the whole tree is empty
         */
    else{
        this->root = nullptr;
    }
    delete(this->root->keys);
    delete(this->root->children);
    delete(this->root);
}
void B_plus_tree::node_merge(struct node *n, struct node *neighbor, int neighbor_index, int key_of_index){
    int i;
    int j;
    int sib_insert_loc;
    int n_end;
    node* tmp;

    /*
     * if the node is leftmost, swap w right sibling
     */
    if (neighbor_index == -1){
        tmp = n;
        n = neighbor;
        neighbor = tmp;
    }

    sib_insert_loc = neighbor->keyCount;

    /*
     * Case:nonleaf node
     * add the key of index and all the children from neighbor
     */
    if (!n->leaf){
        neighbor->keys[sib_insert_loc] = key_of_index;
        neighbor->keyCount++;

        n_end = n->keyCount;

        for (i = sib_insert_loc + 1, j = 0; j < n_end; i++, j++){
            neighbor->keys[i] = n->keys[j];
            neighbor->children[i] = n->children[j];
            neighbor->keyCount++;
            n->keyCount--;
        }
        neighbor->children[i] = n->children[j];
        // all children have the smae parent
        for(i = 0; i < neighbor->keyCount ; i++){
            tmp = (node*)neighbor->children[i];
            tmp->parent = neighbor;
        }
    }

        /*
         * in a leaf add all keys and children to neighbor and set
         * last child of neighbor to next leaf to keep the linked list
         * going
         */
        /*
         * TODO: Have a slight error here
         */
    else{
        return;
        for (i = sib_insert_loc, j = 0; j < n->keyCount; i++, j++) {
            neighbor->keys[i] = n->keys[j];
            neighbor->children[i] = n->children[j];
            neighbor->keyCount++;
        }
        neighbor->children[order - 1] = n->children[order - 1];
    }
    delete_entry(n->parent, key_of_index, n);
    delete(n->keys);
    delete(n->children);
    delete(n);
}
/*
 * takes one child from sibling
 */
void B_plus_tree::get_node_from_sibling(struct node *n, struct node *neighbor, int neighbor_index, int index_of_key,
                                        int k_prime){
    int i;
    node* tmp;

    if (neighbor_index != -1) {
        if (!n->leaf)
            n->children[n->keyCount + 1] = n->children[n->keyCount];
        for (i = n->keyCount; i > 0; i--) {
            n->keys[i] = n->keys[i - 1];
            n->children[i] = n->children[i - 1];
        }
        if (!n->leaf) {
            n->children[0] = neighbor->children[neighbor->keyCount];
            tmp = (node *)n->children[0];
            tmp->parent = n;
            neighbor->children[neighbor->keyCount] = nullptr;
            n->keys[0] = k_prime;
            n->parent->keys[index_of_key] = neighbor->keys[neighbor->keyCount - 1];
        }
        else {
            n->children[0] = neighbor->children[neighbor->keyCount - 1];
            neighbor->children[neighbor->keyCount - 1] = nullptr;
            n->keys[0] = neighbor->keys[neighbor->keyCount - 1];
            n->parent->keys[index_of_key] = n->keys[0];
        }
    }

        /* if the node is the leftmost child, take a child from the right
         * sibling
         */

    else {
        if (n->leaf) {
            n->keys[n->keyCount] = neighbor->keys[0];
            n->children[n->keyCount] = neighbor->children[0];
            n->parent->keys[index_of_key] = neighbor->keys[1];
        }
        else {
            n->keys[n->keyCount] = k_prime;
            n->children[n->keyCount + 1] = neighbor->children[0];
            tmp = (node *)n->children[n->keyCount + 1];
            tmp->parent = n;
            n->parent->keys[index_of_key] = neighbor->keys[0];
        }
        for (i = 0; i < neighbor->keyCount - 1; i++) {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->children[i] = neighbor->children[i + 1];
        }
        if (!n->leaf)
            neighbor->children[i] = neighbor->children[i + 1];
    }

    // increase the keycount on the node by one and decrease the keycount on its
    //sibling by 1
    n->keyCount++;
    neighbor->keyCount--;


    /*
     * Case: n has a neighbor to the left
     * pull the neighbor's last key-pointer pair over from
     * the neighbor's right end to n's left end
     */


}
void B_plus_tree::delete_entry(struct node* n, int key, void* pointer){
    int min_keys;
    node* neighbor;
    int neighbor_index;
    int index_of_key;
    int key_sib_index;
    int capacity;
    // remove key and pointer from node
    n = this->delete_from_node(n, key, pointer);

    /*
     * Delete from the root
     */
    if(n == root){
        this->fix_root();
    }


    // Determine the smallest allowable size of node

    if(n->leaf){
        min_keys = split_node(this->order - 1);
    }
    else{
        min_keys = split_node(this->order) - 1;
    }
    // if after delete node is above threshold, dont do anything
    if(n->keyCount >= min_keys){
        return;
    }
    /*
     * Otherwise merge w sibling
     */
    neighbor_index = get_sib_index(n);
    if(neighbor_index == -1){
        index_of_key = 0;
    }
    else{
        index_of_key = neighbor_index;
    }
    key_sib_index = n->parent->keys[index_of_key];
    if(neighbor_index == -1){
        neighbor = (node*)n->parent->children[1];
    }
    else{
        neighbor = (node*)n->parent->children[neighbor_index];
    }

    if(n->leaf){
        capacity = order;
    }
    else{
        capacity = order - 1;
    }

    /*
     * Merge nodes
     */
    if (neighbor->keyCount + n->keyCount < capacity){
        this->node_merge(n, neighbor, neighbor_index, key_sib_index);
        return;
    }

        /*
         * Otherwise borrow from sibling
         */
    else{
        this->get_node_from_sibling(n, neighbor, neighbor_index, index_of_key, key_sib_index);
        return;
    }
}
/*
 * This is the main delete function that is called to start the process
 * of removal
 */
void B_plus_tree::main_delete(int key){
    node* key_leaf = nullptr;
    val_container* key_record = nullptr;

    key_record = (val_container*) this->locate(key, &key_leaf, true);

    /*
     * Error maybe?
     */
    if (key_record != nullptr and key_leaf != nullptr){
        this->delete_entry(key_leaf, key, key_record);
        delete(key_record);
    }
}



/*
 * The following groups of function definitions are involved in the search process
 */
int B_plus_tree::get_range(int key_start, int key_end, int *returned_keys, void **returned_pointers) {
    int i, num_found;
    num_found = 0;
    node * n = (node*) locate(key_start, nullptr, false);
    if (n == nullptr) return 0;
    for (i = 0; i < n->keyCount && n->keys[i] < key_start; i++) ;
    if (i == n->keyCount) return 0;
    while (n != nullptr) {
        for (; i < n->keyCount && n->keys[i] <= key_end; i++) {
            returned_keys[num_found] = n->keys[i];
            returned_pointers[num_found] = n->children[i];
            num_found++;
        }
        n = (node*)n->children[order - 1];
        i = 0;
    }
    return num_found;
}
void B_plus_tree::search(int key1, int key2){
    int i;
    int array_size = key2 - key1 + 1;
    int returned_keys[array_size];
    void* returned_pointers[array_size];
    int num_found = get_range(key1, key2, returned_keys, returned_pointers);
    if (!num_found)
        std::cout << "value was not found" << std::endl;
    else {
        for (i = 0; i < num_found; i++)
            printf("Key: %d   Location: %p  Value: %f\n", returned_keys[i],returned_pointers[i],((val_container *)returned_pointers[i])->value);
    }
}
int main(int argc, char* argv[]){
    // regex
    std::vector<int> keys;

    std::regex initialize("Initialize");
    std::regex insert("Insert");
    std::regex del("Delete");
    std::regex search("Search");
    std::smatch matches;

    //set up to read file line by line
    std::string temp;
    std::ifstream file;
    file.open(argv[1]);
    if (!file.is_open())
    {
        std::cout << "Wrong path entered" << std::endl;
        exit(0);
    }
    B_plus_tree* tree;
    while(std::getline(file, temp))
    {
        if(std::regex_search(temp, matches, initialize))
        {
            tree = B_plus_tree::initialize(getInt(temp));
        }
        if(std::regex_search(temp, matches, insert))
        {
            int key = getInt(temp.substr(0, temp.find(',')));
            double value;
            if(temp.find('-') != std::string::npos){
                value = getDouble(temp.substr(temp.find('-')));
            }
            else{
                value = getDouble(temp.substr(temp.find(',')));
            }
            tree->insert(key, value);
            keys.push_back(key);
        }
        if(std::regex_search(temp, matches, del))
        {
            tree->main_delete(getInt(temp));
        }
        if(std::regex_search(temp, matches, search) and temp.find(',') == std::string::npos)
        {
            int key1 = getInt(temp.substr(0, temp.find(')')));
            tree->search(key1, key1);
        }
        if(std::regex_search(temp, matches, search) and temp.find(',') != std::string::npos)
        {
            std::sort(keys.begin(), keys.begin() + keys.size());
            int key1 = getInt(temp.substr(0, temp.find(',')));
            int key2 = getInt(temp.substr(temp.find(','), temp.find(')')));
            for(int i = 0; i < keys.size(); i++){
                if(keys[i] >= key1 and keys[i] <= key2){
                    tree->search(keys[i], keys[i]);
                }
            }
        }
    }
    return 0;
}
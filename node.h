#ifndef NODE_H
#define NODE_H

#define READSIZE 4096

struct Node {
    char* path;
    int hash;
    struct Node* left;
    struct Node* right;
};

struct Node* init_tree();
struct Node* add_node(struct Node*, char*);
int hashString(char* filename);
void insert_node(struct Node*, struct Node*);
int lookupNode(struct Node*, char*);
void printTree(struct Node*, int);

#endif 
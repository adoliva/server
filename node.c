#include "node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

struct Node* init_tree()
{
    struct Node* head = NULL;

    system("find /home/remote/server/webpages -type f > results.txt");

    int fd = open("results.txt", O_RDONLY);
    if(fd < 0)
    {
        printf("find or reading failed\n");
        return NULL;
    }

    char buffer[READSIZE];
    int n = read(fd, buffer, sizeof(buffer));
    if(n < 0)
    {
        printf("Read failed in tree");
        return NULL;
    }

    char* tokptr;
    char *line = NULL;

    line = strtok_r(buffer, "\n", &tokptr);

    while(1)
    {
        line = strtok_r(NULL, "\n", &tokptr);

        if(!line)
            break;
    
        printf("line: %s\n", line);
        head = add_node(head, line);
    }
    printf("DONE\n");

    if(unlink("results.txt") < 0)
    {
	    printf("file cannot be deleted\n");
        return NULL;
    }
    printf("Deleted file\n");

    return head;
}

struct Node* add_node(struct Node* head, char* filename)
{
    if(!filename)
        return NULL;

    struct Node* new_node = malloc(sizeof(struct Node));
    if(!new_node)
    {
        printf("Failed to allocate memory\n");
        return NULL;
    }

    new_node->path = malloc(strlen(filename));
    strncpy(new_node->path, filename, strlen(filename));

    int hash = hashString(filename);
    if(hash < 0)
    {
        printf("Hash failed\n");
        return NULL;
    }
    new_node->hash = hash;

    if(!head)
    {
        printf("Head failed\n");
        return new_node;
    }

    printf("Inserting node: %s", new_node->path);
    insert_node(head, new_node);

    return head;
}

int hashString(char* filename)
{
    int fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        printf("Failed to open file in node\n");
        return 0;
    }

    int hash = 0;
    char buffer[READSIZE];
    memset(buffer, 0, sizeof(buffer));

    while(1)
    {
        int n = read(fd, buffer, sizeof(buffer));
        if(n < 0)
        {
            printf("Failed read in node\n");
            return -1;
        }
        else if(n == 0)
        {
            printf("EOF\n");
            break;
        }
        else
        {
            printf("hashing for: %s\n", filename);
            for(unsigned int i = 0; i < sizeof(buffer) - 1; i++)
                hash += buffer[i];
        }
    }
    
    printf("Hash for %s: %d\n", filename, hash);
    return hash;
}

//Once I add the Merkle Tree, I'll change this to a struct Node* return value and return the balanced tree
void insert_node(struct Node* head, struct Node* new_node)
{
    struct Node* curr = head;
    while(curr)
    {
        if(curr->hash == new_node->hash)
        {
            return;
        }
        else if(curr->hash < new_node->hash)
        {
            if(!curr->left)
            {
                curr->left = new_node;
            }
            else
            {
                curr = curr->left;
            }
        }
        else
        {
            if(!curr->right)
            {
                curr->right = new_node;
            }
            else
            {
                curr = curr->right;
            }
        }
    }
    return;
}

void printTree(struct Node* curr, int level)
{
    if (curr == NULL)
        return;
    for (int i = 0; i < level; i++)
        printf(i == level - 1 ? "|-" : "  ");
    printf("%s: %d\n", curr->path, curr->hash);
    printTree(curr->left, level + 1);
    printTree(curr->right, level + 1);
}

int lookupNode(struct Node* head, char* tag)
{
    if(!head)
    {
        printf("Node not initialized\n");
    }

    if(!tag)
    {
        printf("Tag not initialized\n");
        return -1;
    }

    struct Node* curr = head;

    int hash = strtol(tag, NULL, 10);
    printf("Tag: %d from \n", hash);

    if(hash <= 0)
    {
        printf("Hash < 0\n");
        return -1;
    }

    while(curr)
    {
        if(!curr)
        {
            return 0;
        }
        else if(curr->hash == hash)
        {
            return 1;
        }
        else if(curr->hash < hash)
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
    }

    return 0;
}

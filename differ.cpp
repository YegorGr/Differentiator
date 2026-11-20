#include "differ.h"

node_t* NewNode (type_n type, elem_t value, node_t* left_node, node_t* right_node)
{
    if (left_node == right_node)
    {
        fprintf(stderr, "The same ptr on right and left nodes! r: [%p], l: [%p]", left_node, right_node);
        return nullptr;
    }

    node_t* ptr = (node_t*) calloc (1, sizeof(node_t));
    if (ptr == nullptr)
    {
        fprintf(stderr, "No memory!");
        return nullptr;
    }

    ptr->type  = type;
    ptr->value = value;
    ptr->left  = left_node;
    ptr->right = right_node;

    return ptr;
}


node_t* DiffFunc (const node_t* node)
{
    switch (node->type)
    {
    case NUM:
        return NewNode (NUM, 0, NULL, NULL);
        break;
    case VAR:
        return NewNode ();
    default:
        break;
    }
}

void DestroyNode (node_t* node)
{
    if (node == nullptr) return;
    
    free(node->left);
    free(node->right);
    free(node);
    
}




#include "differ.h"

node_t* NewNode (type_n type, val_t value, node_t* left_node, node_t* right_node)
{
    if (left_node == right_node && left_node != nullptr)
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

node_t* NewNum (double num)
{
    val_t value = {};
    value.num = num;
    return NewNode(NUM, value, nullptr, nullptr);
}

node_t* NewVar (int var)
{
    val_t value = {};
    value.var = var;
    return NewNode(VAR, value, nullptr, nullptr);
}

node_t* NewOp (cmd_n op, node_t* left, node_t* right)
{
    val_t value = {};
    value.op = op;
    return NewNode(OP, value, left, right);
}

void DumpInfix (const node_t* node, FILE* out)
{
    if (node == nullptr)
        return;

    switch (node->type)
    {
        case OP:
            PrintOp(node, out);
            break;
        
        case NUM:
            fprintf(out, "%g", node->value.num);
            break;

        case VAR:
            break;

        default:
            fprintf(stderr, "Incorrect operator!");
            return;
    }
}

void PrintOp (const node_t* node, FILE* out)
{
    assert(node);

    const char* sym = OpToStr((cmd_n) node->value.op);
    if (sym == nullptr)
    {
        fprintf(stderr, "Unknown operation code: %g\n", node->value.num);
        return;
    }

    fprintf(out, "(");
    DumpInfix(node->left, out);
    fprintf(out, "%s", sym);
    DumpInfix(node->right, out);
    fprintf(out, ")");

    return;
}

const char* OpToStr (cmd_n oper)
{
    switch (oper)
    {
        case ADD:  return "+";
        case SUB:  return "-";
        case MULL: return "*";
        case DIV:  return "/";
        default:   return nullptr;
    }
}


// node_t* DiffFunc (const node_t* node)
// {
//     switch (node->type)
//     {
//     case NUM:
//         return NewNode (NUM, 0, NULL, NULL);
//         break;
//     case VAR:
//         return NewNode ();
//     default:
//         break;
//     }
// }

// void DestroyNode (node_t* node)
// {
//     if (node == nullptr) return;
    
//     free(node->left);
//     free(node->right);
//     free(node);

//     return;
    
// }




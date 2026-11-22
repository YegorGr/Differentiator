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

node_t* NewVar (char var)
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

void DumpToFile (const node_t* node, const char* filename)
{
    FILE* out = fopen(filename, "w");
    if (out == nullptr)
    {
        fprintf(stderr, "Cannot open file %s for Infix FIle Dump\n", filename);
        return;
    }

    DumpInfix(node, out);

    fclose(out);
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
            fprintf(out, "%c", node->value.var);
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

void DestroyNode (node_t* node)
{
    if (node == nullptr)
        return;

    DestroyNode(node->left);
    DestroyNode(node->right);
    free(node);
}

node_t* CopyNode (const node_t* node)
{
    if (node == nullptr)
        return nullptr;

    return NewNode(node->type, node->value, CopyNode(node->left),
                   CopyNode(node->right));
}

void DumpNodeDot (const node_t* node, FILE* dot)
{
    if (node == nullptr)
        return;

    switch (node->type)
    {
        case OP:
            fprintf(dot, "    node_%p [shape=Mrecord, style=filled, fillcolor=\"#FFB3BA\", "
                         "label=\"OP | %s\"];\n", node, OpToStr(node->value.op));
            break;

        case NUM:
            fprintf(dot, "    node_%p [shape=Mrecord, style=filled, fillcolor=\"#BAFFC9\", "
                         "label=\"NUM | %g\"];\n", node, node->value.num);
            break;

        case VAR:
            fprintf(dot, "    node_%p [shape=Mrecord, style=filled, fillcolor=\"#BAE1FF\", "
                         "label=\"VAR | %c\"];\n", node, node->value.var);
            break;

        default:
            fprintf(dot, "    node_%p [shape=Mrecord, style=filled, fillcolor=\"#CCCCCC\", "
                         "label=\"UNKNOWN\"];\n", node);
            break;
    }

    if (node->left != nullptr)
    {
        DumpNodeDot(node->left, dot);
        fprintf(dot, "    node_%p -> node_%p \n", node, node->left);
    }

    if (node->right != nullptr)
    {
        DumpNodeDot(node->right, dot);
        fprintf(dot, "    node_%p -> node_%p \n", node, node->right);
    }
}

void GraphDump (const node_t* root, const char* filename)
{
    assert(filename);

    FILE* dot = fopen(filename, "w");
    if (dot == nullptr)
    {
        fprintf(stderr, "Cannot open file %s for GraphDump\n", filename);
        return;
    }

    fprintf(dot, "digraph Tree {\n");
    fprintf(dot, "    rankdir=TB;\n");
    fprintf(dot, "    node [fontname=\"Arial\"];\n");

    DumpNodeDot(root, dot);

    fprintf(dot, "}\n");
    fclose(dot);

    char cmd[MAX_BUFFER] = {};
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o dump.png", filename);

    system(cmd);
}

#define _DL Diff(node->left,  diff_var)
#define _DR Diff(node->right, diff_var)
#define _CL CopyNode(node->left)
#define _CR CopyNode(node->right)

node_t* Diff (const node_t* node, char diff_var)
{
    if (node == nullptr)
        return nullptr;

    switch(node->type)
    {
        case NUM: return _NUM(0);
        case VAR: return _NUM(node->value.var == diff_var ? 1 : 0);
        case OP:  return DiffOp(node, diff_var);

        default:
            fprintf(stderr, "Error: unknown node type in Diff!\n");
            return nullptr;
    }
}

node_t* DiffAdd (const node_t* node, char diff_var)
{
    return _ADD(_DL, _DR);
}

node_t* DiffSub (const node_t* node, char diff_var)
{
    return _SUB(_DL, _DR);
}

node_t* DiffMul (const node_t* node, char diff_var)
{
    node_t* left_part  = _MULL(_DL, _CR);
    node_t* right_part = _MULL(_CL, _DR);

    return _ADD(left_part, right_part);
}

node_t* DiffDiv (const node_t* node, char diff_var)
{
    node_t* left_part_up  = _MULL(_DL, _CR);
    node_t* right_part_up = _MULL(_CL, _DR); 
    
    node_t* numerator   = _SUB(left_part_up, right_part_up);
    node_t* denominator = _MULL(_CR, _CR);

    return _DIV(numerator, denominator);
}

node_t* DiffOp (const node_t* node, char diff_var)
{
    switch (node->value.op)
    {
        case ADD:  return DiffAdd(node, diff_var);
        case SUB:  return DiffSub(node, diff_var);
        case MULL: return DiffMul(node, diff_var);
        case DIV:  return DiffDiv(node, diff_var);
    
        default:
            fprintf(stderr, "Unknown command");
            return nullptr;
    }
}

#undef _DL
#undef _DR
#undef _CL
#undef _CR


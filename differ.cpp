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

#ifdef DEBUG_DUMP
    #define PRINT_NODE(dot, node, color, type_str, val_fmt, val)                                \
        fprintf((dot), "    node_%p [shape=Mrecord, style=filled, fillcolor=\"%s\", "           \
                       "label=\"{ { %s | " val_fmt " } | Ptr: %p | "                            \
                       "{ <left> Ptr(L): %p | <right> Ptr(R): %p } }\"];\n",                    \
                       (node), (color), (type_str), (val), (node), (node->left), (node->right)) 
#else
    #define PRINT_NODE(dot, node, color, type_str, val_fmt, val)                                \
        fprintf((dot), "    node_%p [shape=Mrecord, style=filled, fillcolor=\"%s\", "           \
                       "label=\"{ %s | " val_fmt " }\"];\n", (node), (color), (type_str), (val))
#endif

void DumpNodeDot (const node_t* node, FILE* dot)
{
    if (node == nullptr)
        return;

    switch (node->type)
    {
        case OP:
            PRINT_NODE(dot, node, "#FFB3BA", "OP",  "%s", OpToStr(node->value.op));
            break;

        case NUM:
            PRINT_NODE(dot, node, "#BAFFC9", "NUM", "%g", node->value.num);
            break;

        case VAR:
            PRINT_NODE(dot, node, "#BAE1FF", "VAR", "%c", node->value.var);
            break;

        default:
            fprintf(dot, "    node_%p [shape=Mrecord, style=filled, fillcolor=\"#CCCCCC\", "
                         "label=\"UNKNOWN\"];\n", node);
            break;
    }

    if (node->left != nullptr)
    {
        DumpNodeDot(node->left, dot);

        #ifdef DEBUG_DUMP
            fprintf(dot, "    node_%p:left -> node_%p \n", node, node->left);
        #else
            fprintf(dot, "    node_%p -> node_%p \n", node, node->left);
        #endif
    }

    if (node->right != nullptr)
    {
        DumpNodeDot(node->right, dot);

        #ifdef DEBUG_DUMP
            fprintf(dot, "    node_%p:right -> node_%p \n", node, node->right);
        #else
            fprintf(dot, "    node_%p -> node_%p \n", node, node->right);
        #endif
    }
}

void GraphDump (const node_t* root, const char* name_dot, const char* name_png)
{
    assert(name_dot);
    assert(name_png);

    FILE* dot = fopen(name_dot, "w");
    if (dot == nullptr)
    {
        fprintf(stderr, "Cannot open file %s for GraphDump\n", name_dot);
        return;
    }

    fprintf(dot, "digraph Tree {\n");

    fprintf(dot, "    rankdir=TB;\n");
    fprintf(dot, "    nodesep=0.5;\n");
    fprintf(dot, "    ranksep=0.8;\n");
    fprintf(dot, "    splines=curved;\n");
    fprintf(dot, "    node [fontname=\"Arial\"];\n");

    DumpNodeDot(root, dot);

    fprintf(dot, "}\n");
    fclose(dot);

    char cmd[MAX_BUFFER] = {};
    snprintf(cmd, sizeof(cmd), "dot -Tpng %s -o %s", name_dot, name_png);

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

bool IsEqual (double a, double b)
{
    return fabs(a - b) < EPSILON;
}

node_t* ReplaceNum (node_t* node, double val, bool* status)
{
    DestroyNode(node);

    *status = true;

    return _NUM(val);
}

node_t* ReplaceChild (node_t* node, node_t* delete_node, node_t* connect_node, bool* status)
{
    DestroyNode(delete_node);
    free(node);
    *status = true;

    return connect_node;
}

bool IsNum (const node_t* node, double val)
{
    return node != nullptr && node->type == NUM && IsEqual(node->value.num, val);
}

node_t* Simplify (node_t* node)
{
    bool status = true;
    while (status)
    {
        status = false;

        node = SimplifyConst(node, &status);
        node = SimplifyBase (node, &status);
    }

    return node;
}

node_t* SimplifyConst (node_t* node, bool* status)
{
    if (node == nullptr || node->type != OP) return node;

    node->left  = SimplifyConst(node->left,  status);
    node->right = SimplifyConst(node->right, status);

    if (node->left->type != NUM || node->right->type != NUM) return node;


    double first_num  = node->left->value.num;
    double second_num = node->right->value.num;
    double result     = 0.0;

    switch(node->value.op)
    {
        case ADD:  result = first_num + second_num; break;
        case SUB:  result = first_num - second_num; break;
        case MULL: result = first_num * second_num; break;
        case DIV:
            if (IsEqual(second_num, 0.0))
                return node;
            result = first_num / second_num;
            break;
        
            default:
            return node;
    }

    return ReplaceNum(node, result, status);
}

node_t* SimplifyBase (node_t* node, bool* status)
{
    if (node == nullptr || node->type != OP) return node;

    node->left  = SimplifyBase(node->left,  status);
    node->right = SimplifyBase(node->right, status);

    switch (node->value.op)
    {
    case ADD:
        if (IsNum(node->left,  0.0)) return ReplaceChild(node, node->left, node->right, status);
        if (IsNum(node->right, 0.0)) return ReplaceChild(node, node->right, node->left, status);
        break;

    case SUB:
        if (IsNum(node->right, 0.0)) return ReplaceChild(node, node->right, node->left, status);
        break;
    
    case MULL:
        if (IsNum(node->left,  0.0) || IsNum(node->right, 0.0)) return ReplaceNum(node, 0.0, status);
        if (IsNum(node->left, 1.0))  return ReplaceChild(node, node->left, node->right, status);
        if (IsNum(node->right, 1.0)) return ReplaceChild(node, node->right, node->left, status);
        break;

    case DIV:
        if (IsNum(node->left,  0.0)) return ReplaceNum(node, 0.0, status);
        if (IsNum(node->right, 1.0)) return ReplaceChild(node, node->right, node->left, status);
        break;
 
    default:
        break;
    }

    return node;
}

void SkipSpaces (char** string)
{
    while (isspace(**string))
        (*string)++;
}

node_t* OperInfix (char oper, node_t* left, node_t* right)
{
    switch (oper)
        {
            case '+': return _ADD(left, right);
            case '-': return _SUB(left, right);
            case '*': return _MULL(left, right);
            case '/': return _DIV(left, right);
        
            default:
                fprintf(stderr, "Unknown operator: %c", oper);
                return nullptr;
        }
}

node_t* DigitInfix (char* string)
{
    double val = NAN;
    memcpy(&val, string, sizeof(double));

    return _NUM(val);
}

node_t* AlphaInfix (char* string)
{
    char var = *string;

    return _VAR(var);
}

node_t* InfixRead (char* string)
{
    SkipSpaces(&string);

    if (*string == '(')
    {
        string++;

        node_t* left = InfixRead(string);
        SkipSpaces(&string);

        char oper = *string;
        string++;

        node_t* right = InfixRead(string);
        SkipSpaces(&string);

        if (*string == ')') string++;

        return OperInfix(oper, left, right);
    }

    if (isdigit(*string)) return DigitInfix(string);

    if (isalpha(*string)) return AlphaInfix(string);

    return nullptr;
}
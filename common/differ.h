#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define _VAR(x) NewVar(x)
#define _NUM(n) NewNum(n)

#define _ADD(a, b)  NewOp(ADD,  (a), (b))
#define _SUB(a, b)  NewOp(SUB,  (a), (b))
#define _MULL(a, b) NewOp(MULL, (a), (b))
#define _DIV(a, b)  NewOp(DIV,  (a), (b))

const int MAX_VARS   = 8;
const int MAX_BUFFER = 256;
const double EPSILON = 1e-7;

struct value_name
{
    char   name;     
    double value;
};

struct var_array_t
{
    value_name  items [MAX_VARS];
    int         count;
};

enum type_n 
{
    OP,
    NUM,
    VAR,
};

enum cmd_n
{
    ADD,
    SUB,
    MULL,
    DIV,
};

union val_t
{
    double num;
    cmd_n  op;
    char   var;
};

struct node_t 
{
    type_n  type;
    val_t   value;
    node_t* left;
    node_t* right;
};

struct parser_t
{
    const char* str;
    size_t      pos;
};

node_t* NewNode     (type_n type, val_t value, node_t* left_node, node_t* right_node);
void    DestroyNode (node_t* node);
node_t* CopyNode    (const node_t* node);

node_t* NewNum (double num);
node_t* NewVar (char var);
node_t* NewOp  (cmd_n op, node_t* left, node_t* right);

void DumpToFile  (const node_t* node, const char* filename);
void DumpInfix   (const node_t* node, FILE* out);
void PrintOp     (const node_t* node, FILE* out);

void GraphDump   (const node_t* root, const char* name_dot, const char* name_png);
void DumpNodeDot (const node_t* node, FILE* dot);

node_t* Diff    (const node_t* node, char diff_var);
node_t* DiffAdd (const node_t* node, char diff_var);
node_t* DiffSub (const node_t* node, char diff_var);
node_t* DiffMul (const node_t* node, char diff_var);
node_t* DiffDiv (const node_t* node, char diff_var);
node_t* DiffOp  (const node_t* node, char diff_var);

const char* OpToStr    (cmd_n oper);

bool    IsNum          (const node_t* node, double val);
bool    IsEqual        (double a, double b);
node_t* ReplaceNum     (node_t* node, double val, bool* status);
node_t* ReplaceChild   (node_t* node, node_t* delete_node, node_t* connect_node, bool* status);

node_t* Simplify       (node_t* node);
node_t* SimplifyConst  (node_t* node, bool* status);
node_t* SimplifyBase   (node_t* node, bool* status);

void    SkipSpaces (char** string);
node_t* OperInfix  (char oper, node_t* left, node_t* right);
node_t* DigitInfix (char** string);
node_t* AlphaInfix (char** string);
node_t* InfixRead  (char** string);

void    SetVar     (var_array_t* array, char name, double value);
double  GetVar     (var_array_t* array, char name);

double  SolverOp   (node_t* node, var_array_t* array);
double  SolveTree  (node_t* node, var_array_t* array);

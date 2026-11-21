#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#define _VAR(i) NewVar(i);
#define _NUM(n) NewNum(n);

#define _ADD(a, b)  NewOp(ADD,  (a), (b));
#define _SUB(a, b)  NewOp(SUB,  (a), (b));
#define _MULL(a, b) NewOp(MULL, (a), (b));
#define _DIV(a, b)  NewOp(DIV,  (a), (b));

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
    int    var;
};

struct node_t 
{
    type_n  type;
    val_t   value;
    node_t* left;
    node_t* right;
};

node_t* NewNode  (type_n type, val_t value, node_t* left_node, node_t* right_node);

node_t* NewNum (double num);
node_t* NewVar (int var);
node_t* NewOp  (cmd_n op, node_t* left, node_t* right);

void DumpInfix (const node_t* node, FILE* out);
void PrintOp   (const node_t* node, FILE* out);

const char* OpToStr (cmd_n oper);



// node_t* DiffFunc (const node_t* node);

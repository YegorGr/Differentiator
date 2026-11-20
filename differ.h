#pragma once 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

typedef double elem_t;

enum type_n 
{
    OP,
    NUM,
    VAR,
};

enum cmd_n
{
    ADD,
    MULL,
    DIV,
};

struct node_t 
{
    type_n  type;
    double  value;
    node_t* left;
    node_t* right;
};

node_t* NewNode  (type_n type, elem_t value, node_t* left_node, node_t* right_node);

node_t* DiffFunc (const node_t* node);

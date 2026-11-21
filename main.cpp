#include "differ.h"

int main ()
{
    FILE* out = fopen("out.txt", "w");

    node_t* node1 = NewNum(7);
    node_t* node2 = NewNum(3);

    node_t* node3 = NewOp(DIV, node1, node2);

    node_t* node4 = NewNum(5);

    node_t* node5 = NewOp(ADD, node3, node4);

    DumpInfix(node5, out);

    fclose(out);
}

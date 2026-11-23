#include "differ.h"

int main ()
{
    node_t* node1 = NewNum(7);
    node_t* node2 = NewNum(3);

    node_t* node3 = NewOp(DIV, node1, node2);

    node_t* node6 = _VAR('x');
    node_t* node7 = _NUM(6);

    node_t* node8 = _ADD(node6, node7);

    node_t* node5 = NewOp(ADD, node3, node8);

    node5 = Diff(node5, 'x');
    
    node5 = Simplify(node5);

    GraphDump(node5, "dump.dot", "dump.png");

    DumpToFile(node5, "out.txt");

    DestroyNode(node5);
}

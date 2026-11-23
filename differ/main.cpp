#include "differ.h"
#include "fileinfo.h"

int main (int argc, const char* argv[])
{
    var_array_t array = {};

    FileInfo fileinfo = {};
    fileinfo = GetFileInfo(argc, argv);

    char* string = fileinfo.buffer;
    node_t* node_node = InfixRead(&string);

    node_node = Diff(node_node, 'x');
    
    node_node = Simplify(node_node);

    SetVar(&array, 'x', 5);
    printf("Result: %g", SolveTree(node_node, &array));

    GraphDump(node_node, "dump.dot", "dump.png");

    DumpToFile(node_node, "out.txt");

    DestroyNode(node_node);
}

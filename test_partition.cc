#include <iostream>
#include "partition.h"
#include "graph.h"
#include "def.h"

int main(int argc, char** argv)
{
    Int nv = atoi(argv[1]);
    Int m0 = atoi(argv[2]);
    Graph* graph = new Graph(nv,m0);
    Partition* partition = new Partition(graph);

    std::cout << partition->get_mass() << std::endl;
    std::cout << partition->get_modularity() << std::endl;

    delete partition;
    delete graph;
    return 0;
}

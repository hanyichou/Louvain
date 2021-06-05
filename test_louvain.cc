#include <cstdlib>
#include <iostream>
#include "graph.h"
#include "partition.h"
#include "louvain.h"

int main(int argc, char** argv)
{
    using namespace std;

    Int nv = atoi(argv[1]);
    Int m0 = atoi(argv[2]);
    Int maxloop = atoi(argv[3]);
    Float tau = atof(argv[4]);

    Graph* graph = new Graph(nv, m0);
    Partition* partition = new Partition(graph);
    Louvain* louvain = new Louvain(partition, maxloop, tau);

    louvain->run();

    delete louvain;
    delete partition;
    delete graph;

    return 0;
}

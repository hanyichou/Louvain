#include <cstdlib>
#include <iostream>
#include <string>
#include "graph.h"
#include "partition.h"
#include "louvain.h"

int main(int argc, char** argv)
{
    using namespace std;

    Int nv;
    Int maxloop = 1e04;
    Float tau = 1e-6;
    Graph* graph = nullptr;

    if(std::string(argv[1]) == "-r")
    {
        nv = atoi(argv[2]);
        Int m0 = atoi(argv[3]);
        maxloop = atoi(argv[4]);
        tau = atof(argv[5]);
        graph = new Graph(nv, m0);
    }
    else if(std::string(argv[1]) == "-f")
    {
        graph = new Graph(std::string(argv[2]));
        maxloop = atoi(argv[3]);
        tau = atof(argv[4]);
    }

    Partition* partition = new Partition(graph);
    Louvain* louvain = new Louvain(partition, maxloop, tau);

    louvain->run();

    delete louvain;
    delete partition;
    delete graph;

    return 0;
}

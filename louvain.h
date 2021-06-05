#ifndef LOUVAIN_H_
#define LOUVAIN_H_

#include "graph.h"
#include "partition.h"
#include "def.h"

class Louvain
{
  private:
    Partition* partition_;
    Int maxLoop_;
    Float tau_;

    //void show_clusters(const Partition&);
    Float move_vertex();
 
  public:
    Louvain(Partition* partition, const Int& maxLoop, const Float& tau) : 
    partition_(partition), maxLoop_(maxLoop), tau_(tau) {};

    ~Louvain(){};
    void run();
    
};
#endif

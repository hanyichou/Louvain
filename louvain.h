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

    #ifdef USE_SHARED_OMP
    Float move_vertex(const int&, Int*, Int*, Float*);
    #else
    Float move_vertex();
    Float random_move_vertex(Mt19937*);
    #endif
  public:
    Louvain(Partition* partition, const Int& maxLoop, const Float& tau) : 
    partition_(partition), maxLoop_(maxLoop), tau_(tau) {};

    ~Louvain(){};
    #ifdef USE_SHARED_OMP
    void run(const int&);
    #else
    void run(int);
    #endif
};
#endif

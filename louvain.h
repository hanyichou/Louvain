#ifndef LOUVAIN_H_
#define LOUVAIN_H_
#include "graph.h"
class Louvain
{
  private:
    Partion* partion;
    double tau;
    long maxLoop;

    //void show_clusters(const Partition&); 
  public:
    Louvain(const Partition&, const double& _tau, const double& _maxLoop);
    Louvain::Louvain(const Partition& _partition, const double& _tau, 
                     const long& _maxLoop) : partition(&_partition), tau(_tau), maxLoop(_maxLoop)
    {}


    ~Louvain();
    run();
    
};
#endif

#ifndef PARTITION_H_
#define PARTITION_H_

#include "def.h"
#include "heap.h"
#include "graph.h"

typedef struct Community
{
    Int commId;    //community id
    Float ac;  //sum_i w_{ij}, foreach i in C
    Heap<Int,Min> vertices; //heap sorted vertex list
    Heap<Int,Min> flatVertices; //heap sorted flatten vertex list
} Community;

class Partition
{
  private:
    Graph* graph_;

    Int  *commMap_;
    Community **community_;

    Float m_; 
    Float modularity_;
    
  public: 
    Partition(const Graph&);
    ~Partition();
 
    void singleton_partion();    

    void move_vertex(const Int&, const Int&, const Float&); 
    //get informations
    Graph* get_graph(); 
    Int get_comm_id(const Int&);
};
#endif

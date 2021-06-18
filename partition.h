#ifndef PARTITION_H_
#define PARTITION_H_

#include "def.h"
#include "heap.h"
#include "graph.h"

#if 0
typedef struct Community
{
    Int commId;    //community id
    Float ac;  //sum_i w_{ij}, foreach i in C
    HeapList<Int,Min>* vertices; //heap sorted vertex list
    //HeapList<Int,Min>** flatVertices; //heap sorted flatten vertex list
} Community;
#endif

class Partition
{
  private:
    Graph* graph_;

    Int   *commMap_;
    Int   *commNumVertices_;
    //Community **community_;
    Float *ac_;
    //uint32_t* commIdList_;

    Float m_; 
    Float modularity_;
   
  public: 
    Partition(Graph*);
    ~Partition();
 
    void singleton_partition();    
    void aggregate_graph();
    Float compute_modularity();
 
    void move_vertex(const Int&, const Int&, const Int&, 
                     const Float&, const Float&); 
    void move_vertex(Move*, const int&);
    void update_modularity(const float&);

    //get informations
    Graph* get_graph(); 
    Int get_comm_id(const Int&) const;
    Float get_mass() const { return m_;};
    Float get_modularity() const { return modularity_; };
    Int*  get_community() { return commMap_; };
    Int*  get_num_vertices_in_communities() { return commNumVertices_; };
    Float* get_community_order() { return ac_;};
    void move_vertex(const Move*, const int&);
    void show_partition() const;
};
#endif

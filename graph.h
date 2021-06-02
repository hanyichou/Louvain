#ifndef GRAPH_H_
#define GRAPH_H_
#include <list>
#include "heap.h"
#include "def.h"

typedef struct Edge
{
    Int x;
    Float w;
} Edge;

typedef struct EdgeTuple
{
    Int x,y;
    Float w;
} EdgeTuple;

class EdgeTupleMin
{
  private:
    EdgeTupleMin();
  public:
    static bool less(const EdgeTuple& a, const EdgeTuple& b)
    {
        if(a.x < b.x)
            return true;
        else if(a.x > b.x)
            return false;
        else if (a.y <= b.y)
            return true;
        else 
            return false;
    }
};

typedef struct Community
{
    Int commId;    //community id
    Float ac;  //sum_i w_{ij}, foreach i in C
    Heap<Int,Min> vertices; //heap sorted vertex list
    Heap<Int,Min> flatVertices; //heap sorted flatten vertex list
} Community;

class Graph
{
  private:
    Int   totalVertices_;
    Int   max_order_;
    Float *weighted_orders_;
    Float *max_weights_;
    Int   *orders_;

    Int   *indices_;
    Int   totalEdges_;
    Int   *edges_;
    Float *weights_;

    //some helper function here
    void sort_edges(EdgeTuple*, const Int&);
    void create_random_network_ba(const Int& m0);
    void neigh_scan();
    void neigh_scan_weights();
    void neigh_scan_max_weight();
    void neigh_scan_max_order();

  public:      
    Graph(const Int&, const Int&);

    ~Graph()
    {
        delete [] weighted_orders_;
        weighted_orders_ = nullptr;  
      
        delete [] max_weights_;
        max_weights_ = nullptr;

        delete [] orders_;
        orders_ = nullptr;

        delete [] indices_;
        indices_ = nullptr;

        delete [] edges_;
        edges_ = nullptr;

        delete [] weights_;
        weights_ = nullptr;
        
    }
    Int*    get_adjacent_vertices(const Int&);
    Float*  get_weights();
    Int*    get_orders();
    Float*  get_weighted_orders();
    Int     get_num_adjacent_vertices(const Int&);
    Int     get_num_vertices();
    Int     get_num_edges();
    Int     get_max_order();

    //print statistics
    void print_stats();

    void reset_orders_weights();
    void neigh_scan(const int& num_threads);
    void neigh_scan_weights(const int& num_threads);
    void neigh_scan_max_weight(const int& num_threads);
    
};
#if 0
class Partition
{
  private:
    Graph* graph;

    Int  *commMap;
    Community **community;

    double m; 
    double modularity;
    
  public: 
    Partition(const Graph&);
    ~Partition();
 
    //void aggregate_graph();
    //Graph* create_aggregate_graph();
 
    //void singleton_partion();    

    void move_vertex(const Int& vertex_id, const Int& comm_id); 
    //get informations
    Graph* get_graph(); 
    Int get_comm_id(const Int&);
};
#endif
#endif

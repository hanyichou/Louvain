#include "partition.h"

Partition::Partition(const Graph& graph) : graph_(&graph), 
commMap_(nullptr), community_(nullptr), m_(0), modularity_(0)
{
    Int num_vertices = graph_->get_num_vertices();
    commMap_ = new Int [num_vertices];
    community_ = new Community* [num_vertices];
    for(Int i = 0; i < num_vertices; ++i)
        community_[i] = new Community;

    singleton_partition();
}

void Partition::singleton_partition()
{

}

void Partition::move_vertex(const Int& v, const Int& comm_id, const Float& dela)
{

}

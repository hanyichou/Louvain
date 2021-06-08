#include <iostream>
#include "partition.h"
#ifdef USE_OMP
Float Partition::compute_modularity()
{
    Float mod = 0.;
    Int num = graph_->get_num_vertices();
    
    uint32_t* comm_id_list = new uint32_t[(num+31)>>5];
    for(Int i = 0; i < (num+31)>>5; ++i)
        comm_id_list[i] = 0;

    #pragma omp parallel for reduction(+:mod)
    for(Int u = 0; u < num; ++u)
    {
        Int* edges = graph_->get_adjacent_vertices(u);
        Int n = graph_->get_num_adjacent_vertices(u);
        Float* w = graph_->get_adjacent_weights(u);

        Int my_comm_id = commMap_[u];

        for(Int j = 0; j < n; ++j) 
        {
            Int v = edges[j]; 
            if(commMap_[v] == my_comm_id)
                mod += w[j];
        }

        Int pos = my_comm_id>>5;
        uint32_t flag = 1<<(my_comm_id%32);
        #pragma omp critical
        {
            if(!(comm_id_list[pos] & flag))
            {
                comm_id_list[pos] |= flag;
                Float ac = ac_[my_comm_id];
                mod -= ac*ac/(2.*m_);
            }
        }
    }
    mod /= (2.*m_);
    delete [] comm_id_list;
    return mod;
}
#else
Float Partition::compute_modularity()
{
    Float mod = 0.;
    Int num = graph_->get_num_vertices();
    
    uint32_t* comm_id_list = new uint32_t[(num+31)>>5];
    for(Int i = 0; i < (num+31)>>5; ++i)
        comm_id_list[i] = 0;

    for(Int u = 0; u < num; ++u)
    {
        Int* edges = graph_->get_adjacent_vertices(u);
        Int n = graph_->get_num_adjacent_vertices(u);
        Float* w = graph_->get_adjacent_weights(u);

        Int my_comm_id = commMap_[u];

        for(Int j = 0; j < n; ++j) 
        {
            Int v = edges[j]; 
            if(commMap_[v] == my_comm_id)
                mod += w[j];
        }
        uint32_t is_set = comm_id_list[my_comm_id>>5];
        uint32_t flag = 1<<(my_comm_id%32);
        is_set &= flag;

        if(!is_set)
        {
            comm_id_list[my_comm_id>>5] |= flag;
            Float ac = ac_[my_comm_id];
            mod -= ac*ac/(2.*m_);
        }
    }
    mod /= (2.*m_);
    delete [] comm_id_list;
    return mod;
}
#endif

void Partition::singleton_partition()
{
    Int num = graph_->get_num_vertices();
    Float* w = graph_->get_weighted_orders();
    for(Int i = 0; i < num; ++i)
    {
        commMap_[i] = i;
        Float val = w[i];
        ac_[i] += val;
        m_ += val;
    }
    m_ /= 2.;
}

Partition::Partition(Graph* graph) : graph_(graph), 
commMap_(nullptr), ac_(nullptr), m_(0), modularity_(0)
{
    Int num_vertices = graph_->get_num_vertices();
    commMap_    = new Int [num_vertices];
    ac_ = new Float[num_vertices];
    for(Int i = 0; i < num_vertices; ++i)
    {
        commMap_[i] = 0;
        ac_[i] = 0.;
    }
    singleton_partition();
    modularity_ = compute_modularity();
}

Partition::~Partition()
{
    delete [] commMap_;
    delete [] ac_;
}

Int Partition::get_comm_id(const Int& i) const 
{
    return commMap_[i];
}

Graph* Partition::get_graph()
{
    return graph_;
}

void Partition::aggregate_graph()
{

}

void Partition::move_vertex(const Int& v, const Int& src, const Int& dest, const Float& delta, const Float& ki)
{
    modularity_ += delta;
    commMap_[v] = dest;
    ac_[src] -= ki;
    ac_[dest] += ki;
}

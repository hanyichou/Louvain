#include <cstdlib>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>
#include "graph.h"
#include "def.h"
#include "heap.h"

#if 0
Int Graph::vertex_id(const Int& key) 
{
    Int low = 0;
    Int high = totalVertices-1;
    Int mid;
    while(low <= high) 
    {
        mid = (low + high)/2;
        #ifdef DO_PREFETCH
        // low path
        __builtin_prefetch (&indices[(mid + 1 + high)>>1], 0, 1);
        // high path
        __builtin_prefetch (&indices[(low + mid - 1)>>1], 0, 1);
        #endif

        if(indices[mid] < key && indices[mid+1] <= key)
            low = mid + 1; 
        else if(indices[mid].x <= key && indices[mid+1] > key)
            return mid;
        else if(indices[mid] > key)
            high = mid-1;
    }
    return -1;
}
#endif

Int Graph::get_num_vertices()
{
    return totalVertices_;
}

Int Graph::get_num_edges()
{
    return totalEdges_;
}

Int Graph::get_num_adjacent_vertices(const Int& i)
{
    return indices_[i+1]-indices_[i];
}

Int* Graph::get_adjacent_vertices(const Int& i)
{
    Int first = indices_[i];
    return edges_+first; 
}

Int* Graph::get_orders()
{
    return orders_;
}
 
Float* Graph::get_weights()
{
    return weights_;
}

Float* Graph::get_weighted_orders()
{
    return weighted_orders_;
}

Int Graph::get_max_order()
{
    return max_order_;
}

/*
void Graph::compute_order()
{
    memset(order, sizeof(long)*numVertex);
    memset(weight_order, sizeof(double)*numVertex);

    for(long vi = 0; vi < numVertex; ++vi)
    {
        long numEdges = get_num_edge();
        long* edge = get_edge(vi);
        double* weight = get_weight(vi);
 
        for(long j = 0; j < numEdge; ++j)
        {
            long vj = edge[j];
            double wij = weight[j];
            order[vi] += 1;
            weight_order[vi] += wij;
        }
    }
}
*/

void Graph::sort_edges(EdgeTuple* edgeList, const Int& num)
{
    Heap<EdgeTuple, EdgeTupleMin> heap(edgeList, num);
    edges_   = new Int[num];
    weights_ = new Float[num];
    for(Int i = 0; i < num; ++i)
    {
        EdgeTuple e = heap.pop_back();
        edges_[i] = e.y;
        weights_[i] = e.w;
    }
}

void Graph::create_random_network_ba(const Int& m0)
{
    EdgeTuple* edgeList = new EdgeTuple[2*totalVertices_*m0];
    std::default_random_engine rand_gen1, rand_gen2;
    std::normal_distribution<Float> gaussian(1.0,1.0);

    indices_ = new Int[totalVertices_+1];
    indices_[0] = 0;
    for(Int i = 0; i < totalVertices_; ++i)
        indices_[i+1] = 0;

    totalEdges_ = 0;
    for(int i = 0; i < m0; ++i)
    {
        Float w = fabs(gaussian(rand_gen1));
        edgeList[totalEdges_+0] = {0,   i+1, w};
        edgeList[totalEdges_+1] = {i+1, 0,   w};
        indices_[1]   += 1;
        indices_[i+2] += 1;
        totalEdges_ += 2;
    }
    for(Int i = m0+1; i < totalVertices_; ++i)
    {
        std::uniform_int_distribution<Int> uniform(0,totalEdges_-1);
        for(Int j = 0; j < m0; ++j)
        {
            EdgeTuple e  = edgeList[uniform(rand_gen2)];
            Float w = fabs(gaussian(rand_gen1));
            edgeList[totalEdges_+2*j+0] = {i, e.x, w};
            edgeList[totalEdges_+2*j+1] = {e.x, i, w};
            indices_[i+1]   += 1;
            indices_[e.x+1] += 1;
        }
        totalEdges_ += 2*m0;
    }
    for(Int i = 1; i <= totalVertices_; ++i)
        indices_[i] += indices_[i-1];
    sort_edges(edgeList, totalEdges_);
    delete [] edgeList;
}

void Graph::reset_orders_weights()
{
    for(Int i = 0; i < totalVertices_; ++i)
    {
        orders_[i] = 0;
        weighted_orders_[i] = 0.;
        max_weights_[i] = 0.;
    }
}

void Graph::neigh_scan_max_order()
{
    Int o;
    for(Int i = 0; i < totalVertices_; ++i)
        if((o = orders_[i]) > max_order_)
            max_order_ = o;    
}

Graph::Graph(const Int& totalVertices, const Int& m0) :
totalVertices_(totalVertices), max_order_(0), 
weighted_orders_(nullptr), max_weights_(nullptr), orders_(nullptr),
indices_(nullptr), totalEdges_(0), edges_(nullptr), 
weights_(nullptr)
{
    create_random_network_ba(m0);
    
    weighted_orders_ = new Float [totalVertices_];
    max_weights_ = new Float[totalVertices_];
    orders_  = new Int[totalVertices_];
    for(Int i = 0; i < totalVertices_; ++i)
    {
        weighted_orders_[i] = 0.;
        max_weights_[i] = 0.;
        orders_[i] = 0;
    }
    neigh_scan();
    neigh_scan_weights();
    neigh_scan_max_weight();
    neigh_scan_max_order();
}

void Graph::neigh_scan()
{
    for(Int i = 0; i < totalVertices_; ++i)
    {
        Int start = indices_[i];
        Int end = indices_[i+1];
        for(Int j = start; j < end; ++j)
            orders_[i] += 1;
    }
}

void Graph::neigh_scan_weights()
{
    for(Int i = 0; i < totalVertices_; ++i)
    {
        Int start = indices_[i];
        Int end = indices_[i+1];
        for(Int j = start; j < end; ++j)
        {
            Int u = edges_[j];
            Float w = weights_[u];
            weighted_orders_[i] += w;
        }
    }
}

void Graph::neigh_scan_max_weight()
{
    for(Int i = 0; i < totalVertices_; ++i)
    {
        Int start = indices_[i];
        Int end = indices_[i+1];
        Float max = 0.;
        for(Int j = start; j < end; ++j)
        {
            Int u = edges_[j];
            Float w = weights_[u];
            if(max < w)
                max = w;
        }
        max_weights_[i] = max;
    }
}

void Graph::neigh_scan(const int& num_threads)
{
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for(Int i = 0; i < totalVertices_; ++i)
    {
        Int start = indices_[i];
        Int end = indices_[i+1];
        for(Int j = start; j < end; ++j)
            orders_[i] += 1;
    }
}

void Graph::neigh_scan_weights(const int& num_threads)
{
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for(Int i = 0; i < totalVertices_; ++i)
    {
        Int start = indices_[i];
        Int end = indices_[i+1];
        for(Int j = start; j < end; ++j)
        {
            Int u = edges_[j];
            Float w = weights_[u];
            weighted_orders_[i] += w;
        }
    }
}

void Graph::neigh_scan_max_weight(const int& num_threads)
{
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for(Int i = 0; i < totalVertices_; ++i)
    {
        Int start = indices_[i];
        Int end = indices_[i+1];
        Float max = 0;
        for(Int j = start; j < end; ++j)
        {
            Int u = edges_[j];
            Float w = weights_[u];
            if(max < w)
                max = w;
        }
        max_weights_[i] = max;
    }
}

void Graph::print_stats()
{
    std::vector<Int> pdeg(totalVertices_, 0);
    for (Int v = 0; v < totalVertices_; v++)
    {
        Int num = get_num_adjacent_vertices(v);
        pdeg[v] += num;
    }
            
    std::sort(pdeg.begin(), pdeg.end());
    Float loc = (Float)(totalVertices_ + 1)/2.0;
    Int median;
    if (fmod(loc, 1) != 0)
        median = pdeg[(Int)loc]; 
    else 
        median = (pdeg[(Int)floor(loc)] + pdeg[((Int)floor(loc)+1)]) / 2;
    Int spdeg = std::accumulate(pdeg.begin(), pdeg.end(), 0);
    Int mpdeg = *(std::max_element(pdeg.begin(), pdeg.end()));
    std::transform(pdeg.cbegin(), pdeg.cend(), pdeg.cbegin(),
                   pdeg.begin(), std::multiplies<Int>{});

    Int psum_sq = std::accumulate(pdeg.begin(), pdeg.end(), 0);

    Float paverage = (Float) spdeg / totalVertices_;
    Float pavg_sq  = (Float) psum_sq / totalVertices_;
    Float pvar     = std::abs(pavg_sq - (paverage*paverage));
    Float pstddev  = sqrt(pvar);

    std::cout << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Graph characteristics" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Number of vertices: " << totalVertices_ << std::endl;
    std::cout << "Number of edges: " << totalEdges_ << std::endl;
    std::cout << "Maximum number of edges: " << mpdeg << std::endl;
    std::cout << "Median number of edges: " << median << std::endl;
    std::cout << "Expected value of X^2: " << pavg_sq << std::endl;
    std::cout << "Variance: " << pvar << std::endl;
    std::cout << "Standard deviation: " << pstddev << std::endl;
    std::cout << "--------------------------------------" << std::endl;
}

#if 0
void Partition::destroy_partition()
{
    delete [];
    delete [];
}

void Partition::singleton_partition()
{
    destroy_partition();
        
}

Partition::Partition(const Graph& g): graph(&g), commMap(NULL), 
community(NULL)
{
    singleton_partition();
}

void Partition::set_graph(const Graph& g)
{ 
    graph = &g;
    singleton_partition();
}

long Partition::get_comm_id(const long& i)
{
    return commMap[i];
}

Community* Partition::get_community(const long& i)
{
    return community[i]; 
}
#endif

#include <iostream>
#include <map>
#include "louvain.h"

Float Louvain::move_vertex()
{
    Graph* graph = partition->get_graph();
    Int num_vertices = graph->get_num_vertices();
    Float total_delta = 0.;

    Float *weights = graph->get_weights();
    Float *weighted_orders = graph->get_weighted_orders();
    Int   *orders = graph->get_orders();

    Int max_order = graph->get_max_order();
    Float* ecj = new Float [max_order](0);

    for(Int v = 0; v < num_vertices; ++v)
    {
        Int   *edges   = graph->get_adjacent_vertices(v);
        long num_edges = graph->get_num_adjacent_vertices(v);
        long my_comm_id = get_comm_id(v);
         
        //loop through all neighboring clusters
        long unique_id = 0;
        std::map<Int, Int> neighCommIdMap;

        Float e_ci = 0.;
        Float ki = weighted_orders[v];
          
        for(Int j = 0; j < num_edges; ++j)
        {
           Int u = edges[j];
           Float w_vu = weights[u];

           Int comm_id = graph->get_comm_id(u);
           if(comm_id != my_comm_id)
           {
               std::map<Int,Int>::iterator iter;
               if((iter = neighCommIdMap.find(comm_id)) == neighCommId.end())
               {
                   localCommIdMap.insert({comm_id, unique_id});
                   e_cj[unique_id] = w_vu; //graph->get_weight(vi,j);
                   unique_id++;
               }
               else
                   e_cj[iter->second] += w_vu;
           }
           else if(vi != vj)
               e_ci += w_vu;
        }

        //determine the best move
        Float ac_i = community[my_comm_id]->ac-ki;
        Float delta = 0;
        Int destCommId = my_comm_id;

        for(auto iter = neighCommIdMap.begin(); iter != neighCommIdMap.end(); ++iter)
        {
            Float val = e_cj[iter->second];
            val -= e_ci + ki*(ac_i-community[iter->first]->ac)/(0.5*m);
            val /= m;
            if( val > delta)
            {
                destCommId = iter->first;
                delta = val;
            }
        }
        //move the node
        if(destCommId != my_comm_id)
            partition->move_vertex(v, my_comm_id, destCommId, delta);
    } 
    return total_delta; 
}

void Louvain::run(Partition& partion)
{
    bool done = false;
    do
    {
        long count = 0;
        while(1)
        {
            double delta = move_vertex();
            if(delta < tau)
                break;
            else if(count > maxLoop)
            {
                cout << "do not converge in the inner loop\n";
                break;
            }
        }    
        if(!done)
        {
            partition.aggregate_graph();
            partition.singleton_partition();
        }
    } while (!done);

    show_partition(partition);
}

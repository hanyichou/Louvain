#include <iostream>
#include <map>
#include "louvain.h"

Float Louvain::move_vertex()
{
    Graph* graph = partition_->get_graph();
    Int num_vertices = graph->get_num_vertices();
    Float total_delta = 0.;

    Float *weights = graph->get_weights();
    Float *weighted_orders = graph->get_weighted_orders();
    Int   *orders = graph->get_orders();
    Float* ac = partition_-> get_community_order();

    Int max_order = graph->get_max_order();
    Float* e_cj = new Float [max_order]();
    Float m = partition_->get_mass();
 
    for(Int v = 0; v < num_vertices; ++v)
    {
        Int   *edges   = graph->get_adjacent_vertices(v);
        long num_edges = graph->get_num_adjacent_vertices(v);
        long my_comm_id = partition_->get_comm_id(v);
         
        //loop through all neighboring clusters
        long unique_id = 0;
        std::map<Int, Int> neighCommIdMap;

        Float e_ci = 0.;
        Float ki = weighted_orders[v];
          
        for(Int j = 0; j < num_edges; ++j)
        {
           Int u = edges[j];
           Float w_vu = weights[u];

           Int comm_id = partition_->get_comm_id(u);
           if(comm_id != my_comm_id)
           {
               std::map<Int,Int>::iterator iter;
               if((iter = neighCommIdMap.find(comm_id)) == neighCommIdMap.end())
               {
                   neighCommIdMap.insert({comm_id, unique_id});
                   e_cj[unique_id] = w_vu; //graph->get_weight(vi,j);
                   unique_id++;
               }
               else
                   e_cj[iter->second] += w_vu;
           }
           else if(v != u)
               e_ci += w_vu;
        }

        //determine the best move
        Float ac_i = ac[my_comm_id]-ki;
        Float delta = 0;
        Int destCommId = my_comm_id;

        for(auto iter = neighCommIdMap.begin(); iter != neighCommIdMap.end(); ++iter)
        {
            Float val = e_cj[iter->second];
            val -= (e_ci - ki*(ac_i-ac[iter->first])/(2.*m));
            val /= m;
            if(val > delta)
            {
                destCommId = iter->first;
                delta = val;
            }
        }
        //move the node
        if(destCommId != my_comm_id)
        {
            partition_->move_vertex(v, my_comm_id, destCommId, delta, ki);
            total_delta += delta;
        }
    } 
    delete e_cj;
    return total_delta; 
}

void Louvain::run()
{
    bool done = false;
    std::cout << "Original Q: " << partition_->get_modularity() << std::endl;
    do
    {
        Int count = 0;
        Float delta;
        while(true)
        {
            delta = move_vertex();
            std::cout << "LOOP\t\t\t\tdQ\n";
            std::cout << "---------------------------------"; 
            std::cout << count <<"\t\t\t\t" << delta << std::endl;
            count++;
            if(delta < tau_)
                break;
            else if(count > maxLoop_)
            {
                std::cout << "do not converge in the inner loop\n";
                break;
            }
        }
        //just for now
        //TODO: check the community, done if each community contain one memeber
        if(delta < tau_)
            done = true;    
        if(!done)
        {
            partition_->aggregate_graph();
            //partition.singleton_partition();
        }
    } while (!done);
    std::cout << "Final Q: " << partition_->get_modularity() << std::endl;
    std::cout << "Check Final Q: " << partition_->compute_modularity() << std::endl;    
    //show_partition(partition);
}

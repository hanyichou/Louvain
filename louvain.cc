#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include "louvain.h"
#include "def.h"
#include <omp.h>

#ifndef USE_SHARED_OMP
Float Louvain::random_move_vertex(Mt19937* rand_gen)
{
    Graph* graph = partition_->get_graph();
    Int num_vertices = graph->get_num_vertices();

    Float *w = graph->get_weighted_orders();
    //Float* ac = partition_-> get_community_order();
    //int num_threads = omp_get_num_threads();
    //omp_set_num_threads(num_threads);
    //std::cout << "number of threads = " << num_threads << std::endl;

    int num_threads = NUM_THREADS;
    Move* moves = new Move[num_threads];
    omp_set_num_threads(num_threads);
 
    #pragma omp parallel
    {
        //randomly choose a vertex and an edge,
        //move the vertex to that cluster
        //int num_threads = omp_get_num_threads();
        //omp_set_num_threads(num_threads);
        //std::cout << "number of threads = " << omp_get_num_threads() << std::endl;
        //
        Int my_thread_id = omp_get_thread_num();
        Int num = (Int)(num_vertices/num_threads);
        Int start = my_thread_id*num;
        Int end = start+num;
        if(my_thread_id+1 == num_threads)
            end = num_vertices;

        //Int v = rand() % (end-start)+start;
        std::uniform_int_distribution<Int> dist0(start, end-1);

        Int v = dist0(rand_gen[my_thread_id]);

        Int* edges    = graph->get_adjacent_vertices(v);
        Int num_edges = graph->get_num_adjacent_vertices(v);
        Int my_comm_id = partition_->get_comm_id(v);

        if(num_edges > 0)
        {
            std::uniform_int_distribution<Int> dist1(0, num_edges-1);
            Int e = dist1(rand_gen[my_thread_id]); 
            Int target_comm_id =  partition_->get_comm_id(edges[e]);
            //comm_id[v] = target_id;
            moves[my_thread_id] = {v, my_comm_id, target_comm_id, w[v]};
        }
        else
            moves[my_thread_id] = {v, my_comm_id, my_comm_id, w[v]};
        //#pragma omp critical
        //std::cout << my_thread_id << " " << v << " " << my_comm_id << " " << target_comm_id << std::endl;
        //#pragma omp barrier
    }

    Float Q_old = partition_->get_modularity();
    partition_->move_vertex(moves, num_threads);

    Float Q = partition_->compute_modularity();

    Float dQ = Q-Q_old;
    //std::cout << dQ << std::endl;
    if(dQ < 0.)
    {
        //restore the move
        for(int i = 0; i < num_threads; ++i)
        {
            Move move = moves[i];
            moves[i] = {move.id, move.dest_comm, move.src_comm, move.ac}; 
        }
        partition_->move_vertex(moves, num_threads);
    }
    else
        partition_->update_modularity(Q);

    //std::cout << Q << std::endl;
    delete [] moves;
    return (dQ < 0.) ? 0. : dQ;
}
#endif

#ifdef USE_SHARED_OMP
Float Louvain::move_vertex(const int& num_batches, Int* comm_ids_new, Int* comm_nums_new, Float* ac_new)
{
    Graph* graph = partition_->get_graph();
    Int num_vertices = graph->get_num_vertices();

    Int* comm_ids = partition_->get_community();
    Int* comm_nums = partition_->get_num_vertices_in_communities();

    Float *weighted_orders = graph->get_weighted_orders();
    Float* ac = partition_-> get_community_order();

    Int max_order = graph->get_max_order();

    Float** e_cj = new Float* [NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; ++i) 
        e_cj[i] = new Float [max_order];

    for(int n = 0; n < NUM_THREADS; ++n)
        for(Int i = 0; i < max_order; ++i)
            e_cj[n][i] = 0;

    Float m = partition_->get_mass();

    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel for
    for(Int i = 0; i < num_vertices; ++i)
    {
        ac_new[i] = ac[i];
        comm_ids_new[i] = comm_ids[i];    
        comm_nums_new[i] = comm_nums[i];
    }

    for(int n = 0; n < num_batches; ++n)
    {
        Int start = n*((Int)(num_vertices/num_batches));
        Int end = start+((Int)(num_vertices/num_batches)); 
        end = (end > num_vertices) ? num_vertices : end;

        #pragma omp parallel for
        for(Int v = start; v < end; ++v)
        {
            Int   *edges   = graph->get_adjacent_vertices(v);
            Int num_edges = graph->get_num_adjacent_vertices(v);
            Float *weights = graph->get_adjacent_weights(v);
            Int my_comm_id = comm_ids[v];
            int my_thread_id = omp_get_thread_num();

            //loop through all neighboring clusters
            Int unique_id = 0;
            std::unordered_map<Int, Int> neighCommIdMap;

            Float e_ci = 0.;
            Float ki = weighted_orders[v];
            for(Int j = 0; j < num_edges; ++j)
            {
                Int u = edges[j];
                Float w_vu = weights[j];

                //Int comm_id = partition_->get_comm_id(u);
                Int comm_id = comm_ids[u];
                if(comm_id != my_comm_id)
                {
                    std::unordered_map<Int,Int>::iterator iter;
                    if((iter = neighCommIdMap.find(comm_id)) == neighCommIdMap.end())
                    {
                        neighCommIdMap.insert({comm_id, unique_id});
                        e_cj[my_thread_id][unique_id] = w_vu; //graph->get_weight(vi,j);
                        unique_id++;
                    }
                    else
                        e_cj[my_thread_id][iter->second] += w_vu;
                }
                else if(v != u)
                    e_ci += w_vu;
            }

            //determine the best move
            Float ac_i;
            ac_i = ac[my_comm_id]-ki;

            Float delta = 0;
            Int destCommId = my_comm_id;

            for(auto iter = neighCommIdMap.begin(); iter != neighCommIdMap.end(); ++iter)
            {
                Float val = e_cj[my_thread_id][iter->second];
                val -= (e_ci - ki*(ac_i-ac[iter->first])/(2.*m));
                val /= m;
                if(val > delta)
                {
                    destCommId = iter->first;
                    delta = val;
                }
            }
            if(destCommId != my_comm_id)
            {
                if(comm_nums[destCommId] == 1 and 
                   comm_nums[my_comm_id] == 1)
                {
                    if(my_comm_id > destCommId)
                    {
                        comm_ids_new[v] = destCommId;
                        #pragma omp critical
                        {
                            ac_new[my_comm_id] -= ki;
                            ac_new[destCommId] += ki;
                            comm_nums_new[my_comm_id]--;
                            comm_nums_new[destCommId]++;
                        }
                    }
                }
                else
                {
                    comm_ids_new[v] = destCommId;
                    #pragma omp critical
                    {
                        ac_new[my_comm_id] -= ki;
                        ac_new[destCommId] += ki;
                        comm_nums_new[my_comm_id]--;
                        comm_nums_new[destCommId]++;
                    }
                }
            }
        }

        #pragma omp parallel for
        for(Int i = 0; i < num_vertices; ++i)
        {
            ac[i] = ac_new[i];
            comm_ids[i] = comm_ids_new[i];    
            comm_nums[i] = comm_nums_new[i];
        }
    }
    for(int i = 0; i < NUM_THREADS; ++i) 
        delete e_cj[i];
    delete e_cj;

    Float Q_old = partition_->get_modularity();
    Float Q = partition_->compute_modularity();
    partition_->update_modularity(Q);

    return Q-Q_old; 
}

void Louvain::run(const int& num_batches)
{
    bool done = false;
    std::cout << "Original Q: " << partition_->get_modularity() << std::endl;

    Graph* graph = partition_->get_graph();
    Int num_vertices = graph->get_num_vertices();

    Float* ac = new Float [num_vertices];
    Int* comm_id = new Int [num_vertices];
    Int* comm_num = new Int [num_vertices];

    do
    {
        Int count = 0;
        Float delta;
        while(true)
        {
            delta = move_vertex(num_batches, comm_id, comm_num, ac);
            std::cout << "LOOP\t\tdQ\n";
            std::cout << "-----------------------------\n"; 
            std::cout << count <<"\t\t" << delta << std::endl;
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

    //partition_->show_partition();

    delete [] comm_id;
    delete [] ac;   
    delete [] comm_num;
}
#else
Float Louvain::move_vertex()
{
    Graph* graph = partition_->get_graph();
    Int num_vertices = graph->get_num_vertices();
    Float total_delta = 0.;

    Float *weighted_orders = graph->get_weighted_orders();
    Float* ac = partition_-> get_community_order();

    Int max_order = graph->get_max_order();

    Float* e_cj = new Float [max_order];
    for(Int i = 0; i < max_order; ++i)
        e_cj[i] = 0;

    Float m = partition_->get_mass();
 
    for(Int v = 0; v < num_vertices; ++v)
    {
        Int   *edges   = graph->get_adjacent_vertices(v);
        Int num_edges = graph->get_num_adjacent_vertices(v);
        Float *weights = graph->get_adjacent_weights(v);
        Int my_comm_id = partition_->get_comm_id(v);
         
        //loop through all neighboring clusters
        Int unique_id = 0;
        std::unordered_map<Int, Int> neighCommIdMap;

        Float e_ci = 0.;
        Float ki = weighted_orders[v];
        for(Int j = 0; j < num_edges; ++j)
        {
           Int u = edges[j];
           Float w_vu = weights[j];

           Int comm_id = partition_->get_comm_id(u);
           if(comm_id != my_comm_id)
           {
               std::unordered_map<Int,Int>::iterator iter;
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
            //std::cout << delta << " " << partition_->compute_modularity() << std::endl;
            total_delta += delta;
        }
    } 
    delete e_cj;
    return total_delta; 
}

void Louvain::run(int randomized)
{
    bool done = false;
    std::cout << "Original Q: " << partition_->get_modularity() << std::endl;

    Mt19937 *rand_gen = nullptr;

    if(randomized)
    {
        //int num_threads = omp_get_num_threads();
        int num_threads = 16;
        rand_gen = new Mt19937 [num_threads] ();
        for(int i = 0; i < num_threads; ++i)
            rand_gen[i].seed((1<<(num_threads/4))+num_threads);
    }
    do
    {
        Int count = 0;
        Float delta;
        while(true)
        {
            if(!randomized)
                 delta = move_vertex();
            else
                 delta = random_move_vertex(rand_gen);
            std::cout << "LOOP\t\tdQ\n";
            std::cout << "-----------------------------\n"; 
            std::cout << count <<"\t\t" << delta << std::endl;
            count++;
            if(randomized and delta < tau_)
                randomized = 0;
            else if(delta < tau_)
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

    //partition_->show_partition();

    delete [] rand_gen;
}

#endif

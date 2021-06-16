// ***********************************************************************
//
//                              NEVE
//
// ***********************************************************************
//
//       Copyright (2019) Battelle Memorial Institute
//                      All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// ************************************************************************ 



#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include <cassert>
#include <cstdlib>
#include <cfloat>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <omp.h>

#include "graph.h"

#define NTIMES 50 

int main(int argc, char **argv)
{
    if(argc < 3)
        exit(-1);

    double t0, td, td0, td1;
    Int n;
    int n_threads;
    Graph* g = nullptr;
    if (std::string(argv[1]) == "-r")
    {
        n = (Int)atoi(argv[2]);
        const Int m0 = (Int)atoi(argv[3]);
        n_threads = atoi(argv[4]);

        td0 = omp_get_wtime();
        g = new Graph(n, m0);
    }
    else if (std::string(argv[1]) == "-f")
    {
        n_threads = atoi(argv[3]);
        td0 = omp_get_wtime();
        g = new Graph(std::string(argv[2]));
    }
    assert(g != nullptr);
    g->print_stats();

    td1 = omp_get_wtime();
    td = td1 - td0;

    n = g->get_num_vertices();
    std::cout << "Time to generate graph of " 
              << n << " vertices (in s): " << td << std::endl;

    const Int nv = g->get_num_vertices();
    const Int ne = g->get_num_edges();

    const std::size_t count_nbrscan = 2*nv*sizeof(Int)+4*ne*sizeof(Int);
    const std::size_t count_nbrsum = 2*nv*sizeof(Int) + 2*ne*(sizeof(Float) + 2*sizeof(Float));
    const std::size_t count_nbrmax = 2*nv*sizeof(Int) + nv*sizeof(Float) + 2*ne*(sizeof(Float));

    std::printf("Total memory required (Neighbor Scan) = %.1f KiB = %.1f MiB = %.1f GiB.\n",
        ( (double) (count_nbrscan) / 1024.0),
        ( (double) (count_nbrscan) / 1024.0/1024.0),
        ( (double) (count_nbrscan) / 1024.0/1024.0/1024.0));
    std::printf("Total memory required (Neighbor Sum ) = %.1f KiB = %.1f MiB = %.1f GiB.\n",
        ( (double) (count_nbrsum) / 1024.0),
        ( (double) (count_nbrsum) / 1024.0/1024.0),
        ( (double) (count_nbrsum) / 1024.0/1024.0/1024.0));
    std::printf("Total memory required (Neighbor Max ) = %.1f KiB = %.1f MiB = %.1f GiB.\n",
        ( (double) (count_nbrmax) / 1024.0),
        ( (double) (count_nbrmax) / 1024.0/1024.0),
        ( (double) (count_nbrmax) / 1024.0/1024.0/1024.0));

    std::printf("Each kernel will be executed %d times.\n", NTIMES);
    std::printf(" The *best* time for each kernel (excluding the first iteration)\n");
    std::printf(" will be used to compute the reported bandwidth.\n");

    int quantum;
    if  ( (quantum = omp_get_wtick()) >= 1)
        std::printf("Your clock granularity/precision appears to be "
                "%d microseconds.\n", quantum);
    else 
    {
        std::printf("Your clock granularity appears to be "
                "less than one microsecond.\n");
        quantum = 1;
    }

    t0 = omp_get_wtime();
    g->neigh_scan(n_threads);
    t0 = 1.0E6 * (omp_get_wtime() - t0);
    std::printf("Each test below will take on the order"
        " of %d microseconds.\n", (int) t0);
    std::printf("   (= %d clock ticks)\n", (int) (t0/quantum) );
    std::printf("Increase the size of the graph if this shows that\n");
    std::printf("you are not getting at least 20 clock ticks per test.\n");

    double times[3][NTIMES]; 
    double avgtime[3] = {0}, maxtime[3] = {0}, mintime[3] = {FLT_MAX,FLT_MAX,FLT_MAX};

    for (int k = 0; k < NTIMES; k++)
    {
        g->reset_orders_weights();
        times[0][k] = omp_get_wtime();
        g->neigh_scan(n_threads);
        times[0][k] = omp_get_wtime() - times[0][k];
        times[1][k] = omp_get_wtime();
        g->neigh_scan_weights(n_threads);
        times[1][k] = omp_get_wtime() - times[1][k];
        times[2][k] = omp_get_wtime();
        g->neigh_scan_max_weight(n_threads);
        times[2][k] = omp_get_wtime() - times[2][k];
    }

    for (int k = 1; k < NTIMES; k++) // note -- skip first iteration
    {
        for (int j = 0; j < 3; j++)
        {
            avgtime[j] = avgtime[j] + times[j][k];
            mintime[j] = std::min(mintime[j], times[j][k]);
            maxtime[j] = std::max(maxtime[j], times[j][k]);
        }
    }

    std::string label[3] = {"Neighbor Copy:    ", "Neighbor Add :    ", "Neighbor Max :    "};
    double bytes[3] = { (double)count_nbrscan, (double)count_nbrsum, (double)count_nbrmax };

    printf("Function            Best Rate MB/s  Avg time     Min time     Max time\n");
    for (int j = 0; j < 3; j++) 
    {
        avgtime[j] = avgtime[j]/(double)(NTIMES-1);
        std::printf("%s%12.1f  %12.6f  %11.6f  %11.6f\n", label[j].c_str(),
                1.0E-06 * bytes[j]/mintime[j], avgtime[j], mintime[j],
                maxtime[j]);
    }
    delete g; 
    return 0;
}

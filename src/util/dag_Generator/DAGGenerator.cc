//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "DAGGenerator.h"
#include <algorithm>
#include <numeric>
#include <glob.h>
#include <iomanip>
#include <fstream>
#include <vector>

#include <string>
#include <sstream>
#include <iterator>
#include <iostream>


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_PER_RANK 5 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 10
#define MIN_RANKS 6   /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 10
#define PERCENT 10     /* Chance of having an Edge.  */
#define NOV 20

namespace fogstream {

DAGGenerator::DAGGenerator() {
    this->graph.clear();

    this->generateDAG();
}

DAGGenerator::~DAGGenerator() {
    // TODO Auto-generated destructor stub
}

void DAGGenerator::generateDAG(){
    int i, j, k,nodes = 0;
    srand (time (NULL));

    int ranks = MIN_RANKS
            + (rand () % (MAX_RANKS - MIN_RANKS + 1));

    for (i = 0; i < ranks; i++){
        /* New nodes of 'higher' rank than all nodes generated till now.  */
        int new_nodes = MIN_PER_RANK
                + (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

        /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
        for (j = 0; j < nodes; j++)
            for (k = 0; k < new_nodes; k++)
                if ( (rand () % 100) < PERCENT)
                    this->graph.push_back({j,k + nodes});
        //EV_INFO << j << " -> " << k + nodes  << ";" << endl; /* An Edge.  */

        nodes += new_nodes; /* Accumulate into old node set.  */
    }

    sort(this->graph.begin(), this->graph.end(),
            []( const dag &left, const dag &right )
            { return ( left.from < right.from); });

    vector<int> sources;
    int last = -1;
    for (auto from = this->graph.begin(); from != this->graph.end(); ++from){
        if (last != from->from){

            bool bSrc = true;
            for (auto to = this->graph.begin(); to != this->graph.end(); ++to){
                if (from->from == to->to){
                    bSrc = false;
                    break;
                }
            }
            if (bSrc){
                sources.push_back(from->from);
            }
        }
        last = from->from;
    }


    sort(this->graph.begin(), this->graph.end(),
            []( const dag &left, const dag &right )
            { return ( left.to < right.to); });
    vector<int> sinks;
    last = -1;
    for (auto to = this->graph.begin(); to != this->graph.end(); ++to){
        if (last != to->to){
            bool bSnk = true;
            for (auto from = this->graph.begin(); from != this->graph.end(); ++from){
                if (from->from == to->to){
                    bSnk = false;
                    break;
                }
            }
            if (bSnk){
                sinks.push_back(to->to);
            }
        }
        last = to->to;
    }

    EV_INFO << "Sources: " << sources.size() << " Sinks: " << sinks.size() << endl;

    for (auto dg = this->graph.begin(); dg != this->graph.end(); ++dg){
        EV_INFO << dg->from << " -> " << dg->to << ";" << endl;
    }



}
/*
void DAGGenerator::generateDAG2(int e){
    int i, j, edge[e][2], count;

    bool check[21];

    // Build a connection between two random vertex.
    i = 0;
    while(i < e){
        edge[i][0] = intrand(NOV+1);
        edge[i][1] = intrand(NOV+1);

        for(j = 1; j <= 20; j++)check[j] = false;

        if(CheckAcyclic(edge, i, check, edge[i][0]) == true)
            i++;
    }

    // Print the random graph.
    EV_INFO << "The generated random random graph is: " << endl;
    for(i = 0; i < NOV; i++){
        count = 0;
        EV_INFO << i+1 << "-> { ";
        for(j = 0; j < e; j++){
            if(edge[j][0] == i+1){
                EV_INFO << edge[j][1] << "  ";
                count++;
            }else if(edge[j][1] == i+1){
                count++;
            }else if(j == e-1 && count == 0)
                EV_INFO << "Isolated Vertex!";
        }
        EV_INFO << " }" << endl;
    }

    // Print the random graph.
        EV_INFO << "The generated random random graph is 1: " << endl;
        for(i = 0; i < NOV; i++){
            for(j = 0; j < e; j++){
                EV_INFO <<"[ "<< i+1 << "," << edge[j][1] <<" ]" << endl;
            }
        }
}

bool DAGGenerator::CheckAcyclic(int edge[][2], int ed, bool check[], int v){
    int i;
    bool value;
    // If the current vertex is visited already, then the graph contains cycle.
    if(check[v] == true){
        return false;
    }else{
        check[v] = true;
        // For each vertex, go for all the vertex connected to it.
        for(i = ed; i >= 0; i--){
            if(edge[i][0] == v){
                return CheckAcyclic(edge, ed, check, edge[i][1]);
            }
        }
    }
    // In case, if the path ends then reassign the vertexes visited in that path to false again.
    check[v] = false;

    if(i == 0)
        return true;
}
 */
} /* namespace fogstream */

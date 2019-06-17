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

#ifndef UTIL_DAG_GENERATOR_DAGGENERATOR_H_
#define UTIL_DAG_GENERATOR_DAGGENERATOR_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {
struct dag{
    int from;
    int to;
};

class DAGGenerator {
protected:
    vector<dag> graph;

public:
    DAGGenerator();
    virtual ~DAGGenerator();
    void generateDAG();
    //void generateDAG2(int e);
    //bool CheckAcyclic(int edge[][2], int ed, bool check[], int v);
};

} /* namespace fogstream */

#endif /* UTIL_DAG_GENERATOR_DAGGENERATOR_H_ */

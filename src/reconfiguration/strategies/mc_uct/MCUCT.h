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

#ifndef RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_
#define RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_

#include <omnetpp.h>

#include "../../mc_tree/MCTree.h"
#include "../../mcts/MCTS.h"

using namespace std;
using namespace omnetpp;
namespace fogstream {

class MC_UCT: public MCTS {
public:
    MC_UCT(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~MC_UCT();

    int TreePolicy();
    void DefaultPolicy(int newNode, double reward,
            vector<Episode*> episodes);
    int ActionPolicy(State*& state, int nodeId);
    void UpdateValues(int nodeId, Episode*& episode,
            Episode* lastEpisode);
//    double EstimateUCT(int node, int newNode);

    double getConstant() const;
    void setConstant(double constant);

protected:
    double mConstant = .9;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_ */

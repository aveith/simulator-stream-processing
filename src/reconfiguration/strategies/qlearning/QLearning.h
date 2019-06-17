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

#ifndef RECONFIGURATION_STRATEGIES_QLEARNING_QLEARNING_H_
#define RECONFIGURATION_STRATEGIES_QLEARNING_QLEARNING_H_

#include <omnetpp.h>

#include "../../mc_tree/MCTree.h"
#include "../../mcts/MCTS.h"

using namespace std;
using namespace omnetpp;
namespace fogstream {

class QLearning: public MCTS {
public:
    QLearning(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~QLearning();

    void DefaultPolicy(int newNode, double reward,
            vector<Episode*> episodes);
    int ActionPolicy(State*& state, int nodeId);
    void UpdateValues(int nodeId, Episode*& episode,
            Episode* lastEpisode);
    double getAlpha() const;
    void setAlpha(double alpha);
    double getGamma() const;
    void setGamma(double gamma);
    double getEpsilon() const;
    void setEpsilon(double epsilon);

protected:
    double mAlpha;
    double mGamma;
    double mEpsilon;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_ */

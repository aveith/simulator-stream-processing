#ifndef RECONFIGURATION_STRATEGIES_MC_EPSILONGREEDY_MCEPSILONGREEDY_H_
#define RECONFIGURATION_STRATEGIES_MC_EPSILONGREEDY_MCEPSILONGREEDY_H_
#include <omnetpp.h>

#include "../../mc_tree/MCTree.h"
#include "../../mcts/MCTS.h"
#include "../mc_basicucb/MC_BasicUCB.h"

using namespace std;
using namespace omnetpp;
namespace fogstream {

class MC_EpsilonGreedy: public MC_BasicUCB {
public:
    MC_EpsilonGreedy();
    MC_EpsilonGreedy(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~MC_EpsilonGreedy();

//    void DefaultPolicy(int newNode, double reward,
//            vector<Episode*> episodes);
    int ActionPolicy(State*& state, int nodeId) override;
//    virtual void UpdateValues(int nodeId, Episode*& episode,
//            Episode* lastEpisode);

    double getEpsilon() const;
    void setEpsilon(double epsilon);
//    double getConstant() const;
//    void setConstant(double constant);

private:
//    Node*& getNode(int position, int hostId, int parent);
    double mEpsilon;
//    double mConstant = .9;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_ */

#ifndef RECONFIGURATION_STRATEGIES_MC_BASICUCB_MCBASICUCB_H_
#define RECONFIGURATION_STRATEGIES_MC_BASICUCB_MCBASICUCB_H_

#include <omnetpp.h>

#include "../../mc_tree/MCTree.h"
#include "../../mcts/MCTS.h"

using namespace std;
using namespace omnetpp;
namespace fogstream {

class MC_BasicUCB: public MCTS {
public:
    MC_BasicUCB();
    MC_BasicUCB(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~MC_BasicUCB();

    void DefaultPolicy(int newNode, double reward,
            vector<Episode*> episodes);
    int ActionPolicy(State*& state, int nodeId);
    virtual void UpdateValues(int nodeId, Episode*& episode,
            Episode* lastEpisode);

    double getConstant() const;
    void setConstant(double constant);

private:
//    Node*& getNode(int position, int hostId, int parent);
    double mConstant = .9;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_ */

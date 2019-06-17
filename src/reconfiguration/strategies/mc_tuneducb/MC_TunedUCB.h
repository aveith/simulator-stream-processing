#ifndef RECONFIGURATION_STRATEGIES_MC_TUNEDUCB_MCTUNEDUCB_H_
#define RECONFIGURATION_STRATEGIES_MC_TUNEDUCB_MCTUNEDCUCB_H_

#include <omnetpp.h>

#include "../../mc_tree/MCTree.h"
#include "../../mcts/MCTS.h"
#include "../mc_basicucb/MC_BasicUCB.h"

using namespace std;
using namespace omnetpp;
namespace fogstream {

class MC_TunedUCB: public MC_BasicUCB {
public:
    MC_TunedUCB();
    MC_TunedUCB(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~MC_TunedUCB();
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_ */

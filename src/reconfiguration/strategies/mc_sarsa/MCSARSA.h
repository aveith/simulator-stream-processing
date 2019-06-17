#ifndef RECONFIGURATION_STRATEGIES_MC_SARSA_MCSARSA_H_
#define RECONFIGURATION_STRATEGIES_MC_SARSA_MCSARSA_H_

#include <omnetpp.h>

#include "../../mc_tree/MCTree.h"
#include "../../mcts/MCTS.h"
#include "../mc_basicucb/MC_BasicUCB.h"

using namespace std;
using namespace omnetpp;
namespace fogstream {

class MC_SARSA: public MC_BasicUCB {
public:
    MC_SARSA(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~MC_SARSA();

    void DefaultPolicy(int newNode, double reward, vector<Episode*> episodes) override;
//    int ActionPolicy(State*& state, int nodeId);
//    void UpdateValues(int nodeId, Episode*& episode, Episode* lastEpisode);

    double getAlpha() const;
    void setAlpha(double alpha);
//    double getConstant() const;
//    void setConstant(double constant);
    double getGamma() const;
    void setGamma(double gamma);
    double getLambda() const;
    void setLambda(double lambda);

private:
//    Node*& getNode(int position, int hostId, int parent);


protected:
//    double mConstant =  1;
    double mAlpha = 1;
    double mGamma = 1;
    double mLambda = 1;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STRATEGIES_MC_UCT_MCUCT_H_ */

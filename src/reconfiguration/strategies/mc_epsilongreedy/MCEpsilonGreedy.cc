#include "MCEpsilonGreedy.h"

namespace fogstream {

MC_EpsilonGreedy::MC_EpsilonGreedy(cRNG* rng, Environment* env,
        LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setIterateToTerminal(true);
    this->setLogMcts(logMcts);

    this->setConstant(logMcts->getParameters()->getReconfigConstant());
    this->setEpsilon(logMcts->getParameters()->getReconfigEpsilon());
}

MC_EpsilonGreedy::~MC_EpsilonGreedy() {

}

int MC_EpsilonGreedy::ActionPolicy(State*& state, int nodeId) {
    if (nodeId > -1) {
        if (this->getRng()->doubleRand() < this->getEpsilon()) {
            return this->ActionBasicUCB1(this->getConstant(), nodeId,
                    state->getAvailableActions());

        } else {
            int iRand = this->getRng()->intRand(
                    state->getAvailableActions().size());
            return state->getAvailableActions().at(iRand).getHostId();

        }
    } else {
        int iRand = this->getRng()->intRand(
                state->getAvailableActions().size());
        return state->getAvailableActions().at(iRand).getHostId();
    }

}

double MC_EpsilonGreedy::getEpsilon() const {
    return mEpsilon;
}

void MC_EpsilonGreedy::setEpsilon(double epsilon) {
    mEpsilon = epsilon;
}

} /* namespace fogstream */

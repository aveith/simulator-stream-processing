#include "MC_TunedUCB.h"

namespace fogstream {
MC_TunedUCB::MC_TunedUCB(){

}

MC_TunedUCB::MC_TunedUCB(cRNG* rng, Environment* env, LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setIterateToTerminal(true);
    this->setFirstVisit(true);
    this->setLogMcts(logMcts);

    this->setConstant(logMcts->getParameters()->getReconfigConstant());
}

MC_TunedUCB::~MC_TunedUCB() {

}

} /* namespace fogstream */

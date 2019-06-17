#include "MCSARSA.h"

namespace fogstream {

MC_SARSA::MC_SARSA(cRNG* rng, Environment* env, LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setIterateToTerminal(true);
    this->setLogMcts(logMcts);

    this->setConstant(logMcts->getParameters()->getReconfigConstant());
    this->setAlpha(logMcts->getParameters()->getReconfigAlpha());
    this->setGamma(logMcts->getParameters()->getReconfigGamma());
    this->setLambda(logMcts->getParameters()->getReconfigLambda());
    this->setUseRealQValues(true);
}

MC_SARSA::~MC_SARSA() {

}

void MC_SARSA::DefaultPolicy(int newNode, double reward,
        vector<Episode*> episodes) {
    double delta_sum = 0;
    double v_next = 0;
    double v_current = 0;

    for (int i = episodes.size() - 1; i >= 0; --i) {

        if (episodes.at(i)->getId() > -1) {
            v_current =
                    this->getMcTree()->getTree().at(episodes.at(i)->getId())->getQValue();

            //Update current available actions to the node
            if (this->getMcTree()->getTree().at(episodes.at(i)->getId())->getState()->getAvailableActions().size()
                    <= 0
                    && !this->getMcTree()->getTree().at(episodes.at(i)->getId())->isRoot()) {
                this->getMcTree()->RemoveAction(
                        this->getMcTree()->getTree().at(episodes.at(i)->getId())->getParentId(),
                        this->getMcTree()->getTree().at(episodes.at(i)->getId())->getActionTaken().getHostId());
            }
        } else {
//            v_current = reward;
            v_current = episodes.at(i)->getCumulativeReward();
        }

//        double delta = reward + this->getGamma() * v_next - v_current;
        double delta = episodes.at(i)->getCumulativeReward() + this->getGamma() * v_next - v_current;

        delta_sum = this->getLambda() * this->getGamma() * delta_sum + delta;

        if (episodes.at(i)->getId() > -1) {
                this->getMcTree()->getTree().at(episodes.at(i)->getId())->setVisits(
                        this->getMcTree()->getTree().at(episodes.at(i)->getId())->getVisits()
                                + 1);

            this->setAlpha(
                    1
                            / (double) this->getMcTree()->getTree().at(
                                    episodes.at(i)->getId())->getVisits());
            v_current =
                    this->getMcTree()->getTree().at(episodes.at(i)->getId())->getQValue()
                            + (this->getAlpha() * delta_sum);

            this->getMcTree()->getTree().at(episodes.at(i)->getId())->setQValue(
                    v_current);

            //            cout << " Current: " << SIMTIME_DBL(v_current) << endl;
        }

        v_next = v_current;
    }

    this->getMcTree()->getTree().at(0)->setVisits(
            this->getMcTree()->getTree().at(0)->getVisits() + 1);

}

double MC_SARSA::getAlpha() const {
    return mAlpha;
}

void MC_SARSA::setAlpha(double alpha) {
    mAlpha = alpha;
}

double MC_SARSA::getGamma() const {
    return mGamma;
}

void MC_SARSA::setGamma(double gamma) {
    mGamma = gamma;
}

double MC_SARSA::getLambda() const {
    return mLambda;
}

void MC_SARSA::setLambda(double lambda) {
    mLambda = lambda;
}

} /* namespace fogstream */


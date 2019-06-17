#include "MC_BasicUCB.h"

namespace fogstream {

MC_BasicUCB::MC_BasicUCB(cRNG* rng, Environment* env, LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setIterateToTerminal(true);
    this->setLogMcts(logMcts);
    this->setConstant(logMcts->getParameters()->getReconfigConstant());
}

MC_BasicUCB::MC_BasicUCB() {

}

MC_BasicUCB::~MC_BasicUCB() {

}

void MC_BasicUCB::DefaultPolicy(int newNode, double reward,
        vector<Episode*> episodes) {

    for (int i = episodes.size() - 1; i >= 0; --i) {
        if (episodes.at(i)->getId() > -1) {

            this->getMcTree()->getTree().at(episodes.at(i)->getId())->setVisits(
                    this->getMcTree()->getTree().at(episodes.at(i)->getId())->getVisits()
                            + 1);

            double qValue = this->getMcTree()->getTree().at(
                    episodes.at(i)->getId())->getQValue() + reward;
            this->getMcTree()->getTree().at(episodes.at(i)->getId())->setQValue(
                    qValue);

            if (this->getMcTree()->getTree().at(episodes.at(i)->getId())->getState()->getAvailableActions().size()
                    <= 0
                    && !this->getMcTree()->getTree().at(episodes.at(i)->getId())->isRoot()) {
                this->getMcTree()->RemoveAction(
                        this->getMcTree()->getTree().at(episodes.at(i)->getId())->getParentId(),
                        this->getMcTree()->getTree().at(episodes.at(i)->getId())->getActionTaken().getHostId());
            }
        }
    }

    this->getMcTree()->getTree().at(0)->setVisits(
            this->getMcTree()->getTree().at(0)->getVisits() + 1);

}

int MC_BasicUCB::ActionPolicy(State*& state, int nodeId) {

    if (nodeId > -1) {
        return this->ActionBasicUCB1(this->getConstant(), nodeId,
                state->getAvailableActions());

    } else {
        int iRand = this->getRng()->intRand(
                state->getAvailableActions().size());
        return state->getAvailableActions().at(iRand).getHostId();
    }

}

void MC_BasicUCB::UpdateValues(int nodeId, Episode*& episode,
        Episode* lastEpisode) {
    if (this->getLogMcts()->getParameters()->isAggregateCostBasedReward()) {
        episode->setCumulativeReward(
                this->CalcAggCostBasedReward(lastEpisode->getAggregateCost(),
                        this->getMcTree()->getTree().at(0)->getAggregateCost(),
                        lastEpisode->getConstraints().size() > 0));
    } else {
        episode->setCumulativeReward(
                this->CalcLatencyBasedReward(
                        lastEpisode->getState()->getTotalTimePaths(),
                        this->getMcTree()->getTree().at(0)->getState()->getTotalTimePaths(),
                        lastEpisode->getConstraints().size() > 0));
    }

    if (nodeId > -1) {
        this->getMcTree()->getTree().at(nodeId)->setCumulativeReward(
                episode->getCumulativeReward());

        //Remove action from parent node
        if (this->getMcTree()->getTree().at(nodeId)->isConstrained()
                || this->getMcTree()->getTree().at(nodeId)->isTerminal()) {
            //Clear the available actions to the current node
            this->getMcTree()->getTree().at(nodeId)->getState()->getAvailableActions().clear();

            //Remove the current option from the parent node
            this->getMcTree()->RemoveAction(
                    this->getMcTree()->getTree().at(nodeId)->getParentId(),
                    this->getMcTree()->getTree().at(nodeId)->getActionTaken().getHostId());
        }
    }
}

double MC_BasicUCB::getConstant() const {
    return mConstant;
}

void MC_BasicUCB::setConstant(double constant) {
    mConstant = constant;
}

} /* namespace fogstream */

#include "QLearning.h"

namespace fogstream {

QLearning::QLearning(cRNG* rng, Environment* env, LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setCreateAll(true);
    this->setLogMcts(logMcts);
    this->setIterateToTerminal(true);

    this->setAlpha(logMcts->getParameters()->getReconfigAlpha());
    this->setGamma(logMcts->getParameters()->getReconfigGamma());
}

QLearning::~QLearning() {

}

void QLearning::DefaultPolicy(int newNode, double reward,
        vector<Episode*> episodes) {
    for (int i = episodes.size() - 1; i >= 0; --i) {
        //Update current available actions to the node
        if (this->getMcTree()->getTree().at(episodes.at(i)->getId())->getState()->getAvailableActions().size()
                <= 0
                && !this->getMcTree()->getTree().at(episodes.at(i)->getId())->isRoot()) {
            this->getMcTree()->RemoveAction(
                    this->getMcTree()->getTree().at(episodes.at(i)->getId())->getParentId(),
                    this->getMcTree()->getTree().at(episodes.at(i)->getId())->getActionTaken().getHostId());
        }
    }
}

int QLearning::ActionPolicy(State*& state, int nodeId) {
    bool bRandom = false;

    if (this->getRng()->doubleRand() < this->getEpsilon()) {
        bRandom = true;
    } else {
        vector<Node*> tree;

        //        copy_if(this->getMcTree()->getTree().begin(),
        //                this->getMcTree()->getTree().end(), std::back_inserter(tree),
        //                [&](Node & item) {return !item.isConstrained()
        //                    && item.getParentId() == nodeId && item.getActionTaken()->getHostId() > -1
        //                    && item.getState()->getAvailableActions().size() > 0;});

        for (auto it = this->getMcTree()->getTree().begin();
                it != this->getMcTree()->getTree().end(); ++it) {
            if (!it->second->isConstrained()
                    && it->second->getParentId() == nodeId
                    && it->second->getActionTaken().getHostId() > -1
                    && it->second->getState()->getAvailableActions().size()
                            > 0) {
                tree.push_back(it->second);
            }
        }

        if (tree.size() > 0) {
            sort(tree.begin(), tree.end(), []( Node* &left, Node* &right )
            {   return ( left->getQValue() > right->getQValue());});

            vector<int> hosts;
            for (unsigned int it = 0; it < tree.size(); it++) {
                if (tree.at(it)->getQValue() == tree.at(0)->getQValue()) {
                    hosts.push_back(tree.at(it)->getActionTaken().getHostId());
                }
            }

            int iRand = this->getRng()->intRand(hosts.size());
            return hosts.at(iRand);
        } else {
            bRandom = true;
        }
    }

    if (bRandom) {
        int iRand = this->getRng()->intRand(
                state->getAvailableActions().size());
        return state->getAvailableActions().at(iRand).getHostId();
    }

    throw cRuntimeError(
            "Something wrong happen to take an action at Q-Learning policy");
}

void QLearning::UpdateValues(int nodeId, Episode*& episode,
        Episode* lastEpisode) {
    vector<Node*> tree;
    double maxQ = 0;

    //    copy_if(this->getMcTree()->getTree().begin(),
    //            this->getMcTree()->getTree().end(), std::back_inserter(tree),
    //            [&](Node & item) {return !item.isConstrained() && item.getParentId() == nodeId;});

    for (auto it = this->getMcTree()->getTree().begin();
            it != this->getMcTree()->getTree().end(); ++it) {
        if (!it->second->isConstrained()
                && it->second->getParentId() == nodeId) {
            tree.push_back(it->second);
        }
    }

    if (tree.size() > 0) {
        sort(tree.begin(), tree.end(), []( Node* &left, Node* &right )
        {   return ( left->getQValue() > right->getQValue());});
        maxQ = tree.at(0)->getQValue();
    }
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

    this->getMcTree()->getTree().at(nodeId)->setCumulativeReward(
            episode->getCumulativeReward());

    this->getMcTree()->getTree().at(nodeId)->setQValue(
            this->getMcTree()->getTree().at(nodeId)->getQValue()
                    + this->getAlpha()
                            * (episode->getCumulativeReward()
                                    + this->getGamma() * maxQ
                                    - this->getMcTree()->getTree().at(nodeId)->getQValue()));

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

double QLearning::getAlpha() const {
    return mAlpha;
}

void QLearning::setAlpha(double alpha) {
    mAlpha = alpha;
}

double QLearning::getGamma() const {
    return mGamma;
}

void QLearning::setGamma(double gamma) {
    mGamma = gamma;
}

double QLearning::getEpsilon() const {
    return mEpsilon;
}

void QLearning::setEpsilon(double epsilon) {
    mEpsilon = epsilon;
}

} /* namespace fogstream */

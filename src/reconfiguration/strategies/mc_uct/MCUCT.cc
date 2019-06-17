#include "MCUCT.h"

namespace fogstream {

MC_UCT::MC_UCT(cRNG* rng, Environment* env, LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setIterateToTerminal(false);
    this->setLogMcts(logMcts);
    this->setConstant(logMcts->getParameters()->getReconfigConstant());
}

MC_UCT::~MC_UCT() {

}

int MC_UCT::TreePolicy() {
    vector<Node*> tree;

//    copy_if(this->getMcTree()->getTree().begin(),
//            this->getMcTree()->getTree().end(), std::back_inserter(tree),
//            [&](Node & item) {return !item.isConstrained() && item.getState()->getAvailableActions().size()>0;});
//
    for (auto it = this->getMcTree()->getTree().begin(); it != this->getMcTree()->getTree().end(); ++it){
        if (!it->second->isConstrained() && it->second->getState()->getAvailableActions().size()>0){
            tree.push_back(it->second);
        }
    }


    if (tree.size() > 0) {
        vector<Node*> tree_filtered;
        for (unsigned int it =0; it < tree.size(); ++it){
            bool bAvailable = false;
            for (unsigned int i = 0;
                    i < tree.at(it)->getState()->getAvailableActions().size(); ++i) {
                if (!tree.at(it)->getState()->getAvailableActions().at(i).isTaken()) {
                    bAvailable = true;
                    break;
                }
            }
            if (bAvailable) {
                tree_filtered.push_back(tree.at(it));
            }
        }

        if (tree_filtered.size() > 0) {
            sort(tree_filtered.begin(), tree_filtered.end(),
                    []( Node* &left, Node* &right )
                    {   return ( left->getUctValue() > right->getUctValue());});

            return tree_filtered.at(0)->getId();
        } else {
            return -1;
        }
    } else {
        return -1;
    }

}

void MC_UCT::DefaultPolicy(int newNode, double reward,
        vector<Episode*> episodes) {
    this->getMcTree()->getTree().at(newNode)->setVisits(
            this->getMcTree()->getTree().at(newNode)->getVisits() + 1);

    double qValue = this->getMcTree()->getTree().at(newNode)->getQValue() + reward;
    this->getMcTree()->getTree().at(newNode)->setQValue(qValue);

    this->getMcTree()->getTree().at(newNode)->setUctValue(
            this->CalcUCB1(this->getConstant(),
                    this->getMcTree()->getTree().at(newNode)->getQValue(),
                    this->getMcTree()->getTree().at(newNode)->getVisits(),
                    this->getMcTree()->getTree().at(
                                                this->getMcTree()->getTree().at(newNode)->getParentId())->getVisits()));

    if (this->getMcTree()->getTree().at(newNode)->getParentId() != newNode) {
        this->DefaultPolicy(this->getMcTree()->getTree().at(newNode)->getParentId(),
                reward, episodes);
    }

}

int MC_UCT::ActionPolicy(State*& state, int nodeId) {
    vector<Action> vFiltered;
    copy_if(state->getAvailableActions().begin(),
            state->getAvailableActions().end(), std::back_inserter(vFiltered),
            [&](Action &act) {return act.isTaken() == false;});
    if (vFiltered.size() > 0) {
        int iRand = this->getRng()->intRand(vFiltered.size());
        return vFiltered.at(iRand).getHostId();
    }

    throw cRuntimeError(
            "Something wrong happen to take an action at MC-UCT policy");
}

void MC_UCT::UpdateValues(int nodeId, Episode*& episode, Episode* lastEpisode) {
    //Remove action due to the global view of the algorithm
    this->getMcTree()->RemoveAction(
            this->getMcTree()->getTree().at(nodeId)->getParentId(),
            this->getMcTree()->getTree().at(nodeId)->getActionTaken().getHostId());

//    this->getMcTree()->getTree().at(nodeId)->setQValue(
//            episode->getCumulativeReward());
//
//    this->getMcTree()->getTree().at(nodeId)->setUctValue(
//            this->getMcTree()->getTree().at(nodeId)->getCumulativeReward());

}

double MC_UCT::getConstant() const {
    return mConstant;
}

void MC_UCT::setConstant(double constant) {
    mConstant = constant;
}

} /* namespace fogstream */


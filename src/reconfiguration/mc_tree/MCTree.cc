#include "MCTree.h"

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <cmath>

#include "../../env/objects/networkconnection/NetworkConnection.h"

namespace fogstream {

MC_Tree::MC_Tree(Environment* env) {
    this->getTree().clear();
    this->setEnv(env);

}

MC_Tree::~MC_Tree() {
    // TODO Auto-generated destructor stub
}

//vector<Node>& MC_Tree::getTree() {
//    return mTree;
//}

unordered_map<int, Node*> & MC_Tree::getTree() {
    return mTree;
}

int MC_Tree::getNewNodeId() {
    this->setNodeId(this->getNodeId() + 1);
    return this->getNodeId();
}

int MC_Tree::getNewStateID() {
    this->setStateId(this->getStateId() + 1);
    return this->getStateId();
}

//void MC_Tree::setTree(const vector<Node>& tree) {
//    mTree = tree;
//}

void MC_Tree::setTree(const unordered_map<int, Node*> & tree) {
    mTree = tree;
}

//const vector<Node> MC_Tree::getUntriedNodes() {
//    //Get nodes with available actions
//    vector<Node> available_nodes;
//
//    for (auto node = this->getTree().begin(); node != this->getTree().end();
//            ++node) {
//        if (!node->isTerminal()
//                && node->getState()->getAvailableActions().size() > 0
//                && !node->isConstrained()) {
//            available_nodes.push_back(*node);
//
//        }
//    }
//
//    return available_nodes;
//}

//const vector<Node*> MC_Tree::getUntriedNodes() {
//    //Get nodes with available actions
//    vector<Node*> available_nodes;
//
//    for (auto node = this->getTree().begin(); node != this->getTree().end();
//            ++node) {
//        if (!node->second->isTerminal()
//                && node->second->getState()->getAvailableActions().size() > 0
//                && !node->second->isConstrained()) {
//            available_nodes.push_back(node->second);
//
//        }
//    }
//
//    return available_nodes;
//}

//Node& MC_Tree::getNode(int id) {
//    auto it = std::find_if(this->getTree().begin(), this->getTree().end(),
//            [&](Node &node) {return node.getId() == id;});
//
//    if (it != this->getTree().end()) {
//        return *it;
//    }else{
//        throw cRuntimeError("Node does not exist - %d", id);
//    }
//
//}

//Node*& MC_Tree::getNode(int id) {
//
//    auto it = this->getTree().find(id);
//    if (it != this->getTree().end()) {
//        return it->second;
//
//    } else {
//        throw cRuntimeError("Node does not exist - %d", id);
//    }
//
//}

int MC_Tree::addChild(Action action, Node parentNode, bool sourceSink,
        bool terminal) {
    //Fill out the basic fields of the new node
    int id = this->getNewNodeId();

    //Main method onto simulation phase, the algorithm will evaluate the new operator placement and
    //estimate the computation and communication overheads. Also, the algorithm will fill out some
    //field to the node if it does not support (memory, cpu and bandwidth) the execution.
    this->getTree().insert(make_pair(id, new Node(id, parentNode.getId())));

    this->getTree().at(id)->setState(new State(parentNode.getState()));

    this->getTree().at(id)->getState()->getAvailableActions().clear();

    this->getTree().at(id)->setActionTaken(
            Action(action.getHostId(), action.getOriginHostId()));

    this->getTree().at(id)->getState()->setId(this->getNewStateID());

    //Increase the position, it refers to the episode
    //    new_node.getState()->setPosition(parentNode.getState()->getPosition() + 1);

    //Removed the position and changed to LevelMCTS
    // levelMCTS contains the operator unless the position as earlier developed
    this->getTree().at(id)->getState()->setLevelMcts(
            this->getEnv()->GetNextOperatorDeployment(
                    parentNode.getState()->getLevelMcts()));

    //Determine if the operator is a source or a sink. Sinks and sources don't have their hosts changed
    this->getTree().at(id)->setSourceSink(sourceSink);
    //Set if the operator is the last operator of the operator deployment sequence
    //    new_node.isTerminal = new_node.current_state.position
    //            == (int) (this->mOrderOperatorList.size() - 1);
    this->getTree().at(id)->setTerminal(terminal);

    //    this->getTree().push_back(new_node);

    return id;
}

void MC_Tree::addRoot(bool useSlots, int actionHeuristic) {
    //Get ID on the MCTS
    int id = this->getNewNodeId();

    this->getTree().insert(make_pair(id, new Node(id, id)));

    this->getTree().at(id)->setState(new State());
    this->getTree().at(id)->getState()->setId(this->getNewStateID());
    this->getTree().at(id)->getState()->setMap(Mapping());

    //Determine the real and estimate statistics
    this->getTree().at(id)->getState()->getMap().setOperatorMappings(
            vector<MapOperator>());

    this->defineIntialOperatorMapping(
            &this->getTree().at(id)->getState()->getMap().getOperatorMappings());

    this->getTree().at(id)->getState()->setAvailableActions(
            this->getEnv()->getActions(actionHeuristic,
                    this->getTree().at(id)->getState()->getMap(), true,
                    this->getEnv()->GetNextOperatorDeployment(
                            numeric_limits<int>::min())));

    this->getTree().at(id)->getState()->setLevelMcts(
            numeric_limits<int>::min());

    //Set the edge mappings based on the previous operator mapping
    this->getTree().at(id)->getState()->getMap().setEdgeMappings(
            vector<MapEdge>());
    this->defineIntialEdgeMapping(
            &this->getTree().at(id)->getState()->getMap().getEdgeMappings());

    //Based on the total capabilities of the resources, determine the residual capabilities
    // by reducing the current mapping
    this->getTree().at(id)->getState()->getMap().setResidualHostCapabilities(
            unordered_map<int, HostAvailability>());

    this->defineInitialHostCapabilities(
            &this->getTree().at(id)->getState()->getMap().getResidualHostCapabilities(),
            this->getTree().at(id)->getState()->getMap().getOperatorMappings(),
            useSlots);

    //Estimate the residual link caplabilities from the static deployment
    this->getTree().at(id)->getState()->getMap().setResidualLinkCapabilities(
            unordered_map<int, LinkAvailability>());

    this->defineInitialLinkCapabilities(
            &this->getTree().at(id)->getState()->getMap().getResidualLinkCapabilities(),
            this->getTree().at(id)->getState()->getMap().getEdgeMappings(),
            this->getTree().at(id)->getState()->getMap().getOperatorMappings());

    //Get the real path times based on the statistics
    if (!this->getEnv()->isReadTraceFile()) {
        this->getTree().at(id)->getState()->getMap().setRealPathTimes(
                vector<double>());
        for (unsigned int var = 0;
                var
                        < this->getEnv()->getGeneralEnv()->getApplicationPaths().size();
                ++var) {
            this->getTree().at(id)->getState()->getMap().getRealPathTimes().push_back(
                    SIMTIME_DBL(
                            this->getEnv()->getHistoryData()->getPathTime(var,
                                    simTime()
                                            - Patterns::TIME_STATISTIC_EVALUATION,
                                    simTime())));
            this->getTree().at(id)->getState()->getMap().getRealPathTimes().push_back(
                    this->getEnv()->getHistoryData()->getPathCommTime(var,
                            simTime() - Patterns::TIME_STATISTIC_EVALUATION,
                            simTime())
                            + this->getEnv()->getHistoryData()->getPathCompTime(
                                    var,
                                    simTime()
                                            - Patterns::TIME_STATISTIC_EVALUATION,
                                    simTime()));
        }
    } else {
        this->getTree().at(id)->getState()->getMap().setRealPathTimes(
                vector<double>(this->getEnv()->getRealPathTimes()));
    }

    this->getTree().at(id)->getState()->getMap().setEstimatedPathTimes(
            vector<double>());
    this->getEnv()->EstimatePathTimes(
            &this->getTree().at(id)->getState()->getMap().getEstimatedPathTimes(),
            this->getTree().at(id)->getState()->getMap().getOperatorMappings(),
            this->getTree().at(id)->getState()->getMap().getEdgeMappings());

    //Save the mapping
    //    Mapping map = Mapping(operatorMappings, edgeMappings,
    //            residualHostCapabilities, residualLinkCapabilities, realPathTimes,
    //            this->getEnv()->EstimatePathTimes(operatorMappings, edgeMappings));

    this->getTree().at(id)->setRoot(true);
    this->getTree().at(id)->setVisits(0);
    this->getTree().at(id)->setActionTaken(Action());

    int iResources = 0;
    this->getTree().at(id)->getState()->setNumberOperatorsSite(
            this->getEnv()->DetermineNumberUsedHosts(
                    this->getTree().at(id)->getState()->getMap().getOperatorMappings(),
                    iResources));

    this->getTree().at(id)->getState()->setNumberResources(iResources);

    //Summarize all path times
    for (auto st =
            this->getTree().at(id)->getState()->getMap().getRealPathTimes().begin();
            st
                    != this->getTree().at(id)->getState()->getMap().getRealPathTimes().end();
            ++st) {
        this->getTree().at(id)->getState()->setRealTotalTimePaths(
                this->getTree().at(id)->getState()->getRealTotalTimePaths()
                        + *st);
    }

    this->getEnv()->RefreshPathTimes(this->getTree().at(id)->getState());

    this->getEnv()->UpdateMetricsState(this->getTree().at(id)->getState(),
            this->getTree().at(id)->getState());

}

int MC_Tree::getNodeId() const {
    return mNodeID;
}

void MC_Tree::setNodeId(int nodeId) {
    mNodeID = nodeId;
}

int MC_Tree::getStateId() const {
    return mStateID;
}

void MC_Tree::NewState(State* state, Action action, State * &parentState,
        int actionHeuristic) {
    //Increase the position, it refers to the episode
    //    state->setPosition(parentState->getPosition() + 1);
    state->setLevelMcts(
            this->getEnv()->GetNextOperatorDeployment(
                    parentState->getLevelMcts()));

    state->getAvailableActions().clear();

    //    if (state->getPosition() + 1 < this->getEnv()->getOperators().size()) {
    if (this->getEnv()->GetNextOperatorDeployment(state->getLevelMcts())
            != -1) {
        state->setAvailableActions(
                this->getEnv()->getActions(actionHeuristic,
                        parentState->getMap(), false,
                        this->getEnv()->GetNextOperatorDeployment(
                                state->getLevelMcts())));
        //                        state->getPosition() + 1));
    }

    state->getMap().getOperatorMappings().at(
            this->getEnv()->GetIndexOperatorMapping(state->getLevelMcts())).setHostId(
    //            state->getPosition()).setHostId(
            action.getHostId());

    vector<MapOperator> mapOpe;
    for (auto it = state->getMap().getOperatorMappings().begin();
            it != state->getMap().getOperatorMappings().end(); it++) {
        mapOpe.push_back(*it);
    }
    int iResources = 0;
    state->setNumberOperatorsSite(
            this->getEnv()->DetermineNumberUsedHosts(mapOpe, iResources));

    state->setNumberResources(iResources);

    //Restart metrics
    state->getMigration().clear();
    state->setNumberMessagesCloud(0);
    state->setWanTraffic(0);

}

int MC_Tree::addNode(int parentId, bool terminal, bool constrained,
        bool sourceSink, Action& actionTaken, State*& state,
        double cumulativeReward, double reward, int numberMigrations,
        double gainRate, int win, double aggregateCost) {
    //Fill out the basic fields of the new node
    int id = this->getNewNodeId();
    state->setId(this->getNewStateID());
    //    for (unsigned int i = 0; i < this->getTree().at(parentId)->getState()->getAvailableActions().size(); i++){
    //        if (){
    //            break;
    //        }
    //    }

    //    this->getTree().push_back(
    //            Node(id, parentId, false, terminal, constrained, sourceSink,
    //                    actionTaken, state, cumulativeReward, reward,
    //                    numberMigrations, gainRate, win));

    this->getChilds().insert(
            make_pair(make_pair(parentId, actionTaken.getHostId()), id));

    this->getTree().insert(
            make_pair(id,
                    new Node(id, parentId, false, terminal, constrained,
                            sourceSink, actionTaken, new State(state),
                            cumulativeReward, reward, numberMigrations,
                            gainRate, win, aggregateCost)));
    return id;
}

void MC_Tree::setStateId(int stateId) {
    mStateID = stateId;
}

void MC_Tree::RemoveAction(int nodeId, int hostId) {

    //Delete picked action from the available actions
    auto it =
            std::find_if(
                    this->getTree().at(nodeId)->getState()->getAvailableActions().begin(),
                    this->getTree().at(nodeId)->getState()->getAvailableActions().end(),
                    [&](Action &act) {return act.getHostId() == hostId;});

    if (it
            != this->getTree().at(nodeId)->getState()->getAvailableActions().end()) {
        this->getTree().at(nodeId)->getState()->getAvailableActions().erase(it);

    }

    //    for (int i = 0; i < this->getNode(nodeId).getState()->getAvailableActions().size(); ++i) {
    //        if (this->getNode(nodeId).getState()->getAvailableActions().at(i)->getHostId() == hostId){
    //            this->getNode(nodeId).getState()->getAvailableActions().at(i)->setTaken(true);
    //            break;
    //        }
    //    }
}

Environment * &MC_Tree::getEnv() {
    return mEnv;
}

void MC_Tree::setEnv(Environment*& env) {
    mEnv = env;
}

Action& MC_Tree::getAvailableAction(State*& state, int hostId) {
    auto it = std::find_if(state->getAvailableActions().begin(),
            state->getAvailableActions().end(),
            [&](Action &act) {return act.getHostId() == hostId;});

    if (it != state->getAvailableActions().end()) {
        return *it;

    } else {
        throw cRuntimeError(
                "Invalid sequence of available actions, host_id = %d", hostId);
    }
}

double MC_Tree::getBestCumulativeReward() const {
    return mBestCumulativeReward;
}

void MC_Tree::setBestCumulativeReward(double bestCumulativeReward) {
    mBestCumulativeReward = bestCumulativeReward;
}

double MC_Tree::getBestGainRate() const {
    return mBestGainRate;
}

void MC_Tree::setBestGainRate(double bestGainRate) {
    mBestGainRate = bestGainRate;
}

double MC_Tree::getBestQValue() const {
    return mBestQValue;
}

void MC_Tree::setBestQValue(double bestQValue) {
    mBestQValue = bestQValue;
}

double MC_Tree::getBestUcb() const {
    return mBestUCB;
}

void MC_Tree::setBestUcb(double bestUcb) {
    mBestUCB = bestUcb;
}

int MC_Tree::getNumberConstraintNodes() const {
    return mNumberConstraintNodes;
}

void MC_Tree::setNumberConstraintNodes(int numberConstraintNodes) {
    mNumberConstraintNodes = numberConstraintNodes;
}

int MC_Tree::getNumberWinNodes() const {
    return mNumberWinNodes;
}

void MC_Tree::setNumberWinNodes(int numberWinNodes) {
    mNumberWinNodes = numberWinNodes;
}

int MC_Tree::getNodeIdBestLt() const {
    return nNodeIdBestLT;
}

void MC_Tree::setNodeIdBestLt(int nodeIdBestLt) {
    nNodeIdBestLT = nodeIdBestLt;
}

map<pair<int, int>, int> & MC_Tree::getChilds() {
    return mChilds;
}

void MC_Tree::defineIntialOperatorMapping(
        vector<MapOperator> *operatorMappings) {
    for (unsigned int map = 0;
            map < this->getEnv()->getGeneralEnv()->getOperatorMapping().size();
            ++map) {

        //Update Selectivity and Data compression, and expassion factor based on real statistics
        auto updOpe =
                this->getEnv()->getGeneralEnv()->getOperators().find(
                        this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                map)->getOperatorId());
        if (updOpe != this->getEnv()->getGeneralEnv()->getOperators().end()) {
            updOpe->second->setSelectivity(
                    this->getEnv()->getRealStatisticsOperator(
                            this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                    map)->getHostId(),
                            this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                    map)->getOperatorId()).getSelectivity());
            updOpe->second->setDataTransferRate(
                    this->getEnv()->getRealStatisticsOperator(
                            this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                    map)->getHostId(),
                            this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                    map)->getOperatorId()).getDataCompressionExpasionFactor());
            updOpe->second->setStateSize(
                    this->getEnv()->getRealStatisticsOperator(
                            this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                    map)->getHostId(),
                            this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                    map)->getOperatorId()).getStateSize());
        }

        //Estimate the arrival rate and the message sizes to determine the CPU requirements
        double arrivalRate = 0, msgSize = 0;
        if (!this->getEnv()->determineArrivalMetrics(operatorMappings,
                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getOperatorId(),
                arrivalRate, msgSize)) {
            throw cRuntimeError(
                    "Problem to estimate arrival rate and message size!");
        }

        //        operatorMappings->push_back(MapOperator());
        //        operatorMappings->at(operatorMappings->size() - 1).setHostId(
        //                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getHostId());
        //        operatorMappings->at(operatorMappings->size() - 1).setOperatorId(
        //                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getOperatorId());
        //
        //        operatorMappings->at(operatorMappings->size() - 1).setRealStatistics(
        //                OperatorStatistics());
        //        operatorMappings->at(operatorMappings->size() - 1).setEstimateStatistics(
        //                OperatorStatistics());
        //
        //        this->getEnv()->getRealStatisticsOperator(&operatorMappings->at(operatorMappings->size() - 1).getRealStatistics(),
        //                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getHostId(),
        //                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getOperatorId());
        //
        //        this->getEnv()->EstimateStatisticsOperator(&operatorMappings->at(operatorMappings->size() - 1).getEstimateStatistics(),
        //                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getHostId(),
        //                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(map)->getOperatorId(),
        //                msgSize, arrivalRate);

        operatorMappings->push_back(
                MapOperator(
                        this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                map)->getOperatorId(),
                        this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                map)->getHostId(),
                        this->getEnv()->getRealStatisticsOperator(
                                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                        map)->getHostId(),
                                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                        map)->getOperatorId()),
                        this->getEnv()->EstimateStatisticsOperator(
                                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                        map)->getHostId(),
                                this->getEnv()->getGeneralEnv()->getOperatorMapping().at(
                                        map)->getOperatorId(), msgSize,
                                arrivalRate)));

    }
}

void MC_Tree::defineIntialEdgeMapping(vector<MapEdge>* edgeMappings) {
    for (unsigned int iLink = 0;
            iLink < this->getEnv()->getGeneralEnv()->getLinkMapping().size();
            ++iLink) {
        for (unsigned int iConnect = 0;
                iConnect
                        < this->getEnv()->getGeneralEnv()->getNetworkTopology().size();
                iConnect++) {

            if (this->getEnv()->getGeneralEnv()->getNetworkTopology().at(
                    iConnect)->getLinkId()
                    == this->getEnv()->getGeneralEnv()->getLinkMapping().at(
                            iLink)->getLinkId()) {

                edgeMappings->push_back(
                        MapEdge(
                                this->getEnv()->getGeneralEnv()->getLinkMapping().at(
                                        iLink)->getFromOperatorId(),
                                this->getEnv()->getGeneralEnv()->getLinkMapping().at(
                                        iLink)->getToOperatorId(),
                                this->getEnv()->getGeneralEnv()->getNetworkTopology().at(
                                        iConnect)->getSourceId(),
                                this->getEnv()->getGeneralEnv()->getNetworkTopology().at(
                                        iConnect)->getDestinationId(),
                                this->getEnv()->getGeneralEnv()->getLinkMapping().at(
                                        iLink)->getLinkId()));
                break;
            }
        }
    }
}

void MC_Tree::defineInitialHostCapabilities(
        unordered_map<int, HostAvailability>* residualHostCapabilities,
        vector<MapOperator> operatorMappings, bool useSlots) {

    vector<int> resources;
    for (unsigned int iMap = 0; iMap < operatorMappings.size(); iMap++) {
        auto it = std::find(resources.begin(), resources.end(),
                operatorMappings.at(iMap).getHostId());
        if (it == resources.end()) {
            resources.push_back(operatorMappings.at(iMap).getHostId());
        }
    }

    for (auto host = resources.begin(); host != resources.end(); ++host) {
        double memoryReq = 0, cpuReq = 0;
        int slots = 0;
        for (auto mapping = operatorMappings.begin();
                mapping != operatorMappings.end(); ++mapping) {
            if (mapping->getHostId()
                    == this->getEnv()->getGeneralEnv()->getResources().at(*host)->getId()) {
                memoryReq +=
                        mapping->getEstimateStatistics().getMemoryRequirement();
                cpuReq += mapping->getEstimateStatistics().getCompRequirement();
                slots++;

            }
        }

        bool exists = false;
        if (residualHostCapabilities->find(*host)
                != residualHostCapabilities->end()) {
            residualHostCapabilities->at(*host).setAvailableCpu(
                    residualHostCapabilities->at(*host).getAvailableCpu()
                            - cpuReq);
            residualHostCapabilities->at(*host).setAvailableMemory(
                    residualHostCapabilities->at(*host).getAvailableMemory()
                            - memoryReq);
            if (useSlots) {
                residualHostCapabilities->at(*host).setAvailableSlots(
                        residualHostCapabilities->at(*host).getAvailableSlots()
                                != -1 ?
                                residualHostCapabilities->at(*host).getAvailableSlots()
                                        - slots :
                                residualHostCapabilities->at(*host).getAvailableSlots());
            }
            exists = true;
        }

        if (!exists) {
            residualHostCapabilities->insert(
                    make_pair(*host,
                            HostAvailability(*host,
                                    this->getEnv()->getGeneralEnv()->getHostCapabilities().at(
                                            *host).getMemory() - memoryReq,
                                    this->getEnv()->getGeneralEnv()->getHostCapabilities().at(
                                            *host).getCpu() - cpuReq,
                                    this->getEnv()->getGeneralEnv()->getHostCapabilities().at(
                                            *host).getSlotNumber() != -1 ?
                                            this->getEnv()->getGeneralEnv()->getHostCapabilities().at(
                                                    *host).getSlotNumber()
                                                    - slots :
                                            this->getEnv()->getGeneralEnv()->getHostCapabilities().at(
                                                    *host).getSlotNumber())));
        }
    }
}

void MC_Tree::defineInitialLinkCapabilities(
        unordered_map<int, LinkAvailability>* residualLinkCapabilities,
        vector<MapEdge> edgeMappings, vector<MapOperator> operatorMappings) {

    vector<int> links;
    for (unsigned int iLink = 0; iLink < edgeMappings.size(); iLink++) {
        auto it = std::find(links.begin(), links.end(),
                edgeMappings.at(iLink).getLinkId());
        if (it == links.end()) {
            links.push_back(edgeMappings.at(iLink).getLinkId());
        }
    }

    for (auto link = links.begin(); link != links.end(); ++link) {
        double reqBdwth = 0;

        for (auto edge = edgeMappings.begin(); edge != edgeMappings.end();
                edge++) {
            if (edge->getLinkId() == *link) {
                for (auto mapping = operatorMappings.begin();
                        mapping != operatorMappings.end(); ++mapping) {
                    if (mapping->getOperatorId() == edge->getFromOperatorId()) {
                        reqBdwth +=
                                mapping->getEstimateStatistics().getOutputRate()
                                        * mapping->getEstimateStatistics().getOutputMsgSize();

                    }
                }
            }
        }

        bool exists = false;
        if (residualLinkCapabilities->find(*link)
                != residualLinkCapabilities->end()) {

            residualLinkCapabilities->at(*link).setAvailableBandwidth(
                    residualLinkCapabilities->at(*link).getAvailableBandwidth()
                            - reqBdwth);
            exists = true;

        }

        if (!exists) {
            residualLinkCapabilities->insert(
                    make_pair(*link,
                            LinkAvailability(*link,
                                    this->getEnv()->getGeneralEnv()->getLinkCapabilities().at(
                                            *link).getBandwidth() - reqBdwth)));
        }

    }

}

void MC_Tree::setChilds(const map<pair<int, int>, int>& childs) {
    mChilds = childs;
}

} /* namespace fogstream */


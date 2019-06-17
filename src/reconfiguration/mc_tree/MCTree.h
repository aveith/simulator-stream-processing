#ifndef RECONFIGURATION_MC_TREE_MCTREE_H_
#define RECONFIGURATION_MC_TREE_MCTREE_H_

#include <vector>
#include <unordered_map>
#include <cassert>
#include <map>
#include <omnetpp.h>

#include "../../configuration/Configuration.h"
#include "node/Node.h"
#include "state/State.h"
#include "../mapping/Mapping.h"
#include "../statisticsmcts/StatisticsMCTS.h"
#include "../statistics/host/Host.h"
#include "../statistics/link/Link.h"
#include "../environment/Environment.h"
#include "../../util/Patterns.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class MC_Tree {

public:
    MC_Tree(Environment* env);
    virtual ~MC_Tree();

    int getNewNodeId();
    int getNewStateID();

    //    const vector<Node*> getUntriedNodes();

    int addChild(Action action, Node parentNode, bool sourceSink,
            bool terminal);

    int addNode(int parentId, bool terminal, bool constrained, bool sourceSink,
            Action& actionTaken, State*& state, double cumulativeReward,
            double reward, int numberMigrations, double gainRate, int win,
            double aggregateCost);

    void NewState(State* state, Action action, State*& parentState,
            int actionHeuristic);

    void addRoot(bool useSlots, int actionHeuristic);
    void defineIntialOperatorMapping(vector<MapOperator>* map);
    void defineIntialEdgeMapping(vector<MapEdge>* edgeMappings);
    void defineInitialHostCapabilities(
            unordered_map<int, HostAvailability>* residualHostCapabilities,
            vector<MapOperator> operatorMappings, bool useSlots);
    void defineInitialLinkCapabilities(
            unordered_map<int, LinkAvailability>* residualLinkCapabilities,
            vector<MapEdge> edgeMappings, vector<MapOperator> operatorMappings);

    void RemoveAction(int nodeId, int hostId);

    Action& getAvailableAction(State*& state, int hostId);

    Environment*& getEnv();
    void setEnv(Environment*& env);
//    Node*&getNode(int id);
    //    vector<Node>& getTree();
    //    void setTree(const vector<Node>& tree);

    unordered_map<int, Node*>& getTree();
    void setTree(const unordered_map<int, Node*>& tree);

    int getNodeId() const;
    void setNodeId(int nodeId);
    int getStateId() const;
    void setStateId(int stateId);
    double getBestCumulativeReward() const;
    void setBestCumulativeReward(double bestCumulativeReward);
    double getBestGainRate() const;
    void setBestGainRate(double bestGainRate);
    double getBestQValue() const;
    void setBestQValue(double bestQValue);
    double getBestUcb() const;
    void setBestUcb(double bestUcb);
    int getNumberConstraintNodes() const;
    void setNumberConstraintNodes(int numberConstraintNodes);
    int getNumberWinNodes() const;
    void setNumberWinNodes(int numberWinNodes);
    int getNodeIdBestLt() const;
    void setNodeIdBestLt(int nodeIdBestLt);
    map<pair<int, int>, int>& getChilds();
    void setChilds(const map<pair<int, int>, int>& childs);

protected:
    Environment* mEnv;
    //    vector<Node> mTree;
    unordered_map<int, Node*> mTree;
    map<pair<int, int>, int> mChilds;

    int mNodeID = -1;
    int mStateID = -1;

    int mNumberConstraintNodes = 0;
    int mNumberWinNodes = 0;
    int nNodeIdBestLT = 0;

    double mBestCumulativeReward = 0;
    double mBestUCB = 0;
    double mBestQValue = 0;
    double mBestGainRate = 0;

};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MC_TREE_MCTREE_H_ */

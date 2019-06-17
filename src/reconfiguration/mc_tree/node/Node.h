#ifndef RECONFIGURATION_MC_TREE_NODE_NODE_H_
#define RECONFIGURATION_MC_TREE_NODE_NODE_H_

#include <vector>
#include <omnetpp.h>
#include "../action/Action.h"
#include "../state/State.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Node {

public:
    Node();

    Node(int id, int parentId, bool root, bool terminal, bool constrained,
            bool sourceSink, Action actionTaken, State* state,
            double cumulativeReward, double reward, int numberMigrations,
            double gainRate, int win, double aggregateCost);

    Node(int id, int parentID);

    virtual ~Node();

    Action& getActionTaken();
    void setActionTaken(const Action& actionTaken);
    bool isConstrained();
    void setConstrained(bool constrained);
    vector<int>& getConstraints();
    void setConstraints(vector<int>& constraints);
    int getId();
    int getParentId();
    void setParentId(int parentId);
    double& getCumulativeReward();
    void setCumulativeReward(double cumulativeReward);
    bool isRoot();
    void setRoot(bool root);
    bool isSourceSink();
    void setSourceSink(bool sourceSink);
    bool isTerminal();
    void setTerminal(bool terminal);
    int getVisits();
    void setVisits(int visits);

    bool operator==(Node& rhs) {
        return mID == rhs.getId();
    }

    void setId(int id);
    double& getQValue();
    void setQValue(double qValue);
    const double& getUctValue() const;
    void setUctValue(double uctValue);
    int getWin() const;
    void setWin(int win);
    double getReward() const;
    void setReward(double reward);
    double getGainRate() const;
    void setGainRate(double gainRate);
    int getNumberMigrations() const;
    void setNumberMigrations(int numberMigrations);
    double getAggregateCost() const;
    void setAggregateCost(double aggregateCost);
    State*& getState();
    void setState( State* state);
    string toString();

protected:
    int mID = -1;
    bool mRoot = false;
    bool mTerminal = false;
    bool mConstrained = false;
    bool mSourceSink = false;
    int mParentID = 0;
    Action mActionTaken;
    State* mState;
    double mCumulativeReward = 0;
    int mWin = 0; //-1: Worst; 0: Equal; 1: Better
    int mVisits = 0;
    vector<int> mConstraints;
    double mUCTValue = 0;
    double mQValue = 0;
    double mReward = 0;
    int mNumberMigrations = 0;
    double mGainRate = 0;

    double mAggregateCost = 2;

};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MC_TREE_NODE_NODE_H_ */

#ifndef RECONFIGURATION_MCTS_EPISODE_EPISODE_H_
#define RECONFIGURATION_MCTS_EPISODE_EPISODE_H_

#include <vector>
#include <omnetpp.h>
#include "../../mc_tree/state/State.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Episode {
public:
    Episode();
    Episode(State*& parentState, Action action, State*& state,
            double cumulativeReward, vector<int> constraints, int parentId);
    virtual ~Episode();
    double& getCumulativeReward();
    void setCumulativeReward(double cumulativeReward);
    State*& getParentState();
    void setParentState(State* parentState);
    State*& getState();
    void setState(State* state);
    vector<int>& getConstraints();
    void setConstraints(const vector<int>& constraints);
    Action& getAction();
    void setAction(const Action& action);
    int getId() const;
    void setId(int id);
    int getParentId() const;
    void setParentId(int parentId);
    double getGainRate() const;
    void setGainRate(double gainRate);
    int getNumberMigrations() const;
    void setNumberMigrations(int numberMigrations);
    double getReward() const;
    void setReward(double reward);
    int getWin() const;
    void setWin(int win);
    double getAggregateCost() const;
    void setAggregateCost(double aggregateCost);

    string toString();

protected:
    State* mParentState;
    State* mState;
    double mCumulativeReward = 0;
    vector<int> mConstraints;
    Action mAction;
    int mId = -1;
    int mParentID = -1;
    double mReward = 0;
    int mNumberMigrations = 0;
    double mGainRate = 0;
    int mWin = 0;

    double mAggregateCost = 0;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MCTS_EPISODE_EPISODE_H_ */

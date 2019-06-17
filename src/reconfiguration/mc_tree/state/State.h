#ifndef RECONFIGURATION_MC_TREE_STATE_STATE_H_
#define RECONFIGURATION_MC_TREE_STATE_STATE_H_

#include <vector>
#include <omnetpp.h>
#include "../action/Action.h"
#include "../../mapping/Mapping.h"
#include "../../migration/Migration.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class State {

public:
    State();
    State(State* state);
    State(int id, Mapping map, vector<Action> availableActions,
            double totalTimePaths, int levelMcts);
    virtual ~State();

    vector<Action>& getAvailableActions();
    void setAvailableActions(vector<Action> availableActions);
    int getId();
    Mapping& getMap();
    void setMap(const Mapping& map);
    //    int getPosition();
    //    void setPosition(int position);
    double getTotalTimePaths();
    void setTotalTimePaths(double totalTimePaths);

    bool operator==(State*& rhs) {
        return mID == rhs->getId();
    }

    void setId(int id);
    int getNumberResources() const;
    void setNumberResources(int numberResources);
    unordered_map<int, int>& getNumberOperatorsSite();
    void setNumberOperatorsSite(const unordered_map<int, int>& numberOperatorsSite);
    int getLevelMcts() const;
    void setLevelMcts(int levelMcts);
    double getRealTotalTimePaths() const;
    void setRealTotalTimePaths(double realTotalTimePaths);
    int getNumberMessagesCloud() const;
    void setNumberMessagesCloud(int numberMessagesCloud);
    double getWanTraffic() const;
    void setWanTraffic(double wanTraffic);
    vector<Migration>& getMigration();
    void setMigration(const vector<Migration>& migration);
    double getAwsPricing() const;
    void setAwsPricing(double awsPricing);
    double getMicrosoftPricing() const;
    void setMicrosoftPricing(double microsoftPricing);
    int getNumberCloudConnections() const;
    void setNumberCloudConnections(int numberCloudConnections);
    double getAverageSizeCloudMsgs() const;
    void setAverageSizeCloudMsgs(double averageSizeCloudMsgs);
    string toString();

protected:
    int mID = 0;
    Mapping mMap;
    vector<Action> mAvailableActions;
    //    int mPosition;
    int mLevelMCTS = -1;
    double mTotalTimePaths = 0;
    double mRealTotalTimePaths = 0;
    int nNumberResources = 1;
    unordered_map<int, int> nNumberOperatorsSite;

    //TODO - implement the below methods
    double mWANTraffic = 0;
    int mNumberMessagesCloud = 0;
    vector<Migration> mMigration;
    double mMicrosoftPricing = 0;
    double mAWSPricing = 0;
    int mNumberCloudConnections = 0;
    double mAverageSizeCloudMsgs = 0;

};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MC_TREE_STATE_STATE_H_ */

#ifndef RECONFIGURATION_MCTS_MCTS_H_
#define RECONFIGURATION_MCTS_MCTS_H_

#include <vector>
#include <map>
#include <cassert>
#include <unordered_map>
#include <omnetpp.h>

#include <omnetpp.h>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <cmath>
#include <chrono>

#include "../environment/Environment.h"
#include "../mc_tree/MCTree.h"
#include "episode/Episode.h"

using namespace std;
using namespace omnetpp;
using namespace chrono;

namespace fogstream {

class logInfo {
public:
    logInfo(int iteration, double elapsedTime, int numberPaths,
            double iterationTime, double iterationReward,
            double iterationCumulativeReward, double iterationTotalLatencyTime,
            int iterationNumberMigrations, int iterationNumberResources,
            double iterationGainRate, vector<double> iterationPathTimes,
            double rootTotalLatencyTime, double rootAggregateCost,
            double rootWanTraffic, double rootMircrosoftPrice,
            double rootAWSPrice,

            int numberTreeNodes, int numberConstraintNodes, int numberWinNodes,
            double bestTotalLatencyTime, int bestNumberMigrationLt,
            int bestNumberResourcesLt, vector<double> bestPathTimesLt,
            double bestCumulativeReward, double bestUcb, double bestQvalue,
            double bestGainRate, double iterationRealPathTimes,
            vector<int> iterationNumberOperatorsSite,
            vector<int> bestNumberOperatorsSite,
            double iterationWANTraffic, int iterationNumberMessagesCloud,
            double iterationMigrationCosts, double iterationMicrosoftPricing,
            double iterationAWSPricing, int iterationNumberCloudConnections,
            double iterationAverageSizeCloudMsgs, double iterationAggregateCost,
            double bestWANTraffic, int bestNumberMessagesCloud,
            double bestMigrationCosts, double bestMicrosoftPricing,
            double bestAWSPricing, int bestNumberCloudConnections,
            double bestAverageSizeCloudMsgs, double bestAggregateCost);

    void printLogInfo();
    string getLogInfoString();
    double getBestGainRate() const;
    void setBestGainRate(double bestGainRate);
    int getBestNumberMigrationLt() const;
    void setBestNumberMigrationLt(int bestNumberMigrationLt);
    int getBestNumberResourcesLt() const;
    void setBestNumberResourcesLt(int bestNumberResourcesLt);
    vector<double>& getBestPathTimesLt();
    void setBestPathTimesLt(vector<double> bestPathTimesLt);
    double getBestQvalue() const;
    void setBestQvalue(double bestQvalue);
    double getBestCumulativeReward() const;
    void setBestCumulativeReward(double bestCumulativeReward);
    double getBestTotalLatencyTime() const;
    void setBestTotalLatencyTime(double bestTotalLatencyTime);
    double getBestUcb() const;
    void setBestUcb(double bestUcb);
    double getElapsedTime() const;
    void setElapsedTime(double elapsedTime);
    int getIteration() const;
    void setIteration(int iteration);
    double getIterationCumulativeReward() const;
    void setIterationCumulativeReward(double iterationCumulativeReward);
    double getIterationGainRate() const;
    void setIterationGainRate(double iterationGainRate);
    int getIterationNumberMigrations() const;
    void setIterationNumberMigrations(int iterationNumberMigrations);
    int getIterationNumberResources() const;
    void setIterationNumberResources(int iterationNumberResources);
    vector<double>& getIterationPathTimes();
    void setIterationPathTimes(vector<double> iterationPathTimes);
    double getIterationReward() const;
    void setIterationReward(double iterationReward);
    double getIterationTotalLatencyTime() const;
    void setIterationTotalLatencyTime(double iterationTotalLatencyTime);
    int getNumberConstraintNodes() const;
    void setNumberConstraintNodes(int numberConstraintNodes);
    int getNumberPaths() const;
    void setNumberPaths(int numberPaths);
    int getNumberTreeNodes() const;
    void setNumberTreeNodes(int numberTreeNodes);
    int getNumberWinNodes() const;
    void setNumberWinNodes(int numberWinNodes);
    double getRootTotalLatencyTime() const;
    void setRootTotalLatencyTime(double rootTotalLatencyTime);
    const vector<int>& getBestNumberOperatorsSite() const;
    void setBestNumberOperatorsSite(
            const vector<int>& bestNumberOperatorsSite);
    const vector<int>& getIterationNumberOperatorsSite() const;
    void setIterationNumberOperatorsSite(
            const vector<int>& iterationNumberOperatorsSite);
    double getIterationRealPathTimes() const;
    void setIterationRealPathTimes(double iterationRealPathTimes);
    double getIterationTime() const;
    void setIterationTime(double iterationTime);
    double getBestAggregateCost() const;
    void setBestAggregateCost(double bestAggregateCost);
    double getBestAverageSizeCloudMsgs() const;
    void setBestAverageSizeCloudMsgs(double bestAverageSizeCloudMsgs);
    double getBestAwsPricing() const;
    void setBestAwsPricing(double bestAwsPricing);
    double getBestMicrosoftPricing() const;
    void setBestMicrosoftPricing(double bestMicrosoftPricing);
    double getBestMigrationCosts() const;
    void setBestMigrationCosts(double bestMigrationCosts);
    int getBestNumberCloudConnections() const;
    void setBestNumberCloudConnections(int bestNumberCloudConnections);
    int getBestNumberMessagesCloud() const;
    void setBestNumberMessagesCloud(int bestNumberMessagesCloud);
    double getBestWanTraffic() const;
    void setBestWanTraffic(double bestWanTraffic);
    double getIterationAggregateCost() const;
    void setIterationAggregateCost(double iterationAggregateCost);
    double getIterationAverageSizeCloudMsgs() const;
    void setIterationAverageSizeCloudMsgs(double iterationAverageSizeCloudMsgs);
    double getIterationAwsPricing() const;
    void setIterationAwsPricing(double iterationAwsPricing);
    double getIterationMicrosoftPricing() const;
    void setIterationMicrosoftPricing(double iterationMicrosoftPricing);
    double getIterationMigrationCosts() const;
    void setIterationMigrationCosts(double iterationMigrationCosts);
    int getIterationNumberCloudConnections() const;
    void setIterationNumberCloudConnections(
            int iterationNumberCloudConnections);
    int getIterationNumberMessagesCloud() const;
    void setIterationNumberMessagesCloud(int iterationNumberMessagesCloud);
    double getIterationWanTraffic() const;
    void setIterationWanTraffic(double iterationWanTraffic);
    double getRootAggregateCost() const;
    void setRootAggregateCost(double rootAggregateCost);
    double getRootAwsPrice() const;
    void setRootAwsPrice(double rootAwsPrice);
    double getRootMircrosoftPrice() const;
    void setRootMircrosoftPrice(double rootMircrosoftPrice);
    double getRootWanTraffic() const;
    void setRootWanTraffic(double rootWanTraffic);

protected:
    int mIteration;
    double mElapsedTime;

    //Application data
    int mNumberPaths;

    //Iteration data
    double mIterationTime;
    double mIterationReward;
    double mIterationCumulativeReward;
    double mIterationTotalLatencyTime;
    int mIterationNumberMigrations;
    int mIterationNumberResources;
    double mIterationGainRate;
    vector<double> mIterationPathTimes;
    double mIterationRealPathTimes;
    vector<int> mIterationNumberOperatorsSite;

    double mIterationWANTraffic;
    int mIterationNumberMessagesCloud;
    double mIterationMigrationCosts;
    double mIterationMicrosoftPricing;
    double mIterationAWSPricing;
    int mIterationNumberCloudConnections;
    double mIterationAverageSizeCloudMsgs;
    double mIterationAggregateCost;

    //Tree data
    double mRootTotalLatencyTime;
    double mRootAggregateCost;
    double mRootWanTraffic;
    double mRootAWSPrice;
    double mRootMircrosoftPrice;
    int mNumberTreeNodes;
    int mNumberConstraintNodes;
    int mNumberWinNodes;

    //Best Latency Node
    double mBestTotalLatencyTime;
    int mBestNumberMigrationLT;
    int mBestNumberResourcesLT;
    vector<double> mBestPathTimesLT;
    vector<int> mBestNumberOperatorsSite;

    //General data
    double mBestCumulativeReward;
    double mBestUCB;
    double mBestQvalue;
    double mBestGainRate;
    double mBestWANTraffic;
    int mBestNumberMessagesCloud;
    double mBestMigrationCosts;
    double mBestMicrosoftPricing;
    double mBestAWSPricing;
    int mBestNumberCloudConnections;
    double mBestAverageSizeCloudMsgs;
    double mBestAggregateCost;

};

class LogMCTS {
public:
    LogMCTS(int type, string filename, Parameters* parameters);

    void writeIterationLog();
    void writeHeader();
    void saveLogIteration(int iteration, int numberPaths,
            double iterationReward, double& iterationCumulativeReward,
            double iterationTotalLatencyTime, int iterationNumberMigrations,
            int iterationNumberResources, double iterationGainRate,
            vector<double>& iterationPathTimes, double rootTotalLatencyTime,
            double rootAggregateCost, double rootWanTraffic,
            double rootMircrosoftPrice, double rootAWSPrice,

            int numberTreeNodes, int numberConstraintNodes, int numberWinNodes,
            double bestTotalLatencyTime, int bestNumberMigrationLt,
            int bestNumberResourcesLt, vector<double>& bestPathTimesLt,
            double bestCumulativeReward, double bestUcb, double bestQvalue,
            double bestGainRate, double iterationRealPathTimes,
            vector<int> iterationNumberOperatorsSite,
            vector<int> bestNumberOperatorsSite,
            double realPathTimes, double iterationWANTraffic,
            int iterationNumberMessagesCloud, double iterationMigrationCosts,
            double iterationMicrosoftPricing, double iterationAWSPricing,
            int iterationNumberCloudConnections,
            double iterationAverageSizeCloudMsgs, double iterationAggregateCost,
            double bestWANTraffic, int bestNumberMessagesCloud,
            double bestMigrationCosts, double bestMicrosoftPricing,
            double bestAWSPricing, int bestNumberCloudConnections,
            double bestAverageSizeCloudMsgs, double bestAggregateCost, long realNumberMsgs);

    void saveOperatorDeployment(State*& finalDeployment,
            State*& initialDeployment,
            unordered_map<int, OperatorData*> operators);

    string getLogString();
    vector<logInfo>& getDataIterationLog();
    void setDataIterationLog(const vector<logInfo>& dataIterationLog);
    const string& getFileName() const;
    void setFileName(const string& fileName);
    bool isHeader() const;
    void setHeader(bool header);
    const high_resolution_clock::time_point& getInitialTime() const;
    void setInitialTime(const high_resolution_clock::time_point& initialTime);
    Parameters*& getParameters();
    void setParameters(Parameters*& parameters);
    int getType() const;
    void setType(int type);
    int getNumberPaths() const;
    void setNumberPaths(int numberPaths);
    int getNumberSites() const;
    void setNumberSites(int numberSites);
    const high_resolution_clock::time_point& getLastElaspedTime() const;
    void setLastElaspedTime(
            const high_resolution_clock::time_point& lastElaspedTime);
    double getRealPathTimes() const;
    void setRealPathTimes(double realPathTimes);
    long getNumberMsgsApp() const;
    void setNumberMsgsApp(long numberMsgsApp);

protected:
    vector<logInfo> dataIterationLog;
    string mFileName;
    int mType;
    high_resolution_clock::time_point mInitialTime;
    Parameters* mParameters;
    double mRealPathTimes;
    bool mHeader = false;
    int mNumberPaths;
    int nNumberSites;
    long mNumberMsgsApp;
    high_resolution_clock::time_point mLastElaspedTime;
};

class MCTS {

public:
    MCTS();
    MCTS(cRNG* rng, Environment* env, LogMCTS*& logMcts);
    virtual ~MCTS();

    void execute(int iterations);
    int getNode(int parentId, int hostId);

    cRNG*& getRng();
    void setRng(cRNG*& rng);
    MC_Tree*& getMcTree();
    void setMcTree(MC_Tree* mcTree);
    Environment*& getEnv();
    void setEnv(Environment*& env);
    bool isIterateToTerminal() const;
    void setIterateToTerminal(bool iterateToTerminal);
    bool isCreateAll() const;
    void setCreateAll(bool createAll);
    LogMCTS*& getLogMcts();
    void setLogMcts(LogMCTS* logMcts);
    bool isFirstVisit() const;
    void setFirstVisit(bool firstVisit);
    void ParametersCorrection(Node*& node);

    double CalcLatencyBasedReward(double currentLatency,
            double baseLatency, bool isConstrained);

    double CalcAggCostBasedReward(double currentAggCost, double baseAggCost,
            bool isConstrained);

    double CalcUCB1(double constant, double qValue, int visitsNode,
            int visitsParent);
    int ActionBasicUCB1(double constant, int nodeId,
            vector<Action>& availableActions);

    double CalcAggregateCost(State* state);
    double GetMaxMigrationTime(State* state);
    long getNumberMsgsApp() const;
    void setNumberMsgsApp(long numberMsgsApp);
    bool isUseSlots() const;
    void setUseSlots(bool useSlots);
    bool isUseRealQValues() const;
    void setUseRealQValues(bool useRealQValues);

private:
    int SelectNode(); //result the node to expand
    int ExpandNode(int nodeId, vector<Episode*> episodes); //result into an action
    void Simulate(Episode* episode, int action, State*& stateParent, int parentId, bool aggregateReward); // result the position of the new node
    void Rollout(int newNodeId, double reward, vector<Episode*> episodes); //backpropagate the results
    void CreateEpisode(Episode* episode,int nodeId, int action, int parentId,
            State* lastEpisode, double lastAggCost);

    void UpdateStatistics(int nodeId);

    virtual int TreePolicy();
    virtual void DefaultPolicy(int newNode, double reward,
            vector<Episode*> episodes);
    virtual int ActionPolicy(State*& state, int nodeId);
    virtual void UpdateValues(int nodeId, Episode*& episode,
            Episode* lastEpisode);

    vector<int> DefineNumberOperatorsPerSite(
            unordered_map<int, int> sitePlacements);
    void saveLog(int iteration);

    void printNodeDetails(int id);
    void printEpisodeDetails(Episode* episode);

protected:
    cRNG* mRNG;
    MC_Tree* mMcTree;
    Environment* mEnv;
    bool mIterateToTerminal;
    bool mCreateAll = false;
    bool mFirstVisit = false;
    LogMCTS* mLogMCTS;
    long mNumberMsgsApp;

    bool mUseSlots = false;
    bool mUseRealQValues = false;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MCTS_MCTS_H_ */

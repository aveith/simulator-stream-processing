#ifndef RECONFIGURATION_ENVIRONMENT_ENVIRONMENT_H_
#define RECONFIGURATION_ENVIRONMENT_ENVIRONMENT_H_

#include <vector>
#include <unordered_map>
#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <omnetpp.h>
#include <cmath>
#include <chrono>
#include <functional>

#include "../../configuration/Configuration.h"
#include "../../env/Env.h"

#include "../mc_tree/node/Node.h"
#include "../../util/graphS/Graph.h"
#include "../statisticsmcts/StatisticsMCTS.h"
#include "../mcts/episode/Episode.h"
#include "../../modules/simpleM/scheduler/parameters/Parameters.h"
#include "../mapping/operator/MapOperator.h"

using namespace std;
using namespace omnetpp;
using namespace chrono;

namespace fogstream {

class Environment {
private:
    //    bool isValidResource();
    void addBasicActionsFollowingOperatiorMapping(vector<Action>& available,
            Mapping currentMapping, int levelMCTS);
    bool isComputeResourceAlreadyAvailable(vector<Action> available, int id);
    double estimateLatencyBetweenOperators(int operatorID, int dstResource,
            Mapping currentMapping);
    int getMappingHost(int operatorID, Mapping currentMapping);

public:
    Environment();

    Environment(Env* &generalenv, Parameters*& parameters);

    Environment(Env* &generalenv, StatisticsMCTS*& historyData);

    virtual ~Environment();
    void SimulateTransition(Episode* episode, State* parentState, State* state,
            Action action, int parentId, State* root, bool useSltos,
            bool aggregateReward);
    void UpdateLinks(State* state);
    void UpdateHosts(State* state, Action* action);
    void EstimateOperatorRequirements(State* state);
    bool EvaluateLinkConstraints(State* state);
    bool isLinkConstrained(State* state);
    bool EvaluateHostConstraints(State* state, Action* action, bool useSlots);
    void SummarizeExecution(State* state, Action* action);
    void RefreshPathTimes(State* _state);

    bool isSourceSink(int operatorId);

    void SetupDeployableOperators(vector<int> operators);
    int GetIndexOperatorMapping(int levelMCTS);
    int GetNextOperatorDeployment(int operatorId);

    void UpdateMetricsState(State* state, State* root);
    void MetricsNumberofMsgWanTraffic(State* state);
    void MetricMigration(State* state, State* root);
    double MircrosoftIOTHubPricing(int numberofmessages, double msgSize,
            int connections);
    double AWSIOTCorePricing(int numberofmessages, double msgSize,
            int connections);

    //    vector<Action*> getActions(Mapping currentMapping, int position);
    vector<Action> getActions(int actionHeuristic, Mapping currentMapping,
             bool root, int levelMCTS);

    bool determineArrivalMetrics(vector<MapOperator>* operatorMappings,
            int operatorId, double& arrivalRate, double& msgSize);

    OperatorStatistics EstimateStatisticsOperator(int hostID, int OperatorID,
            double msgSize, double arrivalRate);
    OperatorStatistics getRealStatisticsOperator(int hostID, int OperatorID);

    void EstimatePathTimes(vector<double>* pathTimes,
            vector<MapOperator> operatorMappings, vector<MapEdge> edgeMappings);

    bool getAvailablePath(int fromHostID, int toHostID, int fromOperatorID,
            int toOperatorID,
            unordered_map<int, LinkAvailability> &residualLinkCapabilities,
            double msgSize, double arrivaRate, vector<MapEdge> &edgelist);

    void saveTraceFile(Parameters* parameters, vector<int> pathMsgs);
    bool loadTracedFiles(Parameters* parameters);

    StatisticsMCTS*& getHistoryData();
    void setHistoryData(StatisticsMCTS*& historyData);
    bool isReadTraceFile() const;
    void setReadTraceFile(bool readTraceFile);

    void writeDatasetOperatorStatistics(string directory, string fileName,
            vector<OperatorStatistics>& datavector);
    void writeDatasetPathTimes(string directory, string fileName,
            vector<double>& datavector, vector<int> pathMsgs);

    bool readDatasetOperatorStatistics(string fileName,
            vector<OperatorStatistics>& datavector);
    bool readDatasetPathTimes(string fileName, vector<double>& datavector,
            vector<long>& numberMsgs);

    vector<OperatorStatistics>& getOperatorStatistics();
    void setOperatorStatistics(
            const vector<OperatorStatistics>& operatorStatistics);
    vector<double>& getRealPathTimes();
    void setRealPathTimes(vector<double>& realPathTimes);

    vector<int>& getDeployableOperators();
    void setDeployableOperators(vector<int>& deployableOperators);
    unordered_map<int, int>& getOperatorMappingIndex();
    void setOperatorMappingIndex(unordered_map<int, int>& operatorMappingIndex);

    void getMigration(vector<MapOperator> operatorMappings,
            Migration* migration, double stateSize);

    Env*& getGeneralEnv();
    void setGeneralEnv(Env*& generanlEnv);

    unordered_map<int, int> DetermineNumberUsedHosts(vector<MapOperator> mapOpe,
            int &numberHosts);
    vector<long>& getRealNumberMessagesPath();
    void setRealNumberMessagesPath(const vector<long>& realNumberMessagesPath);

    void CPUActionHeuristic(vector<Action>& available, Mapping currentMapping,
            int levelMCTS);

    void LatencyActionHeuristic(vector<Action>& available,
            Mapping currentMapping, int levelMCTS);
    vector<Action>& getRootActions();
    void setRootActions(const vector<Action>& rootActions);

protected:
    Env* mGeneralEnv;

    StatisticsMCTS* mHistoryData;
    vector<OperatorStatistics> mOperatorStatistics;
    vector<double> mRealPathTimes;
    vector<long> mRealNumberMessagesPath;

    bool mReadTraceFile = false;

    vector<int> mDeployableOperators;
    unordered_map<int, int> mOperatorMappingIndex; //First = Operator - Second: Index for operator mappings
    vector<Action> mRootActions;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_ENVIRONMENT_ENVIRONMENT_H_ */

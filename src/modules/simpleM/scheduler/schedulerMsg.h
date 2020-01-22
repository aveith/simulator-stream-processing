#ifndef __FOGSTREAM_SCHEDULERMSG_H_
#define __FOGSTREAM_SCHEDULERMSG_H_
#include <map>
#include <cassert>
#include <unordered_map>

#include "../../../util/graphS/Graph.h"
#include "../../../reconfiguration/statisticsmcts/StatisticsMCTS.h"
#include "../../../reconfiguration/environment/Environment.h"
#include "../../../env/objects/applicationmapping/operatorconnectionmapping/OperatorConnectionMapping.h"
#include "../../../env/objects/applicationmapping/operatormapping/OperatorMapping.h"
#include "../../../env/Env.h"
#include "../../msg/operatorDep/OperatorDep_m.h"

#include "parameters/Parameters.h"
#include <omnetpp.h>
#include "../../../configuration/Configuration.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

struct edge_raw {
    long from;
    long to;
};

struct fix_vertex_position {
    long vertex_id;
    long host_id;
};

class SchedulerMsg: public cSimpleModule {
public:
    Parameters*& getParameters();
    void setParameters(Parameters* parameters);
    Env*& getGenralEnv();
    void setGenralEnv(Env*& genralEnv);
    Environment*& getReconfigEnv();
    void setReconfigEnv(Environment*& reconfigEnv);
    StatisticsMCTS*& getStatistics();
    void setStatistics(StatisticsMCTS*& statistics);
    const vector<fix_vertex_position>& getVertexPositions() const;
    void setVertexPositions(const vector<fix_vertex_position>& vertexPositions);

private:
    void setupEnvironment();
    void SetupSchedulerConnection(cModule *module);
    void configuration();

    void saveToFile(vector<int> vec, const char* file);
    void saveToFileString(std::string text, const char* file);
    vector<fix_vertex_position> parse_position(const char* positions);
    void addDataToHistory(cMessage *msg);
    vector<vector<int>> getAllPossiblePaths(
            vector<OperatorConnection*> taskDependencies);
    void initialize();
    void handleMessage(cMessage *msg);
    void addVertex(cQueue *verticesQueue, int operatorId, int fissionId,
            float seletivity, float dataReduction, int type,
            float stateIncreaseRate, float memoryCost, float cpuCost,
            bool nextSplitted, int nextSplitLength, const char* appName,
            float availableCPU, float availableMem, double timeWindow);
    void addEdge(cQueue *edgesQueue, const char* dstHost, int nextState,
            int nextSubState, int curState, int curSubState,
            const char* appName);
    void addDep(cQueue *depQueue, const char* appName, int curState,
            int curSubState, int nextState, int nextSubState);

    void createNetworkbyXML(const char* networkfile);
    void createAppbyXML(const char* appfile);

    void createDynamicModules();
    cModule* createSpecificModule(const char* moduletype, int hostID,
            int vertexID, int direction);

    void readDotApp(const char* appfile);
    int defineType(vector<edge_raw> edges, int cur);
    virtual int getMaxHopsSink(vector<int> sinks);

    bool isStartReconfig();

    void loadParameters();

    void reconfiguration();

    vector<int> GetOperatorsList();

    void CreateNodeModules();
    void CreateNodeConnections();
    void FillEnvObjectsUsingXML(const char* networkfile);

    vector<edge_raw> ReadAppEdgesFromDot(const char* appfile);
    vector<int> DefineSourcesFromDot(vector<edge_raw> edges);
    vector<int> DefineSinksFromDot(vector<edge_raw> edges);

    void AddOperatorUsingDot(vector<edge_raw> edges, vector<int> sources,
            vector<int> sinks, int operatorId);
    void AddDataSourcesUsingDot(vector<int> sources);
    void AddDataSinksUsingDot(vector<int> sinks);

    void ApplicationDeployment();

    vector<int> ParticpatingHostsMapping();
    void FillOperatorDeploymentObjects(const char* appName, int hostId,
            cQueue* operators, cQueue* operatorDeployments);
    void FillEdgeDeploymentObjects(const char* appName, int hostId,
            cQueue* edge);

    void FillSinkDeploymentObjects(const char* appName, int hostId,
            cQueue* edge, cQueue* operatorDeployments);
    bool FillApplicationPathObject(cQueue* paths);

    void SaveConfigurationFile();

    void CreateSimulation();

    int DefineXMLSourceSinkLocations(int xmlLocation);

    vector<int> GetNumberMessagesPaths();

protected:
    Parameters* mParameters;
    Env* mGenralEnv;
    Environment* mReconfigEnv;
    vector<fix_vertex_position> mVertexPositions;
    StatisticsMCTS* mStatistics = nullptr;

};

} //namespace

#endif

#ifndef ENV_ENV_H_
#define ENV_ENV_H_
#include <omnetpp.h>
#include <unordered_map>
#include <cassert>
#include <map>

#include "objects/application/operatorconnection/OperatorConnection.h"
#include "objects/application/operatordata/OperatorData.h"
#include "objects/applicationmapping/operatorconnectionmapping/OperatorConnectionMapping.h"
#include "objects/applicationmapping/operatormapping/OperatorMapping.h"
#include "objects/datasink/DataSink.h"
#include "objects/datasource/DataSource.h"
#include "objects/networkconnection/NetworkConnection.h"
#include "objects/networkconnection/linkdata/LinkData.h"
#include "objects/networkconnection/resourcedata/ResourceData.h"
#include "objects/networkconnection/linkdata/linkcapability/LinkCapability.h"
#include "objects/networkconnection/resourcedata/resourcecapability/ResourceCapability.h"
#include "objects/application/destinationbranches/DestinationBranches.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Env {
public:
    Env(cRNG* rng);
    virtual ~Env();

    bool getValidPath(int sourceHostId, int destinationHostId,
            double arrivalRate, double arrivalMsgSize, vector<int> &path,
            vector<int>& links);
    vector<int> determineDeployableHosts(int sourceHostId,
            int destinationHostId);
    vector<int> evaluatePaths(vector<vector<int>> paths, vector<int>& links,
            double arrivalRate, double arrivalMsgSize);

    bool EstimateTimesandQueues(double serviceRate, double arrivalRate,
            double additionalOverhead, double &meanTimeinSystem,
            double &meanTimeinQueue, double &meanNumberofMsgsinSystem,
            double &meanNumberinQueue, double &rho);

    void obtaingArrivalMetrics(int operatorId, double& arrivalRate,
            double& arrivalMsgSize);

    //Set up structures
    void addResource(int id, int type, double computAvail, double memAvail,
            double cost, int slotNumber);
    void addLink(int id, double bandwidth, double delay, double cost);
    void addTask(int id, int fissionID, double selectivity,
            double dataTransferRate, double stateRate, int type,
            double computReq, double memReq, bool nextSplitted,
            int nextSplitLength, double timeWindow);
    void addSource(int host, int task, double arrivalRate, double byteEvent);
    void addSink(int host, int task);
    void addResourceDependecyID(int fromNode, int toNode, int linkId);
    void addTaskDependency(int fromId, int toId, double rho);

    vector<OperatorConnection*>& getApplicationTopology();
    void setApplicationTopology(
            vector<OperatorConnection*>& applicationTopology);
    vector<DestinationBranches>& getDstBranches();
    void setDstBranches(vector<DestinationBranches>& dstBranches);
    unordered_map<int, ResourceCapability>& getHostCapabilities();
    void setHostCapabilities(
            unordered_map<int, ResourceCapability>& hostCapabilities);
    unordered_map<int, LinkCapability>& getLinkCapabilities();
    void setLinkCapabilities(
            unordered_map<int, LinkCapability>& linkCapabilities);
    vector<OperatorConnectionMapping*>& getLinkMapping();
    void setLinkMapping(vector<OperatorConnectionMapping*>& linkMapping);
    unordered_map<int, LinkData*>& getLinks();
    void setLinks(unordered_map<int, LinkData*>& links);
    vector<NetworkConnection*>& getNetworkTopology();
    void setNetworkTopology(vector<NetworkConnection*>& networkTopology);
    vector<OperatorMapping*>& getOperatorMapping();
    void setOperatorMapping(vector<OperatorMapping*>& operatorMapping);
    unordered_map<int, OperatorData*>& getOperators();
    void setOperators(unordered_map<int, OperatorData*>& operators);
    unordered_map<int, ResourceCapability>& getResidualHostCapabilities();
    void setResidualHostCapabilities(
            unordered_map<int, ResourceCapability>& residualHostCapabilities);
    unordered_map<int, LinkCapability>& getResidualLinkCapabilities();
    void setResidualLinkCapabilities(
            unordered_map<int, LinkCapability>& residualLinkCapabilities);
    unordered_map<int, ResourceData*>& getResources();
    void setResources(unordered_map<int, ResourceData*>& resources);
    cRNG*& getRng();
    void setRng(cRNG*& rng);
    vector<DataSink*>& getSinks();
    void setSinks(vector<DataSink*>& sinks);
    vector<DataSource*>& getSources();
    void setSources(vector<DataSource*>& sources);
    vector<vector<int> >& getApplicationPaths();
    void setApplicationPaths(vector<vector<int> >& applicationPaths);
    vector<int>& getDeploymentSequence();
    void setDeploymentSequence(vector<int>& deploymentSequence);

    vector<int>& getCloudServers();
    void setCloudServers(vector<int>& cloudServers);
    vector<int>& getEdgeDevices();
    void setEdgeDevices(vector<int>& edgeDevices);
    vector<int>& getGateways();
    void setGateways(vector<int>& gateways);
    vector<int>& getDeployableHosts();
    void setDeployableHosts(vector<int>& deployableHosts);
    map<pair<int, int>, int>& getLinkConnections();
    void setLinkConnections(map<pair<int, int>, int>& linkConnections);
    unordered_map<int, vector<int> >& getPreviousOperators();
    void setPreviousOperators(
            unordered_map<int, vector<int> >& previousOperators);
    unordered_map<int, vector<int> >& getGtwDevices();
    void setGtwDevices(unordered_map<int, vector<int> >& gtwDevices);

protected:
    unordered_map<int, ResourceData*> mResources;
    unordered_map<int, LinkData*> mLinks;
    unordered_map<int, OperatorData*> mOperators;
    unordered_map<int, LinkCapability> mLinkCapabilities;
    unordered_map<int, ResourceCapability> mHostCapabilities;

    unordered_map<int, LinkCapability> mResidualLinkCapabilities;
    unordered_map<int, ResourceCapability> mResidualHostCapabilities;

    vector<NetworkConnection*> mNetworkTopology;
    vector<OperatorConnection*> mApplicationTopology;
    vector<DataSource*> mSources;
    vector<DataSink*> mSinks;
    vector<OperatorMapping*> mOperatorMapping;
    vector<OperatorConnectionMapping*> mLinkMapping;
    vector<DestinationBranches> mDstBranches; //<type, id> //type = 0 - cloud; 1 - edge
    cRNG *mRNG;

    vector<vector<int>> mApplicationPaths;

    vector<int> mDeploymentSequence;

    vector<int> mCloudServers;
    vector<int> mEdgeDevices;
    vector<int> mGateways;

    vector<int> mDeployableHosts;
    map<pair<int, int>, int> mLinkConnections;
    unordered_map<int, vector<int>> mPreviousOperators;
    unordered_map<int, vector<int>> mGtwDevices;
};

} /* namespace fogstream */

#endif /* ENV_ENV_H_ */

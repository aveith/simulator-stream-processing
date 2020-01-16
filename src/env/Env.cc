#include "Env.h"
#include "../util/Patterns.h"
#include "../util/graphS/Graph.h"

namespace fogstream {

Env::Env(cRNG* rng) {
    this->setRng(rng);

}

Env::~Env() {
    // TODO Auto-generated destructor stub
}

void Env::addResource(int id, int type, double computAvail, double memAvail,
        double cost, int slotNumber, int servers) {
    //Add host to the vector
    this->getResources().insert(
            make_pair(id, new ResourceData(id, type, cost)));

    //Add host capabilities to the vector
    this->getHostCapabilities().insert(
            make_pair(id,
                    ResourceCapability(id, memAvail, computAvail, slotNumber, servers)));

    if (type == Patterns::DeviceType::Sensor) {
        this->getEdgeDevices().push_back(id);
        this->getDeployableHosts().push_back(id);

    } else if (type == Patterns::DeviceType::Gateway) {
        this->getGateways().push_back(id);

    } else {
        this->getCloudServers().push_back(id);
        this->getDeployableHosts().push_back(id);

    }

}

void Env::addLink(int id, double bandwidth, double delay, double cost) {
    //Add link to the vector
    this->getLinks().insert(make_pair(id, new LinkData(id, delay, cost)));

    //Add link capabilities
    this->getLinkCapabilities().insert(
            make_pair(id, LinkCapability(id, bandwidth)));
}

void Env::addTask(int id, int fissionID, double selectivity,
        double dataTransferRate, double stateRate, int type, double computReq,
        double memReq, bool nextSplitted, int nextSplitLength,
        double timeWindow) {
    //Validate if the operator already exists on the set
    auto it = this->getOperators().find(id);
    if (it == this->getOperators().end()) {
        //If it does not exist the it will be added to the operator set
        //Add Operator - the operator does not require a counter because the ID comes from the xml
        this->getOperators().insert(
                make_pair(id,
                        new OperatorData(id, fissionID, selectivity,
                                dataTransferRate, stateRate, type, computReq,
                                memReq, timeWindow, nextSplitted,
                                nextSplitLength, -1, -1)));

        EV_WARN << "ID: " << id << " Selectivity: "
                       << to_string(
                               this->getOperators().at(id)->getSelectivity())
                       << " Data Rate: "
                       << to_string(
                               this->getOperators().at(id)->getDataTransferRate())
                       << " State Rate: "
                       << to_string(this->getOperators().at(id)->getStateRate())
                       << " Type: "
                       << to_string(this->getOperators().at(id)->getType())
                       << " CPU: "
                       << to_string(
                               this->getOperators().at(id)->getCPURequirement())
                       << " MEM: "
                       << to_string(
                               this->getOperators().at(id)->getMemoryRequirement())
                       << " TimeWindow: "
                       << to_string(
                               this->getOperators().at(id)->getTimeWindow())
                       << endl;
    }
}

void Env::addResourceDependecyID(int fromNode, int toNode, int linkId) {
    this->getNetworkTopology().push_back(
            new NetworkConnection(fromNode, toNode, linkId));

    this->getLinkConnections().insert(
            make_pair(make_pair(fromNode, toNode), linkId));

    //Determine the gateway connected to de sensor
    if (this->getResources().at(toNode)->getType()
            == Patterns::DeviceType::Gateway) {
        if (this->getResources().at(fromNode)->getType()
                == Patterns::DeviceType::Sensor) {
            this->getResources().at(fromNode)->setGatewayId(toNode);

            if (this->getGtwDevices().find(toNode)
                    != this->getGtwDevices().end()) {
                this->getGtwDevices().at(toNode).push_back(fromNode);
            } else {
                this->getGtwDevices().insert(
                        make_pair(toNode, vector<int>(1, fromNode)));
            }
        }
    }

}

void Env::addTaskDependency(int fromId, int toId, double rho) {
    this->getApplicationTopology().push_back(
            new OperatorConnection(fromId, toId, rho));

    if (this->getPreviousOperators().find(toId)
            != this->getPreviousOperators().end()) {
        this->getPreviousOperators().at(toId).push_back(fromId);
    } else {
        this->getPreviousOperators().insert(
                make_pair(toId, vector<int>(1, fromId)));
    }

}

void Env::addSource(int host, int task, double arrivalRate, double byteEvent) {
    this->getSources().push_back(
            new DataSource(host, task, arrivalRate, byteEvent));

}

void Env::addSink(int host, int task) {
    this->getSinks().push_back(new DataSink(host, task));
}

vector<OperatorConnection*>& Env::getApplicationTopology() {
    return mApplicationTopology;
}

void Env::setApplicationTopology(
        vector<OperatorConnection*>& applicationTopology) {
    mApplicationTopology = applicationTopology;
}

vector<DestinationBranches>& Env::getDstBranches() {
    return mDstBranches;
}

void Env::setDstBranches(vector<DestinationBranches>& dstBranches) {
    mDstBranches = dstBranches;
}

unordered_map<int, ResourceCapability>& Env::getHostCapabilities() {
    return mHostCapabilities;
}

void Env::setHostCapabilities(
        unordered_map<int, ResourceCapability>& hostCapabilities) {
    mHostCapabilities = hostCapabilities;
}

unordered_map<int, LinkCapability>& Env::getLinkCapabilities() {
    return mLinkCapabilities;
}

void Env::setLinkCapabilities(
        unordered_map<int, LinkCapability>& linkCapabilities) {
    mLinkCapabilities = linkCapabilities;
}

vector<OperatorConnectionMapping*>& Env::getLinkMapping() {
    return mLinkMapping;
}

void Env::setLinkMapping(vector<OperatorConnectionMapping*>& linkMapping) {
    mLinkMapping = linkMapping;
}

unordered_map<int, LinkData*>& Env::getLinks() {
    return mLinks;
}

void Env::setLinks(unordered_map<int, LinkData*>& links) {
    mLinks = links;
}

vector<NetworkConnection*>& Env::getNetworkTopology() {
    return mNetworkTopology;
}

void Env::setNetworkTopology(vector<NetworkConnection*>& networkTopology) {
    mNetworkTopology = networkTopology;
}

vector<OperatorMapping*>& Env::getOperatorMapping() {
    return mOperatorMapping;
}

void Env::setOperatorMapping(vector<OperatorMapping*>& operatorMapping) {
    mOperatorMapping = operatorMapping;
}

unordered_map<int, OperatorData*>& Env::getOperators() {
    return mOperators;
}

void Env::setOperators(unordered_map<int, OperatorData*>& operators) {
    mOperators = operators;
}

unordered_map<int, ResourceCapability>& Env::getResidualHostCapabilities() {
    return mResidualHostCapabilities;
}

void Env::setResidualHostCapabilities(
        unordered_map<int, ResourceCapability>& residualHostCapabilities) {
    mResidualHostCapabilities = residualHostCapabilities;
}

unordered_map<int, LinkCapability>& Env::getResidualLinkCapabilities() {
    return mResidualLinkCapabilities;
}

void Env::setResidualLinkCapabilities(
        unordered_map<int, LinkCapability>& residualLinkCapabilities) {
    mResidualLinkCapabilities = residualLinkCapabilities;
}

unordered_map<int, ResourceData*>& Env::getResources() {
    return mResources;
}

void Env::setResources(unordered_map<int, ResourceData*>& resources) {
    mResources = resources;
}

cRNG*& Env::getRng() {
    return mRNG;
}

void Env::setRng(cRNG*& rng) {
    mRNG = rng;
}

vector<DataSink*>& Env::getSinks() {
    return mSinks;
}

void Env::setSinks(vector<DataSink*>& sinks) {
    mSinks = sinks;
}

vector<DataSource*>& Env::getSources() {
    return mSources;
}

void Env::setSources(vector<DataSource*>& sources) {
    mSources = sources;
}

vector<vector<int> >& Env::getApplicationPaths() {
    return mApplicationPaths;
}

void Env::setApplicationPaths(vector<vector<int> >& applicationPaths) {
    mApplicationPaths = applicationPaths;
}

vector<int>& Env::getDeploymentSequence() {
    return mDeploymentSequence;
}

bool Env::getValidPath(int sourceHostId, int destinationHostId,
        double arrivalRate, double arrivalMsgSize, vector<int>& path,
        vector<int>& links) {

    vector<int> deployableHosts = this->determineDeployableHosts(sourceHostId,
            destinationHostId);

    Graph* graphApp = new Graph(this->getResources().size());
    for (unsigned int iLinks = 0; iLinks < this->getNetworkTopology().size();
            ++iLinks) {

        //Add nodes only if exists on the deployable list
        if (find(deployableHosts.begin(), deployableHosts.end(),
                this->getNetworkTopology().at(iLinks)->getSourceId())
                != deployableHosts.end()
                && find(deployableHosts.begin(), deployableHosts.end(),
                        this->getNetworkTopology().at(iLinks)->getDestinationId())
                        != deployableHosts.end()) {
            graphApp->addEdge(
                    this->getNetworkTopology().at(iLinks)->getSourceId(),
                    this->getNetworkTopology().at(iLinks)->getDestinationId());
        }

    }

    //Get all paths between the two nodes only considering the list of deployable hosts
    graphApp->printAllPaths(sourceHostId, destinationHostId);
    vector<vector<int>> paths = graphApp->getPaths();
    delete graphApp;

    path = this->evaluatePaths(paths, links, arrivalRate, arrivalMsgSize);
    return (path.size() > 0);

}

void Env::setDeploymentSequence(vector<int>& deploymentSequence) {
    mDeploymentSequence = deploymentSequence;
}

vector<int>& Env::getCloudServers() {
    return mCloudServers;
}

void Env::setCloudServers(vector<int>& cloudServers) {
    mCloudServers = cloudServers;
}

vector<int>& Env::getEdgeDevices() {
    return mEdgeDevices;
}

void Env::setEdgeDevices(vector<int>& edgeDevices) {
    mEdgeDevices = edgeDevices;
}

vector<int>& Env::getGateways() {
    return mGateways;
}

vector<int> Env::determineDeployableHosts(int sourceHostId,
        int destinationHostId) {
    vector<int> deployableHosts;
    deployableHosts.clear();
    //Add basic data
    deployableHosts.push_back(sourceHostId);
    deployableHosts.push_back(destinationHostId);

    //Add gateways
    if (this->getResources().at(sourceHostId)->getGatewayId() != -1) {
        deployableHosts.push_back(
                this->getResources().at(sourceHostId)->getGatewayId());
    }

    if (this->getResources().at(destinationHostId)->getGatewayId() != -1) {
        deployableHosts.push_back(
                this->getResources().at(destinationHostId)->getGatewayId());
    }

    //add hosts of sources
    for (unsigned int i = 0; i < this->getSources().size(); i++) {
        auto it = find(deployableHosts.begin(), deployableHosts.end(),
                this->getSources().at(i)->getHostId());
        if (it == deployableHosts.end()) {
            deployableHosts.push_back(this->getSources().at(i)->getHostId());
        }
    }

    //Add hosts of sinks
    for (unsigned int i = 0; i < this->getSinks().size(); i++) {
        auto it = find(deployableHosts.begin(), deployableHosts.end(),
                this->getSinks().at(i)->getHostId());
        if (it == deployableHosts.end()) {
            deployableHosts.push_back(this->getSinks().at(i)->getHostId());
        }
    }

    //Add a percentage of gateways as alternative paths
    int external_gateways = ceil(
            this->getGateways().size() * Patterns::PERCENTAGE_EXTERNAL_HOSTS);
    int iCounter = 0;
    if (external_gateways > 0) {
        for (unsigned int i = 0; i < this->getGateways().size(); i++) {
            auto it = find(deployableHosts.begin(), deployableHosts.end(),
                    this->getGateways().at(i));
            if (it == deployableHosts.end()) {
                deployableHosts.push_back(this->getGateways().at(i));
                iCounter++;

                if (iCounter == external_gateways)
                    break;
            }
        }
    }

    //Add a percentage of clouds as alternative paths
    int external_clouds = ceil(
            this->getCloudServers().size()
                    * Patterns::PERCENTAGE_EXTERNAL_HOSTS);
    if (external_gateways > 0) {
        iCounter = 0;
        for (unsigned int i = 0; i < this->getCloudServers().size(); i++) {
            auto it = find(deployableHosts.begin(), deployableHosts.end(),
                    this->getCloudServers().at(i));
            if (it == deployableHosts.end()) {
                deployableHosts.push_back(this->getCloudServers().at(i));
                iCounter++;

                if (iCounter == external_clouds)
                    break;
            }
        }
    }

    return deployableHosts;
}

vector<int> Env::evaluatePaths(vector<vector<int>> paths, vector<int>& links,
        double arrivalRate, double arrivalMsgSize) {
    int iBestPath = -1;
    double dBestTransferTime = std::numeric_limits<double>::max();

    //Validate if the paths have bandwidth to support the volume of data
    //  and get the one with the shortest latency
    for (unsigned int i = 0; i < paths.size(); ++i) {
        double transferTime = 0;
        bool bValid = true;
        vector<int> vLinks;
        for (unsigned int j = 1; j < paths.at(i).size(); ++j) {

            for (unsigned int iLink = 0;
                    iLink < this->getNetworkTopology().size(); ++iLink) {
                //Get the link between two nodes
                if (paths.at(i).at(j - 1)
                        == this->getNetworkTopology().at(iLink)->getSourceId()
                        && paths.at(i).at(j)
                                == this->getNetworkTopology().at(iLink)->getDestinationId()) {

                    //Get the links to apply to the queueing theory models
                    vLinks.push_back(
                            this->getNetworkTopology().at(iLink)->getLinkId());

                    //If the link has capability to support the mapping, it will estimate the transfer time
                    if (this->getResidualLinkCapabilities().at(
                            this->getNetworkTopology().at(iLink)->getLinkId()).getBandwidth()
                            > arrivalRate * arrivalMsgSize) {
                        transferTime +=
                                (arrivalRate * arrivalMsgSize)
                                        / this->getLinkCapabilities().at(
                                                this->getNetworkTopology().at(
                                                        iLink)->getLinkId()).getBandwidth();

                    } else {
                        bValid = false;
                    }

                    break;

                }
            }

            if (!bValid)
                break;
        }

        if (!bValid)
            continue;

        if (transferTime < dBestTransferTime) {
            dBestTransferTime = transferTime;
            iBestPath = i;
            links = vLinks;
        }
    }

    //Return the best path
    vector<int> bestpath;
    if (iBestPath != -1) {
        for (auto it = paths.at(iBestPath).begin();
                it != paths.at(iBestPath).end(); it++) {
            bestpath.push_back(*it);
        }
    }

    return bestpath;
}

bool Env::EstimateTimesandQueues(double serviceRate, double arrivalRate,
        double additionalOverhead, double &meanTimeinSystem,
        double &meanTimeinQueue, double &meanNumberofMsgsinSystem,
        double &meanNumberinQueue, double &rho) {

    rho = arrivalRate / serviceRate;

    meanTimeinSystem = (1 / (serviceRate - arrivalRate)) + additionalOverhead;
    meanTimeinQueue = rho / (serviceRate - arrivalRate);

    meanNumberofMsgsinSystem = rho / (1 - rho);
    meanNumberinQueue = rho * arrivalRate / (serviceRate - arrivalRate);

    return arrivalRate < serviceRate;

}

void Env::setGateways(vector<int>& gateways) {
    mGateways = gateways;
}

vector<int>& Env::getDeployableHosts() {
    return mDeployableHosts;
}

void Env::obtaingArrivalMetrics(int operatorId, double& arrivalRate,
        double& arrivalMsgSize) {
    int iCounter = 0;

    //Identify the upstream operators from operatorId, if exists in the previdousMappings
    // it means that the communication evaluation changed the values.
    for (unsigned int i = 0; i < this->getApplicationTopology().size(); i++) {

        if (this->getApplicationTopology().at(i)->getToOperatorId()
                == operatorId) {
            //Evaluate if communication changes the values of the operator mapping
            for (unsigned int j = 0; j < this->getOperatorMapping().size();
                    j++) {
                if (this->getApplicationTopology().at(i)->getFromOperatorId()
                        == this->getOperatorMapping().at(j)->getOperatorId()) {
                    iCounter++;
                    arrivalRate = arrivalRate
                            + this->getOperatorMapping().at(j)->getOutputRate();
                    arrivalMsgSize =
                            arrivalMsgSize
                                    + this->getOperatorMapping().at(j)->getOutputMsgSize();

                }
            }

        }
    }

    arrivalMsgSize = arrivalMsgSize / iCounter;
}

void Env::setDeployableHosts(vector<int>& deployableHosts) {
    mDeployableHosts = deployableHosts;
}

map<pair<int, int>, int>& Env::getLinkConnections() {
    return mLinkConnections;
}

void Env::setLinkConnections(map<pair<int, int>, int>& linkConnections) {
    mLinkConnections = linkConnections;
}

unordered_map<int, vector<int>>& Env::getPreviousOperators() {
    return mPreviousOperators;
}

void Env::setPreviousOperators(
        unordered_map<int, vector<int> >& previousOperators) {
    mPreviousOperators = previousOperators;
}

unordered_map<int, vector<int> >& Env::getGtwDevices() {
    return mGtwDevices;
}

void Env::setGtwDevices(unordered_map<int, vector<int> >& gtwDevices) {
    mGtwDevices = gtwDevices;
}

} /* namespace fogstream */


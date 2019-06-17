#include <algorithm>
#include <numeric>
#include <glob.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <array>
#include <unistd.h>

#include "schedulerMsg.h"
#include "../../../util/Patterns.h"
#include "../../msg/operator/Operator_m.h"

#include "../../msg/statistic/Statistic_m.h"
#include "../../msg/edges/Edges_m.h"
#include "../../../modules/simpleM/services/MsgQueue.h"
#include "../../../util/graphS/Graph.h"
#include "../../../modules/simpleM/services/MsgQueue.h"
#include "../../../modules/simpleM/services/storage/Storage.h"

#include "../../../reconfiguration/strategies/mc_uct/MCUCT.h"
#include "../../../reconfiguration/strategies/mc_sarsa/MCSARSA.h"
#include "../../../reconfiguration/strategies/qlearning/QLearning.h"
#include "../../../reconfiguration/strategies/mc_basicucb/MC_BasicUCB.h"
#include "../../../reconfiguration/strategies/mc_epsilongreedy/MCEpsilonGreedy.h"
#include "../../../reconfiguration/strategies/mc_tuneducb/MC_TunedUCB.h"

namespace fogstream {

Define_Module(SchedulerMsg);

void SchedulerMsg::initialize() {

    high_resolution_clock::time_point time = high_resolution_clock::now();
    this->setupEnvironment();
    duration<double> iteration_duration = high_resolution_clock::now() - time;
    cout << "Setup Environment - Total time: "
            << to_string(iteration_duration.count()) << "\n" << endl;

    time = high_resolution_clock::now();
    this->configuration();
    iteration_duration = high_resolution_clock::now() - time;
    cout << "Configuration policies - Total time: "
            << to_string(iteration_duration.count()) << "\n" << endl;

    if (this->getGenralEnv()->getOperatorMapping().size()
            != this->getGenralEnv()->getOperators().size()) {
        throw cRuntimeError(
                "Invalid sequence, the scheduler doesn't find a match  for all operators (Total %d and Mapped %d)",
                this->getGenralEnv()->getOperators().size(),
                this->getGenralEnv()->getOperatorMapping().size());
    }

    if (this->getParameters()->isReturnOnlyConfigurationDeployment()) {
        endSimulation();
    } else {
        this->CreateSimulation();
    }

    if (this->getParameters()->isReadFromTracedDataReconfig()) {
        time = high_resolution_clock::now();
        this->reconfiguration();
        iteration_duration = high_resolution_clock::now() - time;
        cout << "Reconfiguration - Total time: "
                << to_string(iteration_duration.count()) << "\n" << endl;
        endSimulation();
    }

    cMessage* appDeploy = new cMessage("APP-0");
    appDeploy->setKind(Patterns::MessageType::ApplicationDeployment);
    scheduleAt(simTime(), appDeploy);

}

vector<fix_vertex_position> SchedulerMsg::parse_position(
        const char* positions) {
    vector<fix_vertex_position> temp;

    cStringTokenizer tokenizer(positions);
    while (tokenizer.hasMoreTokens()) {
        std::vector<int> array =
                cStringTokenizer(tokenizer.nextToken(), "-").asIntVector();
        temp.push_back( { (long) array[0], (long) array[1] });
    }
    return temp;
}

void SchedulerMsg::handleMessage(cMessage *msg) {

    if (msg->isSelfMessage()
            && msg->getKind() == Patterns::MessageType::ApplicationDeployment) {
        delete msg;

        if (this->getGenralEnv()->getOperatorMapping().size() > 0) {
            this->ApplicationDeployment();

        } else {
            throw cRuntimeError("There is no application to deploy!");
        }

    } else if (msg->isSelfMessage()
            && msg->getKind()
                    == Patterns::MessageType::ReconfigurationScheduling) {
        if ((this->isStartReconfig()
                && simTime() > Patterns::TIME_FOR_RECONFIGURATION)
                || (this->getParameters()->isReadFromTracedDataReconfig())) {

            high_resolution_clock::time_point time =
                    high_resolution_clock::now();
            this->reconfiguration();
            duration<double> iteration_duration = high_resolution_clock::now()
                    - time;
            cout << "Reconfiguration - Total time: "
                    << to_string(iteration_duration.count()) << "\n" << endl;

            delete msg;
            endSimulation();

        } else {
            scheduleAt(simTime() + .01, msg);

        }

    } else if (!msg->isSelfMessage()
            && msg->getKind() == Patterns::MessageType::Statistics) {
        this->addDataToHistory(msg);
        delete msg;

    } else {

        delete msg;
    }

}

int SchedulerMsg::defineType(vector<edge_raw> edges, int cur) {
    int type = 0;

    int from = 0, to = 0;

    for (auto ee = edges.begin(); ee != edges.end(); ++ee) {
        if (ee->from == cur)
            ++from;
        if (ee->to == cur)
            ++to;
    }

    if (to > 1) {
        type = Patterns::ForkBranch;

    } else if (from > 1 && to > 1) {
        type = Patterns::JoinBranch;

    } else if (from > 1) {
        type = Patterns::Join;

    } else if (from == 1 && (to == 0 || to == 1)) {
        type = Patterns::Sequential;

    }

    //EV_INFO << " Vertex: " << cur << " type : " << type << endl;

    return type;
}

void SchedulerMsg::saveToFile(vector<int> vec, const char* file) {
    std::ofstream fout(file);
    for (auto x = vec.begin(); x != vec.end(); ++x) {
        fout << *x << '\n';
    }
    fout.close();
}
void SchedulerMsg::saveToFileString(std::string text, const char* file) {
    std::ofstream fout(file, std::ofstream::out | std::ofstream::app);
    fout << text << '\n';

    fout.close();
}

void SchedulerMsg::readDotApp(const char* appfile) {
    vector<edge_raw> edges = this->ReadAppEdgesFromDot(appfile);

    //App validation and
    if (!edges.empty()) {

        vector<int> sources = this->DefineSourcesFromDot(edges);

        vector<int> sinks = this->DefineSinksFromDot(edges);

        for (auto tt = edges.begin(); tt != edges.end(); ++tt) {

            this->AddOperatorUsingDot(edges, sources, tt->from);

            this->AddOperatorUsingDot(edges, sources, tt->to);

        }

        for (auto tt = edges.begin(); tt != edges.end(); ++tt) {
            this->getGenralEnv()->addTaskDependency(tt->from, tt->to, 1);
        }

        this->AddDataSourcesUsingDot(sources);

        this->AddDataSinksUsingDot(sinks);

    } else {
        throw cRuntimeError("The application must have a valid Dot file!");
    }

}

int SchedulerMsg::getMaxHopsSink(vector<int> sinks) {
    int sink = -1;
    //get all possible paths
    Graph* graphApp = new Graph(
            this->getGenralEnv()->getOperators().size() + 2);

    for (unsigned int i = 0;
            i < this->getGenralEnv()->getApplicationTopology().size(); ++i) {
        graphApp->addEdge(
                this->getGenralEnv()->getApplicationTopology().at(i)->getFromOperatorId(),
                this->getGenralEnv()->getApplicationTopology().at(i)->getToOperatorId());
    }

    //Add the dummy operator in front of the data sources
    for (unsigned int i = 0; i < this->getGenralEnv()->getSources().size();
            ++i) {
        graphApp->addEdge(this->getGenralEnv()->getOperators().size(),
                this->getGenralEnv()->getSources().at(i)->getOperatorId());
    }

    //Add the dummy operator after the data sinks
    for (auto ss = sinks.begin(); ss != sinks.end(); ++ss) {
        graphApp->addEdge(*ss, this->getGenralEnv()->getOperators().size() + 1);
    }

    graphApp->printAllPaths(this->getGenralEnv()->getOperators().size(),
            this->getGenralEnv()->getOperators().size() + 1);

    //    graphApp->printVector();
    vector<vector<int>> paths = graphApp->getPaths();

    int maxHops = 0;
    int pos = 0;
    for (unsigned int i = 0; i < paths.size(); ++i) {
        int hops = 0;
        for (unsigned int j = 0; j < paths.at(i).size(); ++j) {
            hops++;
        }

        if (hops > maxHops) {
            maxHops = hops;
            pos = i;
        }
    }
    sink = paths.at(pos).at(paths.at(pos).size() - 2);
    delete graphApp;
    return sink;
}

void SchedulerMsg::addDataToHistory(cMessage* msg) {
    Statistic* dataToSave = dynamic_cast<Statistic*>(msg);

    /*Operator statistics*/
    if (dataToSave->getType() == Patterns::StatisticType::Operator) {
        if (dataToSave->getMeasurement()
                == Patterns::OperatorMeasurement::Event) {
            this->getStatistics()->addEventHistory(dataToSave->getSTimestamp(),
                    dataToSave->getOperatorID(), dataToSave->getHostID(),
                    dataToSave->getMsgSize(), dataToSave->getCompTime(),
                    dataToSave->getInput());
        } else {
            this->getStatistics()->addQueueHistory(dataToSave->getSTimestamp(),
                    dataToSave->getOperatorID(), dataToSave->getHostID(),
                    dataToSave->getQueueSize(), dataToSave->getCommTime());
        }

    } else if (dataToSave->getType() == Patterns::StatisticType::Host) {
        this->getStatistics()->addHostHistory(dataToSave->getSTimestamp(),
                dataToSave->getHostID(), dataToSave->getQueueSize(),
                dataToSave->getMsgSize());

    } else if (dataToSave->getType() == Patterns::StatisticType::Path) {
        this->getStatistics()->addPathHistory(dataToSave->getSTimestamp(),
                dataToSave->getPathID(), dataToSave->getPathTime(),
                dataToSave->getCompTime(), dataToSave->getCommTime());

    } else if (dataToSave->getType()
            == Patterns::StatisticType::InternalState) {
        this->getStatistics()->addStateHistory(dataToSave->getSTimestamp(),
                dataToSave->getOperatorID(), dataToSave->getHostID(),
                dataToSave->getStateSize(),
                dataToSave->getRequiredTimeBuildWindow());
    }

}

vector<vector<int> > SchedulerMsg::getAllPossiblePaths(
        vector<OperatorConnection*> taskDependencies) {
    Graph* graphApp = new Graph(
            this->getGenralEnv()->getOperators().size() + 2);

    for (unsigned int i = 0;
            i < this->getGenralEnv()->getApplicationTopology().size(); ++i) {
        graphApp->addEdge(
                this->getGenralEnv()->getApplicationTopology().at(i)->getFromOperatorId(),
                this->getGenralEnv()->getApplicationTopology().at(i)->getToOperatorId());
    }

    //Add the dummy operator in front of the data sources
    for (unsigned int i = 0; i < this->getGenralEnv()->getSources().size();
            ++i) {
        graphApp->addEdge(this->getGenralEnv()->getOperators().size(),
                this->getGenralEnv()->getSources().at(i)->getOperatorId());
    }

    //Add the dummy operator after the data sinks
    for (unsigned int i = 0; i < this->getGenralEnv()->getSinks().size(); ++i) {
        graphApp->addEdge(
                this->getGenralEnv()->getSinks().at(i)->getOperatorId(),
                this->getGenralEnv()->getOperators().size() + 1);
    }

    graphApp->printAllPaths(this->getGenralEnv()->getOperators().size(),
            this->getGenralEnv()->getOperators().size() + 1);

    vector<vector<int>> paths = graphApp->getPaths();

    for (auto p = paths.begin(); p != paths.end(); ++p) {
        p->erase(
                remove(p->begin(), p->end(),
                        this->getGenralEnv()->getOperators().size()), p->end());
        p->erase(
                remove(p->begin(), p->end(),
                        this->getGenralEnv()->getOperators().size() + 1),
                p->end());
    }

    return paths;
}

void SchedulerMsg::createNetworkbyXML(const char* networkfile) {
    high_resolution_clock::time_point time = high_resolution_clock::now();
    this->FillEnvObjectsUsingXML(networkfile);

    duration<double> iteration_duration = high_resolution_clock::now() - time;
    cout << "---------- Setup Environment - Time to read the network XML: "
            << to_string(iteration_duration.count()) << endl;
    //
    //    time = high_resolution_clock::now();
    //    if (!this->getParameters()->isReturnOnlyConfigurationDeployment())
    //        this->createDynamicModules();
    //
    //    iteration_duration = high_resolution_clock::now() - time;
    //    cout
    //            << "---------- Setup Environment - Time to create the network modules: "
    //            << to_string(iteration_duration.count()) << endl;

}

void SchedulerMsg::createDynamicModules() {
    high_resolution_clock::time_point time = high_resolution_clock::now();
    this->CreateNodeModules();
    duration<double> iteration_duration = high_resolution_clock::now() - time;
    cout << "---------- Creating Simulation - Time to create node modules: "
            << to_string(iteration_duration.count()) << endl;

    time = high_resolution_clock::now();
    this->CreateNodeConnections();
    cout << "---------- Creating Simulation - Time to create node connections: "
            << to_string(iteration_duration.count()) << endl;
}

void SchedulerMsg::CreateNodeConnections() {
    //Create the connections between the hosts
    for (unsigned int it = 0;
            it < this->getGenralEnv()->getNetworkTopology().size(); ++it) {
        if (!this->getGenralEnv()->getResources().at(
                this->getGenralEnv()->getNetworkTopology().at(it)->getDestinationId())->isMapped()
                || !this->getGenralEnv()->getResources().at(
                        this->getGenralEnv()->getNetworkTopology().at(it)->getSourceId())->isMapped())
            continue;

        std::string pathModule =
                this->getParentModule()->getFullPath() + "."
                        + to_string(
                                this->getGenralEnv()->getNetworkTopology().at(
                                        it)->getSourceId()) + "-"
                        + to_string(
                                this->getGenralEnv()->getResources().at(
                                        this->getGenralEnv()->getNetworkTopology().at(
                                                it)->getSourceId())->getType());
        cModule* srcModule = this->getModuleByPath(pathModule.c_str());
        srcModule->setGateSize("outExt", srcModule->gateSize("outExt") + 1);

        pathModule =
                this->getParentModule()->getFullPath() + "."
                        + to_string(
                                this->getGenralEnv()->getNetworkTopology().at(
                                        it)->getDestinationId()) + "-"
                        + to_string(
                                this->getGenralEnv()->getResources().at(
                                        this->getGenralEnv()->getNetworkTopology().at(
                                                it)->getDestinationId())->getType());
        cModule* dstModule = this->getModuleByPath(pathModule.c_str());
        dstModule->setGateSize("inExt", dstModule->gateSize("inExt") + 1);

        string channelName =
                to_string(
                        this->getGenralEnv()->getNetworkTopology().at(it)->getSourceId())
                        + "->"
                        + to_string(
                                this->getGenralEnv()->getNetworkTopology().at(
                                        it)->getDestinationId());
        cDatarateChannel* channel = cDatarateChannel::create(
                channelName.c_str());
        channel->setDatarate(
                this->getGenralEnv()->getLinkCapabilities().at(
                        this->getGenralEnv()->getNetworkTopology().at(it)->getLinkId()).getBandwidth());
        channel->setDelay(
                this->getGenralEnv()->getLinks().at(
                        this->getGenralEnv()->getNetworkTopology().at(it)->getLinkId())->getDelay());

        srcModule->gate("outExt", srcModule->gateSize("outExt") - 1)->connectTo(
                dstModule->gate("inExt", dstModule->gateSize("inExt") - 1),
                channel);
        channel->callInitialize();

        //Create connections to the node manager
        pathModule = dstModule->getFullPath() + ".nodeManager";
        cModule* managerModule = this->getModuleByPath(pathModule.c_str());

        managerModule->setGateSize("inEx", managerModule->gateSize("inEx") + 1);
        dstModule->gate("inExt", dstModule->gateSize("inExt") - 1)->connectTo(
                managerModule->gate("inEx",
                        managerModule->gateSize("inEx") - 1));

        pathModule = srcModule->getFullPath() + ".nodeOutput";
        cModule* outputModule = this->getModuleByPath(pathModule.c_str());

        outputModule->setGateSize("outEx", outputModule->gateSize("outEx") + 1);
        outputModule->gate("outEx", outputModule->gateSize("outEx") - 1)->connectTo(
                srcModule->gate("outExt", srcModule->gateSize("outExt") - 1));

    }
}

void SchedulerMsg::CreateNodeModules() {
    for (auto it = this->getGenralEnv()->getResources().begin();
            it != this->getGenralEnv()->getResources().end(); ++it) {
        if (!it->second->isMapped())
            continue;

        cModuleType *mType = cModuleType::get(Patterns::NAME_PROCESS_HOST);

        if (mType) {
            string name = to_string(it->second->getId()) + "-"
                    + to_string(it->second->getType());

            cModule *module = mType->create(name.c_str(),
                    this->getParentModule());

            if (module) {
                module->buildInside();
                //module->callInitialize();
                module->scheduleStart(simTime());

                cDisplayString& dispstr = module->getDisplayString();
                dispstr.insertTag("is", 0);

                if (it->second->getType() == Patterns::DeviceType::Sensor) {
                    dispstr.setTagArg("is", 0, "s");

                }
                if (it->second->getType() == Patterns::DeviceType::Gateway) {
                    dispstr.setTagArg("is", 0, "l");

                }
                if (it->second->getType() == Patterns::DeviceType::Cloud) {
                    dispstr.setTagArg("is", 0, "vl");
                }

                module->par("avaliableCpu").setDoubleValue(
                        this->getGenralEnv()->getHostCapabilities().at(
                                it->second->getId()).getCpu());
                module->par("totalCpu").setDoubleValue(
                        this->getGenralEnv()->getHostCapabilities().at(
                                it->second->getId()).getCpu());
                module->par("avaliableMem").setDoubleValue(
                        this->getGenralEnv()->getHostCapabilities().at(
                                it->second->getId()).getMemory());
                module->par("totalMem").setDoubleValue(
                        this->getGenralEnv()->getHostCapabilities().at(
                                it->second->getId()).getMemory());
                module->par("id").setIntValue(
                        this->getGenralEnv()->getResources().at(
                                it->second->getId())->getId());

                this->SetupSchedulerConnection(module);

            } else {
                throw cRuntimeError("Error to create module");

            }
        } else {
            throw cRuntimeError("Error to create type module");
        }
    }
}

void SchedulerMsg::SetupSchedulerConnection(cModule *module) {
    //Create connection between modules to the scheduler, in order to
    // send the statistics
    std::string pathModule = this->getParentModule()->getFullPath()
            + ".scheduler";

    if (opp_strcmp(module->getModuleType()->getName(), "MsgQueue") == 0) {
        module->setGateSize("outIn", module->gateSize("outIn") + 1);
    } else {
        module->setGateSize("outExt", module->gateSize("outExt") + 1);
    }

    this->setGateSize("in", this->gateSize("in") + 1);
    string src(module->getName());
    string dst(this->getName());
    string channelName = src + "->" + dst;
    cDatarateChannel* channel = cDatarateChannel::create(channelName.c_str());
    //                    channel->setDatarate(it->link.bandwidth);
    channel->setDelay(0);

    if (opp_strcmp(module->getModuleType()->getName(), "MsgQueue") == 0) {
        module->gate("outIn", module->gateSize("outIn") - 1)->connectTo(
                this->gate("in", this->gateSize("in") - 1), channel);
    } else {
        module->gate("outExt", module->gateSize("outExt") - 1)->connectTo(
                this->gate("in", this->gateSize("in") - 1), channel);
    }
    channel->callInitialize();

    if (opp_strcmp(module->getModuleType()->getName(), "NodeMsg") == 0) {
        string pathModule = module->getFullPath() + ".nodeOutput";
        cModule* outputModule = this->getModuleByPath(pathModule.c_str());
        outputModule->setGateSize("outEx", outputModule->gateSize("outEx") + 1);
        outputModule->gate("outEx", outputModule->gateSize("outEx") - 1)->connectTo(
                module->gate("outExt", module->gateSize("outExt") - 1));
    }

    //Create connection between the the scheduler and the modules, in other to manage
    // the infrastructure

    this->setGateSize("out", this->gateSize("out") + 1);
    dst = module->getName();
    src = this->getName();

    channelName = src + "->" + dst;
    cDatarateChannel* channelFromScheduler = cDatarateChannel::create(
            channelName.c_str());
    channelFromScheduler->setDelay(0);

    if (opp_strcmp(module->getModuleType()->getName(), "NodeMsg") == 0) {
        module->setGateSize("inExt", module->gateSize("inExt") + 1);
        this->gate("out", this->gateSize("out") - 1)->connectTo(
                module->gate("inExt", module->gateSize("inExt") - 1),
                channelFromScheduler);
    } else {
        module->setGateSize("inIn", module->gateSize("inIn") + 1);
        this->gate("out", this->gateSize("out") - 1)->connectTo(
                module->gate("inIn", module->gateSize("inIn") - 1),
                channelFromScheduler);
    }

    if (opp_strcmp(module->getModuleType()->getName(), "NodeMsg") == 0) {
        cModule* managerModule = this->getModuleByPath(
                (module->getFullPath() + ".nodeManager").c_str());

        managerModule->setGateSize("inEx", managerModule->gateSize("inEx") + 1);
        module->gate("inExt", module->gateSize("inExt") - 1)->connectTo(
                managerModule->gate("inEx",
                        managerModule->gateSize("inEx") - 1));
    }
    channelFromScheduler->callInitialize();
}

void SchedulerMsg::createAppbyXML(const char* appfile) {
    xmlDocPtr doc = xmlReadFile(appfile, NULL, 0);
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode;

    int iSinkLocated = -1;
    int iSourceLocated = -1;
    while (cur != NULL) {
        if (cur->type == XML_ELEMENT_NODE
                && !xmlStrcmp(cur->name, (const xmlChar *) "vertex")) {
            int id = atoi((char*) xmlGetProp(cur, (xmlChar*) "id"));
            double selectivity = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "selectivity"));
            double dataTransferRate = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "transferrate"));
            double stateRate = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "staterate"));
            int type = atoi((char*) xmlGetProp(cur, (xmlChar*) "type"));
            double computReq = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "computreq"));
            double memReq = atof((char*) xmlGetProp(cur, (xmlChar*) "memreq"));

            double timeWindow = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "timewindow"));

            this->getGenralEnv()->addTask(id, 0, selectivity, dataTransferRate,
                    stateRate, type, computReq, memReq, false, 0, timeWindow);

        }

        if (cur->type == XML_ELEMENT_NODE
                && !xmlStrcmp(cur->name, (const xmlChar *) "edge")) {
            int src_id = atoi((char*) xmlGetProp(cur, (xmlChar*) "src_id"));
            int dst_id = atoi((char*) xmlGetProp(cur, (xmlChar*) "dst_id"));
            double rho = atof((char*) xmlGetProp(cur, (xmlChar*) "rho"));
            this->getGenralEnv()->addTaskDependency(src_id, dst_id, rho);

        }

        //setup source
        if (cur->type == XML_ELEMENT_NODE
                && !xmlStrcmp(cur->name, (const xmlChar *) "source")) {
            int host_id = atoi((char*) xmlGetProp(cur, (xmlChar*) "host_id"));
            int vertex_id = atoi(
                    (char*) xmlGetProp(cur, (xmlChar*) "vertex_id"));
            long arrivalrate = atol(
                    (char*) xmlGetProp(cur, (xmlChar*) "arrivalrate"));
            long byteevent = atol(
                    (char*) xmlGetProp(cur, (xmlChar*) "byteevent"));

            if ((this->getParameters()->getSourceLocations()
                    == Patterns::XMLLocations::CentralizedCloud
                    && iSourceLocated == -1
                    || this->getParameters()->getSourceLocations()
                            == Patterns::XMLLocations::CentralizedEdge
                            && iSourceLocated == -1)
                    || (this->getParameters()->getSourceLocations()
                            == Patterns::XMLLocations::AllRandom
                            || this->getParameters()->getSourceLocations()
                                    == Patterns::XMLLocations::DescentralizedCloud
                            || this->getParameters()->getSourceLocations()
                                    == Patterns::XMLLocations::DescentralizedEdge)) {

                host_id = this->DefineXMLSourceSinkLocations(
                        this->getParameters()->getSourceLocations());
                iSourceLocated = host_id;
            } else if (this->getParameters()->getSourceLocations()
                    == Patterns::XMLLocations::CentralizedCloud
                    && iSourceLocated != -1
                    || this->getParameters()->getSourceLocations()
                            == Patterns::XMLLocations::CentralizedEdge
                            && iSourceLocated != -1) {
                host_id = iSourceLocated;
            }

            this->getGenralEnv()->addSource(host_id, vertex_id, arrivalrate,
                    byteevent);
        }

        //setup sink
        if (cur->type == XML_ELEMENT_NODE
                && !xmlStrcmp(cur->name, (const xmlChar *) "sink")) {
            int host_id = atoi((char*) xmlGetProp(cur, (xmlChar*) "host_id"));
            int vertex_id = atoi(
                    (char*) xmlGetProp(cur, (xmlChar*) "vertex_id"));

            if ((this->getParameters()->getSinkLocations()
                    == Patterns::XMLLocations::CentralizedCloud
                    && iSinkLocated == -1
                    || this->getParameters()->getSinkLocations()
                            == Patterns::XMLLocations::CentralizedEdge
                            && iSinkLocated == -1)
                    || (this->getParameters()->getSinkLocations()
                            == Patterns::XMLLocations::AllRandom
                            || this->getParameters()->getSinkLocations()
                                    == Patterns::XMLLocations::DescentralizedCloud
                            || this->getParameters()->getSinkLocations()
                                    == Patterns::XMLLocations::DescentralizedEdge)) {
                host_id = this->DefineXMLSourceSinkLocations(
                        this->getParameters()->getSinkLocations());
                iSinkLocated = host_id;
            } else if (this->getParameters()->getSinkLocations()
                    == Patterns::XMLLocations::CentralizedCloud
                    && iSinkLocated != -1
                    || this->getParameters()->getSinkLocations()
                            == Patterns::XMLLocations::CentralizedEdge
                            && iSinkLocated != -1) {
                host_id = iSinkLocated;
            }
            this->getGenralEnv()->addSink(host_id, vertex_id);

        }

        cur = cur->next;
    }
    xmlFreeDoc(doc);
}
cModule* SchedulerMsg::createSpecificModule(const char* moduletype, int hostID,
        int vertexID, int direction) {
    cModule* module;

    std::string pathModule = this->getParentModule()->getFullPath() + "."
            + to_string(
                    this->getGenralEnv()->getResources().at(hostID)->getId())
            + "-"
            + to_string(
                    this->getGenralEnv()->getResources().at(hostID)->getType());

    cModuleType *mType = cModuleType::get(moduletype);

    if (mType) {
        string name = "vertex-" + to_string(vertexID);

        module = mType->create(name.c_str(), this->getParentModule());

        if (module) {
            module->buildInside();
            //module->callInitialize();
            module->scheduleStart(simTime());

            if (direction == 0) {
                module->setGateSize("outExt", module->gateSize("outExt") + 1);

                cModule* dstModule = this->getModuleByPath(pathModule.c_str());
                dstModule->setGateSize("inExt",
                        dstModule->gateSize("inExt") + 1);

                module->gate("outExt", module->gateSize("outExt") - 1)->connectTo(
                        dstModule->gate("inExt",
                                dstModule->gateSize("inExt") - 1));

                //Add connection to the node manager
                pathModule = pathModule + ".nodeManager";
                cModule* managerModule = this->getModuleByPath(
                        pathModule.c_str());

                managerModule->setGateSize("inEx",
                        managerModule->gateSize("inEx") + 1);
                dstModule->gate("inExt", dstModule->gateSize("inExt") - 1)->connectTo(
                        managerModule->gate("inEx",
                                managerModule->gateSize("inEx") - 1));

            } else {
                module->setGateSize("inIn", module->gateSize("inIn") + 1);

                cModule* dstModule = this->getModuleByPath(pathModule.c_str());
                dstModule->setGateSize("outExt",
                        dstModule->gateSize("outExt") + 1);

                dstModule->gate("outExt", dstModule->gateSize("outExt") - 1)->connectTo(
                        module->gate("inIn", module->gateSize("inIn") - 1));

                //Add connection to the node output
                pathModule = pathModule + ".nodeOutput";
                cModule* outputModule = this->getModuleByPath(
                        pathModule.c_str());

                outputModule->setGateSize("outEx",
                        outputModule->gateSize("outEx") + 1);

                outputModule->gate("outEx", outputModule->gateSize("outEx") - 1)->connectTo(
                        dstModule->gate("outExt",
                                dstModule->gateSize("outExt") - 1));

                this->SetupSchedulerConnection(module);

            }

            return module;

        } else {
            throw cRuntimeError("Error to create module");
        }
    } else {
        throw cRuntimeError("Error to create type module");
    }

    return module;
}

void SchedulerMsg::addVertex(cQueue *verticesQueue, int operatorId,
        int fissionId, float seletivity, float dataReduction, int type,
        float stateIncreaseRate, float memoryCost, float cpuCost,
        bool nextSplitted, int nextSplitLength, const char* appName,
        float availableCPU, float availableMem, double timeWindow) {

    char nameOpe[1024];
    snprintf(nameOpe, 1023, "%d", operatorId);

    Operator* vertexApp = new Operator(nameOpe);
    vertexApp->setOperatorId(operatorId);
    vertexApp->setFissionId(fissionId);
    vertexApp->setSelectivity(seletivity);
    vertexApp->setDataReduction(dataReduction);
    vertexApp->setType(type);
    vertexApp->setStateIncreaseRate(stateIncreaseRate);
    vertexApp->setMemoryCost(memoryCost);
    vertexApp->setCpuCost(cpuCost);

    vertexApp->setNextSplitted(nextSplitted);
    vertexApp->setNextSplitLength(nextSplitLength);
    vertexApp->setAppName(appName);
    vertexApp->setAvailableCPU(availableCPU);
    vertexApp->setAvailableMem(availableMem);
    vertexApp->setTimeWindow(timeWindow);

    verticesQueue->insert(vertexApp);

}

void SchedulerMsg::addEdge(cQueue *edgesQueue, const char* dstHost,
        int nextState, int nextSubState, int curState, int curSubState,
        const char* appName) {
    char nameOpe[1024];
    snprintf(nameOpe, 1023, "%d-%d", nextState, nextSubState);
    Edges* edgeApp = new Edges(nameOpe);
    edgeApp->setDstHostName(dstHost);
    edgeApp->setNextState(nextState);
    edgeApp->setNextSubState(nextSubState);
    edgeApp->setCurState(curState);
    edgeApp->setCurSubState(curSubState);
    edgeApp->setAppName(appName);

    edgesQueue->insert(edgeApp);
}

void SchedulerMsg::addDep(cQueue *depQueue, const char* appName, int curState,
        int curSubState, int nextState, int nextSubState) {
    char nameOpe[1024];
    snprintf(nameOpe, 1023, "%d-%d", curState, curSubState);
    OperatorDep* operatorDep = new OperatorDep(nameOpe);
    operatorDep->setAppName(appName);
    operatorDep->setCurState(curState);
    operatorDep->setCurSubState(curSubState);
    operatorDep->setNextState(nextState);
    operatorDep->setNextSubState(nextSubState);

    depQueue->insert(operatorDep);
}

bool SchedulerMsg::isStartReconfig() {
    int pathNumber = 0;
    for (unsigned int iSink = 0;
            iSink < this->getGenralEnv()->getSinks().size(); iSink++) {
        for (unsigned int i = 0;
                i < this->getGenralEnv()->getApplicationPaths().size(); i++) {
            cModule* storageModule =
                    this->getModuleByPath(
                            (this->getParentModule()->getFullPath() + "."
                                    + "vertex-"
                                    + to_string(
                                            this->getGenralEnv()->getSinks().at(
                                                    iSink)->getOperatorId())
                                    + "." + "path-" + to_string(i)).c_str());
            if (storageModule) {
                Storage *storage = check_and_cast<Storage *>(storageModule);
                if (storage->getRcvMsgs()
                        < Patterns::SINK_QUEUE_SIZE_RECONFIG) {
                    return false;
                } else {
                    pathNumber++;

                }
            }

        }

    }

    return (pathNumber
            == (int) this->getGenralEnv()->getApplicationPaths().size()
            && this->getGenralEnv()->getApplicationPaths().size() > 0);

}

Parameters*& SchedulerMsg::getParameters() {
    return mParameters;
}

void SchedulerMsg::setParameters(Parameters* parameters) {
    mParameters = parameters;
}

void SchedulerMsg::loadParameters() {
    this->setParameters(new Parameters());
    this->getParameters()->setDirectoryToSaveFiles(
            par("dir_result").stdstringValue());
    this->getParameters()->setBaseStrategy(par("base_strategy").intValue());

    if (opp_strcmp(par("applicationxml"), "") == 0) {
        this->getParameters()->setApp(par("applicationdot").stdstringValue());
    } else {
        this->getParameters()->setApp(par("applicationxml").stdstringValue());
    }

    this->getParameters()->setNetwork(par("networkxml").stdstringValue());

    this->getParameters()->setSelectivityMin(
            par("selectivity_min").doubleValue());
    this->getParameters()->setSelectivityMax(
            par("selectivity_max").doubleValue());

    this->getParameters()->setDataRateMin(par("datarate_min").doubleValue());
    this->getParameters()->setDataRateMax(par("datarate_max").doubleValue());

    this->getParameters()->setCpuMin(par("cpu_min").doubleValue());
    this->getParameters()->setCpuMax(par("cpu_max").doubleValue());

    this->getParameters()->setCpuSrcMin(par("cpusrc_min").doubleValue());
    this->getParameters()->setCpuSrcMax(par("cpusrc_max").doubleValue());

    this->getParameters()->setMemoryMin(par("memory_min").doubleValue());
    this->getParameters()->setMemoryMax(par("memory_max").doubleValue());

    this->getParameters()->setArrivalRateMin(
            par("arrivalrate_min").doubleValue());
    this->getParameters()->setArrivalRateMax(
            par("arrivalrate_max").doubleValue());

    this->getParameters()->setBytesMsgMin(par("bytesmsg_min").doubleValue());
    this->getParameters()->setBytesMsgMax(par("bytesmsg_max").doubleValue());

    this->getParameters()->setStateRateMin(par("staterate_min").doubleValue());
    this->getParameters()->setStateRateMax(par("staterate_max").doubleValue());

    this->getParameters()->setWindowMin(par("window_min").doubleValue());
    this->getParameters()->setWindowMax(par("window_max").doubleValue());
    this->getParameters()->setOperatorsWithStatePerc(
            par("operators_with_state_perc").doubleValue());

    this->getParameters()->setVertexPosition(
            par("vertex_positions").stdstringValue());

    this->getParameters()->setSaveTracedDataReconfig(
            par("save_traced_data_reconfig").boolValue());
    this->getParameters()->setReadFromTracedDataReconfig(
            par("read_from_traced_data_reconfig").boolValue());
    this->getParameters()->setFinishAfterTracing(
            par("finish_after_tracing").boolValue());

    this->getParameters()->setReconfigConstant(
            par("reconfig_constant").doubleValue());
    this->getParameters()->setReconfigAlpha(
            par("reconfig_alpha").doubleValue());
    this->getParameters()->setReconfigGamma(
            par("reconfig_gamma").doubleValue());
    this->getParameters()->setReconfigLambda(
            par("reconfig_lambda").doubleValue());
    this->getParameters()->setReconfigEpsilon(
            par("reconfig_epsilon").doubleValue());

    this->getParameters()->setReconfigStrategy(
            par("reconfig_strategy").intValue());
    this->getParameters()->setReconfigIterations(
            par("reconfig_iterations").intValue());

    this->getParameters()->setDirTracedDatasets(
            par("dir_trace_datasets").stdstringValue());

    this->getParameters()->setReconfigApplyingRp(
            par("reconfig_applying_rp").boolValue());

    this->getParameters()->setLatencyWeight(
            par("latency_weight").doubleValue());
    this->getParameters()->setMigrationWeight(
            par("migration_weight").doubleValue());
    this->getParameters()->setCostsWeight(par("costs_weight").doubleValue());
    this->getParameters()->setWanTrafficWeight(
            par("wan_traffic_weight").doubleValue());
    this->getParameters()->setLatencyParameter(
            par("latency_parameter").doubleValue());
    this->getParameters()->setMigrationParameter(
            par("migration_parameter").doubleValue());
    this->getParameters()->setCostsParameter(
            par("costs_parameter").doubleValue());
    this->getParameters()->setWanTrafficParamenter(
            par("wan_traffic_paramenter").doubleValue());

    this->getParameters()->setPricingType(par("pricing_type").intValue());
    this->getParameters()->setAggregateCostBasedReward(
            par("aggregate_cost_based_reward").boolValue());
    this->getParameters()->setConfigurationSeeds(
            par("configuration_seeds").intValue());

    this->getParameters()->setTestId(par("test_id").stdstringValue());
    this->getParameters()->setReturnOnlyConfigurationDeployment(
            par("return_only_configuration_deployment").boolValue());

    this->getParameters()->setFileOperatorStatistics(
            par("file_operator_statistics").stdstringValue() == "$" ?
                    "" : par("file_operator_statistics").stdstringValue());
    this->getParameters()->setFilePathStatistics(
            par("file_path_statistics").stdstringValue() == "@" ?
                    "" : par("file_path_statistics").stdstringValue());

    this->getParameters()->setSourceLocations(
            par("source_locations").intValue());
    this->getParameters()->setSinkLocations(par("sink_locations").intValue());

    this->getParameters()->setUseSlots(par("use_slots").boolValue());
    this->getParameters()->setActionHeuristic(
            par("action_heuristic").intValue());

    this->getParameters()->setTimeReconfiguration(
                par("time_reconfiguration").intValue());

}

void SchedulerMsg::reconfiguration() {

    this->getGenralEnv()->getDeploymentSequence() = this->GetOperatorsList();

    if (!this->getParameters()->isReadFromTracedDataReconfig()) {
        this->getReconfigEnv() = new Environment(this->getGenralEnv(),
                this->getStatistics());

    } else {
        this->getReconfigEnv() = new Environment(this->getGenralEnv(),
                this->getParameters());
    }

    if (this->getParameters()->isSaveTracedDataReconfig()) {
        this->getReconfigEnv()->saveTraceFile(this->getParameters(),
                this->GetNumberMessagesPaths());

        if (this->getParameters()->isFinishAfterTracing()) {
            endSimulation();
        }

    }

    LogMCTS* logExec = new LogMCTS(Patterns::ExecutionLogType::IterationLog,
            this->getParameters()->getDirectoryToSaveFiles()
                    + this->getParameters()->getTestId(),
            this->getParameters());

    logExec->setNumberPaths(this->getGenralEnv()->getApplicationPaths().size());
    logExec->setNumberSites(
            this->getGenralEnv()->getCloudServers().size()
                    + this->getGenralEnv()->getGateways().size());

    if (this->getParameters()->getReconfigStrategy()
            == Patterns::ReconfigurationStrategies::MC_UCT) {
        MC_UCT mc = MC_UCT(this->getRNG(25), this->getReconfigEnv(), logExec);
        mc.execute(this->getParameters()->getReconfigIterations());

    } else if (this->getParameters()->getReconfigStrategy()
            == Patterns::ReconfigurationStrategies::MC_SARSA) {
        MC_SARSA mc = MC_SARSA(this->getRNG(25), this->getReconfigEnv(),
                logExec);
        mc.execute(this->getParameters()->getReconfigIterations());

    } else if (this->getParameters()->getReconfigStrategy()
            == Patterns::ReconfigurationStrategies::QLearning) {
        QLearning mc = QLearning(this->getRNG(25), this->getReconfigEnv(),
                logExec);
        mc.execute(this->getParameters()->getReconfigIterations());

    } else if (this->getParameters()->getReconfigStrategy()
            == Patterns::ReconfigurationStrategies::MC_EpsilonGreedy) {
        MC_EpsilonGreedy mc = MC_EpsilonGreedy(this->getRNG(25),
                this->getReconfigEnv(), logExec);
        mc.execute(this->getParameters()->getReconfigIterations());

    } else if (this->getParameters()->getReconfigStrategy()
            == Patterns::ReconfigurationStrategies::MC_BasicUCB) {
        MC_BasicUCB mc = MC_BasicUCB(this->getRNG(25), this->getReconfigEnv(),
                logExec);
        mc.execute(this->getParameters()->getReconfigIterations());

    } else if (this->getParameters()->getReconfigStrategy()
            == Patterns::ReconfigurationStrategies::MC_TunedUCB) {
        MC_TunedUCB mc = MC_TunedUCB(this->getRNG(25), this->getReconfigEnv(),
                logExec);
        mc.execute(this->getParameters()->getReconfigIterations());

    }
}

Env*& SchedulerMsg::getGenralEnv() {
    return mGenralEnv;
}

void SchedulerMsg::setGenralEnv(Env*& genralEnv) {
    mGenralEnv = genralEnv;
}

Environment*& SchedulerMsg::getReconfigEnv() {
    return mReconfigEnv;
}

void SchedulerMsg::setReconfigEnv(Environment*& reconfigEnv) {
    mReconfigEnv = reconfigEnv;
}

void SchedulerMsg::setupEnvironment() {
    //Load parameters from the ned file and
    //  fill the object parameter
    this->loadParameters();

    std::string path = "";
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        path = cwd;
    } else {
        throw cRuntimeError("Current directory %s", cwd);
    }

    /*Setup environment*/
    Env* generalenv = new Env(this->getRNG(4));
    this->setGenralEnv(generalenv);

    this->createNetworkbyXML(
            (path + this->getParameters()->getNetwork()).c_str());

    if (opp_strcmp(this->getParameters()->getVertexPosition().c_str(), "")
            > 0) {
        this->setVertexPositions(
                this->parse_position(
                        this->getParameters()->getVertexPosition().c_str()));
    }

    high_resolution_clock::time_point time = high_resolution_clock::now();

    if (this->getParameters()->getApp().find(".dot") != std::string::npos) {
        this->readDotApp((path + this->getParameters()->getApp()).c_str());

    } else {
        this->createAppbyXML((path + this->getParameters()->getApp()).c_str());

    }
    duration<double> iteration_duration = high_resolution_clock::now() - time;
    cout << "----- Setup Environment - Time to create the app: "
            << to_string(iteration_duration.count()) << endl;

    this->getGenralEnv()->getApplicationPaths() = this->getAllPossiblePaths(
            this->getGenralEnv()->getApplicationTopology());

    this->getStatistics() = new StatisticsMCTS();

}

void SchedulerMsg::configuration() {
    high_resolution_clock::time_point time = high_resolution_clock::now();

    Configuration* configuration = new Configuration(this->getGenralEnv());
    configuration->setupEnvironment(this->getParameters()->getBaseStrategy(),
            this->getParameters()->isUseSlots());

    duration<double> iteration_duration = high_resolution_clock::now() - time;
    cout
            << "----- Configuration policies - Time to determine the operator placements: "
            << to_string(iteration_duration.count()) << endl;

    cout << "\noperator_id\thost_id" << endl;
    for (unsigned int i = 0;
            i < this->getGenralEnv()->getOperatorMapping().size(); i++) {
        cout
                << to_string(
                        this->getGenralEnv()->getOperatorMapping().at(i)->getOperatorId())
                << "\t"
                << to_string(
                        this->getGenralEnv()->getOperatorMapping().at(i)->getHostId())
                << endl;
    }
    this->SaveConfigurationFile();
    delete configuration;
}

vector<int> SchedulerMsg::GetOperatorsList() {
    vector<int> orderOperatorList;
    for (unsigned int it = 0;
            it < this->getGenralEnv()->getOperatorMapping().size(); ++it) {

        if (this->getParameters()->isReconfigApplyingRp()) {
            bool bEdge = false;

            for (auto dst = this->getGenralEnv()->getDstBranches().begin();
                    dst != this->getGenralEnv()->getDstBranches().end();
                    ++dst) {
                for (unsigned int i = 0; i < dst->getApplicationBranch().size();
                        ++i) {
                    if (dst->getApplicationBranch().at(i)
                            == this->getGenralEnv()->getOperatorMapping().at(it)->getOperatorId()) {
                        if (dst->getType() == 1) {
                            bEdge = true;
                            break;
                        }
                    }
                }

            }

            if (bEdge) {
                orderOperatorList.push_back(
                        this->getGenralEnv()->getOperatorMapping().at(it)->getOperatorId());
            }
        } else {

            orderOperatorList.push_back(
                    this->getGenralEnv()->getOperatorMapping().at(it)->getOperatorId());
        }
    }

    return orderOperatorList;
}

void SchedulerMsg::FillEnvObjectsUsingXML(const char* networkfile) {
    xmlDocPtr doc = xmlReadFile(networkfile, NULL, 0);
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    cur = cur->xmlChildrenNode;

    double latency_WAN = uniform((double) par("wanlat_min"),
            (double) par("wanlat_max"), 3);

    while (cur != NULL) {
        if (cur->type == XML_ELEMENT_NODE
                && !xmlStrcmp(cur->name, (const xmlChar *) "host")) {
            int id = atoi((char*) xmlGetProp(cur, (xmlChar*) "id"));
            int type = atoi((char*) xmlGetProp(cur, (xmlChar*) "type"));
            double cpu = atof((char*) xmlGetProp(cur, (xmlChar*) "cpu"));
            double mem = atof((char*) xmlGetProp(cur, (xmlChar*) "mem"));
            double cost = atof((char*) xmlGetProp(cur, (xmlChar*) "cost"));
            int slots = atoi((char*) xmlGetProp(cur, (xmlChar*) "slots"));
            this->getGenralEnv()->addResource(id, type, cpu, mem, cost, slots);
        }

        if (cur->type == XML_ELEMENT_NODE
                && !xmlStrcmp(cur->name, (const xmlChar *) "link")) {
            int id = atoi((char*) xmlGetProp(cur, (xmlChar*) "id"));
            double bandwidth = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "bandwidth"));
            double latency = atof(
                    (char*) xmlGetProp(cur, (xmlChar*) "latency"));
            double cost = atof((char*) xmlGetProp(cur, (xmlChar*) "cost"));
            int src_id = atoi((char*) xmlGetProp(cur, (xmlChar*) "src_id"));
            int dst_id = atoi((char*) xmlGetProp(cur, (xmlChar*) "dst_id"));

            int src_type =
                    this->getGenralEnv()->getResources().at(src_id)->getType();
            int dst_type =
                    this->getGenralEnv()->getResources().at(dst_id)->getType();

            if ((src_type == 0 && dst_type == 1)
                    || (src_type == 1 && dst_type == 0)) {
                latency = uniform((double) par("lanlat_min"),
                        (double) par("lanlat_max"), 1);
                if (src_type == 1 && dst_type == 0) {
                    bandwidth = 1000000000000000000;
                    latency = 0;
                }
            } else if (src_type == 1 && dst_type == 1) {
                //                    latency = uniform((double) par("manlat_min"), (double) par("manlat_max"), 2);
                latency = latency_WAN;
            } else if ((src_type == 1 && dst_type == 2)
                    || (src_type == 2 && dst_type == 1)) {
                //                    latency = uniform((double) par("wanlat_min"), (double) par("wanlat_max"), 3);
                latency = latency_WAN;
            }
            //            }

            EV_INFO << "-*/*/********************************" << src_id << "-"
                           << dst_id << " " << latency << " Btw: " << bandwidth
                           << endl;

            this->getGenralEnv()->addLink(id, bandwidth, latency, cost);
            this->getGenralEnv()->addResourceDependecyID(src_id, dst_id, id);

        }

        cur = cur->next;
    }
    xmlFreeDoc(doc);
}

vector<edge_raw> SchedulerMsg::ReadAppEdgesFromDot(const char* appfile) {

    vector<edge_raw> edges;

    ifstream appDot;
    appDot.open(appfile);

    std::string output;

    if (appDot.is_open()) {
        bool bFirst = true;
        long first = -1;
        while (!appDot.eof()) {
            appDot >> output;
            output = find_and_replace(output, ";}", "");
            output = find_and_replace(output, ";", "");
            if (is_number(output)) {
                if (bFirst) {
                    first = stol(output);
                    bFirst = false;
                } else {
                    long last = stol(output);
                    edges.push_back( { first, last });
                    bFirst = true;
                }
            }
        }
    }
    appDot.close();

    return edges;
}

vector<int> SchedulerMsg::DefineSourcesFromDot(vector<edge_raw> edges) {
    vector<int> sources;
    //Sources definition
    sort(edges.begin(), edges.end(),
            []( const edge_raw &left, const edge_raw &right )
            {   return ( left.from < right.from);});

    int last = -1;
    for (auto from = edges.begin(); from != edges.end(); ++from) {
        if (last != from->from) {

            bool bSrc = true;
            for (auto to = edges.begin(); to != edges.end(); ++to) {
                if (from->from == to->to) {
                    bSrc = false;
                    break;
                }
            }
            if (bSrc) {
                sources.push_back(from->from);

            }
        }
        last = from->from;
    }

    return sources;
}

vector<int> SchedulerMsg::DefineSinksFromDot(vector<edge_raw> edges) {
    vector<int> sinks;
    //Sinks definition
    sort(edges.begin(), edges.end(),
            []( const edge_raw &left, const edge_raw &right )
            {   return ( left.to < right.to);});

    int last = -1;
    for (auto to = edges.begin(); to != edges.end(); ++to) {
        if (last != to->to) {
            bool bSnk = true;
            for (auto from = edges.begin(); from != edges.end(); ++from) {
                if (from->from == to->to) {
                    bSnk = false;
                    break;
                }
            }
            if (bSnk) {
                sinks.push_back(to->to);
            }
        }
        last = to->to;
    }
    return sinks;
}

void SchedulerMsg::AddOperatorUsingDot(vector<edge_raw> edges,
        vector<int> sources, int operatorId) {
    //Define the characteristic of the operator for using on
    //  defining how to break the application by regions
    int type = this->defineType(edges, operatorId);

    bool bSource = false;
    for (auto ss = sources.begin(); ss != sources.end(); ++ss) {
        if (*ss == operatorId) {
            bSource = true;
            break;
        }
    }

    double dWindow = 0;
    int iWindow = bernoulli(this->getParameters()->getOperatorsWithStatePerc(),
            21);
    if (iWindow == 1) {
        dWindow = uniform((double) par("window_min"),
                (double) par("window_max"), 20);
    }

    this->getGenralEnv()->addTask(operatorId, 0,
            uniform((double) par("selectivity_min"),
                    (double) par("selectivity_max"), 10),
            uniform((double) par("datarate_min"), (double) par("datarate_max"),
                    11),
            uniform((double) par("staterate_min"),
                    (double) par("staterate_max"), 17), type,
            bSource ?
                    ((double) par("cpusrc_min") != (double) par("cpusrc_max") ?
                            uniform((double) par("cpusrc_min"),
                                    (double) par("cpusrc_max"), 12) :
                            (double) par("cpusrc_min")) :
                    ((double) par("cpu_min") != (double) par("cpu_max") ?
                            uniform((double) par("cpu_min"),
                                    (double) par("cpu_max"), 13) :
                            (double) par("cpu_min")), //CPU,
            uniform((double) par("memory_min"), (double) par("memory_max"), 14),
            0, false, dWindow);
}

void SchedulerMsg::AddDataSourcesUsingDot(vector<int> sources) {
    //setup source
    for (auto ss = sources.begin(); ss != sources.end(); ++ss) {
        long host_id = -1;
        for (auto fixed = this->getVertexPositions().begin();
                fixed != this->getVertexPositions().end(); ++fixed) {
            if (fixed->vertex_id == *ss) {
                host_id = fixed->host_id;
            }
        }

        if (host_id < 0) {
            int rnd_pos = this->getRNG(18)->intRand(
                    this->getGenralEnv()->getEdgeDevices().size());
            host_id = this->getGenralEnv()->getEdgeDevices().at(rnd_pos);
        }

        long arrivalrate = uniform((double) par("arrivalrate_min"),
                (double) par("arrivalrate_max"), 15);

        long byteevent = uniform((double) par("bytesmsg_min"),
                (double) par("bytesmsg_max"), 16);

        this->getGenralEnv()->addSource(host_id, *ss, arrivalrate, byteevent);

        //        if (!this->getParameters()->isReturnOnlyConfigurationDeployment()) {
        //            cModule* source = this->createSpecificModule(
        //                    Patterns::NAME_PROCESS_SOURCE, host_id, *ss, 0);
        //            if (source) {
        //                source->par("bytesMsg").setIntValue(byteevent);
        //                source->par("departureRate").setIntValue(arrivalrate);
        //                source->par("state").setIntValue(*ss);
        //            }
        //        }

        EV_INFO << " Source ID " << *ss << " Byte Event " << byteevent
                       << " Arrival " << arrivalrate << endl;
    }
}

void SchedulerMsg::AddDataSinksUsingDot(vector<int> sinks) {

    int sinkMax = this->getMaxHopsSink(sinks);
    //    vector<int> cld;

    //setup sink
    for (auto ss = sinks.begin(); ss != sinks.end(); ++ss) {
        long host_id = -1;
        for (auto fixed = this->getVertexPositions().begin();
                fixed != this->getVertexPositions().end(); ++fixed) {
            if (fixed->vertex_id == *ss) {
                host_id = fixed->host_id;
            }
        }

        if (host_id < 0) {
            if (*ss == sinkMax) {
                //                cld.push_back(*ss);
                int rnd = this->getRNG(19)->intRand(
                        this->getGenralEnv()->getCloudServers().size());
                host_id = this->getGenralEnv()->getCloudServers().at(rnd);

            } else {
                int rnd = this->getRNG(18)->intRand(
                        this->getGenralEnv()->getEdgeDevices().size());
                host_id = this->getGenralEnv()->getEdgeDevices().at(rnd);

            }
        }

        this->getGenralEnv()->addSink(host_id, *ss);
        //        if (!this->getParameters()->isReturnOnlyConfigurationDeployment())
        //            this->createSpecificModule(Patterns::NAME_PROCESS_SINK, host_id,
        //                    *ss, 1);
    }

}

void SchedulerMsg::ApplicationDeployment() {
    vector<int> mappedHosts = this->ParticpatingHostsMapping();

    for (auto iHost = mappedHosts.begin(); iHost != mappedHosts.end();
            ++iHost) {

        cMessage* appDeploy = new cMessage("APP-0");
        appDeploy->setKind(Patterns::MessageType::ApplicationDeployment);

        std::string nameHost =
                to_string(
                        this->getGenralEnv()->getResources().at(*iHost)->getId())
                        + "-"
                        + to_string(
                                this->getGenralEnv()->getResources().at(*iHost)->getType());

        //Determine the operator deployments for operators not equal to sinks
        cQueue* operators = new cQueue(
                getQueueVertexHost(nameHost.c_str()).c_str());

        cQueue* operatorDeployments = new cQueue(
                getQueueVertexDep(nameHost.c_str()).c_str());

        operators->clear();
        this->FillOperatorDeploymentObjects(appDeploy->getName(), *iHost,
                operators, operatorDeployments);
        if (operators->getLength() > 0) {
            appDeploy->addObject(operators);
        }

        //Determine the edge deployments for edges of operators not equal to sinks
        cQueue* edgeMappings = new cQueue(
                getQueueEdgeHost(nameHost.c_str()).c_str());
        edgeMappings->clear();

        this->FillEdgeDeploymentObjects(appDeploy->getName(), *iHost,
                edgeMappings);

        //Determine the edge and operator deployments for operators equal to sinks
        this->FillSinkDeploymentObjects(appDeploy->getName(), *iHost,
                edgeMappings, operatorDeployments);

        if (operatorDeployments->getLength() > 0) {
            appDeploy->addObject(operatorDeployments);
        }

        if (edgeMappings->getLength() > 0) {
            appDeploy->addObject(edgeMappings);
        }

        //Send deployment messages
        for (int i = 0; i < this->gateSize("out"); ++i) {
            if (opp_strcmp(nameHost.c_str(),
                    this->gate("out", i)->getNextGate()->getOwnerModule()->getName())
                    == 0) {
                send(appDeploy, "out", i);
                break;
            }
        }

    }

    for (unsigned int iSink = 0;
            iSink < this->getGenralEnv()->getSinks().size(); iSink++) {
        cMessage* appDeploy = new cMessage("APP-0");
        appDeploy->setKind(Patterns::MessageType::ApplicationDeployment);

        //Determine the application paths
        string pathMSG = Patterns::MSG_PATHS;
        cQueue* paths = new cQueue(pathMSG.c_str());
        if (this->FillApplicationPathObject(paths)) {
            appDeploy->addObject(paths);

            //Send deployment messages
            for (int i = 0; i < this->gateSize("out"); ++i) {
                if (opp_strcmp(
                        ("vertex-"
                                + to_string(
                                        this->getGenralEnv()->getSinks().at(
                                                iSink)->getOperatorId())).c_str(),
                        this->gate("out", i)->getNextGate()->getOwnerModule()->getName())
                        == 0) {
                    send(appDeploy, "out", i);
                    break;
                }
            }
        }

    }

    cMessage* rescheduling = new cMessage("Rescheduling");
    rescheduling->setKind(Patterns::MessageType::ReconfigurationScheduling);
    scheduleAt(simTime() + 1000, rescheduling);
}

vector<int> SchedulerMsg::ParticpatingHostsMapping() {
    vector<int> hosts;
    for (unsigned int iMap = 0;
            iMap < this->getGenralEnv()->getOperatorMapping().size(); iMap++) {
        if (find(hosts.begin(), hosts.end(),
                this->getGenralEnv()->getOperatorMapping().at(iMap)->getHostId())
                == hosts.end()) {
            hosts.push_back(
                    this->getGenralEnv()->getOperatorMapping().at(iMap)->getHostId());
        }
    }

    for (unsigned int iMap = 0;
            iMap < this->getGenralEnv()->getLinkMapping().size(); iMap++) {
        for (unsigned int iLink = 0;
                iLink < this->getGenralEnv()->getNetworkTopology().size();
                iLink++) {
            if (this->getGenralEnv()->getNetworkTopology().at(iLink)->getLinkId()
                    == this->getGenralEnv()->getLinkMapping().at(iMap)->getLinkId()) {

                if (find(hosts.begin(), hosts.end(),
                        this->getGenralEnv()->getNetworkTopology().at(iLink)->getSourceId())
                        == hosts.end()) {
                    hosts.push_back(
                            this->getGenralEnv()->getNetworkTopology().at(iLink)->getSourceId());
                }

                if (find(hosts.begin(), hosts.end(),
                        this->getGenralEnv()->getNetworkTopology().at(iLink)->getDestinationId())
                        == hosts.end()) {
                    hosts.push_back(
                            this->getGenralEnv()->getNetworkTopology().at(iLink)->getDestinationId());
                }
                break;
            }
        }
    }
    return hosts;
}

void SchedulerMsg::FillOperatorDeploymentObjects(const char* appName,
        int hostId, cQueue* operators, cQueue* operatorDeployments) {

    for (unsigned int mp = 0;
            mp < this->getGenralEnv()->getOperatorMapping().size(); ++mp) {
        if (this->getGenralEnv()->getOperatorMapping().at(mp)->getHostId()
                == this->getGenralEnv()->getResources().at(hostId)->getId()) {
            this->addVertex(operators,
                    this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getFissionId(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getSelectivity(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getDataTransferRate(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getType(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getStateRate(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getMemoryRequirement(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getCPURequirement(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->isNextSplitted(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getNextSplittedLength(),
                    appName,
                    this->getGenralEnv()->getHostCapabilities().at(
                            this->getGenralEnv()->getResources().at(hostId)->getId()).getCpu(), //mp->availableCPU,
                    this->getGenralEnv()->getResidualHostCapabilities().at(
                            this->getGenralEnv()->getResources().at(hostId)->getId()).getMemory(),
                    this->getGenralEnv()->getOperators().at(
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId())->getTimeWindow());

            for (unsigned int dep = 0;
                    dep < this->getGenralEnv()->getApplicationTopology().size();
                    ++dep) {
                if (this->getGenralEnv()->getApplicationTopology().at(dep)->getFromOperatorId()
                        == this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId()) {

                    this->addDep(operatorDeployments, appName,
                            this->getGenralEnv()->getOperatorMapping().at(mp)->getOperatorId(),
                            this->getGenralEnv()->getOperators().at(
                                    this->getGenralEnv()->getOperatorMapping().at(
                                            mp)->getOperatorId())->getFissionId(),
                            this->getGenralEnv()->getApplicationTopology().at(
                                    dep)->getToOperatorId(), 0);
                }
            }

        }
    }
}

void SchedulerMsg::FillEdgeDeploymentObjects(const char* appName, int hostId,
        cQueue* edge) {
    for (unsigned int mp = 0;
            mp < this->getGenralEnv()->getLinkMapping().size(); ++mp) {

        for (unsigned int iLinks = 0;
                iLinks < this->getGenralEnv()->getNetworkTopology().size();
                iLinks++) {

            if (this->getGenralEnv()->getNetworkTopology().at(iLinks)->getLinkId()
                    != this->getGenralEnv()->getLinkMapping().at(mp)->getLinkId()) {
                continue;
            }

            if (this->getGenralEnv()->getNetworkTopology().at(iLinks)->getSourceId()
                    == this->getGenralEnv()->getResources().at(hostId)->getId()) {
                std::string dstName =
                        to_string(
                                this->getGenralEnv()->getNetworkTopology().at(
                                        iLinks)->getDestinationId()) + "-"
                                + to_string(
                                        this->getGenralEnv()->getResources().at(
                                                this->getGenralEnv()->getNetworkTopology().at(
                                                        iLinks)->getDestinationId())->getType());

                this->addEdge(edge, dstName.c_str(),
                        this->getGenralEnv()->getLinkMapping().at(mp)->getToOperatorId(),
                        0,
                        this->getGenralEnv()->getLinkMapping().at(mp)->getFromOperatorId(),
                        0, appName);

            }
        }
    }
}

void SchedulerMsg::FillSinkDeploymentObjects(const char* appName, int hostId,
        cQueue* edge, cQueue* operatorDeployments) {
    for (unsigned int iSink = 0;
            iSink < this->getGenralEnv()->getSinks().size(); ++iSink) {
        if (this->getGenralEnv()->getSinks().at(iSink)->getHostId()
                == this->getGenralEnv()->getResources().at(hostId)->getId()) {
            std::string dstName;
            dstName += "vertex";
            dstName += "-";
            dstName +=
                    to_string(
                            this->getGenralEnv()->getSinks().at(iSink)->getOperatorId());

            this->addEdge(edge, dstName.c_str(),
                    this->getGenralEnv()->getSinks().at(iSink)->getOperatorId(),
                    0,
                    this->getGenralEnv()->getSinks().at(iSink)->getOperatorId(),
                    0, appName);

            this->addDep(operatorDeployments, appName,
                    this->getGenralEnv()->getSinks().at(iSink)->getOperatorId(),
                    0,
                    this->getGenralEnv()->getSinks().at(iSink)->getOperatorId(),
                    0);

        }
    }
}

bool SchedulerMsg::FillApplicationPathObject(cQueue* paths) {
    if (this->getGenralEnv()->getApplicationPaths().size() > 0) {
        int ss = 0;
        for (unsigned int iPath = 0;
                iPath < this->getGenralEnv()->getApplicationPaths().size();
                ++iPath) {

            string pathStr;

            for (unsigned iOp = 0;
                    iOp
                            < this->getGenralEnv()->getApplicationPaths().at(
                                    iPath).size(); ++iOp) {
                if (iOp == 0) {
                    pathStr = to_string(
                            this->getGenralEnv()->getApplicationPaths().at(
                                    iPath).at(iOp));
                } else {
                    pathStr =
                            pathStr + ";"
                                    + to_string(
                                            this->getGenralEnv()->getApplicationPaths().at(
                                                    iPath).at(iOp));
                }
            }

            ss++;

            OperatorDep* operatorDep = new OperatorDep(pathStr.c_str());

            paths->insert(operatorDep);

        }

        return true;
    }
    return false;
}

void SchedulerMsg::SaveConfigurationFile() {
    std::ofstream fout(
            (opp_strcmp(
                    this->getParameters()->getDirectoryToSaveFiles().c_str(),
                    "") == 0 ?
                    this->getParameters()->getTestId() + ".cfg" :
                    this->getParameters()->getDirectoryToSaveFiles()
                            + this->getParameters()->getTestId() + ".cfg").c_str());
    std::string text =
            "operatorid;hostid;selectivity;datacompressionexpassion;cpureq;memreq;arrivalrate;arrivalmsgsize;departurerate;departuremsgsize;timewindow\n";
    for (unsigned int iMap = 0;
            iMap < this->getGenralEnv()->getOperatorMapping().size(); ++iMap) {

        double arrivalRate = 0, arrivalMsgSize = 0;
        for (unsigned int iSource = 0;
                iSource < this->getGenralEnv()->getSources().size();
                iSource++) {
            if (this->getGenralEnv()->getSources().at(iSource)->getOperatorId()
                    == this->getGenralEnv()->getOperatorMapping().at(iMap)->getOperatorId()) {

                arrivalRate =
                        this->getGenralEnv()->getSources().at(iSource)->getArrivalRate();
                arrivalMsgSize =
                        this->getGenralEnv()->getSources().at(iSource)->getArrivalMsgSize();
                break;
            }
        }
        if (arrivalRate == 0)
            this->getGenralEnv()->obtaingArrivalMetrics(
                    this->getGenralEnv()->getOperatorMapping().at(iMap)->getOperatorId(),
                    arrivalRate, arrivalMsgSize);

        text =
                text
                        + to_string(
                                this->getGenralEnv()->getOperatorMapping().at(
                                        iMap)->getOperatorId()) + ";"
                        + to_string(
                                this->getGenralEnv()->getOperatorMapping().at(
                                        iMap)->getHostId()) + ";"
                        + to_string(
                                this->getGenralEnv()->getOperators().at(
                                        this->getGenralEnv()->getOperatorMapping().at(
                                                iMap)->getOperatorId())->getSelectivity())
                        + ";"
                        + to_string(
                                this->getGenralEnv()->getOperators().at(
                                        this->getGenralEnv()->getOperatorMapping().at(
                                                iMap)->getOperatorId())->getDataTransferRate())
                        + ";"
                        + to_string(
                                this->getGenralEnv()->getOperators().at(
                                        this->getGenralEnv()->getOperatorMapping().at(
                                                iMap)->getOperatorId())->getCPURequirement())
                        + ";"
                        + to_string(
                                this->getGenralEnv()->getOperators().at(
                                        this->getGenralEnv()->getOperatorMapping().at(
                                                iMap)->getOperatorId())->getMemoryRequirement())
                        + ";" + to_string(arrivalRate) + ";"
                        + to_string(arrivalMsgSize) + ";"
                        + to_string(
                                this->getGenralEnv()->getOperatorMapping().at(
                                        iMap)->getOutputRate()) + ";"
                        + to_string(
                                this->getGenralEnv()->getOperatorMapping().at(
                                        iMap)->getOutputMsgSize()) + ";"
                        + to_string(
                                this->getGenralEnv()->getOperators().at(
                                        this->getGenralEnv()->getOperatorMapping().at(
                                                iMap)->getOperatorId())->getTimeWindow())
                        + "\n";

    }
    fout << text;
    fout.close();

}

StatisticsMCTS*& SchedulerMsg::getStatistics() {
    return mStatistics;
}

void SchedulerMsg::setStatistics(StatisticsMCTS*& statistics) {
    mStatistics = statistics;
}

const vector<fix_vertex_position>& SchedulerMsg::getVertexPositions() const {
    return mVertexPositions;
}

void SchedulerMsg::setVertexPositions(
        const vector<fix_vertex_position>& vertexPositions) {
    mVertexPositions = vertexPositions;
}

void SchedulerMsg::CreateSimulation() {
    high_resolution_clock::time_point timeTotal = high_resolution_clock::now();
    high_resolution_clock::time_point time = high_resolution_clock::now();
    this->createDynamicModules();

    duration<double> iteration_duration = high_resolution_clock::now() - time;
    cout << "----- Creating simulation - Time to create the network modules: "
            << to_string(iteration_duration.count()) << endl;

    time = high_resolution_clock::now();
    for (unsigned int iSource = 0;
            iSource < this->getGenralEnv()->getSources().size(); iSource++) {
        cModule* source = this->createSpecificModule(
                Patterns::NAME_PROCESS_SOURCE,
                this->getGenralEnv()->getSources().at(iSource)->getHostId(),
                this->getGenralEnv()->getSources().at(iSource)->getOperatorId(),
                0);
        if (source) {
            source->par("bytesMsg").setIntValue(
                    this->getGenralEnv()->getSources().at(iSource)->getArrivalMsgSize());
            source->par("departureRate").setIntValue(
                    this->getGenralEnv()->getSources().at(iSource)->getArrivalRate());
            source->par("state").setIntValue(
                    this->getGenralEnv()->getSources().at(iSource)->getOperatorId());
        }
    }

    for (unsigned int iSink = 0;
            iSink < this->getGenralEnv()->getSinks().size(); iSink++) {
        this->createSpecificModule(Patterns::NAME_PROCESS_SINK,
                this->getGenralEnv()->getSinks().at(iSink)->getHostId(),
                this->getGenralEnv()->getSinks().at(iSink)->getOperatorId(), 1);
    }

    iteration_duration = high_resolution_clock::now() - time;
    cout
            << "----- Creating simulation - Time to create the source and sink modules: "
            << to_string(iteration_duration.count()) << endl;

    iteration_duration = high_resolution_clock::now() - timeTotal;
    cout << "Creating simulation - Time to create network and basic modules: "
            << to_string(iteration_duration.count()) << "\n" << endl;
}

int SchedulerMsg::DefineXMLSourceSinkLocations(int xmlLocation) {
    vector<int> available;
    int rnd;
    if (xmlLocation == Patterns::XMLLocations::AllRandom) {
        for (unsigned int i = 0;
                i < this->getGenralEnv()->getCloudServers().size(); i++) {
            available.push_back(this->getGenralEnv()->getCloudServers().at(i));
        }

        for (unsigned int i = 0;
                i < this->getGenralEnv()->getEdgeDevices().size(); i++) {
            available.push_back(this->getGenralEnv()->getEdgeDevices().at(i));
        }
        rnd = this->getRNG(19)->intRand(available.size());
        return available.at(rnd);

    } else if (xmlLocation == Patterns::XMLLocations::CentralizedCloud
            || xmlLocation == Patterns::XMLLocations::DescentralizedCloud) {
        for (unsigned int i = 0;
                i < this->getGenralEnv()->getCloudServers().size(); i++) {
            available.push_back(this->getGenralEnv()->getCloudServers().at(i));
        }
        rnd = this->getRNG(19)->intRand(available.size());
        return available.at(rnd);

    } else if (xmlLocation == Patterns::XMLLocations::CentralizedEdge
            || xmlLocation == Patterns::XMLLocations::DescentralizedEdge) {
        for (unsigned int i = 0;
                i < this->getGenralEnv()->getEdgeDevices().size(); i++) {
            available.push_back(this->getGenralEnv()->getEdgeDevices().at(i));
        }
        rnd = this->getRNG(19)->intRand(available.size());
        return available.at(rnd);
    }

    return -1;
}

vector<int> SchedulerMsg::GetNumberMessagesPaths() {
    vector<int> messages(this->getGenralEnv()->getApplicationPaths().size());

    for (unsigned int iSink = 0;
            iSink < this->getGenralEnv()->getSinks().size(); iSink++) {
        for (unsigned int i = 0;
                i < this->getGenralEnv()->getApplicationPaths().size(); i++) {
            cModule* storageModule =
                    this->getModuleByPath(
                            (this->getParentModule()->getFullPath() + "."
                                    + "vertex-"
                                    + to_string(
                                            this->getGenralEnv()->getSinks().at(
                                                    iSink)->getOperatorId())
                                    + "." + "path-" + to_string(i)).c_str());
            if (storageModule) {
                Storage *storage = check_and_cast<Storage *>(storageModule);
                messages.at(i) = storage->getRcvMsgs();
            }

        }

    }
    return messages;
}

} //namespace


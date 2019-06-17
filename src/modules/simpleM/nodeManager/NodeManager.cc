#include "NodeManager.h"
#include "../../msg/routing/RoutingMsg_m.h"
#include "../../msg/operator/Operator_m.h"
#include "../../msg/topicEvent/TopicEvent_m.h"
#include "../../../util/Patterns.h"
#include "../../msg/edges/Edges_m.h"
#include "../../msg/operatorDep/OperatorDep_m.h"
#include "../../simpleM/nodeOutput/NodeOutput.h"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <map>

namespace fogstream {

Define_Module(NodeManager);

void NodeManager::initialize() {
    // TODO - Generated method body

}

void NodeManager::refreshRoutingTable(cMessage *msg) {

    if (msg->hasObject(
            getQueueEdgeHost(this->getParentModule()->getName()).c_str())) {
        cObject *obj = msg->getObject(
                getQueueEdgeHost(this->getParentModule()->getName()).c_str());

        this->AddNetworkRoute(MessageMode::EdgeObject, obj);

    }

    if (msg->hasObject(
            getQueueVertexDep(this->getParentModule()->getName()).c_str())) {

        cObject *obj = msg->getObject(
                getQueueVertexDep(this->getParentModule()->getName()).c_str());

        this->AddNetworkRoute(MessageMode::OperatorDeploymentObject, obj);
    }

}

void NodeManager::handleMessage(cMessage *msg) {
    const char* message = msg->getName();
    std::string messageName(message);

    if (msg->getKind() == Patterns::MessageType::ApplicationDeployment) {

        this->refreshRoutingTable(msg);

        this->applicationDeployment(msg);

    } else if (msg->getKind() == Patterns::MessageType::Topic) {
        //Topic Message
        this->sendMsgApp(msg);

    }
}

void NodeManager::applicationDeployment(cMessage *msg) {
    cModule* lastOpe = this;
    cModule* newOpe = this;
    bool opeCreation = false;

    if (msg->hasObject(
            getQueueVertexHost(this->getParentModule()->getName()).c_str())) {

        cObject *obj = msg->getObject(
                getQueueVertexHost(this->getParentModule()->getName()).c_str());

        cQueue *queueDeploy = check_and_cast<cQueue *>(obj);

        for (cQueue::Iterator it(*queueDeploy); !it.end(); it++) {

            Operator *operatorApp = dynamic_cast<Operator*>(*it);
            std::string nameOperator = getOperatorName(msg->getName(),
                    operatorApp->getOperatorId(), operatorApp->getFissionId());

            newOpe = this->createModule(lastOpe, nameOperator.c_str(),
                    msg->getName(), operatorApp, !opeCreation);

            //All operators are connected with the output
            if (lastOpe != this) {
                this->addConnections(lastOpe, getModuleOutput(), true);
            }

            lastOpe = newOpe;
            opeCreation = true;

        }

        if (opeCreation) {
            this->addConnections(lastOpe, getModuleOutput(), true);
        }

        for (cQueue::Iterator it(*queueDeploy); !it.end(); it++) {
            Operator *src = dynamic_cast<Operator*>(*it);
            std::string srcName = getOperatorName(msg->getName(),
                    src->getOperatorId(), src->getFissionId());

            for (cQueue::Iterator it2(*queueDeploy); !it2.end(); it2++) {
                Operator *dst = dynamic_cast<Operator*>(*it2);
                std::string dstName = getOperatorName(msg->getName(),
                        dst->getOperatorId(), dst->getFissionId());

                if (opp_strcmp(srcName.c_str(), dstName.c_str()) != 0) {

                    std::string pathSrcModule =
                            this->getParentModule()->getFullPath() + "."
                                    + srcName;
                    std::string pathDstModule =
                            this->getParentModule()->getFullPath() + "."
                                    + dstName;

                    //EV_INFO << " +++++++++++++++++++++++++ Src " << pathSrcModule << " Dst. : " << pathDstModule << endl;

                    if (this->getModuleByPath(pathSrcModule.c_str())
                            && this->getModuleByPath(pathDstModule.c_str())) {
                        //EV_INFO << " +++++++++++++++++++++++++ Src " << srcName << " Dst. : " << dstName << endl;
                        this->addConnections(
                                this->getModuleByPath(pathSrcModule.c_str()),
                                this->getModuleByPath(pathDstModule.c_str()),
                                false);
                    }
                }
            }
        }
    }

    delete msg;
}

void NodeManager::sendMsgApp(cMessage *msg) {
    TopicEvent* rcvMsg = dynamic_cast<TopicEvent*>(msg);
    //
    //    simtime_t timeComm = (simTime() - rcvMsg->getLastComm())
    //            + rcvMsg->getTotalComm();
    //    rcvMsg->setTotalComm(timeComm);
    //    rcvMsg->setLastComp(simTime());

    //Memory control
    allocateMemory(this->getParentModule(), rcvMsg->getByteLength());

    bool msgSent = false;

    for (int i = 0; i < this->gateSize("outIn"); ++i) {
        if (this->gate("outIn", i)->getNextGate()->getOwnerModule()->hasPar(
                "appName")) {
            if (opp_strcmp(
                    this->gate("outIn", i)->getNextGate()->getOwnerModule()->par(
                            "appName"), rcvMsg->getAppName()) == 0) {
                if (this->gate("outIn", i)->getNextGate()->getOwnerModule()->hasPar(
                        "operatorId")) {
                    int operatorId =
                            this->gate("outIn", i)->getNextGate()->getOwnerModule()->par(
                                    "operatorId");

                    if (operatorId == rcvMsg->getNextState()) {
                        int fissionId =
                                this->gate("outIn", i)->getNextGate()->getOwnerModule()->par(
                                        "fissionId");

                        if (fissionId == rcvMsg->getNextSubState()) {

                            if (rcvMsg->getNextState() == 1) {
                                //                                simtime_t times = simTime() - rcvMsg->getTimestamp();
                                //                                emit(qTransmissionTime, times);

                            }

                            //TODO - Delay for message queue --- inter and intra3
                            simtime_t delayMsgQueue = 0;
                            std::string pathModule =
                                    this->gate("outIn", i)->getNextGate()->getOwnerModule()->getFullPath()
                                            + ".msgQueue";

                            if (this->getModuleByPath(pathModule.c_str())) {
                                if (this->getModuleByPath(pathModule.c_str())->hasPar(
                                        "serviceMax")) {
                                    delayMsgQueue = this->getModuleByPath(
                                            pathModule.c_str())->par(
                                            "serviceMax");
                                }
                            }

                            msgSent = true;
                            //EV_INFO << "----------------------------- 0 - " << rcvMsg->getName() << " " <<  simTime() << endl;
                            sendDelayed(msg, delayMsgQueue, "outIn", i);
                        }
                    }
                }
            }
        }
    }

    if (!msgSent) {
        //EV_INFO << "----------------------------- 0.1 - " << rcvMsg->getName() << " " << simTime() << endl;
        send(msg, "outIn", 0);
    }
}

cModule* NodeManager::createModule(cModule* srcModule, const char* operatorName,
        const char* appName, Operator* parameters, bool firstModule) {
    cModuleType *mType = cModuleType::get(Patterns::NAME_PROCESS_ELEMENT);

    if (mType) {
        cModule *module = mType->create(operatorName, this->getParentModule());

        if (module) {
            // create internals, and schedule it

            //if (!firstModule){
            this->addConnections(this, module, false);
            //}

            //addConnections(srcModule, module, false);

            module->buildInside();
            module->scheduleStart(simTime());

            this->setParModule(appName, module, parameters, firstModule);

            module->callInitialize();

            this->moduleType.push_back(mType);
            this->operatorModule.push_back(module);

            return module;
        } else {
            throw cRuntimeError("Error to create module!");
        }
    } else {
        throw cRuntimeError("Error to create module!");
    }

}

void NodeManager::addConnections(cModule* srcModule, cModule* dstModule,
        bool endModule) {

    if (srcModule == this) {
        srcModule->setGateSize("outIn", srcModule->gateSize("outIn") + 1);
        dstModule->setGateSize("Queue_port",
                dstModule->gateSize("Queue_port") + 1);

        srcModule->gate("outIn", srcModule->gateSize("outIn") - 1)->connectTo(
                dstModule->gate("Queue_port",
                        dstModule->gateSize("Queue_port") - 1));

    } else if (!endModule) {
        //External connection between Compound modules
        dstModule->setGateSize("Queue_port",
                dstModule->gateSize("Queue_port") + 1);
        srcModule->setGateSize("PE_port", srcModule->gateSize("PE_port") + 1);

        srcModule->gate("PE_port", srcModule->gateSize("PE_port") - 1)->connectTo(
                dstModule->gate("Queue_port",
                        dstModule->gateSize("Queue_port") - 1));

        //Internal connection between parent module and child
        std::string pathModule = srcModule->getFullPath() + ".operatorMsg";
        cModule* operatorMsg = srcModule->getModuleByPath(pathModule.c_str());
        operatorMsg->setGateSize("queueNextOut",
                operatorMsg->gateSize("queueNextOut") + 1);
        operatorMsg->gate("queueNextOut",
                operatorMsg->gateSize("queueNextOut") - 1)->connectTo(
                srcModule->gate("PE_port", srcModule->gateSize("PE_port") - 1));

        //Internal connection between child module and parent
        pathModule = dstModule->getFullPath() + ".msgQueue";
        cModule* queueMsg = dstModule->getModuleByPath(pathModule.c_str());
        queueMsg->setGateSize("inIn", queueMsg->gateSize("inIn") + 1);
        dstModule->gate("Queue_port", dstModule->gateSize("Queue_port") - 1)->connectTo(
                queueMsg->gate("inIn", queueMsg->gateSize("inIn") - 1));

    } else {
        dstModule->setGateSize("inIn", dstModule->gateSize("inIn") + 1);
        srcModule->setGateSize("PE_port", srcModule->gateSize("PE_port") + 1);
        srcModule->gate("PE_port", srcModule->gateSize("PE_port") - 1)->connectTo(
                dstModule->gate("inIn", dstModule->gateSize("inIn") - 1));

        //Internal connection between parent module and child
        std::string pathModule = srcModule->getFullPath() + ".operatorMsg";
        cModule* operatorMsg = srcModule->getModuleByPath(pathModule.c_str());
        operatorMsg->setGateSize("queueNextOut",
                operatorMsg->gateSize("queueNextOut") + 1);
        operatorMsg->gate("queueNextOut",
                operatorMsg->gateSize("queueNextOut") - 1)->connectTo(
                srcModule->gate("PE_port", srcModule->gateSize("PE_port") - 1));
    }

}

cModule* NodeManager::getModuleOutput() {

    std::string pathModule = this->getParentModule()->getFullPath()
            + ".nodeOutput";

    cModule* moduleOutput = this->getModuleByPath(pathModule.c_str());

    return moduleOutput;
}

void NodeManager::setParModule(const char* appName, cModule *module,
        Operator* parameters, bool firstModule) {

    double cpuCost = 0;
    double memCost = 0;

    //Set up parameters for operator
    std::string pathModule = module->getFullPath() + ".operatorMsg";

    if (module->getModuleByPath(pathModule.c_str())->hasPar("selectivity")) {
        module->getModuleByPath(pathModule.c_str())->par("selectivity").setDoubleValue(
                parameters->getSelectivity());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("dataReduction")) {
        module->getModuleByPath(pathModule.c_str())->par("dataReduction").setDoubleValue(
                parameters->getDataReduction());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("type")) {
        module->getModuleByPath(pathModule.c_str())->par("type").setIntValue(
                parameters->getType());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar(
            "stateIncreaseRate")) {
        module->getModuleByPath(pathModule.c_str())->par("stateIncreaseRate").setDoubleValue(
                parameters->getStateIncreaseRate());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("nextSplitted")) {
        module->getModuleByPath(pathModule.c_str())->par("nextSplitted").setBoolValue(
                parameters->getNextSplitted());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar(
            "nextSplitLength")) {
        module->getModuleByPath(pathModule.c_str())->par("nextSplitLength").setIntValue(
                parameters->getNextSplitLength());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("curState")) {
        module->getModuleByPath(pathModule.c_str())->par("curState").setIntValue(
                parameters->getOperatorId());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("curSubState")) {
        module->getModuleByPath(pathModule.c_str())->par("curSubState").setIntValue(
                parameters->getFissionId());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("nextState")) {
        module->getModuleByPath(pathModule.c_str())->par("nextState").setIntValue(
                parameters->getNextState());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("nextSubState")) {
        module->getModuleByPath(pathModule.c_str())->par("nextSubState").setIntValue(
                parameters->getNextSubState());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("appName")) {
        module->getModuleByPath(pathModule.c_str())->par("appName").setStringValue(
                parameters->getAppName());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("availableCPU")) {
        module->getModuleByPath(pathModule.c_str())->par("availableCPU").setDoubleValue(
                parameters->getAvailableCPU());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("availableMem")) {
        module->getModuleByPath(pathModule.c_str())->par("availableMem").setDoubleValue(
                parameters->getAvailableMem());

    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("timeWindow")) {
        module->getModuleByPath(pathModule.c_str())->par("timeWindow").setDoubleValue(
                parameters->getTimeWindow());

    }

    if (module->hasPar("appName")) {
        module->par("appName").setStringValue(appName);
    }

    if (module->hasPar("operatorId")) {
        module->par("operatorId").setIntValue(parameters->getOperatorId());
    }

    if (module->hasPar("fissionId")) {
        module->par("fissionId").setIntValue(parameters->getFissionId());
    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("memoryCost")) {
        module->getModuleByPath(pathModule.c_str())->par("memoryCost").setDoubleValue(
                parameters->getMemoryCost());
        if (module->hasPar("memoryCost")) {
            memCost = module->par("memoryCost");
            memCost += parameters->getMemoryCost();
            module->par("memoryCost").setDoubleValue(memCost);
        }

        if (module->getParentModule()->hasPar("avaliableMem")) {
            memCost = module->getParentModule()->par("avaliableMem");
            memCost -= parameters->getMemoryCost();
            module->getParentModule()->par("avaliableMem").setDoubleValue(
                    memCost);

            memCost = module->getParentModule()->par("avaliableMem");
        }
    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("cpuCost")) {
        module->getModuleByPath(pathModule.c_str())->par("cpuCost").setDoubleValue(
                parameters->getCpuCost());
        if (module->hasPar("cpuCost")) {
            cpuCost = module->par("cpuCost");
            cpuCost += parameters->getCpuCost();
            module->par("cpuCost").setDoubleValue(cpuCost);
        }
        if (module->getParentModule()->hasPar("avaliableCpu")) {
            cpuCost = module->getParentModule()->par("avaliableCpu");
            cpuCost -= parameters->getCpuCost();
            module->getParentModule()->par("avaliableCpu").setDoubleValue(
                    cpuCost);

            cpuCost = module->getParentModule()->par("avaliableCpu");
        }
    }

    //TODO - Set up inter and intra message queue services
    double memCostMsgQueue = 0, cpuCostMsgQueue = 0;
    pathModule = module->getFullPath() + ".msgQueue";
    if (module->getModuleByPath(pathModule.c_str())->hasPar("memoryCost")) {

        //todo - intra and inter cost
        if (firstModule) {
            memCostMsgQueue = Patterns::MEM_EXTERNAL_MSG_QUEUE; // inter const

        } else {
            memCostMsgQueue = Patterns::MEM_INTERNAL_MSG_QUEUE; // intra const
        }

        module->getModuleByPath(pathModule.c_str())->par("memoryCost").setDoubleValue(
                memCostMsgQueue);

        if (module->getParentModule()->hasPar("avaliableMem")) {
            memCost = module->getParentModule()->par("avaliableMem");
            memCost -= memCostMsgQueue;
            module->getParentModule()->par("avaliableMem").setDoubleValue(
                    memCost);

            memCost = module->getParentModule()->par("avaliableMem");
        }
    }

    if (module->getModuleByPath(pathModule.c_str())->hasPar("cpuCost")) {

        //todo - intra and inter cost
        if (firstModule) {
            cpuCostMsgQueue = Patterns::CPU_EXTERNAL_MSG_QUEUE; // inter const
            module->getModuleByPath(pathModule.c_str())->par("serviceMin").setDoubleValue(
                    Patterns::TIME_INTERNAL_MSG_QUEUE_MIN);
            module->getModuleByPath(pathModule.c_str())->par("serviceMax").setDoubleValue(
                    Patterns::TIME_INTERNAL_MSG_QUEUE_MAX);

        } else {
            cpuCostMsgQueue = Patterns::CPU_INTERNAL_MSG_QUEUE; // intra const
            module->getModuleByPath(pathModule.c_str())->par("serviceMin").setDoubleValue(
                    Patterns::TIME_EXTERNAL_MSG_QUEUE_MIN);
            module->getModuleByPath(pathModule.c_str())->par("serviceMax").setDoubleValue(
                    Patterns::TIME_EXTERNAL_MSG_QUEUE_MAX);
        }
        module->getModuleByPath(pathModule.c_str())->par("cpuCost").setDoubleValue(
                cpuCostMsgQueue);

        if (module->getParentModule()->hasPar("avaliableCpu")) {
            cpuCost = module->getParentModule()->par("avaliableCpu");
            cpuCost -= cpuCostMsgQueue;
            module->getParentModule()->par("avaliableCpu").setDoubleValue(
                    cpuCost);

            cpuCost = module->getParentModule()->par("avaliableCpu");
        }
    }

}

void NodeManager::AddNetworkRoute(int messageMode, cObject* obj) {
    cQueue *queueObject = check_and_cast<cQueue *>(obj);
    if (messageMode != MessageMode::NoIteration) {

        NodeOutput* outModule =
                dynamic_cast<NodeOutput*>(this->getModuleOutput());

        for (cQueue::Iterator it(*queueObject); !it.end(); it++) {
            int nextOperator = -1, nextFissionId = -1, currentOperator = -1,
                    currentFissionID = -1;
            string appName = "";
            string hostName = "";

            if (messageMode == MessageMode::EdgeObject) {
                nextOperator = dynamic_cast<Edges*>(*it)->getNextState();
                nextFissionId = dynamic_cast<Edges*>(*it)->getNextSubState();
                currentOperator = dynamic_cast<Edges*>(*it)->getCurState();
                currentFissionID = dynamic_cast<Edges*>(*it)->getCurSubState();
                appName = dynamic_cast<Edges*>(*it)->getAppName();
                hostName = string(dynamic_cast<Edges*>(*it)->getDstHostName());
            } else {
                nextOperator = dynamic_cast<OperatorDep*>(*it)->getNextState();
                nextFissionId =
                        dynamic_cast<OperatorDep*>(*it)->getNextSubState();
                currentOperator =
                        dynamic_cast<OperatorDep*>(*it)->getCurState();
                currentFissionID =
                        dynamic_cast<OperatorDep*>(*it)->getCurSubState();
                appName = string(dynamic_cast<OperatorDep*>(*it)->getAppName());

            }

            if(nextOperator == currentOperator && nextFissionId == currentFissionID){
                hostName = "vertex-" + to_string(nextOperator);
            }

//            if (opp_strcmp(this->getParentModule()->getName(), "40-0") == 0) {
//                cout << "Host: " << hostName << endl;
//            }

            outModule->AddNewRoute(nextOperator, nextFissionId, appName,
                    currentOperator, currentFissionID, hostName);

        }
    }
}

} //namespace

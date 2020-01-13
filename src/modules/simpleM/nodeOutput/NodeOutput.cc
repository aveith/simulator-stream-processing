#include "NodeOutput.h"
#include "../../msg/routing/RoutingMsg_m.h"
#include "../../msg/edges/Edges_m.h"

#include "../../msg/statistic/Statistic_m.h"
#include "../../../util/Patterns.h"

namespace fogstream {

Define_Module(NodeOutput);

RoutingData::RoutingData(int nextProcessingOperatorId,
        int nextProcessingFissionId, string appName,
        int currentProcessingOperatorId, int currentProcessingFissionId,
        string destinationHost, int gateIndex) {
    this->setNextProcessingOperatorId(nextProcessingOperatorId);
    this->setNextProcessingFissionId(nextProcessingFissionId);
    this->setAppName(appName);
    this->setCurrentProcessingOperatorId(currentProcessingOperatorId);
    this->setCurrentProcessingFissionId(currentProcessingFissionId);
    this->setDestinationHost(destinationHost);
    this->setGateIndex(gateIndex);
}

string RoutingData::getAppName() const {
    return mAppName;
}

void RoutingData::setAppName(string appName) {
    mAppName = appName;
}

int RoutingData::getCurrentProcessingFissionId() const {
    return mCurrentProcessingFissionId;
}

void RoutingData::setCurrentProcessingFissionId(
        int currentProcessingFissionId) {
    mCurrentProcessingFissionId = currentProcessingFissionId;
}

int RoutingData::getCurrentProcessingOperatorId() const {
    return mCurrentProcessingOperatorId;
}

void RoutingData::setCurrentProcessingOperatorId(
        int currentProcessingOperatorId) {
    mCurrentProcessingOperatorId = currentProcessingOperatorId;
}

string RoutingData::getDestinationHost() const {
    return mDestinationHost;
}

void RoutingData::setDestinationHost(string destinationHost) {
    mDestinationHost = destinationHost;
}

int RoutingData::getNextProcessingFissionId() const {
    return mNextProcessingFissionId;
}

void RoutingData::setNextProcessingFissionId(int nextProcessingFissionId) {
    mNextProcessingFissionId = nextProcessingFissionId;
}

int RoutingData::getNextProcessingOperatorId() const {
    return mNextProcessingOperatorId;
}

void RoutingData::setNextProcessingOperatorId(int nextProcessingOperatorId) {
    mNextProcessingOperatorId = nextProcessingOperatorId;
}

int RoutingData::getGateIndex() const {
    return mGateIndex;
}

void RoutingData::setGateIndex(int gateIndex) {
    mGateIndex = gateIndex;
}
/*********************************************************
 * NodeOutput class
 *********************************************************/

void NodeOutput::initialize() {

}

void NodeOutput::handleMessage(cMessage *msg) {
    //    if (opp_strcmp(this->getParentModule()->getName(), "28-0") == 0) {
    //        int i = 0;
    //        i = i + 1;
    //    }
    if (msg->getKind() == Patterns::MessageType::Topic) {
        if (this->isLogData()) {
            this->sendDataToHistory(msg);
        }

        this->ScheduleMessage(msg);

    } else if (msg->getKind()
            == Patterns::MessageType::IntercommunicationScheduling
            && msg->isSelfMessage()) {
        //Msg type 2 is self scheduling
        delete msg;
        if (!this->SendMessageToNextOperator()) {
            throw cRuntimeError(
                    "It is not possible to send the message to the next operator!");
        }

    } else if (msg->getKind() == Patterns::MessageType::Statistics) {
        //Forward messages of statistics
        std::string _module_name = "scheduler";
        for (int i = 0; i < this->gateSize(GATE_NAME); ++i) {
            if (opp_strcmp(_module_name.c_str(),
                    this->gate(GATE_NAME, i)->getNextGate()->getNextGate()->getOwnerModule()->getName())
                    == 0) {
                send(msg, GATE_NAME, i);
            }
        }

    }
}

bool NodeOutput::SendMessageToNextOperator() {
    if (this->DetermineQueueSizes() == 0)
        return true;

    if (this->gateSize(GATE_NAME) == 0)
        return false;

    bool bSent = false;

    for (auto destination = this->getScheduledMessages().begin();
            destination != this->getScheduledMessages().end(); ++destination) {
        if (this->getScheduledMessages().at(destination->first)->getLength()
                == 0)
            continue;

        if (((string) this->gate(GATE_NAME, destination->first)->getNextGate()->getNextGate()->getOwnerModule()->getName()).find(
                "vertex") == std::string::npos) {

            //If the channel is not busy, the message is sent
            if (!this->gate(GATE_NAME, destination->first)->getTransmissionChannel()->isBusy()) {
                sendDelayed(
                        dynamic_cast<cMessage*>(this->getScheduledMessages().at(
                                destination->first)->pop()),
                        Patterns::TIME_OUTPUT_MSG_TRANSMISSION, GATE_NAME,
                        destination->first);
                bSent = true;

            } else {
                //If the channel is busy then the message is scheduled
                //refresh the min value to be scheduled
                cMessage* schMsg = new cMessage(Patterns::BASE_SCHEDULING);
                schMsg->setKind(
                        Patterns::MessageType::IntercommunicationScheduling);

                bSent = true;

                scheduleAt(
                        this->gate(GATE_NAME, destination->first)->getTransmissionChannel()->getTransmissionFinishTime(),
                        schMsg);

            }
        } else if (opp_strcmp(((string) "scheduler").c_str(),
                this->gate(GATE_NAME, destination->first)->getNextGate()->getOwnerModule()->getName())
                != 0) {

            //If the next operator is a sink then a message is sent,
            //nothing is checked because the channel doesn't have limitations
            //Send message via indexed channel
            sendDelayed(
                    dynamic_cast<cMessage*>(this->getScheduledMessages().at(
                            destination->first)->pop()),
                    Patterns::TIME_OUTPUT_MSG_TRANSMISSION, GATE_NAME,
                    destination->first);
            bSent = true;

        }

    }

    return bSent;
}

int NodeOutput::DetermineQueueSizes() {
    int totalQueuedMsgs = 0;
    for (auto list = this->getScheduledMessages().begin();
            list != this->getScheduledMessages().end(); list++) {
        totalQueuedMsgs += list->second->getLength();
    }

    return totalQueuedMsgs;
}

void NodeOutput::refreshDisplay() {
    // refresh statistics
    char buf[32];
    //    sprintf(buf, "Wait List:%d", this->workList.getLength());
    sprintf(buf, "Wait List:%d", (int) this->DetermineQueueSizes());
    this->getDisplayString().setTagArg("t", 0, buf);
}

void NodeOutput::sendDataToHistory(cMessage* msg) {
    if (this->getSentRegs() <= Patterns::STATISTIC_MAX_REGS) {
        cPacket* dataToSave = dynamic_cast<cPacket*>(msg);

        /*Save statistics*/
        Statistic* statInput = new Statistic();
        statInput->setName("Host Statistics");
        statInput->setKind(Patterns::MessageType::Statistics);
        statInput->setSTimestamp(simTime());
        statInput->setType(Patterns::StatisticType::Host);
        //    statInput->setMeasurement();
        //    statInput->setOperatorID(this->getParentModule()->par("operatorId"));
        statInput->setHostID(this->getParentModule()->par("id").intValue());
        statInput->setMsgSize(dataToSave->getByteLength());
        //    statInput->setQueueSize(this->workList.getLength());
        statInput->setQueueSize(this->DetermineQueueSizes());
        this->setSentRegs(this->getSentRegs() + 1);

        std::string _module_name = "scheduler";
        for (int i = 0; i < this->gateSize(GATE_NAME); ++i) {
            if (opp_strcmp(_module_name.c_str(),
                    this->gate(GATE_NAME, i)->getNextGate()->getNextGate()->getOwnerModule()->getName())
                    == 0) {
                send(statInput, GATE_NAME, i);
                break;
            }
        }
    }
}

void NodeOutput::ScheduleMessage(cMessage* msg) {

    //When message arrives, it is included into the working list
    TopicEvent* rcvMsg = dynamic_cast<TopicEvent*>(msg);

    int gateIndex = this->DetermineGateIndex(rcvMsg->getNextState(),
            rcvMsg->getNextSubState(), rcvMsg->getAppName());

    if (gateIndex > -1) {
        auto gateQueue = this->getScheduledMessages().find(gateIndex);
        if (gateQueue == this->getScheduledMessages().end()) {
            //Create the queue list of the gateway
            this->getScheduledMessages().insert(
                    make_pair(gateIndex, new cQueue));
        }

        this->getScheduledMessages().at(gateIndex)->insert(rcvMsg);

        this->refreshDisplay();

        //Schedule the message to be sent
        cMessage* schMsg = new cMessage(Patterns::BASE_SCHEDULING);
        schMsg->setKind(Patterns::MessageType::IntercommunicationScheduling);
        scheduleAt(simTime(), schMsg);
    } else {
        throw cRuntimeError(
                "There is no routing on %s to operator id %d and fission id %d!",
                this->getParentModule()->getName(), rcvMsg->getNextState(),
                rcvMsg->getNextSubState());

    }

}

cPacket * NodeOutput::ConsumeMsgFromWorkingList(cQueue * queue) {
    cPacket* queuedConsumed;

    //Get first message from the queue
    queuedConsumed = (cPacket*) queue->pop();

    //Memory control - Free memory on current host due to message was removed from the queue
    freeMemory(this->getParentModule(), queuedConsumed->getByteLength());

    //Refresh information in the resource and working queue list
    this->refreshDisplay();

    return queuedConsumed;
}

int NodeOutput::getSentRegs() const {
    return mSentRegs;
}

void NodeOutput::setSentRegs(int sentRegs) {
    mSentRegs = sentRegs;
}

void NodeOutput::AddNewRoute(int nextProcessingOperatorId,
        int nextProcessingFissionId, string appName,
        int currentProcessingOperatorId, int currentProcessingFissionId,
        string hostName, int gateIndex) {

    int routingTableIndex = this->VerifyRoutingIndex(nextProcessingOperatorId,
            nextProcessingFissionId, appName, currentProcessingOperatorId,
            currentProcessingFissionId);
    if (routingTableIndex > -1) {

        if (hostName != "") {
            this->getRoutingTable().at(routingTableIndex)->setDestinationHost(
                    hostName);

            this->getRoutingTable().at(routingTableIndex)->setGateIndex(
                    this->DefineGateIndex(hostName));
        }

        if (gateIndex > -1)
            this->getRoutingTable().at(routingTableIndex)->setGateIndex(
                    gateIndex);
    } else {
        this->getRoutingTable().push_back(
                new RoutingData(nextProcessingOperatorId,
                        nextProcessingFissionId, appName,
                        currentProcessingOperatorId, currentProcessingFissionId,
                        hostName, this->DefineGateIndex(hostName)));
    }

}

int NodeOutput::DefineGateIndex(string destinationHost) {
    if (destinationHost != "") {

        for (int i = 0; i < this->gateSize(GATE_NAME); ++i) {
            //Evaluate the destination host of the gate
            if (opp_strcmp(destinationHost.c_str(),
                    this->gate(GATE_NAME, i)->getNextGate()->getNextGate()->getOwnerModule()->getName())
                    == 0) {
                return i;

            }
        }

        throw cRuntimeError("Undefined gate!");
    }
    return -1;
}

unordered_map<int, cQueue*>& NodeOutput::getScheduledMessages() {
    return mScheduledMessages;
}

vector<RoutingData*>& NodeOutput::getRoutingTable() {
    return mRoutingTable;
}

int NodeOutput::VerifyRoutingIndex(int nextProcessingOperatorId,
        int nextProcessingFissionId, string appName,
        int currentProcessingOperatorId, int currentProcessingFissionId) {
    for (unsigned int index = 0; index < this->getRoutingTable().size();
            index++) {
        if (this->getRoutingTable().at(index)->getAppName() == appName
                && this->getRoutingTable().at(index)->getNextProcessingOperatorId()
                        == nextProcessingOperatorId
                && this->getRoutingTable().at(index)->getNextProcessingFissionId()
                        == nextProcessingFissionId
                && this->getRoutingTable().at(index)->getCurrentProcessingOperatorId()
                        == currentProcessingOperatorId
                && this->getRoutingTable().at(index)->getCurrentProcessingFissionId()
                        == currentProcessingFissionId) {
            return index;
        }
    }
    return -1;
}

vector<int> NodeOutput::DefineNextOperatorsIndex(int nextOperatorId,
        int nextFissionId, const char * appName) {
    vector<int> operators;
    for (unsigned int index = 0; index < this->getRoutingTable().size();
            index++) {
        if (this->getRoutingTable().at(index)->getCurrentProcessingOperatorId()
                == nextOperatorId
                && this->getRoutingTable().at(index)->getCurrentProcessingFissionId()
                        == nextFissionId
                && this->getRoutingTable().at(index)->getAppName() == appName) {
            operators.push_back(index);
        }

    }
    return operators;
}

int NodeOutput::DetermineGateIndex(int nextProcessingOperatorId,
        int nextProcessingFissionId, const char* appName) {
    for (unsigned int index = 0; index < this->getRoutingTable().size();
            index++) {
        if (this->getRoutingTable().at(index)->getAppName() == appName
                && this->getRoutingTable().at(index)->getNextProcessingOperatorId()
                        == nextProcessingOperatorId
                && this->getRoutingTable().at(index)->getNextProcessingFissionId()
                        == nextProcessingFissionId) {
            return this->getRoutingTable().at(index)->getGateIndex();
        }
    }
    return -1;
}

void NodeOutput::setRoutingTable(const vector<RoutingData*>& routingTable) {
    mRoutingTable = routingTable;
}

void NodeOutput::setScheduledMessages(
        unordered_map<int, cQueue*>& scheduledMessages) {
    mScheduledMessages = scheduledMessages;
}

bool NodeOutput::isLogData() const {
    return mLogData;
}

void NodeOutput::setLogData(bool logData) {
    mLogData = logData;
}

} //namespace


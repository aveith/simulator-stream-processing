#include "OperatorMsg.h"
#include "../../../util/Patterns.h"
#include "../../simpleM/nodeOutput/NodeOutput.h"
#include "../../msg/routing/RoutingMsg_m.h"
#include "../../msg/statistic/Statistic_m.h"

namespace fogstream {

Define_Module(OperatorMsg);

OperatorMsg::OperatorMsg() {

}

void OperatorMsg::sendDataToHistory(cMessage* msg, simtime_t compTime,
        bool bInput) {
    if (this->getSentEventRegs() <= Patterns::STATISTIC_MAX_REGS) {
        cPacket* dataToSave = dynamic_cast<cPacket*>(msg);

        /*Save statistics*/
        Statistic* statInput = new Statistic();
        statInput->setName("Operator Statistics");
        statInput->setKind(Patterns::MessageType::Statistics);
        statInput->setSTimestamp(simTime());
        statInput->setType(Patterns::StatisticType::Operator);
        statInput->setMeasurement(Patterns::OperatorMeasurement::Event);
        statInput->setOperatorID(
                this->getParentModule()->par("operatorId").intValue());
        statInput->setHostID(
                this->getParentModule()->getParentModule()->par("id").intValue());
        statInput->setMsgSize(dataToSave->getByteLength());

        if (bInput) {
            TopicEvent* topicEvent = dynamic_cast<TopicEvent*>(dataToSave);
            simtime_t time = simTime() - topicEvent->getLastState();
            statInput->setCommTime(time);
        } else {
            statInput->setCompTime(compTime);
        }

        statInput->setInput(bInput);

        //Get the size of the state
        double bytesState = 0;
        if (this->getInternalState() != nullptr) {
            bytesState += this->getInternalState()->getBytesSize();
        }

        statInput->setStateSize(bytesState);

        this->setSentEventRegs(this->getSentEventRegs() + 1);

        //Forward the operator statistics the port queueNextOut index 0
        //  -- index 0 refers to the nodeoutput connection
        send(statInput, "queueNextOut", 0);
    }
}

void OperatorMsg::sendStateDataToHistory() {
    if (this->getSentStateRegs() <= Patterns::STATISTIC_MAX_REGS) {
        /*Save statistics*/
        Statistic* statInput = new Statistic();
        statInput->setName("State Statistics");
        statInput->setKind(Patterns::MessageType::Statistics);
        statInput->setSTimestamp(simTime());
        statInput->setType(Patterns::StatisticType::InternalState);
        statInput->setOperatorID(
                this->getParentModule()->par("operatorId").intValue());
        statInput->setHostID(
                this->getParentModule()->getParentModule()->par("id").intValue());
        statInput->setStateSize(this->getInternalState()->getBytesSize());
        statInput->setRequiredTimeBuildWindow(
                simTime() - this->getInternalState()->getTimestamp());
        this->setSentStateRegs(this->getSentStateRegs() + 1);

        //Forward the operator statistics the port queueNextOut index 0
        //  -- index 0 refers to the nodeoutput connection
        send(statInput, "queueNextOut", 0);
    }
}

OperatorMsg::~OperatorMsg() {

}
void OperatorMsg::initialize() {
    //Initialize the global variable by reading from the module parameters
    this->setSelectivity(par("selectivity").doubleValue());
    this->setDataReduction(par("dataReduction").doubleValue());
    this->setType(par("type").intValue());
    this->setStateIncreaseRate(par("stateIncreaseRate").doubleValue());
    this->setMemoryCost(par("memoryCost").doubleValue());
    this->setCpuCost(par("cpuCost").doubleValue());
    this->setNextSplitted(par("nextSplitted").boolValue());
    this->setAvailableCpu(par("availableCPU").doubleValue());
    this->setAvailableMem(par("availableMem").doubleValue());
    this->setTimeWindow(par("timeWindow").doubleValue());

    //Initialize the object state into the operator - TODO - different triggers
    if (this->getTimeWindow() > 0 && this->getInternalState() == nullptr) {
        InternalState* internalState = new InternalState(
                Patterns::InternalStateType::CountTrigger,
                this->getTimeWindow() < 1 ? 1 :  this->getTimeWindow());
        this->setInternalState(internalState);

    }

    EV_WARN << "Selectivity: " << this->getSelectivity() << " Data Reduction: "
                   << this->getDataReduction() << " Type: " << this->getType()
                   << " State Increase: " << this->getStateIncreaseRate()
                   << " Memory Cost: " << this->getMemoryCost() << " Cpu Cost: "
                   << this->getCpuCost() << " Available CPU: "
                   << this->getAvailableCpu() << " Available Mem: "
                   << this->getAvailableMem() << " Time window: "
                   << this->getTimeWindow() << endl;

    cMessage* msg = new cMessage(Patterns::MSG_INITIALISATION);
    msg->setKind(Patterns::MessageType::Topic);
    scheduleAt(simTime(), msg);
}

void OperatorMsg::handleMessage(cMessage *msg) {
    //Messages type:
    //               - 3 : Statistics
    //               - 4 : Wait time to process message - computation time
    //               - 5 : Logic to change or discard messages and forward to the next operation
    //               - self-message : communicate the queues that the operator is ready to messages
    if (msg->isSelfMessage()
            && msg->getKind() == Patterns::MessageType::Topic) {
        //Send messages to the connected queues to communicate that the operator
        //  is ready to receive and compute messages
        delete msg;

        //Send message for all out gates connected
        for (int i = 0; i < gateSize("queuePreviousOut"); ++i) {
            cMessage* msgSend = new cMessage(Patterns::BASE_REQUEST);
            send(msgSend, "queuePreviousOut", i);

        }

    } else if (msg->getKind() == Patterns::MessageType::Statistics) {
        //Forward the queue statistics to the node output
        // the index queueNextOut index 0 refers to the connections with the "nodeoutput" of
        //  the process element
        send(msg, "queueNextOut", 0);

    } else if (msg->getKind() == 4) {
        //Processes the incoming messages - discarding (selectivity) or changing
        //  the message size (increasing/ decreasing the amount of bytes)
        this->processMsg(msg);

    } else {
        //Send information from arrival messages to statistics at the scheduler
        this->sendDataToHistory(msg, 0, true);

        //If the operator is stateful then the compute time will be included into
        //  the last event that comes
        if (this->getInternalState() == nullptr) {
            //Schedule the message following the compute time
            this->ComputingOperation(msg);

        } else {
            //            int operatorId =
            //                    this->getParentModule()->par("operatorId").intValue();

            if (this->getInternalState()->getWindowCounter() == 0) {
                this->getInternalState()->setTimestamp(simTime());
            }

            this->refreshDisplay();
            //Increment the size of the state following the message size (bytes)
            cPacket* rcvMsg = dynamic_cast<cPacket*>(msg);
            this->getInternalState()->setBytesSize(
                    this->getInternalState()->getBytesSize()
                            + rcvMsg->getByteLength());

            //TODO - insert the rules to different triggers
            switch (this->getInternalState()->getType()) {
            case Patterns::InternalStateType::CountTrigger:
                this->getInternalState()->setWindowCounter(
                        this->getInternalState()->getWindowCounter() + 1);
            }

            //Convert message to be use for checking the crossed path
            TopicEvent* topic = dynamic_cast<TopicEvent*>(rcvMsg);

            //Evaluate the message to include it on the internal state
            //            if (this->getInternalState() != nullptr) {
            //                cout << " Before - internal size vector - operator - " << to_string(operatorId)<< " : "
            //                        << to_string(topic->getCrossedPaths().size()) << endl;
            //            }

            //            cout << " Before - size vector: "
            //                    << to_string(topic->getCrossedPaths().size()) << endl;
            if (topic->getCrossedPaths().size() == 0
                    && this->getInternalState()->getFirstMessage() == nullptr) {
                cMessage* dupMsg = dynamic_cast<cMessage*>(topic)->dup();
                this->getInternalState()->setFirstMessage(dupMsg);
            } else {
                //                cout << "Before filling internal state - operator - "
                //                        << to_string(operatorId) << " Size: "
                //                        << to_string(
                //                                this->getInternalState()->getGeneratedTimePaths().size())
                //                        << "Message size: "
                //                        << to_string(topic->getCrossedPaths().size()) << endl;

                this->getInternalState()->setGeneratedTimesVector(
                        topic->getCrossedPaths(),
                        topic->getFirstMGenerationCrossedPath());
                //                cout << " After filling internal state - operator - "
                //                        << to_string(operatorId) << " Size: "
                //                        << to_string(
                //                                this->getInternalState()->getGeneratedTimePaths().size())
                //                        << endl;
            }
            //            cout << " After - size vector: "
            //                    << to_string(topic->getCrossedPaths().size()) << endl;
            //
            //            if (this->getInternalState() != nullptr) {
            //                cout << " After - internal size vector - operator - " << to_string(operatorId)<< " : "
            //                        << to_string(this->getInternalState()->getGeneratedTimePaths().size()) << endl;
            //            }

            if (this->getInternalState()->isReady()) {
                this->sendStateDataToHistory();
                //The window reaches the window size then it will forward the message to
                // the next operation
                msg = dynamic_cast<cMessage*>(topic);
                this->ComputingOperation(msg);

            } else {
                delete topic;
                //Ask for new messages since the window must fulfill its requirements
                this->RequestMessageToQueue();
            }

        }
    }
}

void OperatorMsg::processMsg(cMessage *msg) {
    //Apply to the operator its selectivity following operator parameters
    int operatorId = this->getParentModule()->par("operatorId").intValue();

    if (!this->ApplyDiscarding(msg, operatorId)) {
        TopicEvent* topic;
        //Apply Data expansion or compression over the message bytes
        cPacket* rcvMsg = dynamic_cast<cPacket*>(msg);
        if (this->getInternalState() != nullptr) {
            //Correction to the size of the message, the message size is equal to the average size of the window
            rcvMsg->setByteLength(
                    (int) this->getInternalState()->getBytesSize()
                            / this->getInternalState()->getWindowCounter());

            //prepare the message to include the history of the time window (all paths)
            topic = dynamic_cast<TopicEvent*>(rcvMsg);

            //the field path in the message is clean, but the vector generatedPaths in the topic
            // will receive the old crossed paths
            topic->getFirstMGenerationCrossedPath().clear();
            topic->getCrossedPaths().clear();

            //Copying state information to the output message
            //            cout << "Size Vector : "
            //                    << to_string(
            //                            this->getInternalState()->getGeneratedTimePaths().size())
            //                    << endl;
            for (unsigned int i = 0;
                    i < this->getInternalState()->getGeneratedTimePaths().size();
                    i++) {
                topic->getCrossedPaths().push_back(
                        this->getInternalState()->getGeneratedTimePaths().at(i)->getPath());
                topic->getFirstMGenerationCrossedPath().push_back(
                        this->getInternalState()->getGeneratedTimePaths().at(i)->getGeneratedTimeStamp());
            }

            msg = dynamic_cast<cMessage*>(topic);
        }

        //Validate if this operation is the first in the path, if so, then it requires to start a path
        // into the topic. Otherwise, it will be include the operator to all paths
        topic = dynamic_cast<TopicEvent*>(msg);
        if (topic->getCrossedPaths().size() == 0) {

            if (this->getInternalState() != nullptr) {
                if (this->getInternalState()->getFirstMessage() != nullptr) {
                    topic->getCrossedPaths().push_back( { operatorId });
                    topic->getFirstMGenerationCrossedPath().push_back(
                            (dynamic_cast<TopicEvent*>(this->getInternalState()->getFirstMessage())->getTimeGenerated()));
                    delete this->getInternalState()->getFirstMessage();
                }

            } else {
                //When the fist operator is included to the path
                topic->getCrossedPaths().push_back( { operatorId });
                topic->getFirstMGenerationCrossedPath().push_back(
                        topic->getTimeGenerated());
            }
        } else {
            //Add the operation to all paths IDs
            for (unsigned int i = 0; i < topic->getCrossedPaths().size(); i++) {
                topic->getCrossedPaths().at(i).push_back(operatorId);
            }
        }

        if (this->getInternalState() != nullptr) {
            this->getInternalState()->InitializeState();
            //            cout << "Size Vector  after initializing: "
            //                    << to_string(
            //                            this->getInternalState()->getGeneratedTimePaths().size())
            //                    << endl;
        }

        //        if (topic->getCrossedPaths().size() > 0) {
        //            cout << "Size : " << to_string(topic->getCrossedPaths().size())
        //                    << endl;
        //        }

        //Add the current operation to all paths
        rcvMsg = dynamic_cast<cPacket*>(topic);
        if (this->getDataReduction() > 0) {

            this->ApplyDataExpansionCompression(rcvMsg);
        }

        //Set up the topic last state and changes the size of the state (stateful operators)
        //        simtime_t time = simTime() - topicEvent->getLastState();
        topic = dynamic_cast<TopicEvent*>(rcvMsg);
        topic->setLastState(simTime());

        //Get the service time pattern to the process
        simtime_t delayMsgQueue = this->getServiceDelay();

        //ForwarMsg evaluates the number of connections of the operator
        //  and replicates the message to all connections - if the result is
        //  false, there was a problem to forward the message
        if (!this->ForwardMsg(topic, delayMsgQueue, operatorId)) {
            throw cRuntimeError(
                    "Host %s can not handle the messages of the topic - state %d and substate %d",
                    this->getParentModule()->getName(), topic->getNextState(),
                    topic->getNextSubState());
        }

        //Refresh display of processed messages
        this->setProcessedEvents(this->getProcessedEvents() + 1);
        this->refreshDisplay();

        //Send data to the operator statistics
        this->sendDataToHistory(topic, delayMsgQueue, false);
        delete topic;
    }

    //Send message to the connected queue announcing that the operator is ready to process messages
    this->RequestMessageToQueue();
}

cModule* OperatorMsg::getModuleOutput() {
    string pathModule =
            this->getParentModule()->getParentModule()->getFullPath()
                    + ".nodeOutput";
    cModule* moduleOutput = this->getModuleByPath(pathModule.c_str());
    return moduleOutput;
}

void OperatorMsg::refreshDisplay() {
    // refresh statistics
    char buf[64];
    if (this->getInternalState() != nullptr) {
        sprintf(buf, "Del:%d Prc:%d WindowCounter:%d (Size - %d)",
                this->getDeletedEvents(), this->getProcessedEvents(),
                (int) this->getInternalState()->getWindowCounter(),
                (int) this->getInternalState()->getWindowSize());
    } else {
        sprintf(buf, "Del:%d Prc:%d", this->getDeletedEvents(),
                this->getProcessedEvents());
    }
    this->getDisplayString().setTagArg("t", 0, buf);
}

double OperatorMsg::getTimeWindow() const {
    return mTimeWindow;
}

void OperatorMsg::setTimeWindow(double timeWindow) {
    mTimeWindow = timeWindow;
}

double OperatorMsg::getAvailableCpu() const {
    return mAvailableCPU;
}

void OperatorMsg::setAvailableCpu(double availableCpu) {
    mAvailableCPU = availableCpu;
}

double OperatorMsg::getAvailableMem() const {
    return mAvailableMem;
}

void OperatorMsg::setAvailableMem(double availableMem) {
    mAvailableMem = availableMem;
}

double OperatorMsg::getCpuCost() const {
    return mCPUCost;
}

void OperatorMsg::setCpuCost(double cpuCost) {
    mCPUCost = cpuCost;
}

double OperatorMsg::getDataReduction() const {
    return mDataReduction;
}

void OperatorMsg::setDataReduction(double dataReduction) {
    mDataReduction = dataReduction;
}

int OperatorMsg::getDeletedEvents() const {
    return mDeletedEvents;
}

void OperatorMsg::setDeletedEvents(int deletedEvents) {
    mDeletedEvents = deletedEvents;
}

InternalState*& OperatorMsg::getInternalState() {
    return mInternalState;
}

void OperatorMsg::setInternalState(InternalState*& internalState) {
    mInternalState = internalState;
}

double OperatorMsg::getMemoryCost() const {
    return mMemoryCost;
}

void OperatorMsg::setMemoryCost(double memoryCost) {
    mMemoryCost = memoryCost;
}

int OperatorMsg::getNextSent() const {
    return mNextSent;
}

void OperatorMsg::setNextSent(int nextSent) {
    mNextSent = nextSent;
}

int OperatorMsg::getNextSplitLength() const {
    return mNextSplitLength;
}

void OperatorMsg::setNextSplitLength(int nextSplitLength) {
    mNextSplitLength = nextSplitLength;
}

bool OperatorMsg::isNextSplitted() const {
    return mNextSplitted;
}

void OperatorMsg::setNextSplitted(bool nextSplitted) {
    mNextSplitted = nextSplitted;
}

int OperatorMsg::getProcessedEvents() const {
    return mProcessedEvents;
}

void OperatorMsg::setProcessedEvents(int processedEvents) {
    mProcessedEvents = processedEvents;
}

double OperatorMsg::getSelectivity() const {
    return mSelectivity;
}

void OperatorMsg::setSelectivity(double selectivity) {
    mSelectivity = selectivity;
}

double OperatorMsg::getStateIncreaseRate() const {
    return mStateIncreaseRate;
}

void OperatorMsg::setStateIncreaseRate(double stateIncreaseRate) {
    mStateIncreaseRate = stateIncreaseRate;
}

int OperatorMsg::getType() const {
    return mType;
}

void OperatorMsg::setType(int type) {
    mType = type;
}

bool OperatorMsg::ApplyDiscarding(cMessage*& msg, int operatorId) {
    int iSelectivity = 0;

    if (this->getSelectivity() > 0 && this->getSelectivity() < 1
            && this->getInternalState() == nullptr) {
        iSelectivity = bernoulli(this->getSelectivity(), operatorId);
    }

    if (iSelectivity == 1) {
        //Memory control - Free memory
        freeMemory(this->getParentModule()->getParentModule(),
                dynamic_cast<cPacket*>(msg)->getByteLength());

        //        cout << "Event: " << msg->getName() << " -> discarded" << endl;

        delete msg;
        this->setDeletedEvents(this->getDeletedEvents() + 1);
        this->refreshDisplay();
        return true;
    }

    return false;
}

void OperatorMsg::ApplyDataExpansionCompression(cPacket*& msg) {
    //Memory control - Free memory
    freeMemory(this->getParentModule()->getParentModule(),
            msg->getByteLength());
    double bytes = msg->getByteLength() * this->getDataReduction();
    bytes = bytes < 1 ? 1 : bytes;
    msg->setByteLength((int) ceil(bytes));

    ////Memory control - allocate memory
    allocateMemory(this->getParentModule()->getParentModule(),
            msg->getByteLength());
}

simtime_t OperatorMsg::getServiceDelay() {
    simtime_t delayMsgQueue = Patterns::TIME_INTERNAL_MSG_QUEUE_MAX;
    string pathModule =
            this->gate("queueNextOut", 0)->getNextGate()->getOwnerModule()->getFullPath()
                    + ".msgQueue";
    if (this->getModuleByPath(pathModule.c_str())) {
        if (this->getModuleByPath(pathModule.c_str())->hasPar("serviceMax")) {
            delayMsgQueue = this->getModuleByPath(pathModule.c_str())->par(
                    "serviceMax");
        }
    }

    return delayMsgQueue;
}

bool OperatorMsg::ForwardMsg(TopicEvent*& topicEvent, simtime_t delayMsgQueue,
        int operatorId) {

    topicEvent->setKind(0);
    bool bTreated = false;
    int dupMsg = 0;

    vector<int> operatorIndexes =
            dynamic_cast<NodeOutput*>(this->getModuleOutput())->DefineNextOperatorsIndex(
                    topicEvent->getNextState(), topicEvent->getNextSubState(),
                    topicEvent->getAppName());

    for (unsigned int it = 0; it < operatorIndexes.size(); ++it) {

        TopicEvent* tp = topicEvent->dup();
        tp->setNextState(
                dynamic_cast<NodeOutput*>(this->getModuleOutput())->getRoutingTable().at(
                        operatorIndexes.at(it))->getNextProcessingOperatorId());
        tp->setNextSubState(
                dynamic_cast<NodeOutput*>(this->getModuleOutput())->getRoutingTable().at(
                        operatorIndexes.at(it))->getNextProcessingFissionId());

        tp->setTotalComp(tp->getTotalComp() + (simTime() - tp->getLastComp()));
        tp->setLastComm(simTime());

        tp->setName(
                (((string) tp->getName()) + "->" + to_string(tp->getNextState())
                        + "-" + to_string(tp->getNextSubState())).c_str());

        int iOut = -1;
        bool bSent = false;
        for (int i = 0; i < this->gateSize("queueNextOut"); ++i) {
            if (opp_strcmp(
                    this->gate("queueNextOut", i)->getNextGate()->getNextGate()->getOwnerModule()->getName(),
                    getOperatorName(tp->getAppName(), tp->getNextState(),
                            tp->getNextSubState()).c_str()) == 0) {
                if (dupMsg > 0) {
                    tp->setName(
                            (((string) tp->getName()) + "(" + to_string(dupMsg)
                                    + ")").c_str());
                }
                dupMsg++;
                sendDelayed(tp, delayMsgQueue, "queueNextOut", i);
                bSent = true;
                bTreated = true;
            }

            if (opp_strcmp(
                    this->gate("queueNextOut", i)->getNextGate()->getNextGate()->getOwnerModule()->getName(),
                    ((string) "nodeOutput").c_str()) == 0) {
                iOut = i;
            }
        }

        if (!bSent && iOut > -1) {
            if (dupMsg > 0) {
                tp->setName(
                        (((string) tp->getName()) + "(" + to_string(dupMsg)
                                + ")").c_str());
            }

            dupMsg++;
            sendDelayed(tp, delayMsgQueue, "queueNextOut", iOut);
            bSent = true;
            bTreated = true;
        }

    }

    return bTreated;
}

void OperatorMsg::RequestMessageToQueue() {
    cMessage* msgSend = new cMessage(Patterns::BASE_REQUEST);
    scheduleAt(simTime() + Patterns::TIME_INTERNAL_SCHEDULING, msgSend);
}

void OperatorMsg::ComputingOperation(cMessage* msg) {
    simtime_t computingTime = this->getCpuCost() / this->getAvailableCpu();
    //    cout << "Operator: "
    //            << to_string(this->getParentModule()->par("operatorId").intValue())
    //            << " Computation time: " << to_string(SIMTIME_DBL(computingTime)) << " CPU cost: "
    //            << to_string(this->getCpuCost()) << " CPU Available: "
    //            << to_string(this->getAvailableCpu()) << endl;

    msg->setKind(4);
    scheduleAt(simTime() + computingTime, msg);
}

int OperatorMsg::getSentEventRegs() const {
    return mSentEventRegs;
}

void OperatorMsg::setSentEventRegs(int sentEventRegs) {
    mSentEventRegs = sentEventRegs;
}

int OperatorMsg::getSentStateRegs() const {
    return mSentStateRegs;
}

void OperatorMsg::setSentStateRegs(int sentStateRegs) {
    mSentStateRegs = sentStateRegs;
}

} //namespace

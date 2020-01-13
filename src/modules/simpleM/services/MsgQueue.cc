#include "MsgQueue.h"
#include "../../../util/Patterns.h"
#include "../../msg/topicEvent/TopicEvent_m.h"
#include "../../msg/statistic/Statistic_m.h"
#include "../../msg/operatorDep/OperatorDep_m.h"

namespace fogstream {

Define_Module(MsgQueue);

MsgQueue::MsgQueue() {
    this->getQueueRequest().clear();
    this->getQueueTopic().clear();
    this->getQueueTopic().setName("topic");
    this->setRcvMsgs(0);
}

MsgQueue::~MsgQueue() {

}

void MsgQueue::initialize() {
    this->getParticipatingPaths().clear();
    this->setServiceMin(par("serviceMin").doubleValue());
    this->setServiceMax(par("serviceMax").doubleValue());
    this->setPendingMsg(Patterns::NoPending);
}

void MsgQueue::handleMessage(cMessage *msg) {
    const char* message = msg->getName();
    std::string messageName(message);
    std::string stdRequest(Patterns::BASE_REQUEST);
    std::string stdTopic(Patterns::BASE_TOPIC);

    if (messageName.find(stdRequest) != std::string::npos) {
        this->getQueueRequest().insert(msg);
        //emit(qlenRequest,queueRequest.getLength());

        if (this->getQueueTopic().getLength() > 0) {
            answerRequest();

        } else {
            this->setPendingMsg(Patterns::Pending);

        }
    } else if (messageName.find(stdTopic) != std::string::npos) {
        TopicEvent* topicEvent = dynamic_cast<TopicEvent*>(msg);

        std::string mname = this->getName();
        std::string pattern = "vertex-";
        if (mname.find(pattern) == std::string::npos) {
            topicEvent->setTotalComm(
                    topicEvent->getTotalComm()
                            + (simTime() - topicEvent->getLastComm()));
            topicEvent->setLastComp(simTime());
            //            simtime_t timeComm = (simTime() - topicEvent->getLastComm())
            //                    + topicEvent->getTotalComm();
            //            this->sendDataToHistory(timeComm);
        }
        //        topicEvent->setTotalComm(timeComm);
        //        emit(qTotalCommTime, timeComm);
        //        emit(qTotalCompTime, topicEvent->getTotalComp());

        simtime_t time = simTime() - topicEvent->getTimeGenerated();
        topicEvent->setLastQueue(simTime());

        this->getQueueTopic().insert(msg);

        this->refreshDisplay(this->getQueueTopic().getLength(), false);

        if (mname.find(pattern) != std::string::npos) {
            EV_INFO << "Total stored messages : "
                           << this->getQueueTopic().getLength() << endl;
        }

        if (this->getPendingMsg() == Patterns::Pending) {
            this->answerRequest();
        }

    }

    //If the queue is a sink then it will be created the
    if (msg->getKind() == Patterns::MessageType::ApplicationDeployment) {

        std::string mname = this->getName();
        std::string pattern = "vertex-";
        if (mname.find(pattern) != std::string::npos) {

            std::string pathM = this->getFullPath() + ".deployer";
            if (!this->getModuleByPath(pathM.c_str())) {

                string pathName = Patterns::MSG_PATHS;
                if (msg->hasObject(pathName.c_str())) {

                    cObject *obj = msg->getObject(pathName.c_str());
                    cQueue *queuePath = check_and_cast<cQueue *>(obj);
                    for (cQueue::Iterator it(*queuePath); !it.end(); it++) {
                        OperatorDep *paths = dynamic_cast<OperatorDep*>(*it);

                        cStringTokenizer tokenizer(paths->getName());
                        while (tokenizer.hasMoreTokens()) {
                            std::vector<int> array = cStringTokenizer(
                                    tokenizer.nextToken(), ";").asIntVector();
                            this->getParticipatingPaths().push_back(array);
                        }

                    }
                }

                this->createDeployerModule(Patterns::NAME_DEPLOYER);
            }
        }
    }
}

void MsgQueue::sendDataToHistory(simtime_t commTime) {
    if (this->getSentRegs() <= Patterns::STATISTIC_MAX_REGS) {
        /*Save statistics*/
        Statistic* statInput = new Statistic();
        statInput->setName("Queue Statistics");
        statInput->setKind(Patterns::MessageType::Statistics);
        statInput->setSTimestamp(simTime());
        statInput->setType(Patterns::StatisticType::Operator);
        statInput->setMeasurement(Patterns::OperatorMeasurement::Queue);
        statInput->setOperatorID(
                this->getParentModule()->par("operatorId").intValue());
        statInput->setHostID(
                this->getParentModule()->getParentModule()->par("id").intValue());
        statInput->setQueueSize(this->getQueueTopic().getLength());
        statInput->setCommTime(commTime);
        this->setSentRegs(this->getSentRegs() + 1);

        send(statInput, "outIn", 0);
    }
}

cQueue& MsgQueue::getQueueTopic() {
    return mQueueTopic;
}

void MsgQueue::setQueueTopic(const cQueue& queueTopic) {
    mQueueTopic = queueTopic;
}

void MsgQueue::createDeployerModule(const char* moduletype) {
    cModule* module;
    cModuleType *mType = cModuleType::get(moduletype);

    if (mType) {
        std::string name = "deployer";

        module = mType->create(name.c_str(), this);

        if (module) {
            module->buildInside();
            //module->callInitialize();
            module->scheduleStart(simTime());

            std::string _module_name = "scheduler";
            for (int i = 0; i < this->gateSize("inIn"); ++i) {
                if (opp_strcmp(_module_name.c_str(),
                        this->gate("inIn", i)->getPreviousGate()->getOwnerModule()->getName())
                        != 0) {

                    module->setGateSize("in", module->gateSize("in") + 1);

                    this->gate("inIn", i)->connectTo(
                            module->gate("in", module->gateSize("in") - 1));

                    module->setGateSize("out", module->gateSize("out") + 1);

                    module->gate("out", module->gateSize("out") - 1)->connectTo(
                            this->gate("outIn", this->gateSize("outIn") - 1));

                    module->callInitialize();
                }
            }

        } else {
            throw cRuntimeError("Error to create module");
        }
    } else {
        throw cRuntimeError("Error to create type module");
    }
}

double MsgQueue::getServiceTime(double min, double max) {
    if (max == 0) {
        return 0;

    } else {
        //TODO - Create a function that use a probability for return the value
        return uniform(min, max);

    }
}

void MsgQueue::answerRequest() {

    simtime_t delayProcess = this->getServiceTime(this->getServiceMin(),
            this->getServiceMax());
    cMessage *msgTopic = (cMessage*) this->getQueueTopic().pop();

    this->refreshDisplay(this->getQueueTopic().getLength(), false);

    cMessage *msgRequest = (cMessage*) this->getQueueRequest().pop();
    //emit(qlenRequest,queueRequest.getLength());

    sendDelayed(msgTopic, delayProcess, "outIn", 0);

    if (this->getQueueRequest().isEmpty()) {
        this->setPendingMsg(Patterns::NoPending);
    }

    delete msgRequest;

}

void MsgQueue::refreshDisplay(long queueLength, bool bEmit) const {
    // refresh statistics
    char buf[32];
    sprintf(buf, "Rcv Msg:%ld", queueLength);
    this->getDisplayString().setTagArg("t", 0, buf);

    if (bEmit) {

    }
}

long MsgQueue::getRcvMsgs() const {
    return mRcvMsgs;
}

void MsgQueue::setRcvMsgs(long rcvMsgs) {
    mRcvMsgs = rcvMsgs;
}

vector<vector<int> >& MsgQueue::getParticipatingPaths() {
    return mParticipatingPaths;
}

void MsgQueue::setParticipatingPaths(
        const vector<vector<int> >& participatingPaths) {
    mParticipatingPaths = participatingPaths;
}

int MsgQueue::getPendingMsg() const {
    return mPendingMsg;
}

void MsgQueue::setPendingMsg(int pendingMsg) {
    mPendingMsg = pendingMsg;
}

cQueue& MsgQueue::getQueueRequest() {
    return mQueueRequest;
}

void MsgQueue::setQueueRequest(const cQueue& queueRequest) {
    mQueueRequest = queueRequest;
}

double MsgQueue::getServiceMax() const {
    return mServiceMax;
}

void MsgQueue::setServiceMax(double serviceMax) {
    mServiceMax = serviceMax;
}

double MsgQueue::getServiceMin() const {
    return mServiceMin;
}

void MsgQueue::setServiceMin(double serviceMin) {
    mServiceMin = serviceMin;
}

int MsgQueue::getSentRegs() const {
    return mSentRegs;
}

void MsgQueue::setSentRegs(int sentRegs) {
    mSentRegs = sentRegs;
}

bool MsgQueue::isLogData() const {
    return mLogData;
}

void MsgQueue::setLogData(bool logData) {
    mLogData = logData;
}

} //namespace



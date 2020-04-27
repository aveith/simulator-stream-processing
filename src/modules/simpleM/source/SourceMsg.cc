#include "SourceMsg.h"
#include "../services/MsgQueue.h"
#include "../../msg/topicEvent/TopicEvent_m.h"
#include "../../../util/Patterns.h"

namespace fogstream {

Define_Module(SourceMsg);

void SourceMsg::initialize() {
    this->eventNumber = 0;

//        if (opp_strcmp(this->getName(), "vertex-6") == 0) {
    cMessage* msg = new cMessage(Patterns::MSG_INITIALISATION);
    scheduleAt(simTime() + Patterns::TIME_INITIALISATION, msg);
//        }
}

void SourceMsg::handleMessage(cMessage *msg) {
    //scheduleAt(simTime()+Patterns::TIME_INITIALISATION, msg);
    simtime_t rate = this->par("departureRate");
    simtime_t arrivalRate = 1 / rate;
//    cout<< "Rate " << rate << endl;
//        cout<< "Data source -> sending only a message to test dataflow" << endl;
    scheduleAt(simTime() + arrivalRate, msg);
    sendEvent();

}

void SourceMsg::sendEvent() {
    //Get module paremeters
    long bytes = this->par("bytesMsg");
    std::string app = this->par("app");
    long state = this->par("state");

    TopicEvent* msgSend = new TopicEvent(
            getEventName(this->eventNumber, state).c_str());

    simtime_t timeStamp = simTime();
    //TODO - Should I create a variation between messages sizes?
    msgSend->setByteLength(bytes);
    msgSend->setAppName(app.c_str());
    msgSend->setNextState(state);
    msgSend->setNextSubState(0);
    msgSend->setSrcID(state);
    msgSend->setTimeGenerated(timeStamp);
    msgSend->setLastQueue(simTime());
    msgSend->setLastState(simTime());
    msgSend->setLastComp(0);
    msgSend->setLastComm(timeStamp);
    msgSend->setTotalComp(0);
    msgSend->setTotalComm(0);

    //TODO - Should I change the pattern between events' send by p.f.d.?
    sendDelayed(msgSend, 0, "outExt", 0);
    this->eventNumber++;
    //    this->refreshDisplay();
}

//
void SourceMsg::refreshDisplay() const {
    // refresh statistics
    char buf[32];
    sprintf(buf, "Sent:%d", this->eventNumber);
    this->getDisplayString().setTagArg("t", 0, buf);
}

} //fogstream

#include "Storage.h"
#include "../../../../util/Patterns.h"
#include "../../../msg/topicEvent/TopicEvent_m.h"
#include "../MsgQueue.h"
namespace fogstream {

Define_Module(Storage);

void Storage::initialize()
{
    this->queueTopic.clear();
    this->queueTopic.setName("topic");
    this->setRcvMsgs(0);

}

void Storage::handleMessage(cMessage *msg){
    const char* message = msg->getName();
    std::string messageName (message);
    std::string stdRequest (Patterns::BASE_REQUEST);
    std::string stdTopic (Patterns::BASE_TOPIC);


//    this->queueTopic.insert(msg);
    //emit(qlenTopic,queueTopic.getLength());

    if (messageName.find(stdTopic)!=std::string::npos ){
        this->setRcvMsgs(this->getRcvMsgs()+1);
        this->refreshDisplay(this->getRcvMsgs());

        MsgQueue* msgQ = dynamic_cast<MsgQueue*> (this->getModuleMsgQueue());
        if (msgQ){
            TopicEvent* rcvMsg = dynamic_cast<TopicEvent*> (msg);
            simtime_t time = simTime() - rcvMsg->getTimeGenerated();
            delete rcvMsg;
//            emit(qTotalTime, time);
//            timeMessageVector.record(time);

//            simtime_t timeComm = (simTime() - rcvMsg->getLastComm()) + rcvMsg->getTotalComm();
//            rcvMsg->setTotalComm(timeComm);
//            msgQ->emitTotalComm(rcvMsg->getTotalComm());
//            msgQ->emitTotalComp(rcvMsg->getTotalComp());

//            emit(qlenTopic, this->queueTopic.getLength());
//            emit(qMsgSize, rcvMsg->getByteLength());

            msgQ->setRcvMsgs(msgQ->getRcvMsgs() + 1);
            msgQ->refreshDisplay(msgQ->getRcvMsgs(), true);
//            msgQ->emitTotalTime(time);
//            emit(qTotalCommTime, rcvMsg->getTotalComm());
//            emit(qTotalCompTime, rcvMsg->getTotalComp());

//            EV_WARN << "---------------------------> Total Time: " << time
//                    << " Comm Time: "  << rcvMsg->getTotalComm()
//                    << " Comp Time: " << rcvMsg->getTotalComp()
//                    << "<------------------------------" << endl;

        }else{
            throw cRuntimeError("Something wrong happen to get the MsgQueue Module!");
        }
    }

}

cModule* Storage::getModuleMsgQueue(){

    cModule* msgQ = this->getParentModule();

    return msgQ;
}

int Storage::getRcvMsgs() const {
    return mRcvMsgs;
}

void Storage::setRcvMsgs(int rcvMsgs) {
    mRcvMsgs = rcvMsgs;
}

void Storage::refreshDisplay(long queueLength) const{
    // refresh statistics
    char buf[32];
    sprintf(buf, "Rcv Msg:%ld", queueLength);
    this->getDisplayString().setTagArg("t", 0, buf);
}

} //namespace

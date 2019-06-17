#include "SinkMsg.h"

namespace fogstream {

Define_Module(SinkMsg);

void SinkMsg::initialize()
{
    // TODO - Generated method body
    cMessage* msg = new cMessage("Init");
    scheduleAt(simTime(), msg);
}

void SinkMsg::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
       if (msg->isSelfMessage()){
           delete msg;
           cMessage* msgSend = new cMessage("Request");

           //EV_INFO <<"Sending request " << endl;
           send(msgSend, "outExt", 0);
       }else{
           //EV_INFO <<"Message was received from " << msg->getSenderModule()->getName() << endl;
           delete msg;

           cMessage* msgSend = new cMessage("Request");
           sendDelayed(msgSend, 5, "outExt", 0);
       }
}

} //namespace

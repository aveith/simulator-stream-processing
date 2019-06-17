
#ifndef __FOGSTREAM_SINKMSG_H_
#define __FOGSTREAM_SINKMSG_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace fogstream {

class SinkMsg : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

} //namespace

#endif

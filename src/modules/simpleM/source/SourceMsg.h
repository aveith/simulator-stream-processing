#ifndef __FOGSTREAM_SOURCEMSG_H_
#define __FOGSTREAM_SOURCEMSG_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace fogstream {

class SourceMsg : public cSimpleModule
{
  private:
    int eventNumber;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void refreshDisplay() const;

    virtual void sendEvent();
};

} //namespace

#endif

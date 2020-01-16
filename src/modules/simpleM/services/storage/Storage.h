#ifndef __FOGSTREAM_STORAGE_H_
#define __FOGSTREAM_STORAGE_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace fogstream {

/**
 * TODO - Generated class
 */
class Storage : public cSimpleModule
{
public:
    int getRcvMsgs() const;
    void setRcvMsgs(int rcvMsgs);

protected:
    int mRcvMsgs;
    static simsignal_t qlat;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual cModule* getModuleMsgQueue();

private:
    cQueue queueTopic;
    virtual void refreshDisplay(long queueLenght) const;

};

} //namespace

#endif

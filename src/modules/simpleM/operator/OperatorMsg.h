#ifndef __FOGSTREAM_OPERATORMSG_H_
#define __FOGSTREAM_OPERATORMSG_H_

#include <omnetpp.h>
#include "../../msg/topicEvent/TopicEvent_m.h"
#include "internalstate/InternalState.h"

using namespace omnetpp;
using namespace std;

namespace fogstream {

class OperatorMsg: public cSimpleModule {
public:
    OperatorMsg();
    virtual ~OperatorMsg();
    double getTimeWindow() const;
    void setTimeWindow(double timeWindow);
    double getAvailableCpu() const;
    void setAvailableCpu(double availableCpu);
    double getAvailableMem() const;
    void setAvailableMem(double availableMem);
    double getCpuCost() const;
    void setCpuCost(double cpuCost);
    double getDataReduction() const;
    void setDataReduction(double dataReduction);
    int getDeletedEvents() const;
    void setDeletedEvents(int deletedEvents);
    InternalState*& getInternalState();
    void setInternalState(InternalState*& internalState);
    double getMemoryCost() const;
    void setMemoryCost(double memoryCost);
    int getNextSent() const;
    void setNextSent(int nextSent);
    int getNextSplitLength() const;
    void setNextSplitLength(int nextSplitLength);
    bool isNextSplitted() const;
    void setNextSplitted(bool nextSplitted);
    int getProcessedEvents() const;
    void setProcessedEvents(int processedEvents);
    double getSelectivity() const;
    void setSelectivity(double selectivity);
    double getStateIncreaseRate() const;
    void setStateIncreaseRate(double stateIncreaseRate);
    int getType() const;
    void setType(int type);
    int getSentEventRegs() const;
    void setSentEventRegs(int sentEventRegs);
    int getSentStateRegs() const;
    void setSentStateRegs(int sentStateRegs);

private:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void refreshDisplay();

    void processMsg(cMessage *msg);
    void sendDataToHistory(cMessage *msg, simtime_t compTime, bool bInput);
    void sendStateDataToHistory();
    cModule* getModuleOutput();
    bool ApplyDiscarding(cMessage*& msg, int operatorId);
    void ApplyDataExpansionCompression(cPacket*& msg);
    simtime_t getServiceDelay();
    bool ForwardMsg(TopicEvent*& topicEvent, simtime_t delayMsgQueue,
            int operatorId);
    void RequestMessageToQueue();
    void ComputingOperation(cMessage* msg);


protected:
    //received by parameters
    bool mNextSplitted;
    int mNextSplitLength = 0;
    double mSelectivity = 0;
    double mDataReduction = 0;
    int mType = -1;
    double mStateIncreaseRate = 0;
    double mMemoryCost = 0;
    double mCPUCost = 0;

    //Internal management
    InternalState* mInternalState = nullptr;
    int mNextSent = 0;
    int mDeletedEvents = 0;
    int mProcessedEvents = 0;
    double mAvailableCPU = 0;
    double mAvailableMem = 0;
    double mTimeWindow = 0;

    int mSentStateRegs = 0;
    int mSentEventRegs = 0;
};

} //namespace

#endif

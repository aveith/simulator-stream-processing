#ifndef __FOGSTREAM_MSGQUEUE_H_
#define __FOGSTREAM_MSGQUEUE_H_

#include <omnetpp.h>

using namespace std;
using namespace omnetpp;


namespace fogstream {

class MsgQueue : public cSimpleModule
{
  public:
    MsgQueue();
    virtual ~MsgQueue();
    double getServiceTime(double min, double max);
    void refreshDisplay(long queueLength, bool bEmit) const;
    void emitTotalTime(simtime_t time);
    void emitTotalComp(simtime_t time);
    void emitTotalComm(simtime_t time);
    cQueue& getQueueTopic();
    void setQueueTopic(const cQueue& queueTopic);
    long getRcvMsgs() const;
    void setRcvMsgs(long rcvMsgs);
    vector<vector<int> >& getParticipatingPaths();
    void setParticipatingPaths(const vector<vector<int> >& participatingPaths);
    int getPendingMsg() const;
    void setPendingMsg(int pendingMsg);
    cQueue& getQueueRequest();
    void setQueueRequest(const cQueue& queueRequest);
    double getServiceMax() const;
    void setServiceMax(double serviceMax);
    double getServiceMin() const;
    void setServiceMin(double serviceMin);
    int getSentRegs() const;
    void setSentRegs(int sentRegs);
    bool isLogData() const;
    void setLogData(bool logData);

  protected:
    vector<vector<int>> mParticipatingPaths;
    double mServiceMin;
    double mServiceMax;
    int mPendingMsg;
    cQueue mQueueRequest;
    cQueue mQueueTopic;
    long mRcvMsgs;
    int mSentRegs = 0;
    bool mLogData = false;

  private:
    void initialize();
    void handleMessage(cMessage *msg);
    void sendDataToHistory(simtime_t commTime);

    void answerRequest();
    void createDeployerModule(const char* moduletype);

};

} //namespace

#endif

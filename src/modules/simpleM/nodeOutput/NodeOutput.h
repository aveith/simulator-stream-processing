#ifndef __FOGSTREAM_NODEOUTPUT_H_
#define __FOGSTREAM_NODEOUTPUT_H_
#include <vector>
#include <unordered_map>
#include <cassert>
#include <map>
#include <omnetpp.h>
#include "../../msg/topicEvent/TopicEvent_m.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {
class RoutingData {
public:
    RoutingData(int nextProcessingOperatorId, int nextProcessingFissionId,
            string appName, int currentProcessingOperatorId,
            int currentProcessingFissionId, string destinationHost,
            int gateIndex);
    string getAppName() const;
    void setAppName(string appName);
    int getCurrentProcessingFissionId() const;
    void setCurrentProcessingFissionId(int currentProcessingFissionId);
    int getCurrentProcessingOperatorId() const;
    void setCurrentProcessingOperatorId(int currentProcessingOperatorId);
    string getDestinationHost() const;
    void setDestinationHost(string destinationHost);
    int getNextProcessingFissionId() const;
    void setNextProcessingFissionId(int nextProcessingFissionId);
    int getNextProcessingOperatorId() const;
    void setNextProcessingOperatorId(int nextProcessingOperatorId);
    int getGateIndex() const;
    void setGateIndex(int gateIndex);

protected:
    int mNextProcessingOperatorId;
    int mNextProcessingFissionId;
    string mAppName ="";

    int mCurrentProcessingOperatorId;
    int mCurrentProcessingFissionId;
    string mDestinationHost = "";
    int mGateIndex = -1;
};

class NodeOutput: public cSimpleModule {
protected:
    static constexpr const char* GATE_NAME = "outEx";

    int mSentRegs = 0;
    vector<RoutingData*> mRoutingTable;
    unordered_map<int, cQueue*> mScheduledMessages;
    bool mLogData = true;

private:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void refreshDisplay();
    virtual void sendDataToHistory(cMessage* msg);

    bool SendMessageToNextOperator();

    cPacket* ConsumeMsgFromWorkingList(cQueue* queue);

    void ScheduleMessage(cMessage* msg);

    int DefineGateIndex(string destinationHost);

    int DetermineQueueSizes();

    int VerifyRoutingIndex(int nextProcessingOperatorId,
            int nextProcessingFissionId, string appName,
            int currentProcessingOperatorId, int currentProcessingFissionId);

    int DetermineGateIndex(int nextProcessingOperatorId,
            int nextProcessingFissionId, const char* appName);

public:
    int getSentRegs() const;
    void setSentRegs(int sentRegs);

    void AddNewRoute(int nextProcessingOperatorId, int nextProcessingFissionId,
            string appName, int currentProcessingOperatorId,
            int currentProcessingFissionId, string hostName = "",
            int gateIndex = -1);

    unordered_map<int, cQueue*>& getScheduledMessages();
    void setScheduledMessages(unordered_map<int, cQueue*>& scheduledMessages);
    vector<RoutingData*>& getRoutingTable();
    void setRoutingTable(const vector<RoutingData*>& routingTable);

    vector<int> DefineNextOperatorsIndex(int nextOperatorId, int nextFissionId,
            const char * appName);
    bool isLogData() const;
    void setLogData(bool logData);
};

} //namespace

#endif

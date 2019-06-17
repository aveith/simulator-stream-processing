#ifndef __FOGSTREAM_NODEMANAGER_H_
#define __FOGSTREAM_NODEMANAGER_H_

#include <omnetpp.h>
#include "../../msg/operator/Operator_m.h"

using namespace omnetpp;


namespace fogstream {

class NodeManager : public cSimpleModule
{

protected:
    std::vector<cModule*> operatorModule;
    std::vector<cModuleType*> moduleType;
    enum MessageMode {
        NoIteration, EdgeObject, OperatorDeploymentObject
    };


private:

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    cModule* createModule(cModule* srcModule, const char* operatorName, const char* appName, Operator* parameters, bool firstModule);
    void addConnections(cModule* srcModule, cModule* dstModule, bool endModule);
    cModule* getModuleOutput();
    void applicationDeployment(cMessage *msg);
    void sendMsgApp(cMessage *msg);
    void setParModule(const char* appName, cModule *module, Operator* parameters, bool firstModule);

    void refreshRoutingTable(cMessage *msg);

    void AddNetworkRoute(int messageMode, cObject *obj);

};

} //namespace

#endif

#ifndef RECONFIGURATION_STATISTICS_HOST_HOST_H_
#define RECONFIGURATION_STATISTICS_HOST_HOST_H_

#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Host {
public:
    Host();
    virtual ~Host();
    double getArrivalMsgSize();

    void setArrivalMsgSize(double arrivalMsgSize);
    double getArrivalRate();
    void setArrivalRate(double arrivalRate);
    double getCommRequirement();
    void setCommRequirement(double commRequirement);
    simtime_t& getCommTime();
    void setCommTime(simtime_t& commTime);
    double getMemoryRequirement();
    void setMemoryRequirement(double memoryRequirement);
    double getQueueSize();
    void setQueueSize(double queueSize);

protected:
    double mArrivalMsgSize = 0;
    double mArrivalRate = 0;
    simtime_t mCommTime = 0;
    double mCommRequirement = 0;
    double mMemoryRequirement = 0; //the sum of all memory requirements mapped the host
    double mQueueSize = 0;
};

class HostAvailability {
public:
    HostAvailability();
    HostAvailability(int hostId, double availableMemory, double availableCpu,
            int availableSlots);
    virtual ~HostAvailability();

    double getAvailableCpu();
    void setAvailableCpu(double availableCpu);
    double getAvailableMemory();
    void setAvailableMemory(double availableMemory);
    int getHostId();
    void setHostId(int hostId);
    string toString();
    int getAvailableSlots() const;
    void setAvailableSlots(int availableSlots);

protected:
    int mHostID;
    double mAvailableMemory;
    double mAvailableCPU;
    int mAvailableSlots;

};
} /* namespace fogstream */

#endif /* RECONFIGURATION_STATISTICS_HOST_HOST_H_ */

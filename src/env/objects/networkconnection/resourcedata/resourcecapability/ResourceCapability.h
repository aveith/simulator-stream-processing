#ifndef CONFIGURATION_OBJECTS_NETWORKCONNECTION_RESOURCEDATA_RESOURCECAPABILITY_RESOURCECAPABILITY_H_
#define CONFIGURATION_OBJECTS_NETWORKCONNECTION_RESOURCEDATA_RESOURCECAPABILITY_RESOURCECAPABILITY_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class ResourceCapability {
public:
    ResourceCapability(int hostId, double memory, double cpu, int slotNumber, int servers);
    virtual ~ResourceCapability();
    double getCpu() const;
    void setCpu(double cpu);
    int getHostId() const;
    void setHostId(int hostId);
    double getMemory() const;
    void setMemory(double memory);
    int getSlotNumber() const;
    void setSlotNumber(int slotNumber);
    int getServers() const;
    void setServers(int servers);

private:
    int mHostID = -1;
    double mMemory = 0; //bytes
    double mCPU = 0; //IPS
    int mSlotNumber = -1;
    int mServers = 1;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_NETWORKCONNECTION_RESOURCEDATA_RESOURCECAPABILITY_RESOURCECAPABILITY_H_ */

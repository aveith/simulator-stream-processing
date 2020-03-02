#ifndef CONFIGURATION_TANEJA_RESOURCE_RESOURCEBASE_H_
#define CONFIGURATION_TANEJA_RESOURCE_RESOURCEBASE_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class ResourceBase {
public:
    ResourceBase();
    ResourceBase(int hostId, double cpu, double memory, double bandwidth);

    virtual ~ResourceBase();
    double getBandwidth() const;
    void setBandwidth(double bandwidth);
    double getCpu() const;
    void setCpu(double cpu);
    double getMemory() const;
    void setMemory(double memory);
    int getHostId() const;
    void setHostId(int hostId);

protected:
    int mHostId;
        double mMemory;
        double mCPU;
        double mBandwidth;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_TANEJA_RESOURCE_RESOURCEBASE_H_ */

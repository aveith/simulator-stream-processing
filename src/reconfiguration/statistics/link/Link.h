#ifndef RECONFIGURATION_STATISTICS_LINK_LINK_H_
#define RECONFIGURATION_STATISTICS_LINK_LINK_H_

#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Link {
public:
    Link();
    virtual ~Link();

    double getArrivalMsgSize() const;
    void setArrivalMsgSize(double arrivalMsgSize);
    double getArrivalRate() const;
    void setArrivalRate(double arrivalRate);
    double getCommRequirement() const;
    void setCommRequirement(double commRequirement);
    double getCommTime() const;
    void setCommTime(double commTime = 0);
    double getMemoryRequirement() const;
    void setMemoryRequirement(double memoryRequirement);
    double getQueueSize() const;
    void setQueueSize(double queueSize);

protected:
    double mArrivalMsgSize = 0;
    double mArrivalRate = 0;
    double mCommTime = 0;
    double mCommRequirement = 0;
    double mMemoryRequirement = 0; //the sum of all memory requirements mapped the host
    double mQueueSize = 0;
};

class LinkAvailability {
public:
    LinkAvailability();
    LinkAvailability(int linkId, double availableBandwidth);
    virtual ~LinkAvailability();

    void setAvailableBandwidth(double availableBandwidth);
    void setLinkId(int linkId);
    double getAvailableBandwidth() const;
    int getLinkId() const;

    string toString();

private:
    int mLinkID;
    double mAvailableBandwidth;
};
} /* namespace fogstream */

#endif /* RECONFIGURATION_STATISTICS_LINK_LINK_H_ */

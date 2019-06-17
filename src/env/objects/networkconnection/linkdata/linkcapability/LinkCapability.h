#ifndef CONFIGURATION_OBJECTS_NETWORKCONNECTION_LINKDATA_LINKCAPABILITY_LINKCAPABILITY_H_
#define CONFIGURATION_OBJECTS_NETWORKCONNECTION_LINKDATA_LINKCAPABILITY_LINKCAPABILITY_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class LinkCapability {
public:
    LinkCapability(int linkId, double bandwidth);
    virtual ~LinkCapability();
    double getBandwidth() const;
    void setBandwidth(double bandwidth);
    int getLinkId() const;
    void setLinkId(int linkId);

private:
    int mLinkID;
    double mBandwidth;

};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_NETWORKCONNECTION_LINKDATA_LINKCAPABILITY_LINKCAPABILITY_H_ */

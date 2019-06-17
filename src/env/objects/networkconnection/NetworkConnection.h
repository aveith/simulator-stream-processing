#ifndef CONFIGURATION_OBJECTS_NETWORKCONNECTION_NETWORKCONNECTION_H_
#define CONFIGURATION_OBJECTS_NETWORKCONNECTION_NETWORKCONNECTION_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class NetworkConnection {
public:
    NetworkConnection(int sourceId, int destinationId, int linkId);
    virtual ~NetworkConnection();
    int getDestinationId() const;
    void setDestinationId(int destinationId);
    int getLinkId() const;
    void setLinkId(int linkId);
    int getSourceId() const;
    void setSourceId(int sourceId);

protected:
    int mSourceID;
    int mDestinationID;
    int mLinkID;
};

} /* namespace fogstream */

#endif

#ifndef CONFIGURATION_OBJECTS_DATASINK_DATASINK_H_
#define CONFIGURATION_OBJECTS_DATASINK_DATASINK_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class DataSink {
public:
    DataSink(int hostId, int operatorId);
    virtual ~DataSink();
    int getHostId() const;
    void setHostId(int hostId);
    int getOperatorId() const;
    void setOperatorId(int operatorId);

protected:
    int mHostID;
    int mOperatorID;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_DATASINK_DATASINK_H_ */

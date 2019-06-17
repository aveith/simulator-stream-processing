#ifndef CONFIGURATION_OBJECTS_DATASOURCE_DATASOURCE_H_
#define CONFIGURATION_OBJECTS_DATASOURCE_DATASOURCE_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class DataSource {
public:
    DataSource(int hostId, int operatorId, double arrivalRate,
            double arrivalMsgSize);
    virtual ~DataSource();
    double getArrivalMsgSize() const;
    void setArrivalMsgSize(double arrivalMsgSize);
    double getArrivalRate() const;
    void setArrivalRate(double arrivalRate);
    int getHostId() const;
    void setHostId(int hostId);
    int getOperatorId() const;
    void setOperatorId(int operatorId);

protected:
    int mHostID;
    int mOperatorID;
    double mArrivalRate;
    double mArrivalMsgSize;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_DATASOURCE_DATASOURCE_H_ */

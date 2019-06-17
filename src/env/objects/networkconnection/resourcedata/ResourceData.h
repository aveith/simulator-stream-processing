#ifndef CPLEXSCHEDULER_OBJECTS_RESOURCEDATA_RESOURCEDATA_H_
#define CPLEXSCHEDULER_OBJECTS_RESOURCEDATA_RESOURCEDATA_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class ResourceData {
public:
    ResourceData(int id, int type, double cost);

    virtual ~ResourceData();

    double getCost() const;
    void setCost(double cost);
    int getId() const;
    void setId(int id);
    int getType() const;
    void setType(int type);
    int getGatewayId() const;
    void setGatewayId(int gatewayId);
    bool isMapped() const;
    void setMapped(bool mapped);

private:
    int mID;
    int mType;
    double mCost;
    int mGatewayId = -1;
    bool mMapped = false;
};

} /* namespace fogstream */

#endif /* CPLEXSCHEDULER_OBJECTS_RESOURCEDATA_RESOURCEDATA_H_ */

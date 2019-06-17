#ifndef CPLEXSCHEDULER_OBJECTS_LINKDATA_LINKDATA_H_
#define CPLEXSCHEDULER_OBJECTS_LINKDATA_LINKDATA_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class LinkData {
public:
    LinkData(int id, double delay, double cost);
    virtual ~LinkData();
    double getCost() const;
    void setCost(double cost);
    double getDelay() const;
    void setDelay(double delay);
    int getId() const;
    void setId(int id);

private:
    int mID;
    double mDelay;
    double mCost;
};

} /* namespace fogstream */

#endif /* CPLEXSCHEDULER_OBJECTS_LINKDATA_LINKDATA_H_ */

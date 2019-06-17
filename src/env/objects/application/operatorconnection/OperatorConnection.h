#ifndef CONFIGURATION_OBJECTS_APPLICATION_OPERATORCONNECTION_OPERATORCONNECTION_H_
#define CONFIGURATION_OBJECTS_APPLICATION_OPERATORCONNECTION_OPERATORCONNECTION_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class OperatorConnection {
public:
    OperatorConnection(int fromOperatorId, int toOperatorId, double rho);
    virtual ~OperatorConnection();
    int getFromOperatorId() const;
    void setFromOperatorId(int fromOperatorId);
    double getRho() const;
    void setRho(double rho);
    int getToOperatorId() const;
    void setToOperatorId(int toOperatorId);

protected:
    int mFromOperatorID;
    int mToOperatorID;
    double mRHO;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_APPLICATION_OPERATORCONNECTION_OPERATORCONNECTION_H_ */

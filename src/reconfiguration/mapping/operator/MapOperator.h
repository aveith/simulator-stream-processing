#ifndef RECONFIGURATION_MAPPING_OPERATOR_MAPOPERATOR_H_
#define RECONFIGURATION_MAPPING_OPERATOR_MAPOPERATOR_H_

#include "../../statistics/operator/OperatorStatistics.h"

#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class MapOperator {
public:
    MapOperator();
    MapOperator(int operatorId, int hostId, OperatorStatistics realStatistics,
            OperatorStatistics estimateStatistics);
    virtual ~MapOperator();

    OperatorStatistics& getEstimateStatistics();
    void setEstimateStatistics(const OperatorStatistics& estimateStatistics);
    int getHostId() const;
    void setHostId(int hostId);
    int getOperatorId() const;
    void setOperatorId(int operatorId);
    OperatorStatistics& getRealStatistics();
    void setRealStatistics(const OperatorStatistics& realStatistics);
    string toString();
protected:
    int mOperatorID;
    int mHostID;

    OperatorStatistics mRealStatistics;
    OperatorStatistics mEstimateStatistics;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MAPPING_OPERATOR_MAPOPERATOR_H_ */

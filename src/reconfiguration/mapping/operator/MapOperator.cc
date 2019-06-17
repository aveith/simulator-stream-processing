#include "MapOperator.h"

namespace fogstream {

MapOperator::MapOperator() {
    // TODO Auto-generated constructor stub

}

MapOperator::MapOperator(int operatorId, int hostId,
        OperatorStatistics realStatistics,
        OperatorStatistics estimateStatistics) {
    this->setOperatorId(operatorId);
    this->setHostId(hostId);
    this->setRealStatistics(realStatistics);
    this->setEstimateStatistics(estimateStatistics);
}

MapOperator::~MapOperator() {
    // TODO Auto-generated destructor stub
}

OperatorStatistics& MapOperator::getEstimateStatistics() {
    return mEstimateStatistics;
}

void MapOperator::setEstimateStatistics(
        const OperatorStatistics& estimateStatistics) {
    mEstimateStatistics = estimateStatistics;
}

int MapOperator::getHostId() const {
    return mHostID;
}

void MapOperator::setHostId(int hostId) {
    mHostID = hostId;
}

int MapOperator::getOperatorId() const {
    return mOperatorID;
}

void MapOperator::setOperatorId(int operatorId) {
    mOperatorID = operatorId;
}

OperatorStatistics& MapOperator::getRealStatistics() {
    return mRealStatistics;
}

void MapOperator::setRealStatistics(const OperatorStatistics& realStatistics) {
    mRealStatistics = realStatistics;
}
string MapOperator::toString() {
    return "MapOperator;" + to_string(this->getHostId()) + ";"
            + to_string(this->getOperatorId());
}

} /* namespace fogstream */


#include "DataSource.h"

namespace fogstream {

DataSource::DataSource(int hostId, int operatorId, double arrivalRate, double arrivalMsgSize) {
    this->setHostId(hostId);
    this->setOperatorId(operatorId);
    this->setArrivalRate(arrivalRate);
    this->setArrivalMsgSize(arrivalMsgSize);

}

DataSource::~DataSource() {
    // TODO Auto-generated destructor stub
}

} /* namespace fogstream */

double fogstream::DataSource::getArrivalMsgSize() const {
    return mArrivalMsgSize;
}

void fogstream::DataSource::setArrivalMsgSize(double arrivalMsgSize) {
    mArrivalMsgSize = arrivalMsgSize;
}

double fogstream::DataSource::getArrivalRate() const {
    return mArrivalRate;
}

void fogstream::DataSource::setArrivalRate(double arrivalRate) {
    mArrivalRate = arrivalRate;
}

int fogstream::DataSource::getHostId() const {
    return mHostID;
}

void fogstream::DataSource::setHostId(int hostId) {
    mHostID = hostId;
}

int fogstream::DataSource::getOperatorId() const {
    return mOperatorID;
}

void fogstream::DataSource::setOperatorId(int operatorId) {
    mOperatorID = operatorId;
}

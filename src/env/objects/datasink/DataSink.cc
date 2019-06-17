#include "DataSink.h"

namespace fogstream {

DataSink::DataSink(int hostId, int operatorId) {
    this->setHostId(hostId);
    this->setOperatorId(operatorId);

}

DataSink::~DataSink() {
    // TODO Auto-generated destructor stub
}

int DataSink::getHostId() const {
    return mHostID;
}

void DataSink::setHostId(int hostId) {
    mHostID = hostId;
}

int DataSink::getOperatorId() const {
    return mOperatorID;
}

void DataSink::setOperatorId(int operatorId) {
    mOperatorID = operatorId;
}

} /* namespace fogstream */

#include "OperatorConnection.h"

namespace fogstream {

OperatorConnection::OperatorConnection(int fromOperatorId, int toOperatorId, double rho) {
    this->setFromOperatorId(fromOperatorId);
    this->setToOperatorId(toOperatorId);
    this->setRho(rho);

}

OperatorConnection::~OperatorConnection() {
    // TODO Auto-generated destructor stub
}

int OperatorConnection::getFromOperatorId() const {
    return mFromOperatorID;
}

void OperatorConnection::setFromOperatorId(int fromOperatorId) {
    mFromOperatorID = fromOperatorId;
}

double OperatorConnection::getRho() const {
    return mRHO;
}

void OperatorConnection::setRho(double rho) {
    mRHO = rho;
}

int OperatorConnection::getToOperatorId() const {
    return mToOperatorID;
}

void OperatorConnection::setToOperatorId(int toOperatorId) {
    mToOperatorID = toOperatorId;
}

} /* namespace fogstream */

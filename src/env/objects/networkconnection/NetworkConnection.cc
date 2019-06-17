#include "NetworkConnection.h"

namespace fogstream {

NetworkConnection::NetworkConnection(int sourceId, int destinationId, int linkId) {
    this->setSourceId(sourceId);
    this->setDestinationId(destinationId);
    this->setLinkId(linkId);
}

NetworkConnection::~NetworkConnection() {
    // TODO Auto-generated destructor stub
}

int NetworkConnection::getDestinationId() const {
    return mDestinationID;
}

void NetworkConnection::setDestinationId(int destinationId) {
    mDestinationID = destinationId;
}

int NetworkConnection::getLinkId() const {
    return mLinkID;
}

void NetworkConnection::setLinkId(int linkId) {
    mLinkID = linkId;
}

int NetworkConnection::getSourceId() const {
    return mSourceID;
}

void NetworkConnection::setSourceId(int sourceId) {
    mSourceID = sourceId;
}

} /* namespace fogstream */

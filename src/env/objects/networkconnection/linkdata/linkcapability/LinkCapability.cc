#include "LinkCapability.h"

namespace fogstream {

LinkCapability::LinkCapability(int linkId, double bandwidth) {
    this->setLinkId(linkId);
    this->setBandwidth(bandwidth);
}

LinkCapability::~LinkCapability() {
    // TODO Auto-generated destructor stub
}

double LinkCapability::getBandwidth() const {
    return mBandwidth;
}

void LinkCapability::setBandwidth(double bandwidth) {
    mBandwidth = bandwidth;
}

int LinkCapability::getLinkId() const {
    return mLinkID;
}

void LinkCapability::setLinkId(int linkId) {
    mLinkID = linkId;
}

} /* namespace fogstream */

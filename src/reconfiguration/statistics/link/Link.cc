//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Link.h"

namespace fogstream {

Link::Link() {
    // TODO Auto-generated constructor stub

}

Link::~Link() {
    // TODO Auto-generated destructor stub
}

double Link::getArrivalMsgSize() const {
    return mArrivalMsgSize;
}

void Link::setArrivalMsgSize(double arrivalMsgSize) {
    mArrivalMsgSize = arrivalMsgSize;
}

double Link::getArrivalRate() const {
    return mArrivalRate;
}

void Link::setArrivalRate(double arrivalRate) {
    mArrivalRate = arrivalRate;
}

double Link::getCommRequirement() const {
    return mCommRequirement;
}

void Link::setCommRequirement(double commRequirement) {
    mCommRequirement = commRequirement;
}

double Link::getCommTime() const {
    return mCommTime;
}

void Link::setCommTime(double commTime) {
    mCommTime = commTime;
}

double Link::getMemoryRequirement() const {
    return mMemoryRequirement;
}

void Link::setMemoryRequirement(double memoryRequirement) {
    mMemoryRequirement = memoryRequirement;
}

double Link::getQueueSize() const {
    return mQueueSize;
}

void Link::setQueueSize(double queueSize) {
    mQueueSize = queueSize;
}

LinkAvailability::LinkAvailability() {
    // TODO Auto-generated constructor stub

}

LinkAvailability::LinkAvailability(int linkId, double availableBandwidth) {
    this->setLinkId(linkId);
    this->setAvailableBandwidth(availableBandwidth);
}

LinkAvailability::~LinkAvailability() {
    // TODO Auto-generated destructor stub
}

void LinkAvailability::setAvailableBandwidth(double availableBandwidth) {
    mAvailableBandwidth = availableBandwidth;
}

void LinkAvailability::setLinkId(int linkId) {
    mLinkID = linkId;
}

double LinkAvailability::getAvailableBandwidth() const {
    return mAvailableBandwidth;
}

int LinkAvailability::getLinkId() const {
    return mLinkID;
}

string LinkAvailability::toString() {
    return "linkavailability;linkid=" + to_string(this->getLinkId()) + ";bdwt="
            + to_string(this->getAvailableBandwidth());
}
} /* namespace fogstream */


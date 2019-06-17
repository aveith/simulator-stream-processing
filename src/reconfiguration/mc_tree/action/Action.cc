#include "Action.h"

namespace fogstream {

Action::Action() {
    // TODO Auto-generated constructor stub

}

Action::Action(int hostId, int originHostId) {
    this->setHostId(hostId);
    this->setOriginHostId(originHostId);
}

Action::~Action() {
    // TODO Auto-generated destructor stub
}

int Action::getHostId() const {
    return mHostID;
}

void Action::setHostId(int hostId) {
    mHostID = hostId;
}

int Action::getOriginHostId() const {
    return mOriginHostID;
}

void Action::setOriginHostId(int originHostId) {
    mOriginHostID = originHostId;
}

bool Action::isTaken() const {
    return mTaken;
}

void Action::setTaken(bool taken) {
    mTaken = taken;
}

string Action::toString() {
    return "Action;" + to_string(this->getHostId()) + ";"
            + to_string(this->getOriginHostId()) + ";"
            + to_string(this->isTaken());
}

} /* namespace fogstream */


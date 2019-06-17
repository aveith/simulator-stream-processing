
#include "DestinationBranches.h"

namespace fogstream {

DestinationBranches::DestinationBranches(int type, vector<int>& applicationBranch) {
    this->setType(type);
    this->setApplicationBranch(applicationBranch);

}

DestinationBranches::~DestinationBranches() {
    // TODO Auto-generated destructor stub
}

const vector<int>& DestinationBranches::getApplicationBranch() const {
    return mApplicationBranch;
}

void DestinationBranches::setApplicationBranch(
        const vector<int>& applicationBranch) {
    mApplicationBranch = applicationBranch;
}

int DestinationBranches::getType() const {
    return mType;
}

void DestinationBranches::setType(int type) {
    mType = type;
}

} /* namespace fogstream */

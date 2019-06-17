#include "LinkData.h"

namespace fogstream {

LinkData::LinkData(int id, double delay, double cost) {
    this->setId(id);
    this->setDelay(delay);
    this->setCost(cost);
}

LinkData::~LinkData() {
    // TODO Auto-generated destructor stub
}

double LinkData::getCost() const {
    return mCost;
}

void LinkData::setCost(double cost) {
    mCost = cost;
}

double LinkData::getDelay() const {
    return mDelay;
}

void LinkData::setDelay(double delay) {
    mDelay = delay;
}

int LinkData::getId() const {
    return mID;
}

void LinkData::setId(int id) {
    mID = id;
}
} /* namespace fogstream */

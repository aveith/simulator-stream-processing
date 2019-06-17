#include "ResourceData.h"

namespace fogstream {

ResourceData::ResourceData(int id, int type, double cost) {
    this->setId(id);
    this->setType(type);
    this->setCost(cost);
}

ResourceData::~ResourceData() {
    // TODO Auto-generated destructor stub
}

double ResourceData::getCost() const {
    return mCost;
}

void ResourceData::setCost(double cost) {
    mCost = cost;
}

int ResourceData::getId() const {
    return mID;
}

void ResourceData::setId(int id) {
    mID = id;
}

int ResourceData::getType() const {
    return mType;
}

void ResourceData::setType(int type) {
    mType = type;
}

int ResourceData::getGatewayId() const {
    return mGatewayId;
}

void ResourceData::setGatewayId(int gatewayId) {
    mGatewayId = gatewayId;
}

bool ResourceData::isMapped() const {
    return mMapped;
}

void ResourceData::setMapped(bool mapped) {
    mMapped = mapped;
}

} /* namespace fogstream */


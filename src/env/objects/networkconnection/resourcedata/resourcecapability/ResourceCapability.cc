#include "ResourceCapability.h"

namespace fogstream {

ResourceCapability::ResourceCapability(int hostId, double memory, double cpu,
        int slotNumber) {
    this->setHostId(hostId);
    this->setMemory(memory);
    this->setCpu(cpu);
    this->setSlotNumber(slotNumber);
}

ResourceCapability::~ResourceCapability() {
    // TODO Auto-generated destructor stub
}

double ResourceCapability::getCpu() const {
    return mCPU;
}

void ResourceCapability::setCpu(double cpu) {
    mCPU = cpu;
}

int ResourceCapability::getHostId() const {
    return mHostID;
}

void ResourceCapability::setHostId(int hostId) {
    mHostID = hostId;
}

double ResourceCapability::getMemory() const {
    return mMemory;
}

void ResourceCapability::setMemory(double memory) {
    mMemory = memory;
}

} /* namespace fogstream */

int fogstream::ResourceCapability::getSlotNumber() const {
    return mSlotNumber;
}

void fogstream::ResourceCapability::setSlotNumber(int slotNumber) {
    mSlotNumber = slotNumber;
}

#include "ResourceCapability.h"

namespace fogstream {

ResourceCapability::ResourceCapability(int hostId, double memory, double cpu,
        int slotNumber, int servers) {
    this->setHostId(hostId);
    this->setMemory(memory);
    this->setCpu(cpu);
    this->setSlotNumber(slotNumber);
    this->setServers(servers);
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

int ResourceCapability::getSlotNumber() const {
    return mSlotNumber;
}

void ResourceCapability::setSlotNumber(int slotNumber) {
    mSlotNumber = slotNumber;
}

int ResourceCapability::getServers() const {
    return mServers;
}

void ResourceCapability::setServers(int servers) {
    mServers = servers;
}


} /* namespace fogstream */


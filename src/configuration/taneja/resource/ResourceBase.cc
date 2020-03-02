#include "ResourceBase.h"

namespace fogstream {

ResourceBase::ResourceBase() {

}

ResourceBase::ResourceBase(int hostId, double cpu, double memory, double bandwidth){
    this->setHostId(hostId);
    this->setCpu(cpu);
    this->setMemory(memory);
    this->setBandwidth(bandwidth);
}

ResourceBase::~ResourceBase() {
    // TODO Auto-generated destructor stub
}

double ResourceBase::getBandwidth() const {
    return mBandwidth;
}

void ResourceBase::setBandwidth(double bandwidth) {
    mBandwidth = bandwidth;
}

double ResourceBase::getCpu() const {
    return mCPU;
}

void ResourceBase::setCpu(double cpu) {
    mCPU = cpu;
}

double ResourceBase::getMemory() const {
    return mMemory;
}

void ResourceBase::setMemory(double memory) {
    mMemory = memory;
}

} /* namespace fogstream */

int fogstream::ResourceBase::getHostId() const {
    return mHostId;
}

void fogstream::ResourceBase::setHostId(int hostId) {
    mHostId = hostId;
}

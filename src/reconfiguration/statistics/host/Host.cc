#include "Host.h"

namespace fogstream {

Host::Host() {
    // TODO Auto-generated ructor stub

}

Host::~Host() {
    // TODO Auto-generated destructor stub
}

double Host::getArrivalMsgSize() {
    return mArrivalMsgSize;
}

void Host::setArrivalMsgSize(double arrivalMsgSize) {
    mArrivalMsgSize = arrivalMsgSize;
}

double Host::getArrivalRate() {
    return mArrivalRate;
}

void Host::setArrivalRate(double arrivalRate) {
    mArrivalRate = arrivalRate;
}

double Host::getCommRequirement() {
    return mCommRequirement;
}

void Host::setCommRequirement(double commRequirement) {
    mCommRequirement = commRequirement;
}

simtime_t& Host::getCommTime() {
    return mCommTime;
}

void Host::setCommTime(simtime_t& commTime) {
    mCommTime = commTime;
}

double Host::getMemoryRequirement() {
    return mMemoryRequirement;
}

void Host::setMemoryRequirement(double memoryRequirement) {
    mMemoryRequirement = memoryRequirement;
}

double Host::getQueueSize() {
    return mQueueSize;
}

void Host::setQueueSize(double queueSize) {
    mQueueSize = queueSize;
}

HostAvailability::HostAvailability() {
    // TODO Auto-generated ructor stub

}

HostAvailability::HostAvailability(int hostId, double availableMemory,
        double availableCpu, int availableSlots) {
    this->setHostId(hostId);
    this->setAvailableMemory(availableMemory);
    this->setAvailableCpu(availableCpu);
    this->setAvailableSlots(availableSlots);
}

HostAvailability::~HostAvailability() {
    // TODO Auto-generated destructor stub
}

double HostAvailability::getAvailableCpu() {
    return mAvailableCPU;
}

void HostAvailability::setAvailableCpu(double availableCpu) {
    mAvailableCPU = availableCpu;
}

double HostAvailability::getAvailableMemory() {
    return mAvailableMemory;
}

void HostAvailability::setAvailableMemory(double availableMemory) {
    mAvailableMemory = availableMemory;
}

int HostAvailability::getHostId() {
    return mHostID;
}

void HostAvailability::setHostId(int hostId) {
    mHostID = hostId;
}

string HostAvailability::toString() {
    return "hostavailability;host_id=" + to_string(this->getHostId()) + ";cpu="
            + to_string(this->getAvailableCpu()) + ";mem="
            + to_string(this->getAvailableMemory());
}

int HostAvailability::getAvailableSlots() const {
    return mAvailableSlots;
}

void HostAvailability::setAvailableSlots(int availableSlots) {
    mAvailableSlots = availableSlots;
}
} /* namespace fogstream */



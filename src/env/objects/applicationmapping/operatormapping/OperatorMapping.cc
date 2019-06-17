#include "OperatorMapping.h"

namespace fogstream {

OperatorMapping::OperatorMapping(OperatorMapping* map){
    this->setCompTime(map->getCompTime());
    this->setHostId(map->getHostId());
    this->setInputRate(map->getInputRate());
    this->setOperatorId(map->getOperatorId());
    this->setOutputRate(map->getOutputRate());
    this->setQueueSize(map->getQueueSize());
    this->setRequiredCpu(map->getRequiredCpu());
    this->setRequiredMemory(map->getRequiredMemory());
    this->setInputMsgSize(map->getInputMsgSize());
    this->setOutputMsgSize(map->getOutputMsgSize());
}

OperatorMapping::OperatorMapping() {
    // TODO Auto-generated constructor stub

}

OperatorMapping::~OperatorMapping() {
    // TODO Auto-generated destructor stub
}

double OperatorMapping::getCompTime() const {
    return mCompTime;
}

void OperatorMapping::setCompTime(double compTime) {
    mCompTime = compTime;
}

int OperatorMapping::getHostId() const {
    return mHostID;
}

void OperatorMapping::setHostId(int hostId) {
    mHostID = hostId;
}

double OperatorMapping::getInputRate() const {
    return mInputRate;
}

void OperatorMapping::setInputRate(double inputRate) {
    mInputRate = inputRate;
}

int OperatorMapping::getOperatorId() const {
    return mOperatorID;
}

void OperatorMapping::setOperatorId(int operatorId) {
    mOperatorID = operatorId;
}

double OperatorMapping::getOutputRate() const {
    return mOutputRate;
}

void OperatorMapping::setOutputRate(double outputRate) {
    mOutputRate = outputRate;
}

double OperatorMapping::getQueueSize() const {
    return mQueueSize;
}

void OperatorMapping::setQueueSize(double queueSize) {
    mQueueSize = queueSize;
}

double OperatorMapping::getRequiredCpu() const {
    return mRequiredCPU;
}

void OperatorMapping::setRequiredCpu(double requiredCpu) {
    mRequiredCPU = requiredCpu;
}

double OperatorMapping::getRequiredMemory() const {
    return mRequiredMemory;
}

void OperatorMapping::setRequiredMemory(double requiredMemory) {
    mRequiredMemory = requiredMemory;
}

} /* namespace fogstream */

double fogstream::OperatorMapping::getInputMsgSize() const {
    return mInputMsgSize;
}

void fogstream::OperatorMapping::setInputMsgSize(double inputMsgSize) {
    mInputMsgSize = inputMsgSize;
}

double fogstream::OperatorMapping::getOutputMsgSize() const {
    return mOutputMsgSize;
}

void fogstream::OperatorMapping::setOutputMsgSize(double outputMsgSize) {
    mOutputMsgSize = outputMsgSize;
}

#include "OperatorData.h"

namespace fogstream {

OperatorData::OperatorData(){

}

OperatorData::OperatorData(int id, int fissionId, double selectivity,
        double dataTransferRate, double stateRate, int type,
        double cpuRequirement, double memoryRequirement, double timeWindow,
        bool nextSplitted, int nextSplittedLength, int characteristicId,
        double stateSize) {
    this->setId(id);
    this->setFissionId(fissionId);
    this->setSelectivity(selectivity);
    this->setDataTransferRate(dataTransferRate);
    this->setStateRate(stateRate);
    this->setType(type);
    this->setCPURequirement(cpuRequirement);
    this->setMemoryRequirement(memoryRequirement);
    this->setTimeWindow(timeWindow);
    this->setNextSplitted(nextSplitted);
    this->setNextSplittedLength(nextSplittedLength);
    this->setCharacteristicId(characteristicId);
    this->setStateSize(stateSize);

}

OperatorData::~OperatorData() {
    // TODO Auto-generated destructor stub
}


int OperatorData::getCharacteristicId() const {
    return mCharacteristicId;
}

void OperatorData::setCharacteristicId(int characteristicId) {
    mCharacteristicId = characteristicId;
}

double OperatorData::getCPURequirement() const {
    return mCPURequirement;
}

void OperatorData::setCPURequirement(double cpuRequirement) {
    mCPURequirement = cpuRequirement;
}

double OperatorData::getDataTransferRate() const {
    return mDataTransferRate;
}

void OperatorData::setDataTransferRate(double dataTransferRate) {
    mDataTransferRate = dataTransferRate;
}

int OperatorData::getFissionId() const {
    return mFissionID;
}

void OperatorData::setFissionId(int fissionId) {
    mFissionID = fissionId;
}

int OperatorData::getId() const {
    return mID;
}

void OperatorData::setId(int id) {
    mID = id;
}

double OperatorData::getMemoryRequirement() const {
    return mMemoryRequirement;
}

void OperatorData::setMemoryRequirement(double memoryRequirement) {
    mMemoryRequirement = memoryRequirement;
}

bool OperatorData::isNextSplitted() const {
    return mNextSplitted;
}

void OperatorData::setNextSplitted(bool nextSplitted) {
    mNextSplitted = nextSplitted;
}

double OperatorData::getSelectivity() const {
    return mSelectivity;
}

void OperatorData::setSelectivity(double selectivity) {
    mSelectivity = selectivity;
}

double OperatorData::getStateRate() const {
    return mStateRate;
}

void OperatorData::setStateRate(double stateRate) {
    mStateRate = stateRate;
}

double OperatorData::getStateSize() const {
    return mStateSize;
}

void OperatorData::setStateSize(double stateSize) {
    mStateSize = stateSize;
}

double OperatorData::getTimeWindow() const {
    return mTimeWindow;
}

void OperatorData::setTimeWindow(double timeWindow) {
    mTimeWindow = timeWindow;
}

int OperatorData::getType() const {
    return mType;
}

void OperatorData::setType(int type) {
    mType = type;
}

int OperatorData::getNextSplittedLength() const {
    return nNextSplittedLength;
}

void OperatorData::setNextSplittedLength(int nextSplittedLength) {
    nNextSplittedLength = nextSplittedLength;
}

} /* namespace fogstream */

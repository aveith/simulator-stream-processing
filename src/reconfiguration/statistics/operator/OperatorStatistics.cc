#include "OperatorStatistics.h"

namespace fogstream {

OperatorStatistics::OperatorStatistics() {
    // TODO Auto-generated constructor stub

}

OperatorStatistics::OperatorStatistics(int hostId, int operatorId,
        double arrivalMsgSize, double arrivalRate, double compTime,
        double compRequirement, double memoryRequirement, double queueSize,
        double outputMsgSize, double outputRate, double selectivity,
        double dataCompressionExpasionFactor, double stateSize,
        double requiredTimeBuildWindow) {
    this->setHostId(hostId);
    this->setOperatorId(operatorId);
    this->setArrivalRate(arrivalRate);
    this->setArrivalMsgSize(arrivalMsgSize);
    this->setCompTime(compTime);
    this->setCompRequirement(compRequirement);
    this->setMemoryRequirement(memoryRequirement);
    this->setQueueSize(queueSize);
    this->setOutputMsgSize(outputMsgSize);
    this->setOutputRate(outputRate);
    this->setSelectivity(selectivity);
    this->setDataCompressionExpasionFactor(dataCompressionExpasionFactor);
    this->setStateSize(stateSize);
    this->setRequiredTimeBuildWindow(requiredTimeBuildWindow);
}

OperatorStatistics::~OperatorStatistics() {
    // TODO Auto-generated destructor stub
}

double OperatorStatistics::getArrivalMsgSize() const {
    return mArrivalMsgSize;
}

void OperatorStatistics::setArrivalMsgSize(double arrivalMsgSize) {
    mArrivalMsgSize = arrivalMsgSize;
}

double OperatorStatistics::getArrivalRate() const {
    return mArrivalRate;
}

void OperatorStatistics::setArrivalRate(double arrivalRate) {
    mArrivalRate = arrivalRate;
}

double OperatorStatistics::getCompRequirement() const {
    return mCompRequirement;
}

void OperatorStatistics::setCompRequirement(double compRequirement) {
    mCompRequirement = compRequirement;
}

double OperatorStatistics::getCompTime() const {
    return mCompTime;
}

void OperatorStatistics::setCompTime(double compTime) {
    mCompTime = compTime;
}

double OperatorStatistics::getMemoryRequirement() const {
    return mMemoryRequirement;
}

void OperatorStatistics::setMemoryRequirement(double memoryRequirement) {
    mMemoryRequirement = memoryRequirement;
}

double OperatorStatistics::getOutputMsgSize() const {
    return mOutputMsgSize;
}

void OperatorStatistics::setOutputMsgSize(double outputMsgSize) {
    mOutputMsgSize = outputMsgSize;
}

double OperatorStatistics::getOutputRate() const {
    return mOutputRate;
}

void OperatorStatistics::setOutputRate(double outputRate) {
    mOutputRate = outputRate;
}

double OperatorStatistics::getQueueSize() const {
    return mQueueSize;
}

void OperatorStatistics::setQueueSize(double queueSize) {
    mQueueSize = queueSize;
}

double OperatorStatistics::getDataCompressionExpasionFactor() const {
    return mDataCompressionExpasionFactor;
}

void OperatorStatistics::setDataCompressionExpasionFactor(
        double dataCompressionExpasionFactor) {
    mDataCompressionExpasionFactor = dataCompressionExpasionFactor;
}

double OperatorStatistics::getSelectivity() const {
    return mSelectivity;
}

void OperatorStatistics::setSelectivity(double selectivity) {
    mSelectivity = selectivity;
}

int OperatorStatistics::getHostId() const {
    return mHostId;
}

void OperatorStatistics::setHostId(int hostId) {
    mHostId = hostId;
}

int OperatorStatistics::getOperatorId() const {
    return mOperatorId;
}

void OperatorStatistics::setOperatorId(int operatorId) {
    mOperatorId = operatorId;
}

string OperatorStatistics::toString() {
    return to_string(this->getHostId()) + ";" + to_string(this->getOperatorId())
            + ";" + to_string(this->getArrivalMsgSize()) + ";"
            + to_string(this->getArrivalRate()) + ";"
            + to_string(this->getCompTime()) + ";"
            + to_string(this->getCompRequirement()) + ";"
            + to_string(this->getMemoryRequirement()) + ";"
            + to_string(this->getQueueSize()) + ";"
            + to_string(this->getOutputMsgSize()) + ";"
            + to_string(this->getOutputRate()) + ";"
            + to_string(this->getSelectivity()) + ";"
            + to_string(this->getDataCompressionExpasionFactor()) + ";"
            + to_string(this->getStateSize()) + ";"
            + to_string(this->getRequiredTimeBuildWindow());
}

string OperatorStatistics::toStringDetail() {
    return "opestat;host=" + to_string(this->getHostId()) + ";ope=" + to_string(this->getOperatorId())
            + ";arrivalmsgsize=" + to_string(this->getArrivalMsgSize()) + ";arrivalrate="
            + to_string(this->getArrivalRate()) + ";comptime="
            + to_string(this->getCompTime()) + ";compreq="
            + to_string(this->getCompRequirement()) + ";memoryreq="
            + to_string(this->getMemoryRequirement()) + ";queue="
            + to_string(this->getQueueSize()) + ";outputmsgsize="
            + to_string(this->getOutputMsgSize()) + ";outputrate="
            + to_string(this->getOutputRate()) + ";selectivity="
            + to_string(this->getSelectivity()) + ";datarate="
            + to_string(this->getDataCompressionExpasionFactor()) + ";statesize="
            + to_string(this->getStateSize()) + ";timewindow="
            + to_string(this->getRequiredTimeBuildWindow());
}
double OperatorStatistics::getStateSize() const {
    return mStateSize;
}

void OperatorStatistics::setStateSize(double stateSize) {
    mStateSize = stateSize;
}

double OperatorStatistics::getRequiredTimeBuildWindow() const {
    return mRequiredTimeBuildWindow;
}

void OperatorStatistics::setRequiredTimeBuildWindow(
        double requiredTimeBuildWindow) {
    mRequiredTimeBuildWindow = requiredTimeBuildWindow;
}

} /* namespace fogstream */


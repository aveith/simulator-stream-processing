#include "OperatorConnectionMapping.h"

namespace fogstream {
OperatorConnectionMapping::OperatorConnectionMapping(
        OperatorConnectionMapping* mapping) {
    this->setLinkId(mapping->getLinkId());
    this->setFromOperatorId(mapping->getFromOperatorId());
    this->setToOperatorId(mapping->getToOperatorId());
    this->setInputRate(mapping->getInputRate());
    this->setOutputRate(mapping->getOutputRate());
    this->setQueueSize(mapping->getQueueSize());
    this->setCommTime(mapping->getCommTime());
    this->setRequiredBandwidth(mapping->getRequiredBandwidth());
}

OperatorConnectionMapping::OperatorConnectionMapping(int fromOperatorId,
        int toOperatorId, int linkId, double inputRate, double outputRate,
        double queueSize, double commTime, double requiredBandwidth) {
    this->setFromOperatorId(fromOperatorId);
    this->setToOperatorId(toOperatorId);
    this->setLinkId(linkId);
    this->setInputRate(inputRate);
    this->setOutputRate(outputRate);
    this->setQueueSize(queueSize);
    this->setCommTime(commTime);
    this->setRequiredBandwidth(requiredBandwidth);
}

OperatorConnectionMapping::~OperatorConnectionMapping() {
    // TODO Auto-generated destructor stub
}

int OperatorConnectionMapping::getLinkId() const {
    return mLinkID;
}

void OperatorConnectionMapping::setLinkId(int linkId) {
    mLinkID = linkId;
}

int fogstream::OperatorConnectionMapping::getFromOperatorId() const {
    return mFromOperatorID;
}

void fogstream::OperatorConnectionMapping::setFromOperatorId(
        int fromOperatorId) {
    mFromOperatorID = fromOperatorId;
}

int fogstream::OperatorConnectionMapping::getToOperatorId() const {
    return mToOperatorID;
}

void fogstream::OperatorConnectionMapping::setToOperatorId(int toOperatorId) {
    mToOperatorID = toOperatorId;
}

double OperatorConnectionMapping::getCommTime() const {
    return mCommTime;
}

void OperatorConnectionMapping::setCommTime(double commTime) {
    mCommTime = commTime;
}

double OperatorConnectionMapping::getRequiredBandwidth() const {
    return mRequiredBandwidth;
}

void OperatorConnectionMapping::setRequiredBandwidth(double requiredBandwidth) {
    mRequiredBandwidth = requiredBandwidth;
}

} /* namespace fogstream */

double fogstream::OperatorConnectionMapping::getInputRate() const {
    return mInputRate;
}

void fogstream::OperatorConnectionMapping::setInputRate(double inputRate) {
    mInputRate = inputRate;
}

double fogstream::OperatorConnectionMapping::getOutputRate() const {
    return mOutputRate;
}

void fogstream::OperatorConnectionMapping::setOutputRate(double outputRate) {
    mOutputRate = outputRate;
}

double fogstream::OperatorConnectionMapping::getQueueSize() const {
    return mQueueSize;
}

void fogstream::OperatorConnectionMapping::setQueueSize(double queueSize) {
    mQueueSize = queueSize;
}

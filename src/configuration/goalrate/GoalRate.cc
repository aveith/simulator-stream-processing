#include "GoalRate.h"

namespace fogstream {
GoalRate::GoalRate() {

}

GoalRate::GoalRate(OperatorMapping*& currentOperatorMapping,
        vector<OperatorMapping*>& previousOperatorMapping,
        vector<OperatorConnectionMapping*>& linkMapping, bool constrained,
        double totalRate) {
    this->setCurrentOperatorMapping(currentOperatorMapping);
    this->setPreviousOperatorMapping(previousOperatorMapping);
    this->setLinkMapping(linkMapping);
    this->setConstrained(constrained);
    this->setTotalRate(totalRate);
}

GoalRate::~GoalRate() {
    this->clear();
}

bool GoalRate::isConstrained() const {
    return mConstrained;
}

void GoalRate::setConstrained(bool constrained) {
    mConstrained = constrained;
}

OperatorMapping*& GoalRate::getCurrentOperatorMapping() {
    return mCurrentOperatorMapping;
}

void GoalRate::setCurrentOperatorMapping(
         OperatorMapping* currentOperatorMapping) {
    mCurrentOperatorMapping = currentOperatorMapping;
}

vector<OperatorMapping*>& GoalRate::getPreviousOperatorMapping() {
    return mPreviousOperatorMapping;
}

void GoalRate::setPreviousOperatorMapping(
        const vector<OperatorMapping*>& previousOperatorMapping) {
    mPreviousOperatorMapping = previousOperatorMapping;
}

double GoalRate::getTotalRate() const {
    return mTotalRate;
}

void GoalRate::setTotalRate(double totalRate) {
    mTotalRate = totalRate;
}

vector<OperatorConnectionMapping*>& GoalRate::getLinkMapping() {
    return mLinkMapping;
}

void GoalRate::setLinkMapping(
        const vector<OperatorConnectionMapping*>& linkMapping) {
    mLinkMapping = linkMapping;
}

void GoalRate::clear() {

    for (unsigned int i = 0; i < this->getLinkMapping().size(); i++) {
        delete this->getLinkMapping().at(i);
    }
    this->getLinkMapping().clear();

    for (unsigned int i = 0; i < this->getPreviousOperatorMapping().size();
            i++) {
        delete this->getPreviousOperatorMapping().at(i);
    }
    this->getPreviousOperatorMapping().clear();

    if (this->getCurrentOperatorMapping() != nullptr) {
        delete this->getCurrentOperatorMapping();
        this->getCurrentOperatorMapping() = nullptr;
    }
}

void GoalRate::cloneObject(GoalRate* rate) {
    this->clear();
    this->setCurrentOperatorMapping(new OperatorMapping(rate->getCurrentOperatorMapping()));
    for (unsigned int i = 0; i < rate->getPreviousOperatorMapping().size(); i++){
        this->getPreviousOperatorMapping().push_back(new OperatorMapping(rate->getPreviousOperatorMapping().at(i)));
    }

    for (unsigned int i = 0; i < rate->getLinkMapping().size();i++){
        this->getLinkMapping().push_back(new OperatorConnectionMapping(rate->getLinkMapping().at(i)));
    }
    this->setConstrained(rate->isConstrained());
    this->setTotalRate(rate->getTotalRate());
}

} /* namespace fogstream */


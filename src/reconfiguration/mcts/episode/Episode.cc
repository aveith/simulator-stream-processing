#include "Episode.h"

namespace fogstream {

Episode::Episode() {
    // TODO Auto-generated constructor stub

}

Episode::~Episode() {
    delete this->getParentState();
    delete this->getState();
}

Episode::Episode(State*& parentState, Action action, State*& state,
        double cumulativeReward, vector<int> constraints, int parentId) {
    this->setParentState(parentState);
    this->setState(state);
    this->setCumulativeReward(cumulativeReward);
    this->setConstraints(constraints);
    this->setAction(action);
    this->setParentId(parentId);
}

double& Episode::getCumulativeReward() {
    return mCumulativeReward;
}

void Episode::setCumulativeReward(double cumulativeReward) {
    mCumulativeReward = cumulativeReward;
}

State*& Episode::getParentState() {
    return mParentState;
}

void Episode::setParentState(State* parentState) {
    mParentState = parentState;
}

State*& Episode::getState() {
    return mState;
}

void Episode::setState(State* state) {
    mState = state;
}

vector<int>& Episode::getConstraints() {
    return mConstraints;
}

void Episode::setConstraints(const vector<int>& constraints) {
    mConstraints = constraints;
}

Action& Episode::getAction() {
    return mAction;
}

void Episode::setAction(const Action& action) {
    mAction = action;
}

int Episode::getId() const {
    return mId;
}

void Episode::setId(int id) {
    mId = id;
}

int Episode::getParentId() const {
    return mParentID;
}

void Episode::setParentId(int parentId) {
    mParentID = parentId;
}

double Episode::getGainRate() const {
    return mGainRate;
}

void Episode::setGainRate(double gainRate) {
    mGainRate = gainRate;
}

int Episode::getNumberMigrations() const {
    return mNumberMigrations;
}

void Episode::setNumberMigrations(int numberMigrations) {
    mNumberMigrations = numberMigrations;
}

double Episode::getReward() const {
    return mReward;
}

void Episode::setReward(double reward) {
    mReward = reward;
}

int Episode::getWin() const {
    return mWin;
}

void Episode::setWin(int win) {
    mWin = win;
}

double Episode::getAggregateCost() const {
    return mAggregateCost;
}

void Episode::setAggregateCost(double aggregateCost) {
    mAggregateCost = aggregateCost;
}

string fogstream::Episode::toString() {
    return "episode;id=" + to_string(this->getId()) + ";constraints="
            + to_string(this->getConstraints().size()) + ";parentid="
            + to_string(this->getParentId()) + ";reward="
            + to_string(this->getReward()) + ";cumulativereward="
            + to_string(this->getCumulativeReward()) + ";migrations="
            + to_string(this->getNumberMigrations()) + ";gainrate="
            + to_string(this->getGainRate()) + ";win=" + to_string(getWin())
            + ";aggregatecost=" + to_string(this->getAggregateCost());
}
} /* namespace fogstream */


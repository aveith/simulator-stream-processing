#include "Node.h"

namespace fogstream {
Node::Node() {

}

Node::Node(int id, int parentId, bool root, bool terminal, bool constrained,
        bool sourceSink, Action actionTaken, State* state,
        double cumulativeReward, double reward, int numberMigrations,
        double gainRate, int win, double aggregateCost) {
    this->setId(id);
    this->setRoot(root);
    this->setParentId(parentId);
    this->setTerminal(terminal);
    this->setConstrained(constrained);
    this->setSourceSink(sourceSink);
    this->setActionTaken(Action(actionTaken));

    this->setState(new State(state));

    this->setCumulativeReward(cumulativeReward);
    this->setReward(reward);
    this->setNumberMigrations(numberMigrations);
    this->setGainRate(gainRate);
    this->setWin(win);
    this->setAggregateCost(aggregateCost);
}

Node::Node(int id, int parentID) {
    this->setId(id);
    this->setParentId(parentID);

    //Initial values for using into backpropagation phase
    this->setCumulativeReward(0);
    //    this->setVisits(1);
}

Node::~Node() {
    delete this->mState;
}

Action& Node::getActionTaken() {
    return mActionTaken;
}

void Node::setActionTaken(const Action& actionTaken) {
    mActionTaken = actionTaken;
}

bool Node::isConstrained() {
    return mConstrained;
}

void Node::setConstrained(bool constrained) {
    mConstrained = constrained;
}

vector<int>& Node::getConstraints() {
    return mConstraints;
}

void Node::setConstraints(vector<int>& constraints) {
    mConstraints = constraints;
}

int Node::getId() {
    return mID;
}

void Node::setId(int id) {
    mID = id;
}

int Node::getParentId() {
    return mParentID;
}

void Node::setParentId(int parentId) {
    mParentID = parentId;
}

double& Node::getCumulativeReward() {
    return mCumulativeReward;
}

void Node::setCumulativeReward(double cumulativeReward) {
    mCumulativeReward = cumulativeReward;
}

bool Node::isRoot() {
    return mRoot;
}

void Node::setRoot(bool root) {
    mRoot = root;
}

bool Node::isSourceSink() {
    return mSourceSink;
}

void Node::setSourceSink(bool sourceSink) {
    mSourceSink = sourceSink;
}

bool Node::isTerminal() {
    return mTerminal;
}

void Node::setTerminal(bool terminal) {
    mTerminal = terminal;
}

int Node::getVisits() {
    return mVisits;
}

void Node::setVisits(int visits) {
    mVisits = visits;
}

double& Node::getQValue() {
    return mQValue;
}

void Node::setQValue(double qValue) {
    mQValue = qValue;
}

const double& Node::getUctValue() const {
    return mUCTValue;
}

void Node::setUctValue(double uctValue) {
    mUCTValue = uctValue;
}

int Node::getWin() const {
    return mWin;
}

void Node::setWin(int win) {
    mWin = win;
}

double Node::getReward() const {
    return mReward;
}

void Node::setReward(double reward) {
    mReward = reward;
}

double Node::getGainRate() const {
    return mGainRate;
}

void Node::setGainRate(double gainRate) {
    mGainRate = gainRate;
}

int Node::getNumberMigrations() const {
    return mNumberMigrations;
}

void Node::setNumberMigrations(int numberMigrations) {
    mNumberMigrations = numberMigrations;
}

double Node::getAggregateCost() const {
    return mAggregateCost;
}

void Node::setAggregateCost(double aggregateCost) {
    mAggregateCost = aggregateCost;
}

State*& fogstream::Node::getState() {
    return mState;
}

void fogstream::Node::setState(State* state) {
    mState = state;
}

string Node::toString() {
    return "Node;id=" + to_string(this->getId()) + ";root="
            + to_string(this->isRoot()) + ";terminal="
            + to_string(this->isTerminal()) + ";constrained="
            + to_string(this->isConstrained()) + ";sourcesink="
            + to_string(this->isSourceSink()) + ";parentid="
            + to_string(this->getParentId()) + ";cumulativereward="
            + to_string(this->getCumulativeReward()) + ";win="
            + to_string(this->getWin()) + ";visits="
            + to_string(this->getVisits()) + ";uct="
            + to_string(this->getUctValue()) + ";qvalue="
            + to_string(this->getQValue()) + ";reward="
            + to_string(this->getReward()) + ";migrations="
            + to_string(this->getNumberMigrations()) + ";gainrate="
            + to_string(this->getGainRate()) + ";aggregatecost="
            + to_string(this->getAggregateCost());
}

} /* namespace fogstream */


#include "State.h"

namespace fogstream {

State::State() {

}

State::State(int id, Mapping map, vector<Action> availableActions,
        double totalTimePaths, int levelMcts) {
    this->setId(id);
    this->setMap(map);
    this->setAvailableActions(availableActions);
    //    this->setPosition(position);
    this->setTotalTimePaths(totalTimePaths);
    this->setLevelMcts(levelMcts);

}

State::~State() {

}

State::State(State* state) {
    this->setId(state->getId());
    this->setMap(state->getMap());

    this->setAvailableActions(state->getAvailableActions());
    //    this->setPosition(state->getPosition());
    this->setTotalTimePaths(state->getTotalTimePaths());
    this->setNumberResources(state->getNumberResources());
    this->setNumberOperatorsSite(state->getNumberOperatorsSite());
    this->setLevelMcts(state->getLevelMcts());
    this->setWanTraffic(state->getWanTraffic());
    this->setNumberMessagesCloud(state->getNumberMessagesCloud());
    this->setMigration(state->getMigration());
    this->setMicrosoftPricing(state->getMicrosoftPricing());
    this->setAwsPricing(state->getAwsPricing());
    this->setNumberCloudConnections(state->getNumberCloudConnections());
    this->setAverageSizeCloudMsgs(state->getAverageSizeCloudMsgs());

}

vector<Action>& State::getAvailableActions() {
    return mAvailableActions;
}

void State::setAvailableActions(vector<Action> availableActions) {
    mAvailableActions = availableActions;
}

int State::getId() {
    return mID;
}

void State::setId(int id) {
    mID = id;
}

Mapping& State::getMap() {
    return mMap;
}

void State::setMap(const Mapping& map) {
    mMap = map;
}

//int State::getPosition() {
//    return mPosition;
//}
//
//void State::setPosition(int position) {
//    mPosition = position;
//}

double State::getTotalTimePaths() {
    return mTotalTimePaths;
}

void State::setTotalTimePaths(double totalTimePaths) {
    mTotalTimePaths = totalTimePaths;
}

int State::getLevelMcts() const {
    return mLevelMCTS;
}

void State::setLevelMcts(int levelMcts) {
    mLevelMCTS = levelMcts;
}

int State::getNumberResources() const {
    return nNumberResources;
}

void State::setNumberResources(int numberResources) {
    nNumberResources = numberResources;
}

unordered_map<int, int>& State::getNumberOperatorsSite() {
    return nNumberOperatorsSite;
}

void State::setNumberOperatorsSite(
        const unordered_map<int, int>& numberOperatorsSite) {
    nNumberOperatorsSite = numberOperatorsSite;
}

double State::getRealTotalTimePaths() const {
    return mRealTotalTimePaths;
}

void State::setRealTotalTimePaths(double realTotalTimePaths) {
    mRealTotalTimePaths = realTotalTimePaths;
}

vector<Migration>& State::getMigration() {
    return mMigration;
}

void State::setMigration(const vector<Migration>& migration) {
    mMigration = migration;
}

int State::getNumberMessagesCloud() const {
    return mNumberMessagesCloud;
}

void State::setNumberMessagesCloud(int numberMessagesCloud) {
    mNumberMessagesCloud = numberMessagesCloud;
}

double State::getWanTraffic() const {
    return mWANTraffic;
}

void State::setWanTraffic(double wanTraffic) {
    mWANTraffic = wanTraffic;
}

double State::getAwsPricing() const {
    return mAWSPricing;
}

void State::setAwsPricing(double awsPricing) {
    mAWSPricing = awsPricing;
}

double State::getMicrosoftPricing() const {
    return mMicrosoftPricing;
}

void State::setMicrosoftPricing(double microsoftPricing) {
    mMicrosoftPricing = microsoftPricing;
}

int State::getNumberCloudConnections() const {
    return mNumberCloudConnections;
}

void State::setNumberCloudConnections(int numberCloudConnections) {
    mNumberCloudConnections = numberCloudConnections;
}

double State::getAverageSizeCloudMsgs() const {
    return mAverageSizeCloudMsgs;
}

void State::setAverageSizeCloudMsgs(double averageSizeCloudMsgs) {
    mAverageSizeCloudMsgs = averageSizeCloudMsgs;
}

string State::toString() {
    return "state;id=" + to_string(this->getId()) + ";levelmcts="
            + to_string(this->getLevelMcts()) + ";totaltimepaths="
            + to_string(this->getTotalTimePaths()) + ";totalrealtimepaths="
            + to_string(this->getRealTotalTimePaths()) + ";numberresources="
            + to_string(this->getNumberResources()) + ";wantraffic="
            + to_string(this->getWanTraffic()) + ";numbermsgscloud="
            + to_string(this->getNumberMessagesCloud()) + ";microsoft="
            + to_string(this->getMicrosoftPricing()) + ";aws="
            + to_string(this->getAwsPricing()) + ";numbercloudconnections="
            + to_string(this->getNumberCloudConnections())
            + ";averagesizemsgtocloud="
            + to_string(this->getAverageSizeCloudMsgs());
}
} /* namespace fogstream */


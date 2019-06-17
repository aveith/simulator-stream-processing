#include "InternalState.h"
#include "../../../../util/Patterns.h"

namespace fogstream {

GeneratedTime::GeneratedTime(vector<int> path, simtime_t generatedTimeStamp) {
    this->setPath(path);
    this->setGeneratedTimeStamp(generatedTimeStamp);
}

simtime_t& GeneratedTime::getGeneratedTimeStamp() {
    return mGeneratedTimeStamp;
}

void GeneratedTime::setGeneratedTimeStamp(simtime_t& generatedTimeStamp) {
    mGeneratedTimeStamp = generatedTimeStamp;
}

vector<int>& GeneratedTime::getPath() {
    return mPath;
}

void GeneratedTime::setPath(vector<int>& path) {
    mPath = path;
}

//***************InternalState**********************************/

InternalState::InternalState(int type, double windowSize) {
    this->setType(type);
    this->setWindowSize(windowSize);
    this->InitializeState();

}

InternalState::~InternalState() {
    // TODO Auto-generated destructor stub
}

double InternalState::getBytesSize() const {
    return mBytesSize;
}

void InternalState::setBytesSize(double bytesSize) {
    mBytesSize = bytesSize;
}

const simtime_t& InternalState::getTimestamp() const {
    return mTimestamp;
}

void InternalState::setTimestamp(const simtime_t& timestamp) {
    mTimestamp = timestamp;
}

int InternalState::getType() const {
    return mType;
}

void InternalState::setType(int type) {
    mType = type;
}

double InternalState::getWindowCounter() {
    return mWindowCounter;
}

void InternalState::setWindowCounter(double windowCounter) {
    mWindowCounter = windowCounter;
}

bool InternalState::isReady() {
    switch (this->getType()) {
    case Patterns::InternalStateType::CountTrigger:
        return floor(this->getWindowCounter()) == floor(this->getWindowSize());
    }
}

void InternalState::InitializeState() {
    this->setBytesSize(0);
    this->setWindowCounter(0);
    this->setTimestamp(simTime());
    this->getGeneratedTimePaths().clear();
    this->getFirstMessage() = nullptr;
}

double InternalState::getWindowSize() const {
    return mWindowSize;
}

void InternalState::setWindowSize(double windowSize) {
    mWindowSize = windowSize;
}/* namespace fogstream */

vector<GeneratedTime*>& InternalState::getGeneratedTimePaths() {
    return mGeneratedTimePaths;
}

void InternalState::setGeneratedTimesVector(vector<vector<int> > paths,
        vector<simtime_t> firstMGenerationCrossedPath) {
    //First, the algorithm checks if the path already exists to the internal state,
    // then if the path doesn't exist it will include to the internal state
    for (int i = 0; i < paths.size(); i++) {
        if (this->getGeneratedTimePaths().size() == 0) {
            this->getGeneratedTimePaths().push_back(
                    new GeneratedTime(paths.at(i),
                            firstMGenerationCrossedPath.at(i)));
        } else {
            bool bExists = false;
            for (int j = 0; j < this->getGeneratedTimePaths().size(); j++) {
                if (this->getGeneratedTimePaths().at(j)->getPath()
                        == paths.at(i)) {
                    bExists = true;
                    break;
                }
            }

            if (!bExists) {
                this->getGeneratedTimePaths().push_back(
                        new GeneratedTime(paths.at(i),
                                firstMGenerationCrossedPath.at(i)));
            }
        }
    }
}

void InternalState::setGeneratedTimePaths(
        vector<GeneratedTime*>& generatedTimePaths) {
    mGeneratedTimePaths = generatedTimePaths;
}

cMessage*& InternalState::getFirstMessage() {
    return mFirstMessage;
}

void InternalState::setFirstMessage(cMessage*& firstMessage) {
    mFirstMessage = firstMessage;
}

}
;


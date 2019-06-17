#ifndef MODULES_SIMPLEM_OPERATOR_INTERNALSTATE_INTERNALSTATE_H_
#define MODULES_SIMPLEM_OPERATOR_INTERNALSTATE_INTERNALSTATE_H_

#include <omnetpp.h>
#include <vector>

using namespace std;
using namespace omnetpp;

namespace fogstream {


class GeneratedTime {
public:
    GeneratedTime(vector<int> path, simtime_t generatedTimeStamp);
    simtime_t& getGeneratedTimeStamp();
    void setGeneratedTimeStamp(simtime_t& generatedTimeStamp);
    vector<int>& getPath();
    void setPath(vector<int>& path);

protected:
    vector<int> mPath;
    simtime_t mGeneratedTimeStamp;
};

struct CrossedTimePaths {
    vector<GeneratedTime*> crossedPaths;
};

class InternalState {
public:
    InternalState(int type, double windowSize);
    virtual ~InternalState();
    double getBytesSize() const;
    void setBytesSize(double bytesSize);
    const simtime_t& getTimestamp() const;
    void setTimestamp(const simtime_t& timestamp);
    int getType() const;
    void setType(int type);
    double getWindowCounter();
    void setWindowCounter(double windowCounter);
    double getWindowSize() const;
    void setWindowSize(double windowSize);

    bool isReady();
    void InitializeState();
    void setGeneratedTimesVector(vector<vector<int>> paths, vector<simtime_t> firstMGenerationCrossedPath);
    vector<GeneratedTime*>& getGeneratedTimePaths() ;
    void setGeneratedTimePaths(
            vector<GeneratedTime*>& generatedTimePaths);
    cMessage*& getFirstMessage();
    void setFirstMessage(cMessage*& firstMessage);

protected:
    int mType = 0;
    double mWindowCounter = 0;
    double mWindowSize;
    simtime_t mTimestamp;
    double mBytesSize = 0;
    vector<GeneratedTime*> mGeneratedTimePaths;
    cMessage* mFirstMessage = nullptr;
};

} /* namespace fogstream */

#endif /* MODULES_SIMPLEM_OPERATOR_INTERNALSTATE_INTERNALSTATE_H_ */

#ifndef CONFIGURATION_OBJECTS_APPLICATION_OPERATORDATA_OPERATORDATA_H_
#define CONFIGURATION_OBJECTS_APPLICATION_OPERATORDATA_OPERATORDATA_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class OperatorData {
public:
    OperatorData();
    OperatorData(int id, int fissionId, double selectivity,
            double dataTransferRate, double stateRate, int type,
            double cpuRequirement, double memoryRequirement, double timeWindow,
            bool nextSplitted, int nextSplittedLength, int characteristicId,
            double stateSize);
    virtual ~OperatorData();
    int getCharacteristicId() const;
    void setCharacteristicId(int characteristicId);
    double getCPURequirement() const;
    void setCPURequirement(double cpuRequirement);
    double getDataTransferRate() const;
    void setDataTransferRate(double dataTransferRate);
    int getFissionId() const;
    void setFissionId(int fissionId);
    int getId() const;
    void setId(int id);
    double getMemoryRequirement() const;
    void setMemoryRequirement(double memoryRequirement);
    bool isNextSplitted() const;
    void setNextSplitted(bool nextSplitted);
    double getSelectivity() const;
    void setSelectivity(double selectivity);
    double getStateRate() const;
    void setStateRate(double stateRate);
    double getStateSize() const;
    void setStateSize(double stateSize);
    double getTimeWindow() const;
    void setTimeWindow(double timeWindow);
    int getType() const;
    void setType(int type);
    int getNextSplittedLength() const;
    void setNextSplittedLength(int nextSplittedLength);

protected:
    int mID;
    int mFissionID;
    double mSelectivity;
    double mDataTransferRate;
    double mStateRate;
    int mType;
    double mCPURequirement;
    double mMemoryRequirement;
    double mTimeWindow;
    bool mNextSplitted;
    int nNextSplittedLength;
    int mCharacteristicId = -1;
    double mStateSize;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_APPLICATION_OPERATORDATA_OPERATORDATA_H_ */

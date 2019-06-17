#ifndef RECONFIGURATION_STATISTICS_OPERATOR_OPERATOR_H_
#define RECONFIGURATION_STATISTICS_OPERATOR_OPERATOR_H_

#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class OperatorStatistics {

public:
    OperatorStatistics();
    OperatorStatistics(int hostId, int operatorId, double arrivalMsgSize,
            double arrivalRate, double compTime, double compRequirement,
            double memoryRequirement, double queueSize, double outputMsgSize,
            double outputRate, double selectivity,
            double dataCompressionExpasionFactor, double stateSize, double requiredTimeBuildWindow);
    virtual ~OperatorStatistics();

    double getArrivalMsgSize() const;
    void setArrivalMsgSize(double arrivalMsgSize);
    double getArrivalRate() const;
    void setArrivalRate(double arrivalRate);
    double getCompRequirement() const;
    void setCompRequirement(double compRequirement);
    double getCompTime() const;
    void setCompTime(double compTime);
    double getMemoryRequirement() const;
    void setMemoryRequirement(double memoryRequirement);
    double getOutputMsgSize() const;
    void setOutputMsgSize(double outputMsgSize);
    double getOutputRate() const;
    void setOutputRate(double outputRate);
    double getQueueSize() const;
    void setQueueSize(double queueSize);
    double getDataCompressionExpasionFactor() const;
    void setDataCompressionExpasionFactor(double dataCompressionExpasionFactor);
    double getSelectivity() const;
    void setSelectivity(double selectivity);
    int getHostId() const;
    void setHostId(int hostId);
    int getOperatorId() const;
    void setOperatorId(int operatorId);

    string toString();
    string toStringDetail();
    double getStateSize() const;
    void setStateSize(double stateSize);
    double getRequiredTimeBuildWindow() const;
    void setRequiredTimeBuildWindow(double requiredTimeBuildWindow);

protected:
    double mArrivalMsgSize;
    double mArrivalRate;
    double mCompTime;
    double mCompRequirement;
    double mMemoryRequirement;
    double mQueueSize;
    double mOutputMsgSize;
    double mOutputRate;
    double mSelectivity;
    double mDataCompressionExpasionFactor;
    int mHostId;
    int mOperatorId;
    double mStateSize;
    double mRequiredTimeBuildWindow;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_STATISTICS_OPERATOR_OPERATOR_H_ */


#ifndef CONFIGURATION_OBJECTS_APPLICATIONMAPPING_OPERATORMAPPING_OPERATORMAPPING_H_
#define CONFIGURATION_OBJECTS_APPLICATIONMAPPING_OPERATORMAPPING_OPERATORMAPPING_H_

namespace fogstream {

class OperatorMapping {
public:
    OperatorMapping();
    OperatorMapping(OperatorMapping* map);
    virtual ~OperatorMapping();
    double getCompTime() const;
    void setCompTime(double compTime);
    int getHostId() const;
    void setHostId(int hostId);
    double getInputRate() const;
    void setInputRate(double inputRate);
    int getOperatorId() const;
    void setOperatorId(int operatorId);
    double getOutputRate() const;
    void setOutputRate(double outputRate);
    double getQueueSize() const;
    void setQueueSize(double queueSize);
    double getRequiredCpu() const;
    void setRequiredCpu(double requiredCpu);
    double getRequiredMemory() const;
    void setRequiredMemory(double requiredMemory);
    double getInputMsgSize() const;
    void setInputMsgSize(double inputMsgSize);
    double getOutputMsgSize() const;
    void setOutputMsgSize(double outputMsgSize);

private:
    int mHostID =-1;
    int mOperatorID =-1;
    double mCompTime = 0;
    double mInputRate = 0;
    double mOutputRate = 0;
    double mInputMsgSize = 0;
    double mOutputMsgSize = 0;
    double mRequiredCPU = 0;
    double mRequiredMemory = 0;
    double mQueueSize = 0;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_APPLICATIONMAPPING_OPERATORMAPPING_OPERATORMAPPING_H_ */

#ifndef CONFIGURATION_OBJECTS_APPLICATIONMAPPING_OPERATORCONNECTIONMAPPING_OPERATORCONNECTIONMAPPING_H_
#define CONFIGURATION_OBJECTS_APPLICATIONMAPPING_OPERATORCONNECTIONMAPPING_OPERATORCONNECTIONMAPPING_H_

namespace fogstream {

class OperatorConnectionMapping {
public:
    OperatorConnectionMapping(OperatorConnectionMapping* mapping);
    OperatorConnectionMapping(int fromOperatorId, int toOperatorId, int linkId,
            double inputRate, double outputRate, double queueSize,
            double commTime, double requiredBandwidth);
    virtual ~OperatorConnectionMapping();
    int getLinkId() const;
    void setLinkId(int linkId);
    int getFromOperatorId() const;
    void setFromOperatorId(int fromOperatorId);
    int getToOperatorId() const;
    void setToOperatorId(int toOperatorId);
    double getInputRate() const;
    void setInputRate(double inputRate);
    double getOutputRate() const;
    void setOutputRate(double outputRate);
    double getQueueSize() const;
    void setQueueSize(double queueSize);
    double getCommTime() const;
    void setCommTime(double commTime);
    double getRequiredBandwidth() const;
    void setRequiredBandwidth(double requiredBandwidth);

protected:
    int mFromOperatorID = -1;
    int mToOperatorID = -1;
    int mLinkID = 1;
    double mInputRate = 0;
    double mOutputRate = 0;
    double mQueueSize = 0;
    double mCommTime = 0;
    double mRequiredBandwidth = 0;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_OBJECTS_APPLICATIONMAPPING_OPERATORCONNECTIONMAPPING_OPERATORCONNECTIONMAPPING_H_ */

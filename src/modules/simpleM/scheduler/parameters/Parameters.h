#ifndef MODULES_SIMPLEM_SCHEDULER_PARAMETERS_PARAMETERS_H_
#define MODULES_SIMPLEM_SCHEDULER_PARAMETERS_PARAMETERS_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Parameters {
public:
    Parameters();
    virtual ~Parameters();

    string toStringBaseParameters();
    string getHashFileName();

    const string& getApp() const;
    void setApp(const string& app);
    double getArrivalRateMax() const;
    void setArrivalRateMax(double arrivalRateMax);
    double getArrivalRateMin() const;
    void setArrivalRateMin(double arrivalRateMin);
    int getBaseStrategy() const;
    void setBaseStrategy(int baseStrategy);
    double getBytesMsgMax() const;
    void setBytesMsgMax(double bytesMsgMax);
    double getBytesMsgMin() const;
    void setBytesMsgMin(double bytesMsgMin);
    double getCpuMax() const;
    void setCpuMax(double cpuMax);
    double getCpuMin() const;
    void setCpuMin(double cpuMin);
    double getCpuSrcMax() const;
    void setCpuSrcMax(double cpuSrcMax);
    double getCpuSrcMin() const;
    void setCpuSrcMin(double cpuSrcMin);
    double getDataRateMax() const;
    void setDataRateMax(double dataRateMax);
    double getDataRateMin() const;
    void setDataRateMin(double dataRateMin);
    const string& getDirectoryToSaveFiles() const;
    void setDirectoryToSaveFiles(const string& directoryToSaveFiles);
    bool isFinishAfterTracing() const;
    void setFinishAfterTracing(bool finishAfterTracing);
    double getMemoryMax() const;
    void setMemoryMax(double memoryMax);
    double getMemoryMin() const;
    void setMemoryMin(double memoryMin);
    string& getNetwork();
    void setNetwork(const string& network);
    bool isReadFromTracedDataReconfig() const;
    void setReadFromTracedDataReconfig(bool readFromTracedDataReconfig);
    double getReconfigAlpha() const;
    void setReconfigAlpha(double reconfigAlpha);
    double getReconfigConstant() const;
    void setReconfigConstant(double reconfigConstant);
    double getReconfigEpsilon() const;
    void setReconfigEpsilon(double reconfigEpsilon);
    double getReconfigGamma() const;
    void setReconfigGamma(double reconfigGamma);
    int getReconfigIterations() const;
    void setReconfigIterations(int reconfigIterations);
    double getReconfigLambda() const;
    void setReconfigLambda(double reconfigLambda);
    int getReconfigStrategy() const;
    void setReconfigStrategy(int reconfigStrategy);
    bool isSaveTracedDataReconfig() const;
    void setSaveTracedDataReconfig(bool saveTracedDataReconfig);
    double getSelectivityMax() const;
    void setSelectivityMax(double selectivityMax);
    double getSelectivityMin() const;
    void setSelectivityMin(double selectivityMin);
    double getStateRateMax() const;
    void setStateRateMax(double stateRateMax);
    double getStateRateMin() const;
    void setStateRateMin(double stateRateMin);
    const string& getVertexPosition() const;
    void setVertexPosition(const string& vertexPosition);
    const string& getDirTracedDatasets() const;
    void setDirTracedDatasets(const string& dirTracedDatasets);
    bool isReconfigApplyingRp() const;
    void setReconfigApplyingRp(bool reconfigApplyingRp);
    double getCostsParameter() const;
    void setCostsParameter(double costsParameter);
    double getCostsWeight() const;
    void setCostsWeight(double costsWeight);
    double getLatencyParameter() const;
    void setLatencyParameter(double latencyParameter);
    double getLatencyWeight() const;
    void setLatencyWeight(double latencyWeight);
    double getMigrationParameter() const;
    void setMigrationParameter(double migrationParameter);
    double getMigrationWeight() const;
    void setMigrationWeight(double migrationWeight);
    double getWanTrafficParamenter() const;
    void setWanTrafficParamenter(double wanTrafficParamenter);
    double getWanTrafficWeight() const;
    void setWanTrafficWeight(double wanTrafficWeight);
    int getPricingType() const;
    void setPricingType(int pricingType);
    bool isAggregateCostBasedReward() const;
    void setAggregateCostBasedReward(bool aggregateCostBasedReward);
    int getConfigurationSeeds() const;
    void setConfigurationSeeds(int configurationSeeds);
    double getOperatorsWithStatePerc() const;
    void setOperatorsWithStatePerc(double operatorsWithStatePerc);
    double getWindowMax() const;
    void setWindowMax(double windowMax);
    double getWindowMin() const;
    void setWindowMin(double windowMin);
    string getTestId();
    void setTestId(string testId);
    bool isReturnOnlyConfigurationDeployment() const;
    void setReturnOnlyConfigurationDeployment(
            bool returnOnlyConfigurationDeployment);
    const string& getFileOperatorStatistics() const;
    void setFileOperatorStatistics(const string& fileOperatorStatistics);
    const string& getFilePathStatistics() const;
    void setFilePathStatistics(const string& filePathStatistics);
    int getSinkLocations() const;
    void setSinkLocations(int sinkLocations);
    int getSourceLocations() const;
    void setSourceLocations(int sourceLocations);
    bool isUseSlots() const;
    void setUseSlots(bool useSlots);
    int getActionHeuristic() const;
    void setActionHeuristic(int actionHeuristic);
    int getTimeReconfiguration() const;
    void setTimeReconfiguration(int timeReconfiguration);

protected:
    string mDirectoryToSaveFiles;
    int mBaseStrategy;
    string mApp;
    string mNetwork;
    double mSelectivityMin;
    double mSelectivityMax;
    double mDataRateMin;
    double mDataRateMax;
    double mCPUMin;
    double mCPUMax;
    double mCPUSrcMin;
    double mCPUSrcMax;
    double mMemoryMin;
    double mMemoryMax;
    double mArrivalRateMin;
    double mArrivalRateMax;
    double mBytesMsgMin;
    double mBytesMsgMax;
    double mStateRateMin;
    double mStateRateMax;
    double mWindowMin;
    double mWindowMax;
    double mOperatorsWithStatePerc;
    string mVertexPosition;
    bool mSaveTracedDataReconfig;
    bool mReadFromTracedDataReconfig;
    bool mFinishAfterTracing;
    double mReconfigConstant;
    double mReconfigAlpha;
    double mReconfigGamma;
    double mReconfigLambda;
    double mReconfigEpsilon;
    int mReconfigStrategy;
    int mReconfigIterations;
    string mDirTracedDatasets;
    bool mReconfigApplyingRP;

    double mLatencyWeight;
    double mMigrationWeight;
    double mCostsWeight;
    double mWanTrafficWeight;

    double mLatencyParameter;
    double mMigrationParameter;
    double mCostsParameter;
    double mWanTrafficParamenter;

    int mPricingType;
    bool mAggregateCostBasedReward;
    int mConfigurationSeeds;

    bool mReturnOnlyConfigurationDeployment;
    string mFileOperatorStatistics;
    string mFilePathStatistics;
    string mTestID;

    int mSourceLocations = 0;
    int mSinkLocations = 0;
    bool mUseSlots = false;
    int mActionHeuristic = 0;
    int mTimeReconfiguration = -1;
};

} /* namespace fogstream */

#endif /* MODULES_SIMPLEM_SCHEDULER_PARAMETERS_PARAMETERS_H_ */

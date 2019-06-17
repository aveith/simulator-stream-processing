//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "../parameters/Parameters.h"

namespace fogstream {

Parameters::Parameters() {
    // TODO Auto-generated constructor stub

}

Parameters::~Parameters() {
    // TODO Auto-generated destructor stub
}

const string& Parameters::getApp() const {
    return mApp;
}

void Parameters::setApp(const string& app) {
    mApp = app;
}

double Parameters::getArrivalRateMax() const {
    return mArrivalRateMax;
}

void Parameters::setArrivalRateMax(double arrivalRateMax) {
    mArrivalRateMax = arrivalRateMax;
}

double Parameters::getArrivalRateMin() const {
    return mArrivalRateMin;
}

void Parameters::setArrivalRateMin(double arrivalRateMin) {
    mArrivalRateMin = arrivalRateMin;
}

int Parameters::getBaseStrategy() const {
    return mBaseStrategy;
}

void Parameters::setBaseStrategy(int baseStrategy) {
    mBaseStrategy = baseStrategy;
}

double Parameters::getBytesMsgMax() const {
    return mBytesMsgMax;
}

void Parameters::setBytesMsgMax(double bytesMsgMax) {
    mBytesMsgMax = bytesMsgMax;
}

double Parameters::getBytesMsgMin() const {
    return mBytesMsgMin;
}

void Parameters::setBytesMsgMin(double bytesMsgMin) {
    mBytesMsgMin = bytesMsgMin;
}

double Parameters::getCpuMax() const {
    return mCPUMax;
}

void Parameters::setCpuMax(double cpuMax) {
    mCPUMax = cpuMax;
}

double Parameters::getCpuMin() const {
    return mCPUMin;
}

void Parameters::setCpuMin(double cpuMin) {
    mCPUMin = cpuMin;
}

double Parameters::getCpuSrcMax() const {
    return mCPUSrcMax;
}

void Parameters::setCpuSrcMax(double cpuSrcMax) {
    mCPUSrcMax = cpuSrcMax;
}

double Parameters::getCpuSrcMin() const {
    return mCPUSrcMin;
}

void Parameters::setCpuSrcMin(double cpuSrcMin) {
    mCPUSrcMin = cpuSrcMin;
}

double Parameters::getDataRateMax() const {
    return mDataRateMax;
}

void Parameters::setDataRateMax(double dataRateMax) {
    mDataRateMax = dataRateMax;
}

double Parameters::getDataRateMin() const {
    return mDataRateMin;
}

void Parameters::setDataRateMin(double dataRateMin) {
    mDataRateMin = dataRateMin;
}

const string& Parameters::getDirectoryToSaveFiles() const {
    return mDirectoryToSaveFiles;
}

void Parameters::setDirectoryToSaveFiles(const string& directoryToSaveFiles) {
    mDirectoryToSaveFiles = directoryToSaveFiles;
}

bool Parameters::isFinishAfterTracing() const {
    return mFinishAfterTracing;
}

void Parameters::setFinishAfterTracing(bool finishAfterTracing) {
    mFinishAfterTracing = finishAfterTracing;
}

double Parameters::getMemoryMax() const {
    return mMemoryMax;
}

void Parameters::setMemoryMax(double memoryMax) {
    mMemoryMax = memoryMax;
}

double Parameters::getMemoryMin() const {
    return mMemoryMin;
}

void Parameters::setMemoryMin(double memoryMin) {
    mMemoryMin = memoryMin;
}

string& Parameters::getNetwork() {
    return mNetwork;
}

void Parameters::setNetwork(const string& network) {
    mNetwork = network;
}

bool Parameters::isReadFromTracedDataReconfig() const {
    return mReadFromTracedDataReconfig;
}

void Parameters::setReadFromTracedDataReconfig(
        bool readFromTracedDataReconfig) {
    mReadFromTracedDataReconfig = readFromTracedDataReconfig;
}

double Parameters::getReconfigAlpha() const {
    return mReconfigAlpha;
}

void Parameters::setReconfigAlpha(double reconfigAlpha) {
    mReconfigAlpha = reconfigAlpha;
}

double Parameters::getReconfigConstant() const {
    return mReconfigConstant;
}

void Parameters::setReconfigConstant(double reconfigConstant) {
    mReconfigConstant = reconfigConstant;
}

double Parameters::getReconfigEpsilon() const {
    return mReconfigEpsilon;
}

void Parameters::setReconfigEpsilon(double reconfigEpsilon) {
    mReconfigEpsilon = reconfigEpsilon;
}

double Parameters::getReconfigGamma() const {
    return mReconfigGamma;
}

void Parameters::setReconfigGamma(double reconfigGamma) {
    mReconfigGamma = reconfigGamma;
}

int Parameters::getReconfigIterations() const {
    return mReconfigIterations;
}

void Parameters::setReconfigIterations(int reconfigIterations) {
    mReconfigIterations = reconfigIterations;
}

double Parameters::getReconfigLambda() const {
    return mReconfigLambda;
}

void Parameters::setReconfigLambda(double reconfigLambda) {
    mReconfigLambda = reconfigLambda;
}

int Parameters::getReconfigStrategy() const {
    return mReconfigStrategy;
}

void Parameters::setReconfigStrategy(int reconfigStrategy) {
    mReconfigStrategy = reconfigStrategy;
}

bool Parameters::isSaveTracedDataReconfig() const {
    return mSaveTracedDataReconfig;
}

void Parameters::setSaveTracedDataReconfig(bool saveTracedDataReconfig) {
    mSaveTracedDataReconfig = saveTracedDataReconfig;
}

double Parameters::getSelectivityMax() const {
    return mSelectivityMax;
}

void Parameters::setSelectivityMax(double selectivityMax) {
    mSelectivityMax = selectivityMax;
}

double Parameters::getSelectivityMin() const {
    return mSelectivityMin;
}

void Parameters::setSelectivityMin(double selectivityMin) {
    mSelectivityMin = selectivityMin;
}

double Parameters::getStateRateMax() const {
    return mStateRateMax;
}

void Parameters::setStateRateMax(double stateRateMax) {
    mStateRateMax = stateRateMax;
}

double Parameters::getStateRateMin() const {
    return mStateRateMin;
}

void Parameters::setStateRateMin(double stateRateMin) {
    mStateRateMin = stateRateMin;
}

const string& Parameters::getVertexPosition() const {
    return mVertexPosition;
}

void Parameters::setVertexPosition(const string& vertexPosition) {
    mVertexPosition = vertexPosition;
}

const string& Parameters::getDirTracedDatasets() const {
    return mDirTracedDatasets;
}

void Parameters::setDirTracedDatasets(const string& dirTracedDatasets) {
    mDirTracedDatasets = dirTracedDatasets;
}

string Parameters::toStringBaseParameters() {
    return to_string(this->getBaseStrategy()) + this->getApp()
            + this->getNetwork() + to_string(this->getSelectivityMin())
            + to_string(this->getSelectivityMax())
            + to_string(this->getDataRateMin())
            + to_string(this->getDataRateMax()) + to_string(this->getCpuMin())
            + to_string(this->getCpuMax()) + to_string(this->getCpuSrcMin())
            + to_string(this->getCpuSrcMax()) + to_string(this->getMemoryMin())
            + to_string(this->getMemoryMax())
            + to_string(this->getArrivalRateMin())
            + to_string(this->getArrivalRateMax())
            + to_string(this->getBytesMsgMin())
            + to_string(this->getBytesMsgMax())
            + to_string(this->getStateRateMin())
            + to_string(this->getStateRateMax())
            + to_string(this->getWindowMin()) + to_string(this->getWindowMax())
            + to_string(this->getWindowMin())
            + to_string(this->getOperatorsWithStatePerc())
            + to_string(this->getConfigurationSeeds())
            + this->getVertexPosition();

}

string Parameters::getHashFileName() {
    return to_string(hash<string> { }(this->toStringBaseParameters()));
}

bool Parameters::isReconfigApplyingRp() const {
    return mReconfigApplyingRP;
}

void Parameters::setReconfigApplyingRp(bool reconfigApplyingRp) {
    mReconfigApplyingRP = reconfigApplyingRp;
}

double Parameters::getCostsParameter() const {
    return mCostsParameter;
}

void Parameters::setCostsParameter(double costsParameter) {
    mCostsParameter = costsParameter;
}

double Parameters::getCostsWeight() const {
    return mCostsWeight;
}

void Parameters::setCostsWeight(double costsWeight) {
    mCostsWeight = costsWeight;
}

double Parameters::getLatencyParameter() const {
    return mLatencyParameter;
}

void Parameters::setLatencyParameter(double latencyParameter) {
    mLatencyParameter = latencyParameter;
}

double Parameters::getLatencyWeight() const {
    return mLatencyWeight;
}

void Parameters::setLatencyWeight(double latencyWeight) {
    mLatencyWeight = latencyWeight;
}

double Parameters::getMigrationParameter() const {
    return mMigrationParameter;
}

void Parameters::setMigrationParameter(double migrationParameter) {
    mMigrationParameter = migrationParameter;
}

double Parameters::getMigrationWeight() const {
    return mMigrationWeight;
}

void Parameters::setMigrationWeight(double migrationWeight) {
    mMigrationWeight = migrationWeight;
}

double Parameters::getWanTrafficParamenter() const {
    return mWanTrafficParamenter;
}

void Parameters::setWanTrafficParamenter(double wanTrafficParamenter) {
    mWanTrafficParamenter = wanTrafficParamenter;
}

double Parameters::getWanTrafficWeight() const {
    return mWanTrafficWeight;
}

void Parameters::setWanTrafficWeight(double wanTrafficWeight) {
    mWanTrafficWeight = wanTrafficWeight;
}

int Parameters::getPricingType() const {
    return mPricingType;
}

void Parameters::setPricingType(int pricingType) {
    mPricingType = pricingType;
}

bool Parameters::isAggregateCostBasedReward() const {
    return mAggregateCostBasedReward;
}

void Parameters::setAggregateCostBasedReward(bool aggregateCostBasedReward) {
    mAggregateCostBasedReward = aggregateCostBasedReward;
}

int Parameters::getConfigurationSeeds() const {
    return mConfigurationSeeds;
}

void Parameters::setConfigurationSeeds(int configurationSeeds) {
    mConfigurationSeeds = configurationSeeds;
}

double Parameters::getOperatorsWithStatePerc() const {
    return mOperatorsWithStatePerc;
}

void Parameters::setOperatorsWithStatePerc(double operatorsWithStatePerc) {
    mOperatorsWithStatePerc = operatorsWithStatePerc;
}

double Parameters::getWindowMax() const {
    return mWindowMax;
}

void Parameters::setWindowMax(double windowMax) {
    mWindowMax = windowMax;
}

double Parameters::getWindowMin() const {
    return mWindowMin;
}

void Parameters::setWindowMin(double windowMin) {
    mWindowMin = windowMin;
}

string Parameters::getTestId() {
    return mTestID;
}

void Parameters::setTestId(string testId) {
    mTestID = testId;
}

bool Parameters::isReturnOnlyConfigurationDeployment() const {
    return mReturnOnlyConfigurationDeployment;
}

void Parameters::setReturnOnlyConfigurationDeployment(
        bool returnOnlyConfigurationDeployment) {
    mReturnOnlyConfigurationDeployment = returnOnlyConfigurationDeployment;
}

const string& Parameters::getFileOperatorStatistics() const {
    return mFileOperatorStatistics;
}

void Parameters::setFileOperatorStatistics(
        const string& fileOperatorStatistics) {
    mFileOperatorStatistics = fileOperatorStatistics;
}

const string& Parameters::getFilePathStatistics() const {
    return mFilePathStatistics;
}

void Parameters::setFilePathStatistics(const string& filePathStatistics) {
    mFilePathStatistics = filePathStatistics;
}

int Parameters::getSinkLocations() const {
    return mSinkLocations;
}

void Parameters::setSinkLocations(int sinkLocations) {
    mSinkLocations = sinkLocations;
}

int Parameters::getSourceLocations() const {
    return mSourceLocations;
}

void Parameters::setSourceLocations(int sourceLocations) {
    mSourceLocations = sourceLocations;
}

int Parameters::getActionHeuristic() const {
    return mActionHeuristic;
}

void Parameters::setActionHeuristic(int actionHeuristic) {
    mActionHeuristic = actionHeuristic;
}


bool Parameters::isUseSlots() const {
    return mUseSlots;
}

void Parameters::setUseSlots(bool useSlots) {
    mUseSlots = useSlots;
}

int Parameters::getTimeReconfiguration() const {
    return mTimeReconfiguration;
}

void Parameters::setTimeReconfiguration(int timeReconfiguration) {
    mTimeReconfiguration = timeReconfiguration;
}

} /* namespace fogstream */



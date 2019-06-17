#include "Mapping.h"

namespace fogstream {

Mapping::Mapping() {
    // TODO Auto-generated constructor stub

}

Mapping::Mapping(vector<MapOperator> operatorMappings,
        vector<MapEdge> edgeMappings,
        unordered_map<int, HostAvailability> residualHostCapabilities,
        unordered_map<int, LinkAvailability> residualLinkCapabilities,
        vector<double> realPathTimes, vector<double> estimatedPathTimes) {
    this->setOperatorMappings(operatorMappings);
    this->setEdgeMappings(edgeMappings);
    this->setResidualHostCapabilities(residualHostCapabilities);
    this->setResidualLinkCapabilities(residualLinkCapabilities);
    this->setRealPathTimes(realPathTimes);
    this->setEstimatedPathTimes(estimatedPathTimes);
}

Mapping::~Mapping() {
    // TODO Auto-generated destructor stub
}

vector<MapEdge>& Mapping::getEdgeMappings() {
    return mEdgeMappings;
}

void Mapping::setEdgeMappings(const vector<MapEdge>& edgeMappings) {
    mEdgeMappings = edgeMappings;
}

vector<double>& Mapping::getEstimatedPathTimes() {
    return mEstimatedPathTimes;
}

void Mapping::setEstimatedPathTimes(const vector<double>& estimatedPathTimes) {
    mEstimatedPathTimes = estimatedPathTimes;
}

vector<MapOperator>& Mapping::getOperatorMappings() {
    return mOperatorMappings;
}

void Mapping::setOperatorMappings(const vector<MapOperator>& operatorMappings) {
    mOperatorMappings = operatorMappings;
}

vector<double>& Mapping::getRealPathTimes() {
    return mRealPathTimes;
}

void Mapping::setRealPathTimes(const vector<double>& realPathTimes) {
    mRealPathTimes = realPathTimes;
}

unordered_map<int, HostAvailability>& Mapping::getResidualHostCapabilities() {
    return mResidualHostCapabilities;
}

void Mapping::setResidualHostCapabilities(
        const unordered_map<int, HostAvailability>& residualHostCapabilities) {
    mResidualHostCapabilities = residualHostCapabilities;
}

unordered_map<int, LinkAvailability>& Mapping::getResidualLinkCapabilities() {
    return mResidualLinkCapabilities;
}

void Mapping::setResidualLinkCapabilities(
        const unordered_map<int, LinkAvailability>& residualLinkCapabilities) {
    mResidualLinkCapabilities = residualLinkCapabilities;
}
string Mapping::toString() {
    string info = "MAP";
    for (unsigned int i = 0; i < this->getRealPathTimes().size(); i++) {
        info = info + "\n     realtimepath=" + to_string(i) + ";time="
                + to_string(this->getRealPathTimes().at(i));
    }

    for (unsigned int i = 0; i < this->getEstimatedPathTimes().size(); i++) {
        info = info + "\n     estimatepath=" + to_string(i) + ";time="
                + to_string(this->getEstimatedPathTimes().at(i));
    }
    for (auto map = this->getOperatorMappings().begin();
            map != this->getOperatorMappings().end(); map++) {
        info = info + "\n     mapping operator= "
                + to_string(map->getOperatorId()) + ";host="
                + to_string(map->getHostId());
        info = info + "\n             estimatestatisitcs = " + ";"
                + map->getEstimateStatistics().toStringDetail();
    }

    for (auto map = this->getResidualHostCapabilities().begin();
            map != this->getResidualHostCapabilities().end(); map++) {
        info = info + "\n\n\n             residualhost = " + ";"
                + map->second.toString();
    }

    for (auto map = this->getResidualLinkCapabilities().begin();
            map != this->getResidualLinkCapabilities().end(); map++) {
        info = info + "\n\n\n             residuallink = " + ";"
                + map->second.toString();
    }

    return info;

    //    vector<MapEdge> mEdgeMappings;
    //
}

} /* namespace fogstream */


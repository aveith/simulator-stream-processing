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

#ifndef RECONFIGURATION_MAPPING_MAPPING_H_
#define RECONFIGURATION_MAPPING_MAPPING_H_

#include <vector>
#include <unordered_map>
#include <cassert>
#include <map>
#include <omnetpp.h>

#include "../statistics/host/Host.h"
#include "../statistics/link/Link.h"
#include "../mapping/operator/MapOperator.h"
#include "../mapping/edge/MapEdge.h"
#include "../statistics/operator/OperatorStatistics.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Mapping {

public:
    Mapping();
    Mapping(vector<MapOperator> operatorMappings, vector<MapEdge> edgeMappings,
            unordered_map<int, HostAvailability> residualHostCapabilities,
            unordered_map<int, LinkAvailability> residualLinkCapabilities,
            vector<double> realPathTimes,
            vector<double> estimatedPathTimes);

    virtual ~Mapping();

    vector<MapEdge>& getEdgeMappings();
    void setEdgeMappings(const vector<MapEdge>& edgeMappings);
    vector<double>& getEstimatedPathTimes();
    void setEstimatedPathTimes(const vector<double>& estimatedPathTimes);
    vector<MapOperator>& getOperatorMappings();
    void setOperatorMappings(const vector<MapOperator>& operatorMappings);
    vector<double>& getRealPathTimes();
    void setRealPathTimes(const vector<double>& realPathTimes);
    unordered_map<int, HostAvailability>& getResidualHostCapabilities();
    void setResidualHostCapabilities(const
            unordered_map<int, HostAvailability>& residualHostCapabilities);
    unordered_map<int, LinkAvailability>& getResidualLinkCapabilities();
    void setResidualLinkCapabilities(const
            unordered_map<int, LinkAvailability>& residualLinkCapabilities);
    string toString();

protected:
    vector<MapOperator> mOperatorMappings;
    vector<MapEdge> mEdgeMappings;

    //Current picture of the resources capabilities
    unordered_map<int, HostAvailability> mResidualHostCapabilities;
    unordered_map<int, LinkAvailability> mResidualLinkCapabilities;

    vector<double> mRealPathTimes;
    vector<double> mEstimatedPathTimes;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MAPPING_MAPPING_H_ */

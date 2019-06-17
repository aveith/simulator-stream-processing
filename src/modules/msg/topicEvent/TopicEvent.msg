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

namespace fogstream;

//
// TODO generated message class
//
cplusplus{{
	#include <vector>
	#include <omnetpp.h>
	typedef std::vector<std::vector<int>> Int2dVector;
	typedef std::vector<omnetpp::simtime_t> SimtimeVector;
}};

class noncobject Int2dVector;
class noncobject SimtimeVector;

packet TopicEvent {
    string appName;
    int nextState;
    int nextSubState;
    int srcID;
    simtime_t timeGenerated;
    simtime_t lastQueue;
    simtime_t lastState;
    simtime_t lastComp;
    simtime_t lastComm;
    simtime_t totalComp;
    simtime_t totalComm;
    Int2dVector crossedPaths;    
    SimtimeVector firstMGenerationCrossedPath;
        
}

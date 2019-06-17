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

#ifndef RESCHEDULER_MCTS_STATISTICSMCTS_STATISTICSMCTS_H_
#define RESCHEDULER_MCTS_STATISTICSMCTS_STATISTICSMCTS_H_
#include <omnetpp.h>
#include <vector>

using namespace std;
using namespace omnetpp;

namespace fogstream {

struct event_history {
    simtime_t sTimestamp;
    int iOperatorID;
    int iHostID;
    double dMsgSize;
    simtime_t sCompTime;
    bool bInput;
    event_history(simtime_t sTimestamp, int iOperatorID, int iHostID,
            double dMsgSize, simtime_t sCompTime, bool bInput) {
        this->sTimestamp = sTimestamp;
        this->iOperatorID = iOperatorID;
        this->iHostID = iHostID;
        this->dMsgSize = dMsgSize;
        this->sCompTime = sCompTime;
        this->bInput = bInput;
    }
};

struct state_history {
    simtime_t sTimestamp;
    int iOperatorID;
    int iHostID;
    double dStateSize;
    simtime_t dRequiredTime;
    state_history(simtime_t sTimestamp, int iOperatorID, int iHostID,
            double dStateSize, simtime_t dRequiredTime) {
        this->sTimestamp = sTimestamp;
        this->iOperatorID = iOperatorID;
        this->iHostID = iHostID;
        this->dStateSize = dStateSize;
        this->dRequiredTime = dRequiredTime;
    }
};

struct queue_history {
    simtime_t sTimestamp;
    int iOperatorID;
    int iHostID;
    int iQueueSize;
    simtime_t sCommTime;
    queue_history(simtime_t sTimestamp, int iOperatorID, int iHostID,
            int iQueueSize, simtime_t sCommTime) {
        this->sTimestamp = sTimestamp;
        this->iOperatorID = iOperatorID;
        this->iHostID = iHostID;
        this->iQueueSize = iQueueSize;
        this->sCommTime = sCommTime;
    }
};

struct host_history {
    simtime_t sTimestamp;
    int iHostID;
    int iQueueSize;
    double dMsgSize;
    host_history(simtime_t sTimestamp, int iHostID, int iQueueSize,
            double dMsgSize) {
        this->sTimestamp = sTimestamp;
        this->iHostID = iHostID;
        this->iQueueSize = iQueueSize;
        this->dMsgSize = dMsgSize;
    }
};

struct path_history {
    simtime_t sTimestamp;
    int iPathID;
    simtime_t sTime;
    simtime_t sCommunicationTime;
    simtime_t sComputationTime;
    path_history(simtime_t sTimestamp, int iPathID, simtime_t sTime,
            simtime_t sCommunicationTime, simtime_t sComputationTime) {
        this->sTimestamp = sTimestamp;
        this->iPathID = iPathID;
        this->sTime = sTime;
        this->sCommunicationTime = sCommunicationTime;
        this->sComputationTime = sComputationTime;
    }
};

class StatisticsMCTS {
public:
    StatisticsMCTS();
    virtual ~StatisticsMCTS();

    void addEventHistory(simtime_t sTimestamp, int iOperatorID, int iHostID,
            double dMsgSize, simtime_t sCompTime, bool bInput);

    void addQueueHistory(simtime_t sTimestamp, int iOperatorID, int iHostID,
            int iQueueSize, simtime_t sCommTime);

    void addHostHistory(simtime_t sTimestamp, int iHostID, int iQueueSize,
            double dMsgSize);

    void addPathHistory(simtime_t sTimestamp, int iPathID, simtime_t sTime,
            simtime_t sComputation, simtime_t sCommunication);

    void addStateHistory(simtime_t sTimestamp, int iOperatorID, int iHostID,
            double dStateSize, simtime_t dRequiredTime);

    /*Operator Statistics*/
    double getOperatorArrivalMsgSize(int iHostID, int iOperatorID,
            simtime_t sBegin, simtime_t sEnd);
    double getOperatorArrivalRate(int iHostID, int iOperatorID,
            simtime_t sBegin, simtime_t sEnd);
    simtime_t getOperatorCompTime(int iHostID, int iOperatorID,
            simtime_t sBegin, simtime_t sEnd);
    double getOperatorQueueSize(int iHostID, int iOperatorID, simtime_t sBegin,
            simtime_t sEnd);
    double getOperatorOutputMsgSize(int iHostID, int iOperatorID,
            simtime_t sBegin, simtime_t sEnd);
    double getOperatorOutputRate(int iHostID, int iOperatorID, simtime_t sBegin,
            simtime_t sEnd);
    double getDataCompressionExpasionFactor(int iHostID, int iOperatorID,
            simtime_t sBegin, simtime_t sEnd);
    double getSelectivity(int iHostID, int iOperatorID, simtime_t sBegin,
            simtime_t sEnd);

    /*Host Statistics*/
    double getHostArrivalMsgSize(int iHostID, simtime_t sBegin, simtime_t sEnd);
    double getHostArrivalRate(int iHostID, simtime_t sBegin, simtime_t sEnd);
    simtime_t getHostCommTime(int iHostID, simtime_t sBegin, simtime_t sEnd);
    double getHostQueueSize(int iHostID, simtime_t sBegin, simtime_t sEnd);

    /*Path Statistics*/
    simtime_t getPathTime(int iPathID, simtime_t sBegin, simtime_t sEnd);
    const simtime_t& getLastSave() const;
    void setLastSave(const simtime_t& lastSave);

    /*State Statistics*/
    double getStateSize(int iHostID, int iOperatorID,
                simtime_t sBegin, simtime_t sEnd);
    double getStateRequireTimeBuildWindow(int iHostID, int iOperatorID,
            simtime_t sBegin, simtime_t sEnd);

    double getPathCompTime(int iPathID, simtime_t sBegin, simtime_t sEnd);
    double getPathCommTime(int iPathID, simtime_t sBegin, simtime_t sEnd);
    vector<state_history*>& getStateHistory();
    void setStateHistory(const vector<state_history*>& stateHistory);
    vector<event_history*>& getEventsHistory();
    void setEventsHistory(const vector<event_history*>& eventsHistory);
    vector<host_history*>& getHostHistory() ;
    void setHostHistory(const vector<host_history*>& hostHistory);
    vector<path_history*>& getPathHistory() ;
    void setPathHistory(const vector<path_history*>& pathHistory);
    vector<queue_history*>& getQueueHistory();
    void setQueueHistory(const vector<queue_history*>& queueHistory);

protected:
    vector<event_history*> cEventsHistory;
    vector<queue_history*> cQueueHistory;
    vector<host_history*> cHostHistory;
    vector<path_history*> cPathHistory;
    vector<state_history*> cStateHistory;
    simtime_t cLastSave = 0;
};

} /* namespace fogstream */

#endif /* RESCHEDULER_MCTS_STATISTICSMCTS_STATISTICSMCTS_H_ */

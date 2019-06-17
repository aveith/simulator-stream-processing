#include "StatisticsMCTS.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include "../../util/Patterns.h"

namespace fogstream {

StatisticsMCTS::StatisticsMCTS() {
    this->getEventsHistory().clear();
    this->getQueueHistory().clear();
    this->getHostHistory().clear();
    this->getPathHistory().clear();
    this->getStateHistory().clear();
}

StatisticsMCTS::~StatisticsMCTS() {
    // TODO Auto-generated destructor stub
}

void StatisticsMCTS::addEventHistory(simtime_t sTimestamp, int iOperatorID,
        int iHostID, double dMsgSize, simtime_t sCompTime, bool bInput) {

    std::vector<event_history*> vFiltered;
    std::copy_if(this->getEventsHistory().begin(),
            this->getEventsHistory().end(), std::back_inserter(vFiltered),
            [&](event_history* &ev) {return ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
    //&& ev->iOperatorID == iOperatorID
    if (vFiltered.size() <= Patterns::STATISTIC_MAX_REGS) {
        this->getEventsHistory().push_back(
                new event_history(sTimestamp, iOperatorID, iHostID, dMsgSize,
                        sCompTime, bInput));
        this->setLastSave(sTimestamp);
    }
}

void StatisticsMCTS::addQueueHistory(simtime_t sTimestamp, int iOperatorID,
        int iHostID, int iQueueSize, simtime_t sCommTime) {
    std::vector<queue_history*> vFiltered;
    std::copy_if(this->cQueueHistory.begin(), this->cQueueHistory.end(),
            std::back_inserter(vFiltered),
            [&](queue_history* &ev) {return ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() <= Patterns::STATISTIC_MAX_REGS) {
        this->getQueueHistory().push_back(
                new queue_history(sTimestamp, iOperatorID, iHostID, iQueueSize,
                        sCommTime));
    }
}

void StatisticsMCTS::addHostHistory(simtime_t sTimestamp, int iHostID,
        int iQueueSize, double dMsgSize) {
    std::vector<host_history*> vFiltered;
    std::copy_if(this->cHostHistory.begin(), this->cHostHistory.end(),
            std::back_inserter(vFiltered),
            [&](host_history* &ev) {return ev->iHostID == iHostID;});

    if (vFiltered.size() <= Patterns::STATISTIC_MAX_REGS) {
        this->getHostHistory().push_back(
                new host_history(sTimestamp, iHostID, iQueueSize, dMsgSize));
    }
}

void StatisticsMCTS::addStateHistory(simtime_t sTimestamp, int iOperatorID,
        int iHostID, double dStateSize, simtime_t dRequiredTime) {
    std::vector<state_history*> vFiltered;
    std::copy_if(this->getStateHistory().begin(), this->getStateHistory().end(),
            std::back_inserter(vFiltered),
            [&](state_history* &ev) {return ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() <= Patterns::STATISTIC_MAX_REGS) {

        this->getStateHistory().push_back(
                new state_history(sTimestamp, iOperatorID, iHostID, dStateSize,
                        dRequiredTime));
    }

}

double StatisticsMCTS::getOperatorArrivalMsgSize(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<event_history*> vFiltered;
    std::copy_if(this->getEventsHistory().begin(),
            this->getEventsHistory().end(), std::back_inserter(vFiltered),
            //            [&](event_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
            [&](event_history* &ev) {return ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
    //&& ev->iOperatorID == iOperatorID
    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                0, // start with first element
                [] (double total, event_history* ev) {return total + ev->dMsgSize;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getOperatorArrivalRate(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<event_history*> vFiltered;
    std::copy_if(this->getEventsHistory().begin(),
            this->getEventsHistory().end(), std::back_inserter(vFiltered),
            //            [&](event_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
            [&](event_history* &ev) {return ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    auto minmax =
            std::minmax_element(vFiltered.begin(), vFiltered.end(),
                    [] (event_history* lhs, event_history* rhs) {return lhs->sTimestamp < rhs->sTimestamp;});

    if (vFiltered.size() > 0) {
        //        cout << "Intial Time: " << SIMTIME_DBL((*minmax.first)->sTimestamp) << " End Time: " << SIMTIME_DBL((*minmax.second)->sTimestamp) << " Size: " << to_string(vFiltered.size()) << endl;
        return vFiltered.size()
                / SIMTIME_DBL(
                        ((*minmax.second)->sTimestamp
                                - (*minmax.first)->sTimestamp));

        //        return vFiltered.size() / (sEnd - sBegin);
    } else {
        return 0;
    }
}

simtime_t StatisticsMCTS::getOperatorCompTime(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<event_history*> vFiltered;
    std::copy_if(this->cEventsHistory.begin(), this->cEventsHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](event_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && !ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
            [&](event_history* &ev) {return !ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (simtime_t) 0, // start with first element
                [] (simtime_t total, event_history* ev) {return total + ev->sCompTime;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getOperatorQueueSize(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<queue_history*> vFiltered;
    std::copy_if(this->cQueueHistory.begin(), this->cQueueHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](queue_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
            [&](queue_history* &ev) {return ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (double) 0, // start with first element
                [] (double total, queue_history* ev) {return total + (double)ev->iQueueSize;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getOperatorOutputMsgSize(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<event_history*> vFiltered;
    std::copy_if(this->cEventsHistory.begin(), this->cEventsHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](event_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && !ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
            [&](event_history* &ev) {return !ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                0, // start with first element
                [] (double total, event_history* ev) {return total + ev->dMsgSize;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getOperatorOutputRate(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<event_history*> vFiltered;
    std::copy_if(this->cEventsHistory.begin(), this->cEventsHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](event_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && !ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});
            [&](event_history* &ev) {return !ev->bInput && ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    auto minmax =
            std::minmax_element(vFiltered.begin(), vFiltered.end(),
                    [] (event_history* &lhs, event_history* &rhs) {return lhs->sTimestamp < rhs->sTimestamp;});

    if (vFiltered.size() > 0) {
        //        return vFiltered.size() / (sEnd - sBegin);
        //        cout << "Operator ID: " << to_string(iOperatorID) << " Range: " << to_string(SIMTIME_DBL((*minmax.second)->sTimestamp - (*minmax.first)->sTimestamp))
        //                << " Messages: " << to_string(vFiltered.size()) << endl;
        return vFiltered.size()
                / SIMTIME_DBL(
                        (*minmax.second)->sTimestamp
                                - (*minmax.first)->sTimestamp);
    } else {
        return 0;
    }
}

double StatisticsMCTS::getHostArrivalMsgSize(int iHostID, simtime_t sBegin,
        simtime_t sEnd) {
    std::vector<host_history*> vFiltered;
    std::copy_if(this->cHostHistory.begin(), this->cHostHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](host_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iHostID == iHostID;});
            [&](host_history* &ev) {return ev->iHostID == iHostID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                0, // start with first element
                [] (double total, host_history* ev) {return total + ev->dMsgSize;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getHostArrivalRate(int iHostID, simtime_t sBegin,
        simtime_t sEnd) {
    std::vector<host_history*> vFiltered;
    std::copy_if(this->cHostHistory.begin(), this->cHostHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](host_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iHostID == iHostID;});
            [&](host_history* &ev) {return ev->iHostID == iHostID;});

    auto minmax =
            std::minmax_element(vFiltered.begin(), vFiltered.end(),
                    [] (host_history* & lhs, host_history*& rhs) {return lhs->sTimestamp < rhs->sTimestamp;});

    if (vFiltered.size() > 0) {
        //        return vFiltered.size() / (sEnd - sBegin);
        return vFiltered.size()
                / SIMTIME_DBL(
                        ((*minmax.second)->sTimestamp
                                - (*minmax.first)->sTimestamp));

    } else {
        return 0;
    }
}

simtime_t StatisticsMCTS::getHostCommTime(int iHostID, simtime_t sBegin,
        simtime_t sEnd) {
    return 0;
}

void StatisticsMCTS::addPathHistory(simtime_t sTimestamp, int iPathID,
        simtime_t sTime, simtime_t sComputation, simtime_t sCommunication) {
    this->getPathHistory().push_back(
            new path_history(sTimestamp, iPathID, sTime, sCommunication,
                    sComputation));

}

double StatisticsMCTS::getHostQueueSize(int iHostID, simtime_t sBegin,
        simtime_t sEnd) {
    std::vector<host_history*> vFiltered;
    std::copy_if(this->cHostHistory.begin(), this->cHostHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](host_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iHostID == iHostID;});
            [&](host_history* &ev) {return ev->iHostID == iHostID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (double) 0, // start with first element
                [] (double total, host_history* ev) {return total + (double)ev->iQueueSize;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

simtime_t StatisticsMCTS::getPathTime(int iPathID, simtime_t sBegin,
        simtime_t sEnd) {
    std::vector<path_history*> vFiltered;
    std::copy_if(this->cPathHistory.begin(), this->cPathHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](path_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iPathID == iPathID;});
            [&](path_history* &ev) {return ev->iPathID == iPathID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (double) 0, // start with first element
                [] (double total, path_history* ev) {return total + SIMTIME_DBL(ev->sTime);})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getPathCommTime(int iPathID, simtime_t sBegin,
        simtime_t sEnd) {
    std::vector<path_history*> vFiltered;
    std::copy_if(this->cPathHistory.begin(), this->cPathHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](path_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iPathID == iPathID;});
            [&](path_history* &ev) {return ev->iPathID == iPathID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (double) 0, // start with first element
                [] (double total, path_history* ev) {return total +SIMTIME_DBL( ev->sCommunicationTime);})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getPathCompTime(int iPathID, simtime_t sBegin,
        simtime_t sEnd) {
    std::vector<path_history*> vFiltered;
    std::copy_if(this->cPathHistory.begin(), this->cPathHistory.end(),
            std::back_inserter(vFiltered),
            //            [&](path_history* &ev) {return ev->sTimestamp >= sBegin && ev->sTimestamp <=sEnd && ev->iPathID == iPathID;});
            [&](path_history* &ev) {return ev->iPathID == iPathID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (double) 0, // start with first element
                [] (double total, path_history* ev) {return total + SIMTIME_DBL(ev->sComputationTime);})
                / vFiltered.size();
    } else {
        return 0;
    }
}

const simtime_t& StatisticsMCTS::getLastSave() const {
    return cLastSave;
}

double StatisticsMCTS::getDataCompressionExpasionFactor(int iHostID,
        int iOperatorID, simtime_t sBegin, simtime_t sEnd) {
    return this->getOperatorOutputMsgSize(iHostID, iOperatorID, sBegin, sEnd)
            / this->getOperatorArrivalMsgSize(iHostID, iOperatorID, sBegin,
                    sEnd);
}

double StatisticsMCTS::getSelectivity(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    return this->getOperatorOutputRate(iHostID, iOperatorID, sBegin, sEnd)
            / this->getOperatorArrivalRate(iHostID, iOperatorID, sBegin, sEnd);
}

void StatisticsMCTS::setLastSave(const simtime_t& lastSave) {
    cLastSave = lastSave;
}

vector<state_history*>& StatisticsMCTS::getStateHistory() {
    return cStateHistory;
}

void StatisticsMCTS::setStateHistory(
        const vector<state_history*>& stateHistory) {
    cStateHistory = stateHistory;
}

vector<event_history*>& StatisticsMCTS::getEventsHistory() {
    return cEventsHistory;
}

void StatisticsMCTS::setEventsHistory(
        const vector<event_history*>& eventsHistory) {
    cEventsHistory = eventsHistory;
}

vector<host_history*>& StatisticsMCTS::getHostHistory() {
    return cHostHistory;
}

void StatisticsMCTS::setHostHistory(const vector<host_history*>& hostHistory) {
    cHostHistory = hostHistory;
}

vector<path_history*>& StatisticsMCTS::getPathHistory() {
    return cPathHistory;
}

void StatisticsMCTS::setPathHistory(const vector<path_history*>& pathHistory) {
    cPathHistory = pathHistory;
}

vector<queue_history*>& StatisticsMCTS::getQueueHistory() {
    return cQueueHistory;
}

void StatisticsMCTS::setQueueHistory(
        const vector<queue_history*>& queueHistory) {
    cQueueHistory = queueHistory;
}

double StatisticsMCTS::getStateSize(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<state_history*> vFiltered;
    std::copy_if(this->getStateHistory().begin(), this->getStateHistory().end(),
            std::back_inserter(vFiltered),
            [&](state_history* &ev) {return ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                0, // start with first element
                [] (double total, state_history* ev) {return total + ev->dStateSize;})
                / vFiltered.size();
    } else {
        return 0;
    }
}

double StatisticsMCTS::getStateRequireTimeBuildWindow(int iHostID, int iOperatorID,
        simtime_t sBegin, simtime_t sEnd) {
    std::vector<state_history*> vFiltered;
    std::copy_if(this->getStateHistory().begin(), this->getStateHistory().end(),
            std::back_inserter(vFiltered),
            [&](state_history* &ev) {return ev->iHostID == iHostID && ev->iOperatorID == iOperatorID;});

    if (vFiltered.size() > 0) {
        return std::accumulate(vFiltered.begin(), vFiltered.end(),
                (double)0, // start with first element
                [] (double total, state_history* ev) {return total + SIMTIME_DBL(ev->dRequiredTime);})
                / vFiltered.size();
    } else {
        return 0;
    }
}
} /* namespace fogstream */


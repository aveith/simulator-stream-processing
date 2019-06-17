#include "Environment.h"
#include <cstdlib>
#include <string>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <cmath>
#include <math.h>
#include "../../util/Patterns.h"

namespace fogstream {

Environment::Environment() {
    // TODO Auto-generated ructor stub

}

Environment::Environment(Env* &generalenv, Parameters*& parameters) {
    this->setGeneralEnv(generalenv);
    this->setReadTraceFile(parameters->isReadFromTracedDataReconfig());
    this->loadTracedFiles(parameters);

    this->SetupDeployableOperators(
            this->getGeneralEnv()->getDeploymentSequence());

}

Environment::Environment(Env* &generalenv, StatisticsMCTS*& historyData) {
    this->setGeneralEnv(generalenv);
    this->setHistoryData(historyData);

    this->SetupDeployableOperators(
            this->getGeneralEnv()->getDeploymentSequence());
}

Environment::~Environment() {
    // TODO Auto-generated destructor stub
}

void Environment::RefreshPathTimes(State* _state) {
    _state->setTotalTimePaths(0);
    for (auto st = _state->getMap().getEstimatedPathTimes().begin();
            st != _state->getMap().getEstimatedPathTimes().end(); ++st) {
        _state->setTotalTimePaths(_state->getTotalTimePaths() + *st);
        //        cout << "Time: " << to_string(SIMTIME_DBL(_state->getTotalTimePaths()))
        //                << endl;
    }
}

void Environment::EstimatePathTimes(vector<double>* pathTimes,
        vector<MapOperator> operatorMappings, vector<MapEdge> edgeMappings) {

    for (unsigned int i = 0;
            i < this->getGeneralEnv()->getApplicationPaths().size(); ++i) {
        double pathTime = 0;

        //        simtime_t comptime = 0;
        //        simtime_t commtime = 0;

        for (unsigned int j = 0;
                j < this->getGeneralEnv()->getApplicationPaths().at(i).size();
                ++j) {

            auto current =
                    std::find_if(operatorMappings.begin(),
                            operatorMappings.end(),
                            [&]( MapOperator &map) {return map.getOperatorId() == this->getGeneralEnv()->getApplicationPaths().at(i).at(j);});

            if (current != operatorMappings.end()) {
                pathTime += current->getEstimateStatistics().getCompTime();
                //                comptime += current->getEstimateStatistics().getCompTime();
            }

            //Communication costs
            if (j > 0) {
                auto previous =
                        std::find_if(operatorMappings.begin(),
                                operatorMappings.end(),
                                [&]( MapOperator &map) {return map.getOperatorId() == this->getGeneralEnv()->getApplicationPaths().at(i).at(j-1);});
                if (previous != operatorMappings.end()) {
                    if (current->getHostId() != previous->getHostId()) {

                        //Calculate the time for transferring using edgeMappings
                        for (auto edgeMapping = edgeMappings.begin();
                                edgeMapping != edgeMappings.end();
                                ++edgeMapping) {
                            if (edgeMapping->getToOperatorId()
                                    == current->getOperatorId()
                                    && edgeMapping->getFromOperatorId()
                                            == previous->getOperatorId()) {

                                auto tsk =
                                        std::find_if(operatorMappings.begin(),
                                                operatorMappings.end(),
                                                [&]( MapOperator &mapping) {return mapping.getOperatorId() == edgeMapping->getFromOperatorId();});

                                if (tsk != operatorMappings.end()) {

                                    auto link =
                                            this->getGeneralEnv()->getLinks().find(
                                                    edgeMapping->getLinkId());

                                    if (link
                                            != this->getGeneralEnv()->getLinks().end()) {

                                        double meanTimeinSystem = 0,
                                                meanTimeinQueue = 0,
                                                meanNumberofMsgsinSystem = 0,
                                                meanNumberinQueue = 0, rho = 0,
                                                firstMeanNumberSystem = 0,
                                                communicationTime = 0;

                                        double serviceRate =
                                                this->getGeneralEnv()->getLinkCapabilities().at(
                                                        link->second->getId()).getBandwidth()
                                                        / (tsk->getEstimateStatistics().getOutputMsgSize()
                                                                * 8);

                                        if (this->getGeneralEnv()->EstimateTimesandQueues(
                                                serviceRate,
                                                tsk->getEstimateStatistics().getOutputRate(),
                                                link->second->getDelay(),
                                                meanTimeinSystem,
                                                meanTimeinQueue,
                                                meanNumberofMsgsinSystem,
                                                meanNumberinQueue, rho)) {

                                            pathTime += meanTimeinSystem;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }

        pathTimes->push_back(pathTime);
    }
}

bool Environment::determineArrivalMetrics(vector<MapOperator>* operatorMappings,
        int operatorId, double& arrivalRate, double& msgSize) {

    for (unsigned int iSource = 0;
            iSource < this->getGeneralEnv()->getSources().size(); iSource++) {
        if (this->getGeneralEnv()->getSources().at(iSource)->getOperatorId()
                == operatorId) {

            arrivalRate =
                    this->getRealStatisticsOperator(
                            this->getGeneralEnv()->getSources().at(iSource)->getHostId(),
                            this->getGeneralEnv()->getSources().at(iSource)->getOperatorId()).getArrivalRate();

            msgSize =
                    this->getRealStatisticsOperator(
                            this->getGeneralEnv()->getSources().at(iSource)->getHostId(),
                            this->getGeneralEnv()->getSources().at(iSource)->getOperatorId()).getArrivalMsgSize();
            return true;
        }
    }

    int iCounter = 0;
    for (unsigned int iPrevious = 0; iPrevious < operatorMappings->size();
            iPrevious++) {
        for (unsigned int iApp = 0;
                iApp < this->getGeneralEnv()->getApplicationTopology().size();
                iApp++) {
            if (this->getGeneralEnv()->getApplicationTopology().at(iApp)->getToOperatorId()
                    == operatorId) {
                if (operatorMappings->at(iPrevious).getOperatorId()
                        == this->getGeneralEnv()->getApplicationTopology().at(
                                iApp)->getFromOperatorId()) {
                    arrivalRate +=
                            operatorMappings->at(iPrevious).getEstimateStatistics().getOutputRate();
                    msgSize +=
                            operatorMappings->at(iPrevious).getEstimateStatistics().getOutputMsgSize();

                    iCounter++;
                }
            }
        }
    }

    if (arrivalRate > 0) {
        msgSize = msgSize / iCounter;
        return true;
    }

    return false;
}

OperatorStatistics Environment::EstimateStatisticsOperator(int hostID,
        int OperatorID, double msgSize, double arrivalRate) {
    OperatorStatistics opeStat;

    //Estimate the new mapping allocation and update the operator and host statistics
    opeStat.setArrivalMsgSize(msgSize);
    opeStat.setArrivalRate(arrivalRate);
    //    opeStat.setRequiredTimeBuildWindow(requiredTime);
    opeStat.setRequiredTimeBuildWindow(0);

    auto ope = this->getGeneralEnv()->getOperators().find(OperatorID);
    if (ope != this->getGeneralEnv()->getOperators().end()) {
        opeStat.setCompRequirement(
                arrivalRate * ope->second->getCPURequirement());
        opeStat.setOutputMsgSize(msgSize * ope->second->getDataTransferRate());
        opeStat.setOutputRate(arrivalRate * ope->second->getSelectivity());
        opeStat.setSelectivity(ope->second->getSelectivity());
        opeStat.setDataCompressionExpasionFactor(
                ope->second->getDataTransferRate());

        opeStat.setStateSize(ope->second->getStateSize());

        auto host = this->getGeneralEnv()->getResources().find(hostID);
        if (host != this->getGeneralEnv()->getResources().end()) {
            double windowTime = 0;
            if (ope->second->getTimeWindow() > 0) {
                double dwindowTime = ope->second->getTimeWindow() / arrivalRate;
                opeStat.setRequiredTimeBuildWindow(dwindowTime);
                //                windowTime = requiredTime;
                //                windowTime = windowTime*.9;
                //                cout << " Operator: " << to_string(OperatorID) << " Value: "
                //                        << to_string(ope->timeWindow) << " Value 1 : "
                //                        << to_string(floor(ope->timeWindow)) << " Value 2 : "
                //                        << to_string(arrivalRate) << " Window Time: "
                //                        << to_string(dwindowTime) << " Window Time (stat): "
                //                        << to_string(opeStat.getRequiredTimeBuildWindow())
                //                        << endl;
            }

            double meanTimeinSystem = 0, meanTimeinQueue = 0,
                    meanNumberofMsgsinSystem = 0, meanNumberinQueue = 0,
                    rho = 0;

            double serviceRate =
                    this->getGeneralEnv()->getHostCapabilities().at(
                            host->second->getId()).getCpu()
                            / ope->second->getCPURequirement();

            if (this->getGeneralEnv()->EstimateTimesandQueues(serviceRate,
                    arrivalRate, windowTime, meanTimeinSystem, meanTimeinQueue,
                    meanNumberofMsgsinSystem, meanNumberinQueue, rho)) {

                opeStat.setCompTime(
                        meanTimeinSystem
                                + opeStat.getRequiredTimeBuildWindow());
                opeStat.setQueueSize(meanNumberofMsgsinSystem);

                opeStat.setMemoryRequirement(
                        (meanNumberofMsgsinSystem * msgSize)
                                + ope->second->getMemoryRequirement());
            }

        }
    }

    return opeStat;
}

OperatorStatistics Environment::getRealStatisticsOperator(int hostID,
        int OperatorID) {

    OperatorStatistics _operator;

    if (!this->isReadTraceFile()) {

        _operator.setHostId(hostID);

        _operator.setOperatorId(OperatorID);

        _operator.setArrivalMsgSize(
                this->getHistoryData()->getOperatorArrivalMsgSize(hostID,
                        OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setArrivalRate(
                this->getHistoryData()->getOperatorArrivalRate(hostID,
                        OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setCompTime(
                SIMTIME_DBL(
                        this->getHistoryData()->getOperatorCompTime(hostID,
                                OperatorID,
                                this->getHistoryData()->getLastSave()
                                        - Patterns::TIME_STATISTIC_EVALUATION,
                                this->getHistoryData()->getLastSave())));

        _operator.setQueueSize(
                this->getHistoryData()->getOperatorQueueSize(hostID, OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setOutputMsgSize(
                this->getHistoryData()->getOperatorOutputMsgSize(hostID,
                        OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setOutputRate(
                this->getHistoryData()->getOperatorOutputRate(hostID,
                        OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setSelectivity(
                this->getHistoryData()->getSelectivity(hostID, OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setDataCompressionExpasionFactor(
                this->getHistoryData()->getDataCompressionExpasionFactor(hostID,
                        OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setStateSize(
                this->getHistoryData()->getStateSize(hostID, OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        _operator.setRequiredTimeBuildWindow(
                this->getHistoryData()->getStateRequireTimeBuildWindow(hostID,
                        OperatorID,
                        this->getHistoryData()->getLastSave()
                                - Patterns::TIME_STATISTIC_EVALUATION,
                        this->getHistoryData()->getLastSave()));

        auto it = this->getGeneralEnv()->getOperators().find(OperatorID);
        if (it != this->getGeneralEnv()->getOperators().end()) {
            _operator.setCompRequirement(
                    _operator.getArrivalRate()
                            * it->second->getCPURequirement());
            _operator.setMemoryRequirement(
                    _operator.getQueueSize() * _operator.getArrivalRate()
                            + it->second->getMemoryRequirement());
        }
    } else {
        auto it =
                std::find_if(this->getOperatorStatistics().begin(),
                        this->getOperatorStatistics().end(),
                        [&](const OperatorStatistics &stat) {return stat.getHostId() ==hostID && stat.getOperatorId()==OperatorID;});
        if (it != this->getOperatorStatistics().end()) {
            _operator = OperatorStatistics(*it);

        }
    }

    return _operator;

}

bool Environment::getAvailablePath(int fromHostID, int toHostID,
        int fromOperatorID, int toOperatorID,
        unordered_map<int, LinkAvailability> &residualLinkCapabilities,
        double msgSize, double arrivaRate, vector<MapEdge> &edgelist) {

    vector<int> path;
    vector<int> links;
    if (this->getGeneralEnv()->getValidPath(fromHostID, toHostID, arrivaRate,
            msgSize, path, links)) {

        if (links.size() > 0) {
            for (unsigned int iLinks = 0; iLinks < links.size(); iLinks++) {

                auto resLink = residualLinkCapabilities.find(links.at(iLinks));

                //Update the residual link capability removing the requirements of the new operator mapping
                if (resLink != residualLinkCapabilities.end()) {
                    residualLinkCapabilities.at(links.at(iLinks)).setAvailableBandwidth(
                            residualLinkCapabilities.at(links.at(iLinks)).getAvailableBandwidth()
                                    - arrivaRate * msgSize);
                } else {
                    auto linkCap =
                            this->getGeneralEnv()->getLinkCapabilities().find(
                                    links.at(iLinks));
                    if (linkCap
                            != this->getGeneralEnv()->getLinkCapabilities().end()) {
                        residualLinkCapabilities.insert(
                                make_pair(links.at(iLinks),
                                        LinkAvailability(links.at(iLinks),
                                                linkCap->second.getBandwidth()
                                                        - arrivaRate
                                                                * msgSize)));
                    }
                }

                for (unsigned int iNet = 0;
                        iNet
                                < this->getGeneralEnv()->getNetworkTopology().size();
                        iNet++) {
                    if (this->getGeneralEnv()->getNetworkTopology().at(iNet)->getLinkId()
                            == links.at(iLinks)) {
                        MapEdge edge(fromOperatorID, toOperatorID,
                                this->getGeneralEnv()->getNetworkTopology().at(
                                        iNet)->getSourceId(),
                                this->getGeneralEnv()->getNetworkTopology().at(
                                        iNet)->getDestinationId(),
                                links.at(iLinks));
                        edgelist.push_back(edge);
                    }
                }

            }

            return false;
        }
    }

    return true;
}

bool Environment::isSourceSink(int operatorId) {
    auto srcIt =
            std::find_if(this->getGeneralEnv()->getSources().begin(),
                    this->getGeneralEnv()->getSources().end(),
                    [&]( DataSource* &src) {return src->getOperatorId() == operatorId;});

    auto skIt =
            std::find_if(this->getGeneralEnv()->getSinks().begin(),
                    this->getGeneralEnv()->getSinks().end(),
                    [&]( DataSink* &sk) {return sk->getOperatorId() == operatorId;});

    return (srcIt != this->getGeneralEnv()->getSources().end()
            || skIt != this->getGeneralEnv()->getSinks().end());
}

StatisticsMCTS*& Environment::getHistoryData() {
    return mHistoryData;
}

void Environment::setHistoryData(StatisticsMCTS*& historyData) {
    mHistoryData = historyData;
}

vector<Action> Environment::getActions(int actionHeuristic,
        Mapping currentMapping, bool root, int levelMCTS) {

    vector<Action> available;
    if (actionHeuristic == Patterns::ActionHeuristics::None) {
        for (unsigned int res = 0;
                res < this->getGeneralEnv()->getDeployableHosts().size();
                ++res) {

            if ((this->getGeneralEnv()->getResources().at(
                    this->getGeneralEnv()->getDeployableHosts().at(res))->getType()
                    != Patterns::DeviceType::Gateway)
                    || (this->getGeneralEnv()->getResources().at(
                            this->getGeneralEnv()->getDeployableHosts().at(res))->getId()
                            == currentMapping.getOperatorMappings().at(
                                    this->GetIndexOperatorMapping(levelMCTS)).getHostId())) {

                Action action(
                        this->getGeneralEnv()->getResources().at(
                                this->getGeneralEnv()->getDeployableHosts().at(
                                        res))->getId(),
                        currentMapping.getOperatorMappings().at(
                                this->GetIndexOperatorMapping(levelMCTS)).getHostId());
                available.push_back(action);

            }
        }
    } else {
        if (root) {
            if (actionHeuristic == Patterns::ActionHeuristics::CPUBased) {
                this->CPUActionHeuristic(available, currentMapping, levelMCTS);

            } else if (actionHeuristic
                    == Patterns::ActionHeuristics::LatencyAware) {
                this->LatencyActionHeuristic(available, currentMapping,
                        levelMCTS);

            }

            for (unsigned int iAction = 0; iAction < available.size();
                    iAction++) {
                this->getRootActions().push_back(
                        Action(available.at(iAction).getHostId(),
                                available.at(iAction).getOriginHostId()));
            }
        } else {
            for (unsigned int iAction = 0;
                    iAction < this->getRootActions().size(); iAction++) {
                available.push_back(
                        Action(this->getRootActions().at(iAction).getHostId(),
                                this->getRootActions().at(iAction).getOriginHostId()));
            }
        }
    }

    return available;
}

void Environment::SimulateTransition(Episode* episode, State* parentState,
        State* state, Action action, int parentId, State* root, bool useSltos,
        bool aggregateReward) {

    episode->setConstraints(vector<int>());
    if (action.getHostId() != action.getOriginHostId()) {

        this->UpdateLinks(state);

        this->UpdateHosts(state, &action);

        bool bConstraint = this->EvaluateLinkConstraints(state);
        if (bConstraint) {
            episode->getConstraints().push_back(
                    Patterns::ConstraintType::Bandwidth);
        }

        this->EstimateOperatorRequirements(state);

        bConstraint = this->EvaluateHostConstraints(state, &action, useSltos);
        if (bConstraint) {
            episode->getConstraints().push_back(Patterns::ConstraintType::CPU);
        }

        this->SummarizeExecution(state, &action);
    }

    //The Metrics include the number of messages sent + received in the cloud,
    //  the sum of bytes that passed through the WAN
    if (aggregateReward) {
        this->UpdateMetricsState(state, root);
    }
    episode->setAction(Action(action));
    episode->setParentState(new State(parentState));
    episode->setParentId(parentId);

}

void Environment::UpdateLinks(State* state) {
    //Update incrementally the edges mapped to links following the operator change
    for (auto it = state->getMap().getEdgeMappings().begin();
            it != state->getMap().getEdgeMappings().end();) {

        if (it->getFromOperatorId()
                == state->getMap().getOperatorMappings().at(
                        this->GetIndexOperatorMapping(state->getLevelMcts())).getOperatorId()
        //                        state->getPosition()).getOperatorId()
                || it->getToOperatorId()
                        == state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getOperatorId()) {
            //                                state->getPosition()).getOperatorId()) {

            //Update the link overviews without the current mapping
            auto link = state->getMap().getResidualLinkCapabilities().find(
                    it->getLinkId());

            if (link != state->getMap().getResidualLinkCapabilities().end()) {
                //Define if the current link is a previous operator for update the residual link capability incrementally
                //Get the size and the rate of output messages from a given operator
                if (it->getToOperatorId()
                        == state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getOperatorId()) {
                    //                                    state->getPosition()).getOperatorId()) {
                    auto prev_ope =
                            find_if(
                                    state->getMap().getOperatorMappings().begin(),
                                    state->getMap().getOperatorMappings().end(),
                                    [&]( MapOperator &ope) {return ope.getOperatorId() == it->getFromOperatorId();});
                    if (prev_ope
                            != state->getMap().getOperatorMappings().end()) {
                        state->getMap().getResidualLinkCapabilities().at(
                                it->getLinkId()).setAvailableBandwidth(
                                state->getMap().getResidualLinkCapabilities().at(
                                        it->getLinkId()).getAvailableBandwidth()
                                        + prev_ope->getEstimateStatistics().getOutputMsgSize()
                                                * 8
                                                * prev_ope->getEstimateStatistics().getOutputRate());
                    }
                }

                if (it->getFromOperatorId()
                        == state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getOperatorId()) {
                    //                                    state->getPosition()).getOperatorId()) {
                    auto next_ope =
                            find_if(
                                    state->getMap().getOperatorMappings().begin(),
                                    state->getMap().getOperatorMappings().end(),
                                    [&]( MapOperator &ope) {return ope.getOperatorId() == it->getToOperatorId();});
                    if (next_ope
                            != state->getMap().getOperatorMappings().end()) {
                        state->getMap().getResidualLinkCapabilities().at(
                                it->getLinkId()).setAvailableBandwidth(
                                state->getMap().getResidualLinkCapabilities().at(
                                        it->getLinkId()).getAvailableBandwidth()
                                        + next_ope->getEstimateStatistics().getOutputMsgSize()
                                                * 8
                                                * next_ope->getEstimateStatistics().getOutputRate());
                    }
                    //                    }
                }
            }
        }

        if (it->getFromOperatorId()
                == state->getMap().getOperatorMappings().at(
                        this->GetIndexOperatorMapping(state->getLevelMcts())).getOperatorId()
        //                        state->getPosition()).getOperatorId()
                || it->getToOperatorId()
                        == state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getOperatorId()) {
            //                                state->getPosition()).getOperatorId()) {
            state->getMap().getEdgeMappings().erase(it);

        } else {
            ++it;
        }

    }
}

void Environment::UpdateHosts(State* state, Action* action) {
    //Update the host overviews without the current mapping
    auto it = state->getMap().getResidualHostCapabilities().find(
            action->getOriginHostId());

    if (it != state->getMap().getResidualHostCapabilities().end()) {
        state->getMap().getResidualHostCapabilities().at(
                action->getOriginHostId()).setAvailableCpu(
                state->getMap().getResidualHostCapabilities().at(
                        action->getOriginHostId()).getAvailableCpu()
                        + state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getEstimateStatistics().getCompRequirement());
        //                                    state->getPosition()).getEstimateStatistics().getCompRequirement());
        state->getMap().getResidualHostCapabilities().at(
                action->getOriginHostId()).setAvailableMemory(
                state->getMap().getResidualHostCapabilities().at(
                        action->getOriginHostId()).getAvailableMemory()
                        + state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getEstimateStatistics().getMemoryRequirement());
        //                                    state->getPosition()).getEstimateStatistics().getMemoryRequirement());

        state->getMap().getResidualHostCapabilities().at(
                action->getOriginHostId()).setAvailableSlots(
                state->getMap().getResidualHostCapabilities().at(
                        action->getOriginHostId()).getAvailableSlots() != -1 ?
                        state->getMap().getResidualHostCapabilities().at(
                                action->getOriginHostId()).getAvailableSlots()
                                + 1 :
                        state->getMap().getResidualHostCapabilities().at(
                                action->getOriginHostId()).getAvailableSlots());
    }

    //Rip of the statistics
    state->getMap().getOperatorMappings().at(
            this->GetIndexOperatorMapping(state->getLevelMcts())).setRealStatistics(
    //            state->getPosition()).setRealStatistics(
            { });
    state->getMap().getOperatorMappings().at(
            this->GetIndexOperatorMapping(state->getLevelMcts())).setEstimateStatistics(
    //            state->getPosition()).setEstimateStatistics(
            { });

    //Update the mapping
    state->getMap().getOperatorMappings().at(
            this->GetIndexOperatorMapping(state->getLevelMcts())).setHostId(
    //            state->getPosition()).setHostId(
            action->getHostId());
}

bool Environment::isLinkConstrained(State* state) {
    bool constraint = false;

    for (unsigned int it = 0;
            it < this->getGeneralEnv()->getApplicationTopology().size(); ++it) {
        bool bConstraint = false;

        if (this->getGeneralEnv()->getApplicationTopology().at(it)->getFromOperatorId()
                == state->getMap().getOperatorMappings().at(
                        this->GetIndexOperatorMapping(state->getLevelMcts())).getOperatorId()) {
            //                        state->getPosition()).getOperatorId()) {
            auto next_ope =
                    find_if(state->getMap().getOperatorMappings().begin(),
                            state->getMap().getOperatorMappings().end(),
                            [&]( MapOperator &ope) {return ope.getOperatorId() == this->getGeneralEnv()->getApplicationTopology().at(it)->getToOperatorId();});
            if (next_ope != state->getMap().getOperatorMappings().end()) {
                if (state->getMap().getOperatorMappings().at(
                        this->GetIndexOperatorMapping(state->getLevelMcts())).getHostId()
                //                        state->getPosition()).getHostId()
                        != next_ope->getHostId()) {
                    bConstraint =
                            this->getAvailablePath(
                                    state->getMap().getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    state->getLevelMcts())).getHostId(), //fromHostID
                                    next_ope->getHostId(), //toHostID
                                    state->getMap().getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    state->getLevelMcts())).getOperatorId(), //fromOperatorID
                                    next_ope->getOperatorId(), //toOperatorID
                                    state->getMap().getResidualLinkCapabilities(),
                                    state->getMap().getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    state->getLevelMcts())).getEstimateStatistics().getOutputMsgSize(),
                                    state->getMap().getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    state->getLevelMcts())).getEstimateStatistics().getOutputRate(),
                                    state->getMap().getEdgeMappings());

                }

                if (bConstraint) {
                    break;
                }
            }
        }

        if (this->getGeneralEnv()->getApplicationTopology().at(it)->getToOperatorId()
                == state->getMap().getOperatorMappings().at(
                        this->GetIndexOperatorMapping(state->getLevelMcts())).getOperatorId()) {
            //                        state->getPosition()).getOperatorId()) {
            auto previous_ope =
                    find_if(state->getMap().getOperatorMappings().begin(),
                            state->getMap().getOperatorMappings().end(),
                            [&]( MapOperator &ope) {return ope.getOperatorId() == this->getGeneralEnv()->getApplicationTopology().at(it)->getFromOperatorId();});
            if (previous_ope->getHostId()
                    != state->getMap().getOperatorMappings().at(
                            this->GetIndexOperatorMapping(
                                    state->getLevelMcts())).getHostId()) {
                //                            state->getPosition()).getHostId()) {
                if (previous_ope
                        != state->getMap().getOperatorMappings().end()) {
                    bConstraint =
                            this->getAvailablePath(previous_ope->getHostId(),
                                    state->getMap().getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    state->getLevelMcts())).getHostId(),
                                    previous_ope->getOperatorId(),
                                    state->getMap().getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    state->getLevelMcts())).getOperatorId(),
                                    state->getMap().getResidualLinkCapabilities(),
                                    previous_ope->getEstimateStatistics().getOutputMsgSize(),
                                    previous_ope->getEstimateStatistics().getOutputRate(),
                                    state->getMap().getEdgeMappings());
                }

                if (bConstraint) {
                    break;
                }
            }
        }
    }

    return constraint;
}
bool Environment::EvaluateLinkConstraints(State* state) {

    return this->isLinkConstrained(state);

}

void Environment::EstimateOperatorRequirements(State* state) {

    //Estimate the arrival rate and the message sizes to determine the CPU requirements
    double arrivalRate = 0;
    double msgSize = 0;
    int count = 0;

    for (unsigned int it = 0;
            it < this->getGeneralEnv()->getApplicationTopology().size(); ++it) {
        if (this->getGeneralEnv()->getApplicationTopology().at(it)->getToOperatorId()
                == state->getMap().getOperatorMappings().at(
                        this->GetIndexOperatorMapping(state->getLevelMcts())).getOperatorId()) {
            //                        state->getPosition()).getOperatorId()) {
            auto previous_ope =
                    find_if(state->getMap().getOperatorMappings().begin(),
                            state->getMap().getOperatorMappings().end(),
                            [&]( MapOperator &ope) {return ope.getOperatorId() == this->getGeneralEnv()->getApplicationTopology().at(it)->getFromOperatorId();});

            if (previous_ope != state->getMap().getOperatorMappings().end()) {
                arrivalRate +=
                        previous_ope->getEstimateStatistics().getOutputRate();
                msgSize +=
                        previous_ope->getEstimateStatistics().getOutputMsgSize();
                count++;

            }
        }
    }
    //The average message size from the different sources
    msgSize = msgSize / count;

    //    cout << "LEVEL: " << state->getLevelMcts() << " index "
    //            << this->GetIndexOperatorMapping(state->getLevelMcts())
    //            << " Operator: "
    //            << to_string(
    //                    state->getMap().getOperatorMappings().at(
    //                            this->GetIndexOperatorMapping(
    //                                    state->getLevelMcts())).getOperatorId())
    //            << endl;

    state->getMap().getOperatorMappings().at(
            this->GetIndexOperatorMapping(state->getLevelMcts())).setEstimateStatistics(
    //            state->getPosition()).setEstimateStatistics(
            this->EstimateStatisticsOperator(
                    state->getMap().getOperatorMappings().at(
                            this->GetIndexOperatorMapping(
                                    state->getLevelMcts())).getHostId(),
                    //                            state->getPosition()).getHostId(),
                    state->getMap().getOperatorMappings().at(
                            this->GetIndexOperatorMapping(
                                    state->getLevelMcts())).getOperatorId(),
                    msgSize,
                    //                            state->getPosition()).getOperatorId(), msgSize,
                    arrivalRate));

}

bool Environment::EvaluateHostConstraints(State* state, Action* action,
        bool useSlots) {
    //Update the host overviews without the current mapping
    auto it = state->getMap().getResidualHostCapabilities().find(
            action->getHostId());
    if (it != state->getMap().getResidualHostCapabilities().end()) {
        state->getMap().getResidualHostCapabilities().at(action->getHostId()).setAvailableCpu(
                state->getMap().getResidualHostCapabilities().at(
                        action->getHostId()).getAvailableCpu()
                        - state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getEstimateStatistics().getCompRequirement());
        //                                    state->getPosition()).getEstimateStatistics().getCompRequirement());
        state->getMap().getResidualHostCapabilities().at(action->getHostId()).setAvailableMemory(
                state->getMap().getResidualHostCapabilities().at(
                        action->getHostId()).getAvailableMemory()
                        - state->getMap().getOperatorMappings().at(
                                this->GetIndexOperatorMapping(
                                        state->getLevelMcts())).getEstimateStatistics().getMemoryRequirement());
        //                                    state->getPosition()).getEstimateStatistics().getMemoryRequirement());

        bool bSlots = true;

        if (useSlots
                && state->getMap().getResidualHostCapabilities().at(
                        action->getHostId()).getAvailableSlots() != -1) {

            state->getMap().getResidualHostCapabilities().at(
                    action->getHostId()).setAvailableSlots(
                    state->getMap().getResidualHostCapabilities().at(
                            action->getHostId()).getAvailableSlots() != -1 ?
                            state->getMap().getResidualHostCapabilities().at(
                                    action->getHostId()).getAvailableSlots()
                                    - 1 :
                            0);
            bSlots = state->getMap().getResidualHostCapabilities().at(
                    action->getHostId()).getAvailableSlots() >= 0;
        }

        return !(state->getMap().getResidualHostCapabilities().at(
                action->getHostId()).getAvailableMemory() >= 0
                && state->getMap().getResidualHostCapabilities().at(
                        action->getHostId()).getAvailableCpu() >= 0 && bSlots);
    }

    return false;
}

void Environment::SummarizeExecution(State* state, Action* action) {
    state->getMap().getEstimatedPathTimes().clear();

    this->EstimatePathTimes(&state->getMap().getEstimatedPathTimes(),
            state->getMap().getOperatorMappings(),
            state->getMap().getEdgeMappings());

    this->RefreshPathTimes(state);
}

bool Environment::isReadTraceFile() const {
    return mReadTraceFile;
}

void Environment::setReadTraceFile(bool readTraceFile) {
    mReadTraceFile = readTraceFile;
}

void Environment::saveTraceFile(Parameters* parameters, vector<int> pathMsgs) {
    string fileDatasetName = parameters->getHashFileName();

    //Operator Statistics
    for (int it = 0; it < this->getGeneralEnv()->getOperatorMapping().size();
            ++it) {
        this->getOperatorStatistics().push_back(
                this->getRealStatisticsOperator(
                        this->getGeneralEnv()->getOperatorMapping().at(it)->getHostId(),
                        this->getGeneralEnv()->getOperatorMapping().at(it)->getOperatorId()));
    }

    this->writeDatasetOperatorStatistics(
            string(parameters->getDirTracedDatasets() + "operatorstatistics"),
            string(
                    parameters->getDirTracedDatasets() + "operatorstatistics/"
                            + fileDatasetName + ".dat"),
            this->getOperatorStatistics());

    //Path Times
    for (unsigned int var = 0;
            var < this->getGeneralEnv()->getApplicationPaths().size(); ++var) {
        this->getRealPathTimes().push_back(
                SIMTIME_DBL(
                        this->getHistoryData()->getPathTime(var,
                                simTime() - Patterns::TIME_STATISTIC_EVALUATION,
                                simTime())));
    }
    this->writeDatasetPathTimes(
            string(parameters->getDirTracedDatasets() + "pathtimes"),
            string(
                    parameters->getDirTracedDatasets() + "pathtimes/"
                            + fileDatasetName + ".dat"),
            this->getRealPathTimes(), pathMsgs);
}

bool Environment::loadTracedFiles(Parameters* parameters) {

    if (parameters->getFileOperatorStatistics() != ""
            || parameters->getFilePathStatistics() != "") {

        if (!this->readDatasetOperatorStatistics(
                string(parameters->getFileOperatorStatistics()),
                this->getOperatorStatistics())) {
            throw cRuntimeError("File %s does not exist!",
                    parameters->getFileOperatorStatistics().c_str());
        }
        //Path Times
        if (!this->readDatasetPathTimes(
                string(parameters->getFilePathStatistics()),
                this->getRealPathTimes(), this->getRealNumberMessagesPath())) {
            throw cRuntimeError("File %s does not exist!",
                    parameters->getFilePathStatistics().c_str());
        }

        if (this->getRealPathTimes().size()
                != this->getGeneralEnv()->getApplicationPaths().size()) {
            throw cRuntimeError(
                    "The input file for path statistics is inconsistent! Input file %d - application paths %d",
                    this->getRealPathTimes().size(),
                    this->getGeneralEnv()->getApplicationPaths().size());
        }

        if (this->getOperatorStatistics().size()
                != this->getGeneralEnv()->getOperators().size()) {
            throw cRuntimeError(
                    "The input file for operator statistics is inconsistent! Input file %d - application %d",
                    this->getOperatorStatistics().size(),
                    this->getGeneralEnv()->getOperators().size());
        }
    } else {

        string fileDatasetName = parameters->getHashFileName();

        //Operator Statistics
        if (!this->readDatasetOperatorStatistics(
                string(
                        parameters->getDirTracedDatasets()
                                + "operatorstatistics/" + fileDatasetName
                                + ".dat"), this->getOperatorStatistics())) {
            throw cRuntimeError("File %s does not exist!",
                    string(
                            parameters->getDirTracedDatasets()
                                    + "operatorstatistics/" + fileDatasetName
                                    + ".dat").c_str());
        }

        //Path Times
        if (!this->readDatasetPathTimes(
                string(
                        parameters->getDirTracedDatasets() + "pathtimes/"
                                + fileDatasetName + ".dat"),
                this->getRealPathTimes(), this->getRealNumberMessagesPath())) {
            throw cRuntimeError("File %s does not exist!",
                    string(
                            parameters->getDirTracedDatasets() + "pathtimes/"
                                    + fileDatasetName + ".dat").c_str());
        }
    }
}

void Environment::writeDatasetOperatorStatistics(string directory,
        string fileName, vector<OperatorStatistics>& datavector) {
    system(string("mkdir -p " + directory).c_str());
    ofstream fileOut(fileName, std::ios::binary);
    for (auto it = datavector.begin(); it != datavector.end(); it++) {
        fileOut << it->toString() << "\n";
    }

    fileOut.close();

}

void Environment::writeDatasetPathTimes(string directory, string fileName,
        vector<double>& datavector, vector<int> pathMsgs) {
    system(string("mkdir -p " + directory).c_str());

    ofstream fileOut(fileName, std::ios::binary);
    for (unsigned int it = 0; it < datavector.size(); it++) {
        fileOut << to_string(datavector.at(it)) << ";"
                << to_string(pathMsgs.at(it)) << "\n";
    }

    fileOut.close();

}

bool Environment::readDatasetOperatorStatistics(string fileName,
        vector<OperatorStatistics>& datavector) {
    struct stat buffer;
    if (stat(fileName.c_str(), &buffer) != 0) {
        return false;
    }

    string line;
    ifstream stream(fileName);

    while (getline(stream, line)) {
        cStringTokenizer tokenizer(line.c_str());
        std::vector<double> array;
        while (tokenizer.hasMoreTokens()) {
            array =
                    cStringTokenizer(tokenizer.nextToken(), ";").asDoubleVector();
        }
        OperatorStatistics newStat((int) array.at(0), (int) array.at(1),
                array.at(2), array.at(3), (double) array.at(4), array.at(5),
                array.at(6), array.at(7), array.at(8), array.at(9),
                array.at(10), array.at(11), array.at(12), array.at(13));
        datavector.push_back(newStat);
    }
    stream.close();

    return true;
}

bool Environment::readDatasetPathTimes(string fileName,
        vector<double>& datavector, vector<long>& numberMsgs) {
    struct stat buffer;
    if (stat(fileName.c_str(), &buffer) != 0) {
        return false;
    }

    string line;
    ifstream stream(fileName);

    while (getline(stream, line)) {
        cStringTokenizer tokenizer(line.c_str());
        std::vector<double> array;
        while (tokenizer.hasMoreTokens()) {
            array =
                    cStringTokenizer(tokenizer.nextToken(), ";").asDoubleVector();
        }
        datavector.push_back(array.at(0));
        numberMsgs.push_back(array.at(1));
    }
    stream.close();
    return true;
}

vector<OperatorStatistics>& Environment::getOperatorStatistics() {
    return mOperatorStatistics;
}

void Environment::setOperatorStatistics(
        const vector<OperatorStatistics>& operatorStatistics) {
    mOperatorStatistics = operatorStatistics;
}

vector<double>& Environment::getRealPathTimes() {
    return mRealPathTimes;
}

void Environment::setRealPathTimes(vector<double>& realPathTimes) {
    mRealPathTimes = realPathTimes;
}

vector<int>& Environment::getDeployableOperators() {
    return mDeployableOperators;
}

void Environment::setDeployableOperators(vector<int>& deployableOperators) {
    mDeployableOperators = deployableOperators;
}

unordered_map<int, int>& Environment::getOperatorMappingIndex() {
    return mOperatorMappingIndex;
}

void Environment::SetupDeployableOperators(vector<int> operators) {
    for (unsigned int i = 0; i < operators.size(); i++) {
        if (!this->isSourceSink(operators.at(i))) {
            this->getDeployableOperators().push_back(operators.at(i));

            int index = 0;
            for (unsigned int j = 0;
                    j < this->getGeneralEnv()->getOperatorMapping().size();
                    j++) {
                if (operators.at(i)
                        == this->getGeneralEnv()->getOperatorMapping().at(j)->getOperatorId()) {
                    index = j;
                    break;
                }
            }

            this->getOperatorMappingIndex().insert(
                    make_pair(operators.at(i), index));
        }
    }
}

int Environment::GetIndexOperatorMapping(int levelMCTS) {
    auto search = this->getOperatorMappingIndex().find(levelMCTS);
    if (search != this->getOperatorMappingIndex().end()) {
        return search->second;
    }

    return -1;
}

int Environment::GetNextOperatorDeployment(int operatorId) {
    int index = -1;
    if (operatorId == numeric_limits<int>::min()) {
        index = 0;

    } else {
        for (unsigned int i = 0; i < this->getDeployableOperators().size();
                i++) {
            if (this->getDeployableOperators().at(i) == operatorId) {
                index = i;
                break;
            }
        }
        if (index > -1) {
            index++;

            if ((unsigned int) index
                    > this->getDeployableOperators().size() - 1) {
                index = -1;
            }
        }
    }

    if (index == -1) {
        return -1;
    } else {
        return this->getDeployableOperators().at(index);
    }
}

void Environment::MetricsNumberofMsgWanTraffic(State* state) {
    double dWanTraffic = 0;
    int iNumberMsgs = 0;
    int iConnectionsWithCloud = 0;
    double dMsgSize = 0;
    for (auto it = state->getMap().getEdgeMappings().begin();
            it != state->getMap().getEdgeMappings().end(); ++it) {

        //Evaluate the source operator from the link mappings
        auto src = this->getGeneralEnv()->getResources().find(
                it->getFromHostId());

        //Evaluate the destination operator from the link mappings
        auto dst = this->getGeneralEnv()->getResources().find(
                it->getToHostId());

        if (src != this->getGeneralEnv()->getResources().end()
                && dst != this->getGeneralEnv()->getResources().end()) {
            //Compute the messages changed between cloud and cloudlets
            if ((src->second->getType() == Patterns::DeviceType::Cloud
                    || dst->second->getType() == Patterns::DeviceType::Cloud)
                    && (src->second->getType() != Patterns::DeviceType::Cloud
                            || dst->second->getType()
                                    != Patterns::DeviceType::Cloud)) {

                if (src->second->getType() == Patterns::DeviceType::Cloud) {
                    //get the operator statistics to include it in the changed messages
                    auto ope =
                            find_if(
                                    state->getMap().getOperatorMappings().begin(),
                                    state->getMap().getOperatorMappings().end(),
                                    [&](MapOperator &map) {return map.getOperatorId() == it->getFromOperatorId();});
                    if (ope != state->getMap().getOperatorMappings().end()) {
                        //messages staring in the cloud with destination the cloudlets
                        iNumberMsgs +=
                                ope->getEstimateStatistics().getOutputRate();
                        dWanTraffic +=
                                ope->getEstimateStatistics().getOutputMsgSize()
                                        * ope->getEstimateStatistics().getOutputRate();

                        dMsgSize +=
                                ope->getEstimateStatistics().getOutputMsgSize();

                        iConnectionsWithCloud++;
                    }
                } else if (dst->second->getType()
                        == Patterns::DeviceType::Cloud) {
                    //get the operator statistics to include it in the changed messages
                    auto ope =
                            find_if(
                                    state->getMap().getOperatorMappings().begin(),
                                    state->getMap().getOperatorMappings().end(),
                                    [&](MapOperator &map) {return map.getOperatorId() == it->getToOperatorId();});
                    if (ope != state->getMap().getOperatorMappings().end()) {
                        //messages staring in the cloudlets with destination the cloud
                        iNumberMsgs +=
                                ope->getEstimateStatistics().getArrivalRate();
                        dWanTraffic +=
                                ope->getEstimateStatistics().getArrivalMsgSize()
                                        * ope->getEstimateStatistics().getArrivalRate();
                        dMsgSize +=
                                ope->getEstimateStatistics().getArrivalMsgSize();
                        iConnectionsWithCloud++;
                    }
                }
            } else if ((src->second->getType() == Patterns::DeviceType::Gateway
                    && dst->second->getType() == Patterns::DeviceType::Gateway)
                    && (src->second->getId() != dst->second->getId())) {
                //get the operator statistics to include it in the changed messages
                auto ope =
                        find_if(state->getMap().getOperatorMappings().begin(),
                                state->getMap().getOperatorMappings().end(),
                                [&](MapOperator &map) {return map.getOperatorId() == it->getFromOperatorId();});
                if (ope != state->getMap().getOperatorMappings().end()) {
                    //messages exchanged between cloudlets
                    dWanTraffic +=
                            ope->getEstimateStatistics().getArrivalMsgSize()
                                    * ope->getEstimateStatistics().getArrivalRate();
                }
            }
        }
    }

    state->setNumberMessagesCloud(iNumberMsgs);
    state->setWanTraffic(dWanTraffic);
    state->setNumberCloudConnections(iConnectionsWithCloud);
    state->setAverageSizeCloudMsgs(dMsgSize / iConnectionsWithCloud);
}

void Environment::MetricMigration(State* state, State* root) {
    //todo - Cardellini T_d
    //    vector<double> mMigrationTimes;
    for (unsigned int i = 0; i < root->getMap().getOperatorMappings().size();
            ++i) {
        if (root->getMap().getOperatorMappings().at(i).getHostId()
                != state->getMap().getOperatorMappings().at(i).getHostId()) {

            state->getMigration().push_back(
                    Migration(
                            root->getMap().getOperatorMappings().at(i).getHostId(),
                            state->getMap().getOperatorMappings().at(i).getHostId(),
                            state->getMap().getOperatorMappings().at(i).getOperatorId()));

            this->getMigration(state->getMap().getOperatorMappings(),
                    &state->getMigration().at(state->getMigration().size() - 1),
                    state->getMap().getOperatorMappings().at(i).getEstimateStatistics().getStateSize());
        }
    }

}

void Environment::setOperatorMappingIndex(
        unordered_map<int, int>& operatorMappingIndex) {
    mOperatorMappingIndex = operatorMappingIndex;
}

void Environment::UpdateMetricsState(State* state, State* root) {
    //Set up the Wan traffic metric and the number of messages exchanged
    //  across cloudlets and cloud to determine the monetary costs
    this->MetricsNumberofMsgWanTraffic(state);

    //Get monetary costs
    state->setMicrosoftPricing(
            this->MircrosoftIOTHubPricing(state->getNumberMessagesCloud(),
                    state->getAverageSizeCloudMsgs(),
                    state->getNumberCloudConnections()));

    state->setAwsPricing(
            this->AWSIOTCorePricing(state->getNumberMessagesCloud(),
                    state->getAverageSizeCloudMsgs(),
                    state->getNumberCloudConnections()));

    if (state != root) {
        //We only evaluate the migrations if the state is not the root,
        // otherwise, the system will have variance between the states
        this->MetricMigration(state, root);
    }

}

void Environment::getMigration(vector<MapOperator> operatorMappings,
        Migration* migration, double stateSize) {

    vector<int> path;
    vector<int> links;

    //Gets the operator code size to include it into the reconfiguration overhead
    double operatorSize = 0;
    auto ope = this->getGeneralEnv()->getOperators().find(
            migration->getOperatorId());
    if (ope != this->getGeneralEnv()->getOperators().end()) {
        operatorSize = ope->second->getMemoryRequirement();
    }

    if (this->getGeneralEnv()->getValidPath(migration->getFromHostId(),
            migration->getToHostId(), 1, operatorSize + stateSize, path,
            links)) {

        for (auto iLink = links.begin(); iLink != links.end(); iLink++) {

            for (unsigned int iConnection = 0;
                    iConnection
                            < this->getGeneralEnv()->getNetworkTopology().size();
                    iConnection++) {
                if (this->getGeneralEnv()->getNetworkTopology().at(iConnection)->getLinkId()
                        == *iLink) {
                    //Update the residual link capability removing the requirements of the new operator mapping
                    migration->getMigrationPlan().push_back(
                            new MigrationPlan(*iLink,
                                    this->getGeneralEnv()->getNetworkTopology().at(
                                            iConnection)->getSourceId(),
                                    this->getGeneralEnv()->getNetworkTopology().at(
                                            iConnection)->getDestinationId(),
                                    (((stateSize + operatorSize) * 8)
                                            / this->getGeneralEnv()->getLinkCapabilities().at(
                                                    *iLink).getBandwidth())
                                            + this->getGeneralEnv()->getLinks().at(
                                                    *iLink)->getDelay()));
                    break;

                }
            }
        }
    } else {
        throw cRuntimeError(
                "Problem to find a valid path to migrate operators!");
    }

}

double Environment::MircrosoftIOTHubPricing(int numberofmessages,
        double msgSize, int connections) { //period will be in milliseconds // msgsize = bytes
    double day = 60 * 60 * 24; ///period always is 1s
    long msgperday = 0;

    //Heart beat messages
    int heartbeat = 20 * 1 * 24 * connections; //the system sends 10 messages to the hub for checking the connection + 10 reply

    int s1 = 0, s2 = 0, s3 = 0;

    //Free tier considers one messages as .5KB
    if (msgSize / 1000 <= .5) {
        msgperday = (ceil(numberofmessages * day) + heartbeat);
    } else {
        msgperday = (ceil(
                ceil(numberofmessages * ceil((msgSize / 1000) / .5)) * day)
                + heartbeat);
    }

    if (msgperday <= 8000) {
        //free of charge
        return 0;

    } else {

        //from Tier 1 4kb refers to a message
        if (msgSize / 1000 <= 4) {
            msgperday = (ceil(numberofmessages * day) + heartbeat);

        } else {
            msgperday = (ceil(
                    ceil(numberofmessages * ceil((msgSize / 1000) / 4)) * day)
                    + heartbeat);
        }

        int msgs = msgperday;
        while (true) {
            if (msgs >= 300000000) {
                msgs -= 300000000;
                s3++;
            } else if (msgs >= 6000000) {
                msgs -= 6000000;
                s2++;
            } else {
                msgs -= 400000;
                s1++;
            }

            if (msgs <= 0) {
                break;
            }

        }

        return 25 * s1 + 250 * s2 + 2500 * s3;
    }

}

double Environment::AWSIOTCorePricing(int numberofmessages, double msgSize,
        int connections) { //period will be in milliseconds
    double day = 60 * 60 * 24;

    long msgperday = 0;

    //Heart beat messages
    int heartbeat = 20 * 1 * 24 * connections; //the system sends 10 messages to the hub for checking the connection + 10 reply

    double minutesConnection = connections * (30 * 24 * 60); //30*24*60  minutes per mont //.08 / 1000000

    if (msgSize / 1000 <= 5) {
        msgperday = ceil(numberofmessages * day) + heartbeat;
    } else {
        msgperday = ceil(
                ceil(numberofmessages * ceil((msgSize / 1000) / 5)) * day)
                + heartbeat;
    }

    if (msgperday * 30 <= 500000 && minutesConnection <= 2250000) {
        return 0;

    } else {
        if (msgperday < 1000000000) {
            return ceil(
                    (msgperday * 1 / 1000000 * 30)
                            + minutesConnection * .08 / 1000000);

        } else if (msgperday >= 1000000000 && msgperday <= 5000000000) {
            int cost = ceil(1000000000 * 1 / 1000000 * 30);
            return ceil(
                    ((msgperday - 1000000000) * .8 / 1000000000 * 30)
                            + minutesConnection * .08 / 1000000) + cost;

        } else {
            int cost = ceil(1000000000 * 1 / 1000000 * 30);
            cost += ceil(5000000000 * .8 / 1000000 * 30);
            return ceil(
                    ((msgperday - 5000000000) * .7 / 1000000000 * 30)
                            + minutesConnection * .08 / 1000000) + cost;
        }
    }

}

Env*& Environment::getGeneralEnv() {
    return mGeneralEnv;
}

unordered_map<int, int> Environment::DetermineNumberUsedHosts(
        vector<MapOperator> mapOpe, int &numberHosts) {

    unordered_map<int, int> hostperSite;
    sort(mapOpe.begin(), mapOpe.end(),
            []( const MapOperator &left, const MapOperator &right )
            {   return ( left.getHostId() < right.getHostId());});

    int iResources = 1;
    for (unsigned int i = 0; i < mapOpe.size(); i++) {
        if (i > 0) {
            if (mapOpe.at(i).getHostId() != mapOpe.at(i - 1).getHostId()) {
                iResources++;
            }
        }

        int iSite =
                this->getGeneralEnv()->getResources().at(
                        mapOpe.at(i).getHostId())->getGatewayId() == -1 ?
                        mapOpe.at(i).getHostId() :
                        this->getGeneralEnv()->getResources().at(
                                mapOpe.at(i).getHostId())->getGatewayId();

        auto site = hostperSite.find(iSite);
        if (site == hostperSite.end()) {
            hostperSite.insert(make_pair(iSite, 1));
        } else {
            hostperSite.at(iSite) = hostperSite.at(iSite) + 1;
        }
    }
    numberHosts = iResources;

    return hostperSite;
}

void Environment::setGeneralEnv(Env*& generanlEnv) {
    mGeneralEnv = generanlEnv;
}

vector<long>& Environment::getRealNumberMessagesPath() {
    return mRealNumberMessagesPath;
}

void Environment::setRealNumberMessagesPath(
        const vector<long>& realNumberMessagesPath) {
    mRealNumberMessagesPath = realNumberMessagesPath;
}

void Environment::CPUActionHeuristic(vector<Action>& available,
        Mapping currentMapping, int levelMCTS) {

    this->addBasicActionsFollowingOperatiorMapping(available, currentMapping,
            levelMCTS);

    int numberCloud = 0;
    //Add cloud resources
    for (unsigned int i = 0;
            i < this->getGeneralEnv()->getCloudServers().size(); i++) {
        if (!this->isComputeResourceAlreadyAvailable(available,
                this->getGeneralEnv()->getCloudServers().at(i))) {
            available.push_back(
                    Action(this->getGeneralEnv()->getCloudServers().at(i),
                            currentMapping.getOperatorMappings().at(
                                    this->GetIndexOperatorMapping(levelMCTS)).getHostId()));
            numberCloud++;

            if (numberCloud == 1)
                break;
        }
    }

    unordered_map<int, ResourceCapability> hostCapacities;

    //Add computing resources from edge sites by evaluating the end-to-end latency
    for (unsigned int i = 0; i < this->getGeneralEnv()->getEdgeDevices().size();
            i++) {
        if (!this->isComputeResourceAlreadyAvailable(available,
                this->getGeneralEnv()->getEdgeDevices().at(i))) {
            if (hostCapacities.find(
                    this->getGeneralEnv()->getResources().at(
                            this->getGeneralEnv()->getEdgeDevices().at(i))->getGatewayId())
                    != hostCapacities.end()) {
                if (hostCapacities.at(
                        this->getGeneralEnv()->getResources().at(
                                this->getGeneralEnv()->getEdgeDevices().at(i))->getGatewayId()).getCpu()
                        < this->getGeneralEnv()->getHostCapabilities().at(
                                this->getGeneralEnv()->getEdgeDevices().at(i)).getCpu()) {
                    hostCapacities.at(
                            this->getGeneralEnv()->getResources().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i))->getGatewayId()).setHostId(
                            this->getGeneralEnv()->getHostCapabilities().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i)).getHostId());
                    hostCapacities.at(
                            this->getGeneralEnv()->getResources().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i))->getGatewayId()).setCpu(
                            this->getGeneralEnv()->getHostCapabilities().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i)).getCpu());

                    hostCapacities.at(
                            this->getGeneralEnv()->getResources().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i))->getGatewayId()).setMemory(
                            this->getGeneralEnv()->getHostCapabilities().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i)).getMemory());

                    hostCapacities.at(
                            this->getGeneralEnv()->getResources().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i))->getGatewayId()).setSlotNumber(
                            this->getGeneralEnv()->getHostCapabilities().at(
                                    this->getGeneralEnv()->getEdgeDevices().at(
                                            i)).getSlotNumber());
                }
            } else {
                hostCapacities.insert(
                        make_pair(
                                this->getGeneralEnv()->getResources().at(
                                        this->getGeneralEnv()->getEdgeDevices().at(
                                                i))->getGatewayId(),
                                ResourceCapability(
                                        this->getGeneralEnv()->getHostCapabilities().at(
                                                this->getGeneralEnv()->getEdgeDevices().at(
                                                        i)).getHostId(),
                                        this->getGeneralEnv()->getHostCapabilities().at(
                                                this->getGeneralEnv()->getEdgeDevices().at(
                                                        i)).getMemory(),
                                        this->getGeneralEnv()->getHostCapabilities().at(
                                                this->getGeneralEnv()->getEdgeDevices().at(
                                                        i)).getCpu(),
                                        this->getGeneralEnv()->getHostCapabilities().at(
                                                this->getGeneralEnv()->getEdgeDevices().at(
                                                        i)).getSlotNumber())));
            }
        }

    }

    int numberEdge = 0;
    for (unsigned int i = 0; i < this->getGeneralEnv()->getGateways().size();
            i++) {
        if (hostCapacities.find(this->getGeneralEnv()->getGateways().at(i))
                != hostCapacities.end()) {
            Action action(
                    hostCapacities.at(
                            this->getGeneralEnv()->getGateways().at(i)).getHostId(),
                    currentMapping.getOperatorMappings().at(
                            this->GetIndexOperatorMapping(levelMCTS)).getHostId());
            available.push_back(action);
            numberEdge++;
            if (numberEdge == 1)
                break;
        }
    }
}

bool Environment::isComputeResourceAlreadyAvailable(vector<Action> available,
        int id) {
    for (unsigned int j = 0; j < available.size(); j++) {
        if (available.at(j).getHostId() == id) {
            return true;
        }
    }

    return false;
}

double Environment::estimateLatencyBetweenOperators(int operatorID,
        int dstResource, Mapping currentMapping) {
    double bytes = 1000, ips = 1000;
    double ltcy = 0;

    if (this->getGeneralEnv()->getPreviousOperators().find(operatorID)
            != this->getGeneralEnv()->getPreviousOperators().end()) {

        for (unsigned int iPrevious = 0;
                iPrevious
                        < this->getGeneralEnv()->getPreviousOperators().at(
                                operatorID).size(); iPrevious++) {

            int from =
                    this->getMappingHost(
                            this->getGeneralEnv()->getPreviousOperators().at(
                                    operatorID).at(iPrevious), currentMapping);

            int fromId = from;
            try {
                if (this->getGeneralEnv()->getResources().at(fromId)->getType()
                        == Patterns::DeviceType::Sensor) {
                    fromId =
                            this->getGeneralEnv()->getResources().at(fromId)->getGatewayId();

                }

            } catch (exception& e) {
                throw cRuntimeError("Estimation error - 0!!");
            }

            auto it = this->getGeneralEnv()->getLinkConnections().find(
                    make_pair(fromId, dstResource));
            try {
                //Determine the latency from one resource to another
                if (it != this->getGeneralEnv()->getLinkConnections().end()) {
                    ltcy += bytes
                            / this->getGeneralEnv()->getLinkCapabilities().at(
                                    it->second).getBandwidth();

                    ltcy +=
                            this->getGeneralEnv()->getLinks().at(it->second)->getDelay();
                }

            } catch (exception& e) {
                throw cRuntimeError(" Estimation error - 1!!");
            }

            try {
                //if exists a gateway in-between, the latency from the gateway to the sensor is included
                if (this->getGeneralEnv()->getResources().at(fromId)->getType()
                        == Patterns::DeviceType::Sensor) {
                    it = this->getGeneralEnv()->getLinkConnections().find(
                            make_pair(from, fromId));

                    if (it
                            != this->getGeneralEnv()->getLinkConnections().end()) {
                        ltcy +=
                                bytes
                                        / this->getGeneralEnv()->getLinkCapabilities().at(
                                                it->second).getBandwidth();

                        ltcy +=
                                this->getGeneralEnv()->getLinks().at(it->second)->getDelay();
                    }
                }
            } catch (exception& e) {
                throw cRuntimeError("Estimation error - 2!!");
            }

            //Include cpu processing time
            ltcy += ips
                    / this->getGeneralEnv()->getHostCapabilities().at(
                            dstResource).getCpu();

        }

    }

    return ltcy;
}

int Environment::getMappingHost(int operatorID, Mapping currentMapping) {
    for (unsigned int i = 0; i < currentMapping.getOperatorMappings().size();
            i++) {
        if (currentMapping.getOperatorMappings().at(i).getOperatorId()
                == operatorID) {
            return currentMapping.getOperatorMappings().at(i).getHostId();
        }
    }

    return -1;
}

void Environment::LatencyActionHeuristic(vector<Action>& available,
        Mapping currentMapping, int levelMCTS) {

    this->addBasicActionsFollowingOperatiorMapping(available, currentMapping,
            levelMCTS);

    int computeId = -1;
    double latency = numeric_limits<double>::max();

    //Add cloud resources
    for (unsigned int i = 0;
            i < this->getGeneralEnv()->getCloudServers().size(); i++) {

        if (!this->isComputeResourceAlreadyAvailable(available,
                this->getGeneralEnv()->getCloudServers().at(i))) {

            double ltcy =
                    this->estimateLatencyBetweenOperators(
                            currentMapping.getOperatorMappings().at(
                                    this->GetIndexOperatorMapping(levelMCTS)).getOperatorId(),
                            this->getGeneralEnv()->getCloudServers().at(i),
                            currentMapping);

            if (ltcy > 0 && ltcy < latency) {
                latency = ltcy;
                computeId = this->getGeneralEnv()->getCloudServers().at(i);
            }

        }
    }

    if (computeId > -1) {
        Action action(computeId,
                currentMapping.getOperatorMappings().at(
                        this->GetIndexOperatorMapping(levelMCTS)).getHostId());
        available.push_back(action);
    }

    //Add the compute resources with short latency on the same site of previous and next operator
    vector<int> gtw;

    for (unsigned int i = 0; i < currentMapping.getOperatorMappings().size();
            i++) {
        for (unsigned int j = 0;
                j < this->getGeneralEnv()->getApplicationTopology().size();
                j++) {
            int hostGtw = -1;

            //Previous
            if (this->getGeneralEnv()->getApplicationTopology().at(j)->getToOperatorId()
                    == currentMapping.getOperatorMappings().at(
                            this->GetIndexOperatorMapping(levelMCTS)).getOperatorId()
                    && this->getGeneralEnv()->getApplicationTopology().at(j)->getFromOperatorId()
                            == currentMapping.getOperatorMappings().at(i).getOperatorId()) {
                if (this->getGeneralEnv()->getResources().at(
                        currentMapping.getOperatorMappings().at(i).getHostId())->getType()
                        == Patterns::DeviceType::Sensor) {
                    hostGtw =
                            this->getGeneralEnv()->getResources().at(
                                    currentMapping.getOperatorMappings().at(i).getHostId())->getGatewayId();
                }
            }

            //Next
            if (this->getGeneralEnv()->getApplicationTopology().at(j)->getFromOperatorId()
                    == currentMapping.getOperatorMappings().at(
                            this->GetIndexOperatorMapping(levelMCTS)).getOperatorId()
                    && this->getGeneralEnv()->getApplicationTopology().at(j)->getToOperatorId()
                            == currentMapping.getOperatorMappings().at(i).getOperatorId()) {
                if (this->getGeneralEnv()->getResources().at(
                        currentMapping.getOperatorMappings().at(i).getHostId())->getType()
                        == Patterns::DeviceType::Sensor) {
                    hostGtw =
                            this->getGeneralEnv()->getResources().at(
                                    currentMapping.getOperatorMappings().at(i).getHostId())->getGatewayId();
                }
            }

            bool exists = false;
            for (unsigned int k = 0; k < gtw.size(); k++) {
                if (gtw.at(k) == hostGtw) {
                    exists = true;
                }
            }

            if (!exists && hostGtw != -1)
                gtw.push_back(hostGtw);
        }
    }

    computeId = -1;
    latency = numeric_limits<double>::max();
    for (unsigned int i = 0; i < gtw.size(); i++) {
        for (unsigned int j = 0;
                j < this->getGeneralEnv()->getGtwDevices().at(gtw.at(i)).size();
                j++) {
            if (!this->isComputeResourceAlreadyAvailable(available,
                    this->getGeneralEnv()->getGtwDevices().at(gtw.at(i)).at(
                            j))) {

                double ltcy =
                        this->estimateLatencyBetweenOperators(
                                currentMapping.getOperatorMappings().at(
                                        this->GetIndexOperatorMapping(
                                                levelMCTS)).getOperatorId(),
                                this->getGeneralEnv()->getGtwDevices().at(
                                        gtw.at(i)).at(j), currentMapping);

                if (ltcy > 0 && ltcy < latency) {
                    latency = ltcy;
                    computeId = this->getGeneralEnv()->getGtwDevices().at(
                            gtw.at(i)).at(j);
                }

            }
        }
    }

    if (computeId > -1) {
        Action action(computeId,
                currentMapping.getOperatorMappings().at(
                        this->GetIndexOperatorMapping(levelMCTS)).getHostId());
        available.push_back(action);
    }

    //Add outside edge device with shortest latency
    computeId = -1;
    latency = numeric_limits<double>::max();
    for (unsigned int i = 0; i < this->getGeneralEnv()->getGateways().size();
            i++) {
        bool exists = false;
        for (unsigned int j = 0; j < gtw.size(); j++) {
            if (gtw.at(j) == this->getGeneralEnv()->getGateways().at(i)) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            for (unsigned int k = 0;
                    k
                            < this->getGeneralEnv()->getGtwDevices().at(
                                    this->getGeneralEnv()->getGateways().at(i)).size();
                    k++) {
                if (!this->isComputeResourceAlreadyAvailable(available,
                        this->getGeneralEnv()->getGtwDevices().at(
                                this->getGeneralEnv()->getGateways().at(i)).at(
                                k))) {

                    double ltcy =
                            this->estimateLatencyBetweenOperators(
                                    currentMapping.getOperatorMappings().at(
                                            this->GetIndexOperatorMapping(
                                                    levelMCTS)).getOperatorId(),
                                    this->getGeneralEnv()->getGtwDevices().at(
                                            this->getGeneralEnv()->getGateways().at(
                                                    i)).at(k), currentMapping);

                    if (ltcy > 0 && ltcy < latency) {
                        latency = ltcy;
                        computeId = this->getGeneralEnv()->getGtwDevices().at(
                                this->getGeneralEnv()->getGateways().at(i)).at(
                                k);
                    }

                }
            }
        }
    }

    if (computeId > -1) {
        Action action(computeId,
                currentMapping.getOperatorMappings().at(
                        this->GetIndexOperatorMapping(levelMCTS)).getHostId());
        available.push_back(action);
    }
}

void Environment::addBasicActionsFollowingOperatiorMapping(
        vector<Action>& available, Mapping currentMapping, int levelMCTS) {

    available.push_back(
            Action(
                    currentMapping.getOperatorMappings().at(
                            this->GetIndexOperatorMapping(levelMCTS)).getHostId(),
                    currentMapping.getOperatorMappings().at(
                            this->GetIndexOperatorMapping(levelMCTS)).getHostId()));

    //Add computing resources of the current operator mappings
    for (unsigned int i = 0; i < currentMapping.getOperatorMappings().size();
            i++) {

        bool exists = false;
        for (unsigned int j = 0; j < available.size(); j++) {
            if (available.at(j).getHostId()
                    == currentMapping.getOperatorMappings().at(i).getHostId()) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            Action action(
                    currentMapping.getOperatorMappings().at(i).getHostId(),
                    currentMapping.getOperatorMappings().at(
                            this->GetIndexOperatorMapping(levelMCTS)).getHostId());
            available.push_back(action);
        }

        //        for (unsigned int j = 0;
        //                j < this->getGeneralEnv()->getApplicationTopology().size();
        //                j++) {
        //            //Get the hosts IDs to
        //            if (this->getGeneralEnv()->getApplicationTopology().at(j)->getFromOperatorId()
        //                    == currentMapping.getOperatorMappings().at(
        //                            this->GetIndexOperatorMapping(levelMCTS)).getOperatorId()) {
        //                for (unsigned int k = 0;
        //                        k < currentMapping.getOperatorMappings().size(); k++) {
        //
        //                    if (currentMapping.getOperatorMappings().at(k).getOperatorId()
        //                            == this->getGeneralEnv()->getApplicationTopology().at(
        //                                    j)->getToOperatorId()) {
        //
        //                        bool exists = false;
        //                        for (unsigned int j = 0; j < available.size(); j++) {
        //                            if (available.at(j).getHostId()
        //                                    == currentMapping.getOperatorMappings().at(
        //                                            k).getHostId()) {
        //                                exists = true;
        //                                break;
        //                            }
        //                        }
        //
        //                        if (!exists) {
        //                            Action action(
        //                                    currentMapping.getOperatorMappings().at(k).getHostId(),
        //                                    currentMapping.getOperatorMappings().at(
        //                                            this->GetIndexOperatorMapping(
        //                                                    levelMCTS)).getHostId());
        //                            available.push_back(action);
        //                        }
        //
        //                        break;
        //                    }
        //                }
        //            }
        //
        //            //Get the hosts IDs from
        //            if (this->getGeneralEnv()->getApplicationTopology().at(j)->getToOperatorId()
        //                    == currentMapping.getOperatorMappings().at(
        //                            this->GetIndexOperatorMapping(levelMCTS)).getOperatorId()) {
        //                for (unsigned int k = 0;
        //                        k < currentMapping.getOperatorMappings().size(); k++) {
        //
        //                    if (currentMapping.getOperatorMappings().at(k).getOperatorId()
        //                            == this->getGeneralEnv()->getApplicationTopology().at(
        //                                    j)->getFromOperatorId()) {
        //
        //                        bool exists = false;
        //                        for (unsigned int j = 0; j < available.size(); j++) {
        //                            if (available.at(j).getHostId()
        //                                    == currentMapping.getOperatorMappings().at(
        //                                            k).getHostId()) {
        //                                exists = true;
        //                                break;
        //                            }
        //                        }
        //
        //                        if (!exists) {
        //                            Action action(
        //                                    currentMapping.getOperatorMappings().at(k).getHostId(),
        //                                    currentMapping.getOperatorMappings().at(
        //                                            this->GetIndexOperatorMapping(
        //                                                    levelMCTS)).getHostId());
        //                            available.push_back(action);
        //                        }
        //
        //                        break;
        //                    }
        //                }
        //            }
        //        }

    }
}

vector<Action>& Environment::getRootActions() {
    return mRootActions;
}

void Environment::setRootActions(const vector<Action>& rootActions) {
    mRootActions = rootActions;
}

}
;

/* namespace fogstream */

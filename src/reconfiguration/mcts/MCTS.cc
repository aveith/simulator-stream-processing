#include "MCTS.h"

#include <omnetpp/simtime.h>
#include <omnetpp/simtime_t.h>
#include <iostream>

#include <fstream>
#include <string>
#include <algorithm>

#include <iostream>
#include <vector>
#include <numeric>
#include <functional>

#include "../mapping/Mapping.h"
#include "../mc_tree/action/Action.h"
#include "../mc_tree/node/Node.h"
#include "../mc_tree/state/State.h"
#include "../migration/Migration.h"

namespace fogstream {
MCTS::MCTS() {

}

MCTS::MCTS(cRNG* rng, Environment* env, LogMCTS*& logMcts) {
    this->setRng(rng);
    this->setEnv(env);
    this->setLogMcts(logMcts);
}

MCTS::~MCTS() {
    // TODO Auto-generated destructor stub
}

cRNG*& MCTS::getRng() {
    return mRNG;
}

void MCTS::setRng(cRNG*& rng) {
    mRNG = rng;
}

void MCTS::execute(int iterations) {
    this->setNumberMsgsApp(
            accumulate(this->getEnv()->getRealNumberMessagesPath().begin(),
                    this->getEnv()->getRealNumberMessagesPath().end(),
                    (long long) 0));

    this->setMcTree(new MC_Tree(this->getEnv()));

    this->getMcTree()->addRoot(this->isUseSlots(),
            this->getLogMcts()->getParameters()->getActionHeuristic());

    this->ParametersCorrection(this->getMcTree()->getTree().at(0));

    this->getMcTree()->getTree().at(0)->setAggregateCost(
            this->CalcAggregateCost(
                    this->getMcTree()->getTree().at(0)->getState()));

    this->UpdateStatistics(0);

    cout << "Real Time: "
            << this->getMcTree()->getTree().at(0)->getState()->getRealTotalTimePaths()
            << " Estimated Time: "
            << this->getMcTree()->getTree().at(0)->getState()->getTotalTimePaths()
            << endl;

    this->getLogMcts()->setInitialTime(high_resolution_clock::now());
    vector<Episode*> episodes;

    //    this->printNodeDetails(0);
    double logTIme = 0;
    for (int i = 0; i < iterations; ++i) {
        duration<double> reconf_duration = high_resolution_clock::now()
                - this->getLogMcts()->getInitialTime();
        if (this->getLogMcts()->getParameters()->getTimeReconfiguration() > 0
                && this->getLogMcts()->getParameters()->getTimeReconfiguration()
                        <= reconf_duration.count()) {
            break;
        }
        episodes.clear();
//                if (i == 29){
//                    int jh = 0;
//                    jh = jh + 1;
//                }

        int nodeId = this->SelectNode();

        if (nodeId > -1) {
            if (this->getEnv()->GetNextOperatorDeployment(
                    this->getMcTree()->getTree().at(nodeId)->getState()->getLevelMcts())
                    != -1) {

                int action = this->ActionPolicy(
                        this->getMcTree()->getTree().at(nodeId)->getState(),
                        nodeId); //Action = HostId

                int nodeTakenAction = this->getNode(nodeId, action);
                episodes.push_back(new Episode());

                this->CreateEpisode(episodes.at(episodes.size() - 1),
                        nodeTakenAction, action, nodeId,
                        this->getMcTree()->getTree().at(nodeId)->getState(),
                        this->getMcTree()->getTree().at(nodeId)->getAggregateCost());
                if (nodeTakenAction == -1) {
                    episodes.at(episodes.size() - 1)->setId(nodeTakenAction);
                }

                if ((this->isIterateToTerminal()
                        && episodes.at(episodes.size() - 1)->getConstraints().size()
                                == 0)) {

                    int iEpisodes = 0;
                    for (unsigned int iOperators = 0;
                            iOperators
                                    < this->getEnv()->getDeployableOperators().size();
                            iOperators++) {
                        if (episodes.at(episodes.size() - 1)->getState()->getLevelMcts()
                                == this->getEnv()->getDeployableOperators().at(
                                        iOperators)) {
                            iEpisodes =
                                    this->getEnv()->getDeployableOperators().size()
                                            - 1 - iOperators;
                            break;
                        }
                    }

                    for (int j = 0; j < iEpisodes; ++j) {
                        if (this->isFirstVisit() && nodeTakenAction == -1) {
                            break;
                        }

                        int actionEpisode = this->ActionPolicy(
                                episodes.at(episodes.size() - 1)->getState(),
                                nodeTakenAction); //Action = HostId

                        nodeTakenAction = this->getNode(
                                episodes.at(episodes.size() - 1)->getId(),
                                actionEpisode);

                        episodes.push_back(new Episode());

                        this->CreateEpisode(episodes.at(episodes.size() - 1),
                                nodeTakenAction, actionEpisode,
                                episodes.at(episodes.size() - 2)->getId(),
                                episodes.at(episodes.size() - 2)->getState(),
                                episodes.at(episodes.size() - 2)->getAggregateCost());

                        if ((episodes.at(episodes.size() - 1)->getConstraints().size()
                                > 0)
                                || (this->isFirstVisit()
                                        && nodeTakenAction == -1)) {
                            break;
                        }
                    }

                }

            }

            int newNode = this->ExpandNode(nodeId, episodes);
            //            if (this->getMcTree()->getTree().at(newNode)->getParentId() != 0){
            //                int isds = 0;
            //                isds = isds +1;
            //            }

            //            cout << newNode << endl;

            this->Rollout(newNode,
                    //                    this->getMcTree()->getTree().at(newNode)->getCumulativeReward(),
                    episodes.at(episodes.size() - 1)->getCumulativeReward(),
                    episodes);

            for (unsigned int iEpisodes = 0; iEpisodes < episodes.size();
                    iEpisodes++) {
                delete episodes.at(iEpisodes);
            }
            //            this->printNodeDetails(newNode);
        } else {
            break;
        }
        high_resolution_clock::time_point timeLog =
                high_resolution_clock::now();
        this->saveLog(i);
        duration<double> iteration_duration = high_resolution_clock::now()
                - timeLog;
        logTIme += iteration_duration.count();

    }
    duration<double> iteration_duration = high_resolution_clock::now()
            - this->getLogMcts()->getInitialTime();
    double reconfigTime = iteration_duration.count() - logTIme;

    cout << "\n\nReal time reconfiguring - Total time: "
            << to_string(reconfigTime) << endl;
    cout << "operator_id\thost_id" << endl;
    for (unsigned int i = 0;
            i
                    < this->getMcTree()->getTree().at(
                            this->getMcTree()->getNodeIdBestLt())->getState()->getMap().getOperatorMappings().size();
            i++) {
        cout
                << to_string(
                        this->getMcTree()->getTree().at(
                                this->getMcTree()->getNodeIdBestLt())->getState()->getMap().getOperatorMappings().at(
                                i).getOperatorId()) << "\t"
                << to_string(
                        this->getMcTree()->getTree().at(
                                this->getMcTree()->getNodeIdBestLt())->getState()->getMap().getOperatorMappings().at(
                                i).getHostId()) << endl;
    }

    //    this->GetTotalMigrationTime(state)
    //Write Log to File
    this->getLogMcts()->writeIterationLog();

    //Write information about operators as well as the initial and final location of placement
    this->getLogMcts()->saveOperatorDeployment(
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState(),
            this->getMcTree()->getTree().at(0)->getState(),
            this->getEnv()->getGeneralEnv()->getOperators());
}

void MCTS::saveLog(int iteration) {
    //Save log
    this->getLogMcts()->saveLogIteration(iteration,
            this->getMcTree()->getTree().at(0)->getState()->getMap().getEstimatedPathTimes().size(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getReward(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getCumulativeReward(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getTotalTimePaths(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getNumberMigrations(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getNumberResources(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getGainRate(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getMap().getEstimatedPathTimes(),
            this->getMcTree()->getTree().at(0)->getState()->getTotalTimePaths(),
            this->getMcTree()->getTree().at(0)->getAggregateCost(),
            this->getMcTree()->getTree().at(0)->getState()->getWanTraffic(),
            this->getMcTree()->getTree().at(0)->getState()->getMicrosoftPricing(),
            this->getMcTree()->getTree().at(0)->getState()->getAwsPricing(),
            this->getMcTree()->getTree().size(),
            this->getMcTree()->getNumberConstraintNodes(),
            this->getMcTree()->getNumberWinNodes(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getTotalTimePaths(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getNumberMigrations(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getNumberResources(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getMap().getRealPathTimes(),
            this->getMcTree()->getBestCumulativeReward(),
            this->getMcTree()->getBestUcb(), this->getMcTree()->getBestQValue(),
            this->getMcTree()->getBestGainRate(),
            std::accumulate(
                    this->getMcTree()->getTree().at(
                            this->getMcTree()->getTree().size() - 1)->getState()->getMap().getRealPathTimes().begin(),
                    this->getMcTree()->getTree().at(
                            this->getMcTree()->getTree().size() - 1)->getState()->getMap().getRealPathTimes().end(),
                    0, [](double a, double b) {return a + b;}),

            this->DefineNumberOperatorsPerSite(
                    this->getMcTree()->getTree().at(
                            this->getMcTree()->getTree().size() - 1)->getState()->getNumberOperatorsSite()),
            this->DefineNumberOperatorsPerSite(
                    this->getMcTree()->getTree().at(
                            this->getMcTree()->getNodeIdBestLt())->getState()->getNumberOperatorsSite()),

            this->getMcTree()->getTree().at(0)->getState()->getRealTotalTimePaths(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getWanTraffic(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getNumberMessagesCloud(),
            this->GetMaxMigrationTime(
                    this->getMcTree()->getTree().at(
                            this->getMcTree()->getTree().size() - 1)->getState()),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getMicrosoftPricing(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getAwsPricing(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getNumberCloudConnections(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getState()->getAverageSizeCloudMsgs(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getTree().size() - 1)->getAggregateCost(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getWanTraffic(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getNumberMessagesCloud(),
            this->GetMaxMigrationTime(
                    this->getMcTree()->getTree().at(
                            this->getMcTree()->getNodeIdBestLt())->getState()),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getMicrosoftPricing(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getAwsPricing(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getNumberCloudConnections(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getState()->getAverageSizeCloudMsgs(),
            this->getMcTree()->getTree().at(
                    this->getMcTree()->getNodeIdBestLt())->getAggregateCost(),
            this->getNumberMsgsApp());
}

int MCTS::SelectNode() {
    return this->TreePolicy();
}

int MCTS::ExpandNode(int nodeId, vector<Episode*> episodes) {
    int iNewNode = -1;
    for (unsigned int i = 0; i < episodes.size(); ++i) {
        bool bCreateNode = false;

        if (this->isCreateAll()) {
            bCreateNode = true;

        } else if (episodes.at(i)->getId() == -1) {
            bCreateNode = true;
        }

        if (bCreateNode) {
            if (episodes.at(i)->getParentId() != -1 && this->isCreateAll()) {
                this->UpdateValues(episodes.at(i)->getParentId(),
                        episodes.at(i), episodes.at(episodes.size() - 1));

            }

            iNewNode = this->getMcTree()->addNode(
                    episodes.at(i)->getParentId(), //ParentID
                    this->getEnv()->GetNextOperatorDeployment(
                            episodes.at(i)->getState()->getLevelMcts()) == -1, //Terminal
                    episodes.at(i)->getConstraints().size() > 0, //Constrained
                    false,
                    episodes.at(i)->getAction(), //TakenAction
                    episodes.at(i)->getState(), //State
                    episodes.at(i)->getCumulativeReward(), //Cumulative Reward
                    episodes.at(i)->getReward(), //Reward
                    episodes.at(i)->getNumberMigrations(), //Number of Migrations
                    episodes.at(i)->getGainRate(), episodes.at(i)->getWin(),
                    episodes.at(i)->getAggregateCost()); //Gain Rate compared to Root

            if (i + 1 < episodes.size() && this->isCreateAll()) {
                episodes.at(i + 1)->setParentId(iNewNode);
            }

            episodes.at(i)->setId(iNewNode);

            this->UpdateValues(iNewNode, episodes.at(i),
                    episodes.at(episodes.size() - 1));

            this->UpdateStatistics(iNewNode);

            if (!this->isCreateAll())
                break;
        }
    }

    if (iNewNode == -1) {
        throw cRuntimeError("Node was not created");
    }

    return iNewNode;

}

void MCTS::Simulate(Episode* episode, int action, State*& stateParent,
        int parentId, bool aggregateReward) {
    episode->setState(new State(stateParent));

    this->getMcTree()->NewState(episode->getState(),
            this->getMcTree()->getAvailableAction(stateParent, action),
            stateParent,
            this->getLogMcts()->getParameters()->getActionHeuristic());

    this->getEnv()->SimulateTransition(episode, stateParent,
            episode->getState(),
            this->getMcTree()->getAvailableAction(stateParent, action),
            parentId, this->getMcTree()->getTree().at(0)->getState(),
            this->isUseSlots(), aggregateReward);

}

MC_Tree*& MCTS::getMcTree() {
    return mMcTree;
}

void MCTS::setMcTree(MC_Tree* mcTree) {
    mMcTree = mcTree;
}

Environment*& MCTS::getEnv() {
    return mEnv;
}

void MCTS::setEnv(Environment*& env) {
    mEnv = env;
}

void MCTS::Rollout(int newNode, double reward, vector<Episode*> episodes) {
    this->DefaultPolicy(this->getMcTree()->getTree().at(newNode)->getParentId(),
            reward, episodes);
}

int MCTS::TreePolicy() {
    if (this->getMcTree()->getTree().at(0)->getState()->getAvailableActions().size()
            > 0) {
        return 0;
    } else {
        return -1;
    }
}

void MCTS::DefaultPolicy(int newNode, double reward,
        vector<Episode*> episodes) {
}

int MCTS::ActionPolicy(State*& state, int nodeId) {
}

bool MCTS::isIterateToTerminal() const {
    return mIterateToTerminal;
}

void MCTS::setIterateToTerminal(bool iterateToTerminal) {
    mIterateToTerminal = iterateToTerminal;
}

int MCTS::ActionBasicUCB1(double constant, int nodeId,
        vector<Action>& availableActions) {
    struct ucb_struc {
        int mHostID;
        double mQUCB = 0;
        ucb_struc(int hostId, double qucb) {
            mHostID = hostId;
            mQUCB = qucb;
        }
    };
    vector<ucb_struc> q_ucb;
    Node* tree = this->getMcTree()->getTree().at(nodeId);

    for (unsigned int i = 0; i < availableActions.size(); ++i) {

        double value;
        auto afterstate = this->getMcTree()->getTree().find(
                this->getNode(nodeId, availableActions.at(i).getHostId()));
        if (afterstate != this->getMcTree()->getTree().end()) {

            if (afterstate->second->isConstrained()) {
                continue;

            } else {
                value = this->CalcUCB1(constant,
                        afterstate->second->getQValue(),
                        afterstate->second->getVisits(), tree->getVisits());
            }
        } else {

            value = numeric_limits<double>::max();
        }

        q_ucb.push_back(ucb_struc(availableActions.at(i).getHostId(), value));
    }

    sort(q_ucb.begin(), q_ucb.end(), []( ucb_struc &left, ucb_struc &right )
    {   return ( left.mQUCB > right.mQUCB);});

    double value_best = q_ucb.at(0).mQUCB;

    vector<int> actions_best;
    for (auto qq = q_ucb.begin(); qq != q_ucb.end(); ++qq) {
        if (qq->mQUCB == value_best)
            actions_best.push_back(qq->mHostID);
    }

    if (actions_best.size() > 1) {
        int iRand = this->getRng()->intRand(actions_best.size());
        return actions_best.at(iRand);
    } else {
        return q_ucb.at(0).mHostID;
    }
}

double MCTS::GetMaxMigrationTime(State* state) {
    //We consider the max migration time because we are following the
    // pause-and-resume approach of operator migrations
    double maxMigrationTime = 0;
    for (unsigned int i = 0; i < state->getMigration().size(); ++i) {

        double migrationTime = 0;
        for (unsigned int j = 0;
                j < state->getMigration().at(i).getMigrationPlan().size();
                ++j) {
            migrationTime += state->getMigration().at(i).getMigrationPlan().at(
                    j)->getRequiredTime();
        }

        if (migrationTime > maxMigrationTime) {
            maxMigrationTime = migrationTime;
        }
    }

    return maxMigrationTime;
}

void MCTS::ParametersCorrection(Node*& node) {
    double base = 1;
    if (this->getLogMcts()->getParameters()->getLatencyParameter() == -1) {
        this->getLogMcts()->getParameters()->setLatencyParameter(
                node->getState()->getTotalTimePaths() * base);
    }

    if (this->getLogMcts()->getParameters()->getWanTrafficParamenter() == -1) {
        this->getLogMcts()->getParameters()->setWanTrafficParamenter(
                node->getState()->getWanTraffic() * base);
    }

    if (this->getLogMcts()->getParameters()->getCostsParameter() == -1) {
        if (this->getLogMcts()->getParameters()->getPricingType()
                == Patterns::PricingType::Microsoft) {
            this->getLogMcts()->getParameters()->setCostsParameter(
                    node->getState()->getMicrosoftPricing() * base);

        } else if (this->getLogMcts()->getParameters()->getPricingType()
                == Patterns::PricingType::AWS) {
            this->getLogMcts()->getParameters()->setCostsParameter(
                    node->getState()->getAwsPricing() * base);

        }
    }

    if (this->getLogMcts()->getParameters()->getMigrationParameter() == -1) {
        //Get the minimal size of the states
        double maxStateSize = std::numeric_limits<double>::min();
        for (auto it = node->getState()->getMap().getOperatorMappings().begin();
                it != node->getState()->getMap().getOperatorMappings().end();
                ++it) {

            //Gets the operator code size to include it into the reconfiguration overhead
            double operatorSize = 0;
            auto ope = this->getEnv()->getGeneralEnv()->getOperators().find(
                    it->getOperatorId());
            if (ope != this->getEnv()->getGeneralEnv()->getOperators().end()) {
                operatorSize = ope->second->getMemoryRequirement();
            }

            if (it->getEstimateStatistics().getStateSize() > 0
                    && (it->getEstimateStatistics().getStateSize()
                            + operatorSize) > maxStateSize) {
                maxStateSize = it->getEstimateStatistics().getStateSize()
                        + operatorSize;
            }
        }

        double bdw = 0;
        double lat = 0;
        for (unsigned int it = 0;
                it
                        < this->getEnv()->getGeneralEnv()->getNetworkTopology().size();
                ++it) {
            if (this->getEnv()->getGeneralEnv()->getResources().at(
                    this->getEnv()->getGeneralEnv()->getNetworkTopology().at(it)->getSourceId())->getType()
                    == Patterns::DeviceType::Cloud) {
                bdw =
                        this->getEnv()->getGeneralEnv()->getLinkCapabilities().at(
                                this->getEnv()->getGeneralEnv()->getNetworkTopology().at(
                                        it)->getLinkId()).getBandwidth();
                lat =
                        this->getEnv()->getGeneralEnv()->getLinks().at(
                                this->getEnv()->getGeneralEnv()->getNetworkTopology().at(
                                        it)->getLinkId())->getDelay();
            }
        }

        if (maxStateSize > 0
                && maxStateSize != std::numeric_limits<double>::min()) {
            this->getLogMcts()->getParameters()->setMigrationParameter(
                    (((maxStateSize * 8) / bdw + lat) * base)
                            + (((maxStateSize * 8) / 100000000 + .0005) * base));

        } else {
            this->getLogMcts()->getParameters()->setMigrationParameter(.5);
        }
    }
}

void MCTS::UpdateValues(int nodeId, Episode*& episode, Episode* lastEpisode) {
}

int MCTS::getNode(int parentId, int hostId) {
    auto it = this->getMcTree()->getChilds().find(make_pair(parentId, hostId));
    if (it != this->getMcTree()->getChilds().end()) {
        return it->second;
    }

    return -1;
}

void MCTS::CreateEpisode(Episode* episode, int nodeId, int action, int parentId,
        State* lastEpisode, double lastAggCost) {

    if (nodeId == -1) {

        this->Simulate(episode, action, lastEpisode, parentId,
                this->getLogMcts()->getParameters()->isAggregateCostBasedReward());

        episode->setAggregateCost(this->CalcAggregateCost(episode->getState()));

        if (this->getLogMcts()->getParameters()->isAggregateCostBasedReward()) {
            episode->setCumulativeReward(
                    this->CalcAggCostBasedReward(episode->getAggregateCost(),
                            this->getMcTree()->getTree().at(0)->getAggregateCost(),
                            episode->getConstraints().size() > 0));
            episode->setReward(
                    this->CalcAggCostBasedReward(episode->getAggregateCost(),
                            lastAggCost, episode->getConstraints().size() > 0));

            episode->setGainRate(
                    1
                            - (episode->getAggregateCost()
                                    / this->getMcTree()->getTree().at(0)->getAggregateCost()));
        } else {
            episode->setCumulativeReward(
                    this->CalcLatencyBasedReward(
                            episode->getState()->getTotalTimePaths(),
                            this->getMcTree()->getTree().at(0)->getState()->getTotalTimePaths(),
                            episode->getConstraints().size() > 0));
            episode->setReward(
                    this->CalcLatencyBasedReward(
                            episode->getState()->getTotalTimePaths(),
                            lastEpisode->getTotalTimePaths(),
                            episode->getConstraints().size() > 0));

            episode->setGainRate(
                    1
                            - (episode->getState()->getTotalTimePaths()
                                    / this->getMcTree()->getTree().at(0)->getState()->getTotalTimePaths()));
        }

        episode->setWin((episode->getCumulativeReward() > 0 ? 1 : 0));

        int numberMigrations = 0;
        for (unsigned int i = 0;
                i
                        < this->getMcTree()->getTree().at(0)->getState()->getMap().getOperatorMappings().size();
                ++i) {
            if (this->getMcTree()->getTree().at(0)->getState()->getMap().getOperatorMappings().at(
                    i).getHostId()
                    != episode->getState()->getMap().getOperatorMappings().at(i).getHostId()) {
                numberMigrations++;
            }
        }
        episode->setNumberMigrations(numberMigrations);

    } else {

        episode->setParentState(
                new State(
                        this->getMcTree()->getTree().at(parentId)->getState()));
        episode->setState(
                new State(this->getMcTree()->getTree().at(nodeId)->getState()));
        episode->setCumulativeReward(
                this->getMcTree()->getTree().at(nodeId)->getCumulativeReward());
        episode->setConstraints(
                vector<int>(
                        this->getMcTree()->getTree().at(nodeId)->getConstraints()));
        episode->setAction(
                Action(
                        this->getMcTree()->getAvailableAction(
                                this->getMcTree()->getTree().at(parentId)->getState(),
                                action)));
        episode->setParentId(
                this->getMcTree()->getTree().at(nodeId)->getParentId());

        episode->setParentId(parentId);
        episode->setId(this->getMcTree()->getTree().at(nodeId)->getId());
        episode->setNumberMigrations(
                this->getMcTree()->getTree().at(nodeId)->getNumberMigrations());
        episode->setGainRate(
                this->getMcTree()->getTree().at(nodeId)->getGainRate());
        episode->setReward(
                this->getMcTree()->getTree().at(nodeId)->getReward());
        episode->setWin(this->getMcTree()->getTree().at(nodeId)->getWin());
        episode->setAggregateCost(
                this->getMcTree()->getTree().at(nodeId)->getAggregateCost());
    }
}

void MCTS::UpdateStatistics(int nodeId) {
    if (this->getMcTree()->getTree().at(nodeId)->isConstrained()) {
        this->getMcTree()->setNumberConstraintNodes(
                this->getMcTree()->getNumberConstraintNodes() + 1);
    }

    //    if (this->getMcTree()->getNode(nodeId).getState()->getTotalTimePaths()
    //            < this->getMcTree()->getNode(this->getMcTree()->getNodeIdBestLt()).getState()->getTotalTimePaths()) {
    //        this->getMcTree()->setNodeIdBestLt(nodeId);
    //    }
    if (!this->getMcTree()->getTree().at(nodeId)->isConstrained()) {
        this->getMcTree()->setNumberWinNodes(
                this->getMcTree()->getNumberWinNodes()
                        + this->getMcTree()->getTree().at(nodeId)->getWin());

        if (this->getMcTree()->getTree().at(nodeId)->getAggregateCost()
                < this->getMcTree()->getTree().at(
                        this->getMcTree()->getNodeIdBestLt())->getAggregateCost()) {
            this->getMcTree()->setNodeIdBestLt(nodeId);
        }

        if (this->getMcTree()->getTree().at(nodeId)->getCumulativeReward()
                > this->getMcTree()->getBestCumulativeReward()) {
            this->getMcTree()->setBestCumulativeReward(
                    this->getMcTree()->getTree().at(nodeId)->getCumulativeReward());
        }

        if (this->getMcTree()->getTree().at(nodeId)->getUctValue()
                > this->getMcTree()->getBestUcb()) {
            this->getMcTree()->setBestUcb(
                    this->getMcTree()->getTree().at(nodeId)->getUctValue());
        }

        if (this->getMcTree()->getTree().at(nodeId)->getQValue()
                > this->getMcTree()->getBestQValue()) {
            this->getMcTree()->setBestQValue(
                    this->getMcTree()->getTree().at(nodeId)->getQValue());
        }

        if (this->getMcTree()->getTree().at(nodeId)->getGainRate()
                > this->getMcTree()->getBestGainRate()) {
            this->getMcTree()->setBestGainRate(
                    this->getMcTree()->getTree().at(nodeId)->getGainRate());
        }
    }
}

double MCTS::CalcLatencyBasedReward(double currentLatency, double baseLatency,
        bool isConstrained) {
    return isConstrained ? -1 : (currentLatency - baseLatency) * -1;
}

double MCTS::CalcAggCostBasedReward(double currentAggCost, double baseAggCost,
        bool isConstrained) {
    return isConstrained ? -.1 : (currentAggCost - baseAggCost) * -1;
}

double MCTS::CalcUCB1(double constant, double qValue, int visitsNode,
        int visitsParent) {
    if (visitsNode > 0 && visitsParent > 0) {
        //
        if (this->isUseRealQValues()) {
            return (qValue)
                    + (constant * sqrt((2 * log(visitsParent)) / (visitsNode)));
        } else {
            return (qValue / visitsNode)
                    + (constant * sqrt((2 * log(visitsParent)) / (visitsNode)));
        }

    } else {

        return qValue;

    }
}

double MCTS::CalcAggregateCost(State* state) {
    //Summarize all migration costs
    double migrationTime = this->GetMaxMigrationTime(state);

    //Load pricing following the input parameter
    double pricing = 0;
    if (this->getLogMcts()->getParameters()->getPricingType()
            == Patterns::PricingType::Microsoft) {
        pricing = state->getMicrosoftPricing();

    } else if (this->getLogMcts()->getParameters()->getPricingType()
            == Patterns::PricingType::AWS) {
        pricing = state->getAwsPricing();

    }

    //    cout << "Latency: "
    //            << to_string((
    //                    state->getTotalTimePaths() == 0
    //                            || this->getLogMcts()->getParameters()->getLatencyParameter()
    //                                    == 0) ? 0 :
    //    this->getLogMcts()->getParameters()->getLatencyWeight()
    //            * (state->getTotalTimePaths()
    //                    / this->getLogMcts()->getParameters()->getLatencyParameter()))
    //
    //                    << " WAN traffic: "
    //                    << to_string((
    //                            state->getWanTraffic() == 0
    //                                    || this->getLogMcts()->getParameters()->getWanTrafficParamenter()
    //                                            == 0) ? 0 :
    //            this->getLogMcts()->getParameters()->getWanTrafficWeight()
    //                    * (state->getWanTraffic()
    //                            / this->getLogMcts()->getParameters()->getWanTrafficParamenter()))
    //                    << " Pricing: "
    //                    << to_string((
    //                            pricing == 0
    //                                    || this->getLogMcts()->getParameters()->getCostsParameter()
    //                                            == 0) ? 0 :
    //            this->getLogMcts()->getParameters()->getCostsWeight()
    //                    * (pricing
    //                            / this->getLogMcts()->getParameters()->getCostsParameter()))
    //                    << " Migration: "
    //                    << to_string((
    //                            migrationTime == 0
    //                                    || this->getLogMcts()->getParameters()->getMigrationParameter()
    //                                            == 0) ?
    //                    0 :
    //                    this->getLogMcts()->getParameters()->getMigrationWeight()
    //                            * (migrationTime
    //                                    / this->getLogMcts()->getParameters()->getMigrationParameter()))
    //                            << endl;

    //Apply the simple additive weighting method based on the loaded weight and parameters
    //from the module scheduler
    return ((state->getTotalTimePaths() == 0
            || this->getLogMcts()->getParameters()->getLatencyParameter() == 0) ?
            0 :
            this->getLogMcts()->getParameters()->getLatencyWeight()
                    * (state->getTotalTimePaths()
                            / this->getLogMcts()->getParameters()->getLatencyParameter())) /* Latency*/
            + ((state->getWanTraffic() == 0
                    || this->getLogMcts()->getParameters()->getWanTrafficParamenter()
                            == 0) ?
                    0 :
                    this->getLogMcts()->getParameters()->getWanTrafficWeight()
                            * (state->getWanTraffic()
                                    / this->getLogMcts()->getParameters()->getWanTrafficParamenter())) /*WAN traffic*/
            + ((pricing == 0
                    || this->getLogMcts()->getParameters()->getCostsParameter()
                            == 0) ?
                    0 :
                    this->getLogMcts()->getParameters()->getCostsWeight()
                            * (pricing
                                    / this->getLogMcts()->getParameters()->getCostsParameter())) /*Monetary Costs*/
            + ((migrationTime == 0
                    || this->getLogMcts()->getParameters()->getMigrationParameter()
                            == 0) ?
                    0 :
                    this->getLogMcts()->getParameters()->getMigrationWeight()
                            * (migrationTime
                                    / this->getLogMcts()->getParameters()->getMigrationParameter())) /*Migration costs*/;
}

bool MCTS::isCreateAll() const {
    return mCreateAll;
}

void MCTS::setCreateAll(bool createAll) {
    mCreateAll = createAll;
}

LogMCTS*& MCTS::getLogMcts() {
    return mLogMCTS;
}

void MCTS::setLogMcts(LogMCTS* logMcts) {
    mLogMCTS = logMcts;
}

bool MCTS::isFirstVisit() const {
    return mFirstVisit;
}

void MCTS::setFirstVisit(bool firstVisit) {
    mFirstVisit = firstVisit;
}

vector<int> MCTS::DefineNumberOperatorsPerSite(
        unordered_map<int, int> sitePlacements) {
    vector<int> counter(
            this->getEnv()->getGeneralEnv()->getCloudServers().size()
                    + this->getEnv()->getGeneralEnv()->getGateways().size(), 0);
    int iPos = 0;
    for (unsigned int i = 0;
            i < this->getEnv()->getGeneralEnv()->getCloudServers().size();
            i++) {
        auto it = sitePlacements.find(
                this->getEnv()->getGeneralEnv()->getCloudServers().at(i));
        if (it != sitePlacements.end()) {
            counter.at(iPos) = it->second;
        }
        iPos++;
    }

    for (unsigned int i = 0;
            i < this->getEnv()->getGeneralEnv()->getGateways().size(); i++) {
        auto it = sitePlacements.find(
                this->getEnv()->getGeneralEnv()->getGateways().at(i));
        if (it != sitePlacements.end()) {
            counter.at(iPos) = it->second;
        }
        iPos++;
    }

    return counter;
}

void MCTS::printNodeDetails(int id) {
    cout << "NODE ID: " << this->getMcTree()->getTree().at(id)->getId() << endl;
    cout << "--------" << this->getMcTree()->getTree().at(id)->toString()
            << endl;
    cout << "--------"
            << this->getMcTree()->getTree().at(id)->getState()->toString()
            << endl;
    cout << "--------"
            << this->getMcTree()->getTree().at(id)->getState()->getMap().toString()
            << endl;
}

void MCTS::printEpisodeDetails(Episode* episode) {
    cout << "EPISODE:" << episode->toString() << endl;

}

long MCTS::getNumberMsgsApp() const {
    return mNumberMsgsApp;
}

void MCTS::setNumberMsgsApp(long numberMsgsApp) {
    mNumberMsgsApp = numberMsgsApp;
}

bool MCTS::isUseSlots() const {
    return mUseSlots;
}

void MCTS::setUseSlots(bool useSlots) {
    mUseSlots = useSlots;
}

bool MCTS::isUseRealQValues() const {
    return mUseRealQValues;
}

void MCTS::setUseRealQValues(bool useRealQValues) {
    mUseRealQValues = useRealQValues;
}

//******************************************************************************************************
//******************LOGMCTS*****************************************************************************
//******************************************************************************************************
LogMCTS::LogMCTS(int type, string filename, Parameters* parameters) {

    string fileStr = filename + ".rlog";

    this->setFileName(fileStr);
    this->setType(type);
    this->setInitialTime(high_resolution_clock::now());
    this->setParameters(parameters);
}

void LogMCTS::saveOperatorDeployment(State*& finalDeployment,
        State*& initialDeployment,
        unordered_map<int, OperatorData*> operators) {
    std::string fileName = this->getParameters()->getDirectoryToSaveFiles()
            + this->getParameters()->getTestId() + ".rfg";

    ofstream ofFile(fileName.c_str());

    //Write Header
    ofFile
            << "test_id;operator_id;selectivity;compression_expansion;window_size;cpu;mem;intial_operator_host_id;"
            << "intial_operator_input_msgs_number;initial_operator_input_msg_size;initial_operator_output_msg_number;"
            << "initial_operator_output_msg_size;initial_state_size;initial_window_time;final_operator_host_id;"
            << "final_operator_input_msgs_number;final_operator_input_msg_size;final_operator_output_msg_number;"
            << "final_operator_output_msg_size;final_state_size;final_window_time;\n";

    //Write document
    for (auto final = finalDeployment->getMap().getOperatorMappings().begin();
            final != finalDeployment->getMap().getOperatorMappings().end();
            final++) {
        for (auto initial =
                initialDeployment->getMap().getOperatorMappings().begin();
                initial
                        != initialDeployment->getMap().getOperatorMappings().end();
                initial++) {

            auto operator_found = operators.find(final->getOperatorId());

            if (final->getOperatorId() == initial->getOperatorId()
                    && operator_found != operators.end()) {
                ofFile << this->getParameters()->getTestId()
                        //test_id
                        << ";" << to_string(initial->getOperatorId())
                        //operator_id
                        << ";"
                        << to_string(
                                1
                                        - initial->getRealStatistics().getSelectivity())
                        //selectivity
                        << ";"
                        << to_string(
                                initial->getRealStatistics().getDataCompressionExpasionFactor())
                        //compression_expansion
                        << ";"
                        << to_string(operator_found->second->getTimeWindow())
                        //window_size
                        << ";"
                        << to_string(
                                operator_found->second->getCPURequirement())
                        //cpu
                        << ";"
                        << to_string(
                                operator_found->second->getMemoryRequirement())
                        //mem
                        << ";" << to_string(initial->getHostId())
                        //intial_operator_host_id
                        << ";"
                        << to_string(
                                initial->getEstimateStatistics().getArrivalRate())
                        //intial_operator_input_msgs_number
                        << ";"
                        << to_string(
                                initial->getEstimateStatistics().getArrivalMsgSize())
                        //initial_operator_input_msg_size
                        << ";"
                        << to_string(
                                initial->getEstimateStatistics().getOutputRate())
                        //initial_operator_output_msg_number
                        << ";"
                        << to_string(
                                initial->getEstimateStatistics().getOutputMsgSize())
                        //initial_operator_output_msg_size
                        << ";"
                        << to_string(
                                initial->getEstimateStatistics().getStateSize())
                        //initial_state_size
                        << ";"
                        << to_string(
                                initial->getRealStatistics().getRequiredTimeBuildWindow())
                        //initial_window_time
                        << ";" << to_string(final->getHostId())
                        //final_operator_host_id
                        << ";"
                        << to_string(
                                final->getEstimateStatistics().getArrivalRate())
                        //final_operator_input_msgs_number
                        << ";"
                        << to_string(
                                final->getEstimateStatistics().getArrivalMsgSize())
                        //final_operator_input_msg_size
                        << ";"
                        << to_string(
                                final->getEstimateStatistics().getOutputRate())
                        //final_operator_output_msg_number
                        << ";"
                        << to_string(
                                final->getEstimateStatistics().getOutputMsgSize())
                        //final_operator_output_msg_size
                        << ";"
                        << to_string(
                                final->getEstimateStatistics().getStateSize())
                        //final_state_size
                        << ";"
                        << to_string(
                                final->getEstimateStatistics().getRequiredTimeBuildWindow()); //final_window_time
                ofFile << "\n";
            }
        }
    }

    ofFile.close();
}

void LogMCTS::writeIterationLog() {
    if (!this->isHeader()) {
        this->writeHeader();
        this->setHeader(true);
    }

    ofstream fileSave(this->getFileName(), ofstream::out | ofstream::app);

    for (auto it = this->getDataIterationLog().begin();
            it != this->getDataIterationLog().end(); ++it) {
        if (Patterns::PRINT_RECONFIGURATION_LOG)
            it->printLogInfo();

        fileSave << this->getLogString() << it->getLogInfoString() << '\n';

    }

    fileSave.close();

    this->getDataIterationLog().clear();
}

void LogMCTS::writeHeader() {

    ofstream ofFile(this->getFileName().c_str());
    cout << this->getFileName() << endl;

    if (this->getType() == Patterns::ExecutionLogType::IterationLog) {
        ofFile
                << "test_id;seed_configuration;strategy;net;app;real_path_times;real_number_msgs_app;bytes_min;bytes_max;arrival_min;arrival_max;data_rate_min;data_rate_max;selectivity_min;selectivity_max;"
                << "iterations;constant;alpha;gamma;lambda;epsilon;reconfig;"

                << "latency_weight;migration_weight;costs_weight;wan_traffic_weight;latency_parameter;migration_parameter;"
                << "costs_parameter;wan_traffic_paramenter;pricing_type;aggregate_cost_based_reward;action_heuristic;"

                << "iteration;time;number_paths;iteration_time;reward;cumulative_reward;latency_time;"
                << "number_migrations;number_resources;gain_rate;";
        ofFile
                << "iteration_wan_traffic;iteration_number_messages_cloud;iteration_migration_costs;iteration_microsoft_pricing;"
                << "iteration_aws_pricing;iteration_number_cloud_connections;iteration_average_size_cloud_msgs;iteration_aggregate_cost;";

        for (int i = 0; i < this->getNumberPaths(); i++) {
            ofFile << "path_" << to_string(i) << "_latency_time;";
        }
        for (int i = 0; i < this->getNumberSites(); i++) {
            ofFile << "site_" << to_string(i) << "_number_operators;";
        }
        ofFile
                << "root_latency_time;root_aggregate_cost;root_wan_traffic;root_microsoft_price;root_aws_price;number_constrained_nodes;number_win_nodes;real_latency_time;best_latency_time;"
                << "best_migrations_number_LT;best_resources_numberLT;";
        for (int i = 0; i < this->getNumberPaths(); i++) {
            ofFile << "best_path_" << to_string(i) << "_latency_time;";
        }
        ofFile << "best_cumulative_reward;best_ucb;best_qvalue;best_gain_rate;";

        for (int i = 0; i < this->getNumberSites(); i++) {
            ofFile << "best_site_" << to_string(i) << "_number_operators;";
        }

        ofFile
                << "best_wan_traffic;best_number_messages_Cloud;best_migration_costs;best_microsoft_pricing;"
                << "best_aws_pricing;best_number_cloud_connections;best_average_size_cloud_msgs;best_aggregate_cost;";
        ofFile << '\n';

    }

    ofFile.close();
}

void LogMCTS::saveLogIteration(int iteration, int numberPaths,
        double iterationReward, double& iterationCumulativeReward,
        double iterationTotalLatencyTime, int iterationNumberMigrations,
        int iterationNumberResources, double iterationGainRate,
        vector<double>& iterationPathTimes, double rootTotalLatencyTime,
        double rootAggregateCost, double rootWanTraffic,
        double rootMircrosoftPrice, double rootAWSPrice,

        int numberTreeNodes, int numberConstraintNodes, int numberWinNodes,
        double bestTotalLatencyTime, int bestNumberMigrationLt,
        int bestNumberResourcesLt, vector<double>& bestPathTimesLt,
        double bestCumulativeReward, double bestUcb, double bestQvalue,
        double bestGainRate, double iterationRealPathTimes,
        vector<int> iterationNumberOperatorsSite,
        vector<int> bestNumberOperatorsSite, double realPathTimes,

        double iterationWANTraffic, int iterationNumberMessagesCloud,
        double iterationMigrationCosts, double iterationMicrosoftPricing,
        double iterationAWSPricing, int iterationNumberCloudConnections,
        double iterationAverageSizeCloudMsgs, double iterationAggregateCost,
        double bestWANTraffic, int bestNumberMessagesCloud,
        double bestMigrationCosts, double bestMicrosoftPricing,
        double bestAWSPricing, int bestNumberCloudConnections,
        double bestAverageSizeCloudMsgs, double bestAggregateCost,
        long realNumberMsgs) {

    high_resolution_clock::time_point time_now = high_resolution_clock::now();

    duration<double, std::milli> elapsedTime = time_now
            - this->getInitialTime();

    duration<double, std::milli> iteration_duration = time_now
            - this->getLastElaspedTime();

    this->setRealPathTimes(realPathTimes);
    this->setNumberMsgsApp(realNumberMsgs);

    this->getDataIterationLog().push_back(
            logInfo(iteration, elapsedTime.count(), numberPaths,
                    iteration_duration.count(), iterationReward,
                    iterationCumulativeReward, iterationTotalLatencyTime,
                    iterationNumberMigrations, iterationNumberResources,
                    iterationGainRate, iterationPathTimes, rootTotalLatencyTime,
                    rootAggregateCost, rootWanTraffic, rootMircrosoftPrice,
                    rootAWSPrice,

                    numberTreeNodes, numberConstraintNodes, numberWinNodes,
                    bestTotalLatencyTime, bestNumberMigrationLt,
                    bestNumberResourcesLt, bestPathTimesLt,
                    bestCumulativeReward, bestUcb, bestQvalue, bestGainRate,
                    iterationRealPathTimes, iterationNumberOperatorsSite,
                    bestNumberOperatorsSite, iterationWANTraffic,
                    iterationNumberMessagesCloud, iterationMigrationCosts,
                    iterationMicrosoftPricing, iterationAWSPricing,
                    iterationNumberCloudConnections,
                    iterationAverageSizeCloudMsgs, iterationAggregateCost,
                    bestWANTraffic, bestNumberMessagesCloud, bestMigrationCosts,
                    bestMicrosoftPricing, bestAWSPricing,
                    bestNumberCloudConnections, bestAverageSizeCloudMsgs,
                    bestAggregateCost));

    this->setLastElaspedTime(time_now);
    if (this->getDataIterationLog().size() >= Patterns::BATCH_SAVE_LOG) {
        this->writeIterationLog();
    }
}

string LogMCTS::getLogString() {
    return this->getParameters()->getTestId() + ";" //test_id
            + to_string(this->getParameters()->getConfigurationSeeds()) + ";" //seed_configuration
            + to_string(this->getParameters()->getBaseStrategy()) + ";" //strategy
            + this->getParameters()->getNetwork() + ";" //net
            + this->getParameters()->getApp() + ";" //app
            + to_string(this->getRealPathTimes()) + ";" //real_path_times
            + to_string(this->getNumberMsgsApp()) + ";" //real_number_msgs_app
            + to_string(this->getParameters()->getBytesMsgMin()) + ";" //bytes_min
            + to_string(this->getParameters()->getBytesMsgMax()) + ";" //bytes_max
            + to_string(this->getParameters()->getArrivalRateMin()) + ";" //arrival_min
            + to_string(this->getParameters()->getArrivalRateMax()) + ";" //arrival_max
            + to_string(this->getParameters()->getDataRateMin()) + ";" //data_rate_min
            + to_string(this->getParameters()->getDataRateMax()) + ";" //data_rate_max
            + to_string(this->getParameters()->getSelectivityMin()) + ";" //selectivity_min
            + to_string(this->getParameters()->getSelectivityMax()) + ";" //selectivity_max
            + to_string(this->getParameters()->getReconfigIterations()) + ";" //iterations
            + to_string(this->getParameters()->getReconfigConstant()) + ";" //constant
            + to_string(this->getParameters()->getReconfigAlpha()) + ";" //alpha
            + to_string(this->getParameters()->getReconfigGamma()) + ";" //gamma
            + to_string(this->getParameters()->getReconfigLambda()) + ";" //lambda
            + to_string(this->getParameters()->getReconfigEpsilon()) + ";" //epsilon
            + to_string(this->getParameters()->getReconfigStrategy()) + ";" //reconfig

            + to_string(this->getParameters()->getLatencyWeight()) + ";" //latency_weight
            + to_string(this->getParameters()->getMigrationWeight()) + ";" //migration_weight
            + to_string(this->getParameters()->getCostsWeight()) + ";" //costs_weight
            + to_string(this->getParameters()->getWanTrafficWeight()) + ";" //wan_traffic_weight
            + to_string(this->getParameters()->getLatencyParameter()) + ";" //latency_parameter
            + to_string(this->getParameters()->getMigrationParameter()) + ";" //migration_parameter
            + to_string(this->getParameters()->getCostsParameter()) + ";" //costs_parameter
            + to_string(this->getParameters()->getWanTrafficParamenter()) + ";" //wan_traffic_paramenter
            + to_string(this->getParameters()->getPricingType()) + ";" //pricing_type
            + to_string(this->getParameters()->isAggregateCostBasedReward()) + ";" //aggregate_cost_based_reward
            + to_string(this->getParameters()->getActionHeuristic()) //Action heuristic
            + ";";
}

vector<logInfo>& LogMCTS::getDataIterationLog() {
    return dataIterationLog;
}

void LogMCTS::setDataIterationLog(const vector<logInfo>& dataIterationLog) {
    this->dataIterationLog = dataIterationLog;
}

const string& LogMCTS::getFileName() const {
    return mFileName;
}

void LogMCTS::setFileName(const string& fileName) {
    mFileName = fileName;
}

bool LogMCTS::isHeader() const {
    return mHeader;
}

void LogMCTS::setHeader(bool header) {
    mHeader = header;
}

const high_resolution_clock::time_point& LogMCTS::getInitialTime() const {
    return mInitialTime;
}

void LogMCTS::setInitialTime(
        const high_resolution_clock::time_point& initialTime) {
    this->setLastElaspedTime(initialTime);
    mInitialTime = initialTime;
}

Parameters*& LogMCTS::getParameters() {
    return mParameters;
}

void LogMCTS::setParameters(Parameters*& parameters) {
    mParameters = parameters;
}

int LogMCTS::getType() const {
    return mType;
}

void LogMCTS::setType(int type) {
    mType = type;
}

int LogMCTS::getNumberPaths() const {
    return mNumberPaths;
}

void LogMCTS::setNumberPaths(int numberPaths) {
    mNumberPaths = numberPaths;
}

int LogMCTS::getNumberSites() const {
    return nNumberSites;
}

void LogMCTS::setNumberSites(int numberSites) {
    nNumberSites = numberSites;
}

const high_resolution_clock::time_point& LogMCTS::getLastElaspedTime() const {
    return mLastElaspedTime;
}

void LogMCTS::setLastElaspedTime(
        const high_resolution_clock::time_point& lastElaspedTime) {
    mLastElaspedTime = lastElaspedTime;
}

long LogMCTS::getNumberMsgsApp() const {
    return mNumberMsgsApp;
}

void LogMCTS::setNumberMsgsApp(long numberMsgsApp) {
    mNumberMsgsApp = numberMsgsApp;
}

//******************************************************************************************************
//******************LOGINFO*****************************************************************************
//******************************************************************************************************
logInfo::logInfo(int iteration, double elapsedTime, int numberPaths,
        double iterationTime, double iterationReward,
        double iterationCumulativeReward, double iterationTotalLatencyTime,
        int iterationNumberMigrations, int iterationNumberResources,
        double iterationGainRate, vector<double> iterationPathTimes,
        double rootTotalLatencyTime, double rootAggregateCost,

        double rootWanTraffic, double rootMircrosoftPrice, double rootAWSPrice,

        int numberTreeNodes, int numberConstraintNodes, int numberWinNodes,
        double bestTotalLatencyTime, int bestNumberMigrationLt,
        int bestNumberResourcesLt, vector<double> bestPathTimesLt,
        double bestCumulativeReward, double bestUcb, double bestQvalue,
        double bestGainRate, double iterationRealPathTimes,
        vector<int> iterationNumberOperatorsSite,
        vector<int> bestNumberOperatorsSite, double iterationWANTraffic,
        int iterationNumberMessagesCloud, double iterationMigrationCosts,
        double iterationMicrosoftPricing, double iterationAWSPricing,
        int iterationNumberCloudConnections,
        double iterationAverageSizeCloudMsgs, double iterationAggregateCost,
        double bestWANTraffic, int bestNumberMessagesCloud,
        double bestMigrationCosts, double bestMicrosoftPricing,
        double bestAWSPricing, int bestNumberCloudConnections,
        double bestAverageSizeCloudMsgs, double bestAggregateCost) {

    this->setIteration(iteration);
    this->setElapsedTime(elapsedTime);
    this->setNumberPaths(numberPaths);
    this->setIterationReward(iterationReward);
    this->setIterationTotalLatencyTime(iterationTotalLatencyTime);
    this->setIterationCumulativeReward(iterationCumulativeReward);
    this->setIterationNumberMigrations(iterationNumberMigrations);
    this->setIterationNumberResources(iterationNumberResources);
    this->setIterationGainRate(iterationGainRate);
    this->setIterationPathTimes(iterationPathTimes);
    this->setRootTotalLatencyTime(rootTotalLatencyTime);
    this->setRootAggregateCost(rootAggregateCost);
    this->setRootWanTraffic(rootWanTraffic);
    this->setRootAwsPrice(rootAWSPrice);
    this->setRootMircrosoftPrice(rootMircrosoftPrice);

    this->setNumberTreeNodes(numberTreeNodes);
    this->setNumberConstraintNodes(numberConstraintNodes);
    this->setNumberWinNodes(numberWinNodes);
    this->setBestTotalLatencyTime(bestTotalLatencyTime);
    this->setBestNumberMigrationLt(bestNumberMigrationLt);
    this->setBestNumberResourcesLt(bestNumberResourcesLt);
    this->setBestPathTimesLt(bestPathTimesLt);
    this->setBestCumulativeReward(bestCumulativeReward);
    this->setBestUcb(bestUcb);
    this->setBestQvalue(bestQvalue);
    this->setBestGainRate(bestGainRate);
    this->setIterationRealPathTimes(iterationRealPathTimes);
    this->setIterationNumberOperatorsSite(iterationNumberOperatorsSite);
    this->setBestNumberOperatorsSite(bestNumberOperatorsSite);
    this->setIterationTime(iterationTime);

    this->setIterationWanTraffic(iterationWANTraffic);
    this->setIterationNumberMessagesCloud(iterationNumberMessagesCloud);
    this->setIterationMigrationCosts(iterationMigrationCosts);
    this->setIterationMicrosoftPricing(iterationMicrosoftPricing);
    this->setIterationAwsPricing(iterationAWSPricing);
    this->setIterationNumberCloudConnections(iterationNumberCloudConnections);
    this->setIterationAverageSizeCloudMsgs(iterationAverageSizeCloudMsgs);
    this->setIterationAggregateCost(iterationAggregateCost);
    this->setBestWanTraffic(bestWANTraffic);
    this->setBestNumberMessagesCloud(bestNumberMessagesCloud);
    this->setBestMigrationCosts(bestMigrationCosts);
    this->setBestMicrosoftPricing(bestMicrosoftPricing);
    this->setBestAwsPricing(bestAWSPricing);
    this->setBestNumberCloudConnections(bestNumberCloudConnections);
    this->setBestAverageSizeCloudMsgs(bestAverageSizeCloudMsgs);
    this->setBestAggregateCost(bestAggregateCost);

}

void logInfo::printLogInfo() {
    cout << "Iteration: " << this->getIteration() << " ElapsedTime: "
            << this->getElapsedTime() << " Reward: "
            << to_string(this->getIterationCumulativeReward())
            << " Total Time Root: "
            << to_string(this->getRootTotalLatencyTime()) << " Best Time : "
            << to_string(this->getBestTotalLatencyTime())
            << " Iteration Latency: "
            << to_string(this->getIterationTotalLatencyTime()) << " Gain Rate: "
            << to_string(this->getIterationGainRate()) << " Iteration Time: "
            << to_string(this->getIterationTime()) << endl;
}

string logInfo::getLogInfoString() {
    string result = to_string(this->getIteration()) + ";"
            + to_string(this->getElapsedTime()) + ";"
            + to_string(this->getNumberPaths()) + ";"
            + to_string(this->getIterationTime()) + ";"
            + to_string(this->getIterationReward()) + ";"
            + to_string(this->getIterationCumulativeReward()) + ";"
            + to_string(this->getIterationTotalLatencyTime()) + ";"
            + to_string(this->getIterationNumberMigrations()) + ";"
            + to_string(this->getIterationNumberResources()) + ";"
            + to_string(this->getIterationGainRate()) + ";";

    result = result + to_string(this->getIterationWanTraffic()) + ";"
            + to_string(this->getIterationNumberMessagesCloud()) + ";"
            + to_string(this->getIterationMigrationCosts()) + ";"
            + to_string(this->getIterationMicrosoftPricing()) + ";"
            + to_string(this->getIterationAwsPricing()) + ";"
            + to_string(this->getIterationNumberCloudConnections()) + ";"
            + to_string(this->getIterationAverageSizeCloudMsgs()) + ";"
            + to_string(this->getIterationAggregateCost()) + ";";

    for (unsigned int i = 0; i < this->getIterationPathTimes().size(); i++) {
        result = result + to_string(this->getIterationPathTimes().at(i)) + ";";

    }

    for (unsigned int i = 0; i < this->getIterationNumberOperatorsSite().size();
            i++) {
        result = result
                + to_string(this->getIterationNumberOperatorsSite().at(i))
                + ";";
    }

    result = result + to_string(this->getRootTotalLatencyTime()) + ";"
            + to_string(this->getRootAggregateCost()) + ";"
            + to_string(this->getRootWanTraffic()) + ";"
            + to_string(this->getRootMircrosoftPrice()) + ";"
            + to_string(this->getRootAwsPrice()) + ";"

            + to_string(this->getNumberConstraintNodes()) + ";"
            + to_string(this->getNumberWinNodes()) + ";"
            + to_string(this->getIterationRealPathTimes()) + ";"
            + to_string(this->getBestTotalLatencyTime()) + ";"
            + to_string(this->getBestNumberMigrationLt()) + ";"
            + to_string(this->getBestNumberResourcesLt()) + ";";

    for (unsigned int i = 0; i < this->getBestPathTimesLt().size(); i++) {
        result = result + to_string(this->getBestPathTimesLt().at(i)) + ";";
    }
    result = result + to_string(this->getBestCumulativeReward()) + ";"
            + to_string(this->getBestUcb()) + ";"
            + to_string(this->getBestQvalue()) + ";"
            + to_string(this->getBestGainRate()) + ";";

    for (unsigned int i = 0; i < this->getBestNumberOperatorsSite().size();
            i++) {
        result = result + to_string(this->getBestNumberOperatorsSite().at(i))
                + ";";
    }

    result = result + to_string(this->getBestWanTraffic()) + ";"
            + to_string(this->getBestNumberMessagesCloud()) + ";"
            + to_string(this->getBestMigrationCosts()) + ";"
            + to_string(this->getBestMicrosoftPricing()) + ";"
            + to_string(this->getBestAwsPricing()) + ";"
            + to_string(this->getBestNumberCloudConnections()) + ";"
            + to_string(this->getBestAverageSizeCloudMsgs()) + ";"
            + to_string(this->getBestAggregateCost()) + ";";

    return result;
}

double logInfo::getBestGainRate() const {
    return mBestGainRate;
}

void logInfo::setBestGainRate(double bestGainRate) {
    mBestGainRate = bestGainRate;
}

int logInfo::getBestNumberMigrationLt() const {
    return mBestNumberMigrationLT;
}

void logInfo::setBestNumberMigrationLt(int bestNumberMigrationLt) {
    mBestNumberMigrationLT = bestNumberMigrationLt;
}

int logInfo::getBestNumberResourcesLt() const {
    return mBestNumberResourcesLT;
}

void logInfo::setBestNumberResourcesLt(int bestNumberResourcesLt) {
    mBestNumberResourcesLT = bestNumberResourcesLt;
}

vector<double>& logInfo::getBestPathTimesLt() {
    return mBestPathTimesLT;
}

void logInfo::setBestPathTimesLt(vector<double> bestPathTimesLt) {
    mBestPathTimesLT = bestPathTimesLt;
}

double logInfo::getBestQvalue() const {
    return mBestQvalue;
}

void logInfo::setBestQvalue(double bestQvalue) {
    mBestQvalue = bestQvalue;
}

double logInfo::getBestCumulativeReward() const {
    return mBestCumulativeReward;
}

void logInfo::setBestCumulativeReward(double bestCumulativeReward) {
    mBestCumulativeReward = bestCumulativeReward;
}

double logInfo::getBestTotalLatencyTime() const {
    return mBestTotalLatencyTime;
}

void logInfo::setBestTotalLatencyTime(double bestTotalLatencyTime) {
    mBestTotalLatencyTime = bestTotalLatencyTime;
}

double logInfo::getBestUcb() const {
    return mBestUCB;
}

void logInfo::setBestUcb(double bestUcb) {
    mBestUCB = bestUcb;
}

double logInfo::getElapsedTime() const {
    return mElapsedTime;
}

void logInfo::setElapsedTime(double elapsedTime) {
    mElapsedTime = elapsedTime;
}

int logInfo::getIteration() const {
    return mIteration;
}

void logInfo::setIteration(int iteration) {
    mIteration = iteration;
}

double logInfo::getIterationCumulativeReward() const {
    return mIterationCumulativeReward;
}

void logInfo::setIterationCumulativeReward(double iterationCumulativeReward) {
    mIterationCumulativeReward = iterationCumulativeReward;
}

double logInfo::getIterationGainRate() const {
    return mIterationGainRate;
}

void logInfo::setIterationGainRate(double iterationGainRate) {
    mIterationGainRate = iterationGainRate;
}

int logInfo::getIterationNumberMigrations() const {
    return mIterationNumberMigrations;
}

void logInfo::setIterationNumberMigrations(int iterationNumberMigrations) {
    mIterationNumberMigrations = iterationNumberMigrations;
}

int logInfo::getIterationNumberResources() const {
    return mIterationNumberResources;
}

void logInfo::setIterationNumberResources(int iterationNumberResources) {
    mIterationNumberResources = iterationNumberResources;
}

vector<double>& logInfo::getIterationPathTimes() {
    return mIterationPathTimes;
}

void logInfo::setIterationPathTimes(vector<double> iterationPathTimes) {
    mIterationPathTimes = iterationPathTimes;
}

double logInfo::getIterationReward() const {
    return mIterationReward;
}

void logInfo::setIterationReward(double iterationReward) {
    mIterationReward = iterationReward;
}

double logInfo::getIterationTotalLatencyTime() const {
    return mIterationTotalLatencyTime;
}

void logInfo::setIterationTotalLatencyTime(double iterationTotalLatencyTime) {
    mIterationTotalLatencyTime = iterationTotalLatencyTime;
}

int logInfo::getNumberConstraintNodes() const {
    return mNumberConstraintNodes;
}

void logInfo::setNumberConstraintNodes(int numberConstraintNodes) {
    mNumberConstraintNodes = numberConstraintNodes;
}

int logInfo::getNumberPaths() const {
    return mNumberPaths;
}

void logInfo::setNumberPaths(int numberPaths) {
    mNumberPaths = numberPaths;
}

int logInfo::getNumberTreeNodes() const {
    return mNumberTreeNodes;
}

void logInfo::setNumberTreeNodes(int numberTreeNodes) {
    mNumberTreeNodes = numberTreeNodes;
}

int logInfo::getNumberWinNodes() const {
    return mNumberWinNodes;
}

void logInfo::setNumberWinNodes(int numberWinNodes) {
    mNumberWinNodes = numberWinNodes;
}

double logInfo::getRootTotalLatencyTime() const {
    return mRootTotalLatencyTime;
}

void logInfo::setRootTotalLatencyTime(double rootTotalLatencyTime) {
    mRootTotalLatencyTime = rootTotalLatencyTime;
}

const vector<int>& logInfo::getBestNumberOperatorsSite() const {
    return mBestNumberOperatorsSite;
}

void logInfo::setBestNumberOperatorsSite(
        const vector<int>& bestNumberOperatorsSite) {
    mBestNumberOperatorsSite = bestNumberOperatorsSite;
}

const vector<int>& logInfo::getIterationNumberOperatorsSite() const {
    return mIterationNumberOperatorsSite;
}

void logInfo::setIterationNumberOperatorsSite(
        const vector<int>& iterationNumberOperatorsSite) {
    mIterationNumberOperatorsSite = iterationNumberOperatorsSite;
}

double logInfo::getIterationRealPathTimes() const {
    return mIterationRealPathTimes;
}

void logInfo::setIterationRealPathTimes(double iterationRealPathTimes) {
    mIterationRealPathTimes = iterationRealPathTimes;
}

double logInfo::getIterationTime() const {
    return mIterationTime;
}

void logInfo::setIterationTime(double iterationTime) {
    mIterationTime = iterationTime;
}

double LogMCTS::getRealPathTimes() const {
    return mRealPathTimes;
}

void LogMCTS::setRealPathTimes(double realPathTimes) {
    mRealPathTimes = realPathTimes;
}

double logInfo::getBestAggregateCost() const {
    return mBestAggregateCost;
}

void logInfo::setBestAggregateCost(double bestAggregateCost) {
    mBestAggregateCost = bestAggregateCost;
}

double logInfo::getBestAverageSizeCloudMsgs() const {
    return mBestAverageSizeCloudMsgs;
}

void logInfo::setBestAverageSizeCloudMsgs(double bestAverageSizeCloudMsgs) {
    mBestAverageSizeCloudMsgs = bestAverageSizeCloudMsgs;
}

double logInfo::getBestAwsPricing() const {
    return mBestAWSPricing;
}

void logInfo::setBestAwsPricing(double bestAwsPricing) {
    mBestAWSPricing = bestAwsPricing;
}

double logInfo::getBestMicrosoftPricing() const {
    return mBestMicrosoftPricing;
}

void logInfo::setBestMicrosoftPricing(double bestMicrosoftPricing) {
    mBestMicrosoftPricing = bestMicrosoftPricing;
}

double logInfo::getBestMigrationCosts() const {
    return mBestMigrationCosts;
}

void logInfo::setBestMigrationCosts(double bestMigrationCosts) {
    mBestMigrationCosts = bestMigrationCosts;
}

int logInfo::getBestNumberCloudConnections() const {
    return mBestNumberCloudConnections;
}

void logInfo::setBestNumberCloudConnections(int bestNumberCloudConnections) {
    mBestNumberCloudConnections = bestNumberCloudConnections;
}

int logInfo::getBestNumberMessagesCloud() const {
    return mBestNumberMessagesCloud;
}

void logInfo::setBestNumberMessagesCloud(int bestNumberMessagesCloud) {
    mBestNumberMessagesCloud = bestNumberMessagesCloud;
}

double logInfo::getBestWanTraffic() const {
    return mBestWANTraffic;
}

void logInfo::setBestWanTraffic(double bestWanTraffic) {
    mBestWANTraffic = bestWanTraffic;
}

double logInfo::getIterationAggregateCost() const {
    return mIterationAggregateCost;
}

void logInfo::setIterationAggregateCost(double iterationAggregateCost) {
    mIterationAggregateCost = iterationAggregateCost;
}

double logInfo::getIterationAverageSizeCloudMsgs() const {
    return mIterationAverageSizeCloudMsgs;
}

void logInfo::setIterationAverageSizeCloudMsgs(
        double iterationAverageSizeCloudMsgs) {
    mIterationAverageSizeCloudMsgs = iterationAverageSizeCloudMsgs;
}

double logInfo::getIterationAwsPricing() const {
    return mIterationAWSPricing;
}

void logInfo::setIterationAwsPricing(double iterationAwsPricing) {
    mIterationAWSPricing = iterationAwsPricing;
}

double logInfo::getIterationMicrosoftPricing() const {
    return mIterationMicrosoftPricing;
}

void logInfo::setIterationMicrosoftPricing(double iterationMicrosoftPricing) {
    mIterationMicrosoftPricing = iterationMicrosoftPricing;
}

double logInfo::getIterationMigrationCosts() const {
    return mIterationMigrationCosts;
}

void logInfo::setIterationMigrationCosts(double iterationMigrationCosts) {
    mIterationMigrationCosts = iterationMigrationCosts;
}

int logInfo::getIterationNumberCloudConnections() const {
    return mIterationNumberCloudConnections;
}

void logInfo::setIterationNumberCloudConnections(
        int iterationNumberCloudConnections) {
    mIterationNumberCloudConnections = iterationNumberCloudConnections;
}

int logInfo::getIterationNumberMessagesCloud() const {
    return mIterationNumberMessagesCloud;
}

void logInfo::setIterationNumberMessagesCloud(
        int iterationNumberMessagesCloud) {
    mIterationNumberMessagesCloud = iterationNumberMessagesCloud;
}

double logInfo::getIterationWanTraffic() const {
    return mIterationWANTraffic;
}

void logInfo::setIterationWanTraffic(double iterationWanTraffic) {
    mIterationWANTraffic = iterationWanTraffic;
}

double logInfo::getRootAggregateCost() const {
    return mRootAggregateCost;
}

void logInfo::setRootAggregateCost(double rootAggregateCost) {
    mRootAggregateCost = rootAggregateCost;
}

double logInfo::getRootAwsPrice() const {
    return mRootAWSPrice;
}

void logInfo::setRootAwsPrice(double rootAwsPrice) {
    mRootAWSPrice = rootAwsPrice;
}

double logInfo::getRootMircrosoftPrice() const {
    return mRootMircrosoftPrice;
}

void logInfo::setRootMircrosoftPrice(double rootMircrosoftPrice) {
    mRootMircrosoftPrice = rootMircrosoftPrice;
}

double logInfo::getRootWanTraffic() const {
    return mRootWanTraffic;
}

void logInfo::setRootWanTraffic(double rootWanTraffic) {
    mRootWanTraffic = rootWanTraffic;
}

} /* namespace fogstream */


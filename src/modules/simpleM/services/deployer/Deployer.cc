#include "Deployer.h"
#include <algorithm>
#include "../../../../util/Patterns.h"
#include "../../../msg/topicEvent/TopicEvent_m.h"
#include "../../../msg/statistic/Statistic_m.h"
#include "../MsgQueue.h"

namespace fogstream {

Define_Module(Deployer);

void Deployer::initialize() {
    // TODO - Generated method body
    this->getParticipatingPaths().clear();
    cModule *mod = getParentModule();
    MsgQueue *queue = check_and_cast<MsgQueue*>(mod);
    if (queue) {
        this->setParticipatingPaths(queue->getParticipatingPaths());
    }
}

void Deployer::handleMessage(cMessage *msg) {
    TopicEvent* topicEvent = dynamic_cast<TopicEvent*>(msg);
    //The Deployer will check all paths that the message crossed including the
    //  messages which were merged on the windows
    for (unsigned int k = 0;
            k < topicEvent->getFirstMGenerationCrossedPath().size(); k++) {

        //Validate the list of operators whose the message crossed over a given path
        int postion = -1;
        for (unsigned int var = 0; var < this->getParticipatingPaths().size();
                ++var) {
            if (this->getParticipatingPaths().at(var)
                    == topicEvent->getCrossedPaths().at(k)) {
                postion = var;
            }

        }

        bool bFind = false;
        int gate = -1;
        if (postion > -1) {
            if (this->getSentRegs() <= Patterns::STATISTIC_MAX_REGS) {
                Statistic* pathStat = new Statistic();
                pathStat->setType(Patterns::StatisticType::Path);
                pathStat->setPathID(postion);
                pathStat->setSTimestamp(simTime());
                pathStat->setKind(Patterns::MessageType::Statistics);

                pathStat->setPathTime(
                        simTime()
                                - topicEvent->getFirstMGenerationCrossedPath().at(
                                        k));
                //            cout << "Path Time : " << SIMTIME_DBL(pathStat->getPathTime()) << endl;
                pathStat->setCommTime(
                        topicEvent->getTotalComm()
                                + (simTime() - topicEvent->getLastComm()));
                pathStat->setCompTime(topicEvent->getTotalComp());

                this->setSentRegs(this->getSentRegs() + 1);

                send(pathStat, "out", 0);
            }

            string mname = "path-" + to_string(postion);

            for (int i = 0; i < this->gateSize("out"); ++i) {
                if (opp_strcmp(mname.c_str(),
                        this->gate("out", i)->getNextGate()->getOwnerModule()->getName())
                        == 0) {
                    gate = i;
                    bFind = true;
                    break;
                }
            }

            if (!bFind) {
                gate = this->createSpecificModule(Patterns::NAME_STORAGE,
                        postion);
            }

            if (gate > -1) {
                TopicEvent* tp = topicEvent->dup();
                send(tp, "out", gate);
            } else {
                throw cRuntimeError("The Storage module was not created!");
            }
        } else {
            throw cRuntimeError("The message does not match a path");
        }
    }

    delete topicEvent;
}

int Deployer::createSpecificModule(const char* moduletype, int path) {
    cModule* module;
    cModuleType *mType = cModuleType::get(moduletype);

    if (mType) {
        string name = "path-" + to_string(path);

        module = mType->create(name.c_str(), this->getParentModule());

        if (module) {
            module->buildInside();
            //module->callInitialize();
            module->scheduleStart(simTime());

            module->setGateSize("in", module->gateSize("in") + 1);

            this->setGateSize("out", this->gateSize("out") + 1);

            this->gate("out", this->gateSize("out") - 1)->connectTo(
                    module->gate("in", module->gateSize("in") - 1));

            module->callInitialize();

            return this->gateSize("out") - 1;

        } else {
            throw cRuntimeError("Error to create module");
        }
    } else {
        throw cRuntimeError("Error to create type module");
    }

    return -1;
}

vector<vector<int> >& Deployer::getParticipatingPaths() {
    return mParticipatingPaths;
}

void Deployer::setParticipatingPaths(
        const vector<vector<int> >& participatingPaths) {
    mParticipatingPaths = participatingPaths;
}

int Deployer::getSentRegs() const {
    return mSentRegs;
}

void Deployer::setSentRegs(int sentRegs) {
    mSentRegs = sentRegs;
}

} //namespace

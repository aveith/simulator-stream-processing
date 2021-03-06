#include <algorithm>
#include <numeric>
#include <glob.h>
#include <iomanip>
#include <fstream>
#include <vector>

#include <ctime>
#include <array>
#include <unistd.h>
#include <numeric>
#include <string>
#include <sstream>
#include <iterator>
#include <iostream>
#include <unordered_set>

#include "../env/objects/networkconnection/resourcedata/resourcecapability/ResourceCapability.h"
#include "../env/objects/networkconnection/linkdata/linkcapability/LinkCapability.h"
#include "../util/Patterns.h"
#include "Configuration.h"

namespace fogstream {

Configuration::Configuration(Env *&env) {
    this->setEnv(env);
    this->setIntialTime(high_resolution_clock::now());
    this->setLimitTime(600);
}

Configuration::~Configuration() {

}

void Configuration::setDstBranches(vector<int> cloudDevices,
        vector<regionsDep> regions) {
    this->getEnv()->getDstBranches().clear();

    int iCloud = 1;

    if (!regions[0].to.empty()) {
        for (unsigned int it = 0; it < regions.size(); ++it) {
            iCloud = 1;
            bool bSink = false;
            for (unsigned int to = 0; to < regions[it].to.size(); ++to) {
                if (regions[it].to[to]
                        == (int) this->getEnv()->getOperators().size() + 1) {
                    bSink = regions[it].to[to]
                            == (int) this->getEnv()->getOperators().size() + 1;
                    break;
                }
            }

            if (bSink) {
                for (auto cloud = cloudDevices.begin();
                        cloud != cloudDevices.end(); ++cloud) {
                    for (unsigned int s = 0;
                            s < this->getEnv()->getSinks().size(); ++s) {

                        if (regions[it].to[regions[it].to.size() - 2]
                                == this->getEnv()->getSinks().at(s)->getOperatorId()) {
                            if (*cloud
                                    == this->getEnv()->getSinks().at(s)->getHostId()) {
                                //EV_INFO << "============ It was found " << s->task.id << endl;
                                iCloud = 0;
                                break;
                            }
                        }
                    }
                }

                this->hierarchySetArea(regions[it].to, regions, iCloud);

                bool bSave = true;

                for (unsigned int i = 0;
                        i < this->getEnv()->getDstBranches().size(); ++i) {
                    if (this->getEnv()->getDstBranches().at(i).getApplicationBranch()
                            == regions[it].to) {
                        bSave = false;
                        this->getEnv()->getDstBranches().at(i).setType(
                                this->getEnv()->getDstBranches().at(i).getType()
                                        == 1 ? 1 : iCloud);
                    }
                }

                if (bSave) {
                    this->getEnv()->getDstBranches().push_back( { iCloud,
                            regions[it].to });
                }
            }
        }

    } else {
        //Evaluate if the last vertex of the pipeline is in the cloud
        for (auto cloud = cloudDevices.begin(); cloud != cloudDevices.end();
                ++cloud) {
            for (unsigned int s = 0; s < this->getEnv()->getSinks().size();
                    ++s) {
                if (regions[0].from[regions[0].from.size() - 2]
                        == this->getEnv()->getSinks().at(s)->getOperatorId()) {
                    if (*cloud
                            == this->getEnv()->getSinks().at(s)->getHostId()) {
                        iCloud = 0;
                        break;
                    }
                }
            }
        }

        this->getEnv()->getDstBranches().push_back(
                { iCloud, regions[0].from });
    }

}

void Configuration::ExecuteStrategies(vector<int> orderedList,
        vector<int> resourceList, vector<regionsDep> regions,
        vector<int> cloudDevices, int strategy, bool isUseSlot) {

    vector<int> tanejaDeployment;

    this->setDstBranches(cloudDevices, regions);

    if (strategy == Patterns::BaseStrategy::RTR
            || strategy == Patterns::BaseStrategy::RTR_RP) {
        this->setDstBranches(cloudDevices, regions);

    } else if (strategy == Patterns::BaseStrategy::Taneja) {
        tanejaDeployment = this->LowerBoundTaneja(orderedList, isUseSlot);

    }

    cout << "\nExecuting the operator placement strategy - "
            << to_string(strategy) << ":" << endl;
    GoalRate *rate = new GoalRate();
    for (auto ope = orderedList.begin(); ope != orderedList.end(); ++ope) {
        cout << "--- Evaluating the placement of the operator: " << *ope
                << endl;

        if (rate->getCurrentOperatorMapping() != nullptr) {
            delete rate->getCurrentOperatorMapping();
            rate->setCurrentOperatorMapping(nullptr);
            for (unsigned int i = 0; i < rate->getLinkMapping().size(); i++) {
                delete rate->getLinkMapping().at(i);
            }
            rate->getLinkMapping().clear();

            for (unsigned int i = 0;
                    i < rate->getPreviousOperatorMapping().size(); i++) {
                delete rate->getPreviousOperatorMapping().at(i);
            }
            rate->getPreviousOperatorMapping().clear();

        }

        //The function returns the rate structure for the operator placement,
        //  the function evaluates the communication and computation
        //  following the strategy rules
        this->determineOperatorPlacement(rate, strategy, *ope, tanejaDeployment,
                isUseSlot);

        if (rate->getCurrentOperatorMapping() == nullptr) {
            throw cRuntimeError(
                    "Constrained environment for deploying operator %d! There is no available rate->",
                    *ope);
        }

        //Update mapping following the return
        bool exists = false;
        for (unsigned int i = 0;
                i < this->getEnv()->getOperatorMapping().size(); i++) {
            if (*ope
                    == this->getEnv()->getOperatorMapping().at(i)->getOperatorId()) {
                exists = true;
                break;
            }
        }

        //Update residual capabilities by reducing the required memory and cpu
        if (!exists) {
            this->getEnv()->getOperatorMapping().push_back(
                    new OperatorMapping(rate->getCurrentOperatorMapping()));

            this->getEnv()->getResidualHostCapabilities().at(
                    rate->getCurrentOperatorMapping()->getHostId()).setCpu(
                    this->getEnv()->getResidualHostCapabilities().at(
                            rate->getCurrentOperatorMapping()->getHostId()).getCpu()
                            - rate->getCurrentOperatorMapping()->getRequiredCpu());

            if (isUseSlot) {
                this->getEnv()->getResidualHostCapabilities().at(
                        rate->getCurrentOperatorMapping()->getHostId()).setSlotNumber(
                        this->getEnv()->getResidualHostCapabilities().at(
                                rate->getCurrentOperatorMapping()->getHostId()).getSlotNumber()
                                != -1 ?
                                this->getEnv()->getResidualHostCapabilities().at(
                                        rate->getCurrentOperatorMapping()->getHostId()).getSlotNumber()
                                        - 1 :
                                -1);
            }

            this->getEnv()->getResidualHostCapabilities().at(
                    rate->getCurrentOperatorMapping()->getHostId()).setMemory(
                    this->getEnv()->getResidualHostCapabilities().at(
                            rate->getCurrentOperatorMapping()->getHostId()).getMemory()
                            - rate->getCurrentOperatorMapping()->getRequiredMemory());

            cout << "Operator: "
                    << rate->getCurrentOperatorMapping()->getOperatorId()
                    << " Host: "
                    << rate->getCurrentOperatorMapping()->getHostId()
                    << " Host type "
                    << this->getEnv()->getResources().at(
                            rate->getCurrentOperatorMapping()->getHostId())->getType()
                    << " Residual CPU: "
                    << this->getEnv()->getResidualHostCapabilities().at(
                            rate->getCurrentOperatorMapping()->getHostId()).getCpu()
                    << endl;

        }

        //Insert the link mappings
        for (unsigned int i = 0; i < rate->getLinkMapping().size(); i++) {
            this->getEnv()->getLinkMapping().push_back(
                    new OperatorConnectionMapping(
                            rate->getLinkMapping().at(i)));

            //Update the residual bandwidth

            this->getEnv()->getResidualLinkCapabilities().at(
                    rate->getLinkMapping().at(i)->getLinkId()).setBandwidth(
                    this->getEnv()->getResidualLinkCapabilities().at(
                            rate->getLinkMapping().at(i)->getLinkId()).getBandwidth()
                            - rate->getLinkMapping().at(i)->getRequiredBandwidth());

            if (this->getEnv()->getResidualLinkCapabilities().at(
                    rate->getLinkMapping().at(i)->getLinkId()).getBandwidth()
                    < 0) {
                throw cRuntimeError(
                        "Constraint of bandwidth for mapping the link %d - operator %d (left %f bits/s) !",
                        rate->getLinkMapping().at(i)->getLinkId(), *ope,
                        this->getEnv()->getResidualLinkCapabilities().at(
                                rate->getLinkMapping().at(i)->getLinkId()).getBandwidth());
            }

        }

        //Update residual link capability considering by removing operator mappings
        for (unsigned int i = 0; i < rate->getPreviousOperatorMapping().size();
                i++) {
            for (unsigned int j = 0;
                    j < this->getEnv()->getOperatorMapping().size(); j++) {
                if (rate->getPreviousOperatorMapping().at(i)->getOperatorId()
                        == this->getEnv()->getOperatorMapping().at(j)->getOperatorId()) {

                    //Update the memory required for communication overhead
                    this->getEnv()->getResidualHostCapabilities().at(
                            rate->getPreviousOperatorMapping().at(i)->getHostId()).setMemory(
                            this->getEnv()->getResidualHostCapabilities().at(
                                    rate->getPreviousOperatorMapping().at(i)->getHostId()).getMemory()
                                    - rate->getPreviousOperatorMapping().at(i)->getRequiredMemory());
                    if (this->getEnv()->getResidualHostCapabilities().at(
                            rate->getPreviousOperatorMapping().at(i)->getHostId()).getMemory()
                            < 0) {
                        throw cRuntimeError(
                                "Constraint for mapping the operator %d - There is no enough bandwidth on host %d!",
                                *ope,
                                rate->getPreviousOperatorMapping().at(i)->getHostId());
                    }
                    break;
                }
            }
        }

        if (rate->isConstrained()) {
            if (rate->isBtwConstraint()) {
                throw cRuntimeError(
                        "Constrained environment for deploying operator %d! The rate structure has Bandwidth constraints.",
                        *ope);
            } else if (rate->isCpuConstraint()) {
                throw cRuntimeError(
                        "Constrained environment for deploying operator %d! The rate structure has CPU constraints.",
                        *ope);
            } else {
                throw cRuntimeError(
                        "Constrained environment for deploying operator %d! The rate structure has constraints.",
                        *ope);
            }
        }

        //Setup all nodes that participate on the operator deployment
        this->getEnv()->getResources().at(
                rate->getCurrentOperatorMapping()->getHostId())->setMapped(
                true);

        if (this->getEnv()->getResources().at(
                rate->getCurrentOperatorMapping()->getHostId())->getGatewayId()
                > -1) {
            this->getEnv()->getResources().at(
                    this->getEnv()->getResources().at(
                            rate->getCurrentOperatorMapping()->getHostId())->getGatewayId())->setMapped(
                    true);
        }

        this->isTimeExceeded();
    }

    delete rate;
    rate = nullptr;

    for (unsigned int iLink = 0;
            iLink < this->getEnv()->getLinkMapping().size(); iLink++) {
        for (unsigned int iNet = 0;
                iNet < this->getEnv()->getNetworkTopology().size(); iNet++) {
            if (this->getEnv()->getLinkMapping().at(iLink)->getLinkId()
                    == this->getEnv()->getNetworkTopology().at(iNet)->getLinkId()) {
                this->getEnv()->getResources().at(
                        this->getEnv()->getNetworkTopology().at(iNet)->getSourceId())->setMapped(
                        true);
                this->getEnv()->getResources().at(
                        this->getEnv()->getNetworkTopology().at(iNet)->getDestinationId())->setMapped(
                        true);
                break;
            }
        }
    }
}

vector<regionsDep> Configuration::getRegions() {

    //get all possible paths
    //It will be included two dummies nodes. One of them will be in front of all data sources
    //  and the another will be after all data sinks, then the application will have only
    //  one data source and one data sink.
    Graph *graphApp = new Graph(this->getEnv()->getOperators().size() + 2);

    for (unsigned int i = 0;
            i < this->getEnv()->getApplicationTopology().size(); ++i) {
        graphApp->addEdge(
                this->getEnv()->getApplicationTopology().at(i)->getFromOperatorId(),
                this->getEnv()->getApplicationTopology().at(i)->getToOperatorId());

        graphApp->setVectorCharacteristic(
                this->getEnv()->getApplicationTopology().at(i)->getToOperatorId(),
                this->getEnv()->getOperators().at(
                        this->getEnv()->getApplicationTopology().at(i)->getToOperatorId())->getType());
    }

    //Add the dummy operator in front of the data sources
    for (unsigned int i = 0; i < this->getEnv()->getSources().size(); ++i) {
        graphApp->addEdge(this->getEnv()->getOperators().size(),
                this->getEnv()->getSources().at(i)->getOperatorId());

        graphApp->setVectorCharacteristic(
                this->getEnv()->getSources().at(i)->getOperatorId(),
                this->getEnv()->getOperators().at(
                        this->getEnv()->getSources().at(i)->getOperatorId())->getType());
    }

    //Set the type of the inserted operator as Joker = dummy
    graphApp->setVectorCharacteristic(this->getEnv()->getOperators().size(),
            Patterns::JokerSource);

    //Add the dummy operator after the data sinks
    for (unsigned int i = 0; i < this->getEnv()->getSinks().size(); ++i) {
        graphApp->addEdge(this->getEnv()->getSinks().at(i)->getOperatorId(),
                this->getEnv()->getOperators().size() + 1);
    }

    //Set the type of the inserted operator as Joker = dummy
    graphApp->setVectorCharacteristic(this->getEnv()->getOperators().size() + 1,
            Patterns::Joker);

    //Generate all paths between the data source dummy operator and
    // the data sink dummy operator
    graphApp->printAllPaths(this->getEnv()->getOperators().size(),
            this->getEnv()->getOperators().size() + 1);
    //graphApp->printVector();

    //obtain the subregions of the graph
    vector<vector<int>> subRegions = graphApp->splitRegions();

    delete graphApp;
    graphApp = nullptr;
    //Create Region's hierarchy
    vector<regionsDep> regHier;

    if (subRegions.size() > 1) {
        for (unsigned int src = 0; src < subRegions.size(); ++src) {
            for (unsigned int dst = 0; dst < subRegions.size(); ++dst) {
                if (subRegions[src] != subRegions[dst]) {
                    if (subRegions[src][subRegions[src].size() - 1]
                            == subRegions[dst][0]) {
                        regHier.push_back(
                                { subRegions[src], subRegions[dst] });
                    }
                }
            }
        }
    } else {
        regHier.push_back( { subRegions[0] });
    }

    return regHier;
}

void Configuration::hierarchySetArea(vector<int> branch,
        vector<regionsDep> regions, int Cloud) {
    //Go through the hierarchy
    for (unsigned int it = 0; it < regions.size(); ++it) {

        if (regions[it].from.size() - 1 > 0) {
            if (branch[0] == regions[it].from[regions[it].from.size() - 1]) {

                this->hierarchySetArea(regions[it].from, regions, Cloud);

                bool bSave = true;

                for (unsigned int i = 0;
                        i < this->getEnv()->getDstBranches().size(); ++i) {
                    if (this->getEnv()->getDstBranches().at(i).getApplicationBranch()
                            == regions[it].from) {
                        bSave = false;
                        this->getEnv()->getDstBranches().at(i).setType(
                                this->getEnv()->getDstBranches().at(i).getType()
                                        == 1 ? 1 : Cloud);
                    }
                }

                if (bSave) {

                    this->getEnv()->getDstBranches().push_back( { Cloud,
                            regions[it].from });
                }

            }

        } else {
            bool bSave = true;

            for (unsigned int i = 0;
                    i < this->getEnv()->getDstBranches().size(); ++i) {
                if (this->getEnv()->getDstBranches().at(i).getApplicationBranch()
                        == regions[it].from) {
                    bSave = false;
                    this->getEnv()->getDstBranches().at(i).setType(
                            this->getEnv()->getDstBranches().at(i).getType()
                                    == 1 ? 1 : Cloud);
                }
            }

            if (bSave) {

                this->getEnv()->getDstBranches().push_back(
                        { Cloud, regions[it].from });
            }
        }

    }
}

vector<DestinationBranches> Configuration::RegionBranchesFromDataSoures(
        vector<regionsDep> regions) {
    vector<DestinationBranches> dataSourceBranches;
    //get all possible paths
    //It will be included two dummies nodes. One of them will be in front of all data sources
    //  and the another will be after all data sinks, then the application will have only
    //  one data source and one data sink.
    Graph *graphApp = new Graph(this->getEnv()->getOperators().size() + 2);

    for (unsigned int i = 0;
            i < this->getEnv()->getApplicationTopology().size(); ++i) {
        graphApp->addEdge(
                this->getEnv()->getApplicationTopology().at(i)->getFromOperatorId(),
                this->getEnv()->getApplicationTopology().at(i)->getToOperatorId());
    }

    //Add the dummy operator in front of the data sources
    for (unsigned int i = 0; i < this->getEnv()->getSources().size(); ++i) {
        graphApp->addEdge(this->getEnv()->getOperators().size(),
                this->getEnv()->getSources().at(i)->getOperatorId());
    }

    //Add the dummy operator after the data sinks
    for (unsigned int i = 0; i < this->getEnv()->getSinks().size(); ++i) {
        graphApp->addEdge(this->getEnv()->getSinks().at(i)->getOperatorId(),
                this->getEnv()->getOperators().size() + 1);
    }

    for (unsigned int iSource = 0;
            iSource < this->getEnv()->getSources().size(); ++iSource) {

        graphApp->printAllPaths(
                this->getEnv()->getSources().at(iSource)->getOperatorId(),
                this->getEnv()->getOperators().size() + 1);

        graphApp->getPaths().size();

        for (unsigned int it = 0; it < regions.size(); ++it) {
            if (this->getEnv()->getSources().at(iSource)->getOperatorId()
                    == regions.at(it).from.at(0)) {
                bool bSave = true;
                for (auto t = dataSourceBranches.begin();
                        t != dataSourceBranches.end(); ++t) {
                    if (t->getApplicationBranch() == regions.at(it).from) {
                        bSave = false;
                        break;
                    }
                }
                if (bSave) {
                    dataSourceBranches.push_back(
                            { (int) graphApp->getPaths().size(),
                                    regions.at(it).from });
                }
            }
        }
    }

    sort(dataSourceBranches.begin(), dataSourceBranches.end(),
            [](const DestinationBranches &left,
                    const DestinationBranches &right) {
                return (left.getType() > right.getType());
            });
    return dataSourceBranches;
}

vector<int> Configuration::orderByRegion(vector<regionsDep> regions) {
    vector<int> listOrder;

    //Clear the global vector to hierarchies
    this->getOrderedByHier().clear();

    if (!regions.at(0).to.empty()) {

        //Determine the regions which have as the first operator a data source
        vector<DestinationBranches> sourceConnections =
                this->RegionBranchesFromDataSoures(regions);

        //Iterate the regions starting on data sources
        for (unsigned int sourceConnection = 0;
                sourceConnection < sourceConnections.size();
                ++sourceConnection) {
            double level = 0;

            //            if (sourceConnections.at(sourceConnection).getApplicationBranch().size()
            //                    == 1) {
            //                bool bSource = false;
            //                for (unsigned int source = 0;
            //                        source < this->getEnv()->getSources().size();
            //                        source++) {
            //                    bSource =
            //                            (sourceConnections.at(sourceConnection).getApplicationBranch().at(
            //                                    0)
            //                                    == this->getEnv()->getSources().at(source)->getOperatorId());
            //                    if (bSource)
            //                        break;
            //                }
            //
            //                if (bSource) {
            //                    bool bExists = false;
            //                    for (unsigned int iRegions = 0; iRegions < regions.size();
            //                            iRegions++) {
            //                        if (sourceConnections.at(sourceConnection).getApplicationBranch()
            //                                == regions.at(iRegions).from) {
            //                            bExists = true;
            //                            break;
            //                        }
            //                    }
            //
            //                    if (bExists)
            //                        continue;
            //                }
            //            }

            for (unsigned int it = 0; it < regions.size(); ++it) {
                if (regions.at(it).from
                        == sourceConnections.at(sourceConnection).getApplicationBranch()) {
                    double sublevel = 0;

                    for (auto reg = regions.at(it).from.begin();
                            reg != regions.at(it).from.end(); ++reg) {

                        if (*reg
                                != (int) this->getEnv()->getOperators().size()
                                        + 1) {
                            //                            taskData task = this->getTask(*reg);
                            bool bSave = true;

                            if (regions.at(it).from.at(
                                    regions.at(it).from.size() - 1) == *reg) {
                                level++;
                            }

                            for (auto ohier = this->getOrderedByHier().begin();
                                    ohier != this->getOrderedByHier().end();
                                    ++ohier) {
                                if (*reg == ohier->operatorID) {
                                    bSave = false;
                                    break;
                                }
                            }

                            if (bSave) {
                                double ll = level + sublevel / 10;
                                bool bSink = false;
                                for (unsigned int sl = 0;
                                        sl < this->getEnv()->getSinks().size();
                                        ++sl) {

                                    if (this->getEnv()->getSinks().at(sl)->getOperatorId()
                                            == *reg) {
                                        bSink = true;
                                    }
                                }

                                if (!bSink) {
                                    this->getOrderedByHier().push_back( { ll,
                                            *reg });
                                } else {
                                    this->getOrderedByHier().push_back( { 100,
                                            *reg });
                                }

                            }

                            sublevel++;
                        }
                    }

                    this->hierarchyOrder(regions.at(it).to, regions, level);
                }
            }
        }
    } else {
        for (auto reg = regions.at(0).from.begin();
                reg != regions.at(0).from.end(); ++reg) {
            if (*reg < (int) regions.at(0).from.size()) {
                listOrder.push_back(*reg);
            }
        }
    }

    //EV_INFO << " Total Tasks " << this->orderByHier.size() << endl;

    sort(this->getOrderedByHier().begin(), this->getOrderedByHier().end(),
            [](const orderListS &left, const orderListS &right) {
                return (left.level < right.level);
            });

    for (auto rr = this->getOrderedByHier().begin();
            rr != this->getOrderedByHier().end(); ++rr) {
        for (unsigned int tt = 0; tt < this->getEnv()->getOperators().size();
                ++tt) {

            if (rr->operatorID
                    == this->getEnv()->getOperators().at(tt)->getId()) {
                listOrder.push_back(rr->operatorID);
                //EV_INFO << "******************* Adding Task " << rr->task.id  << " level " << rr->level << endl;
                break;
            }
        }
    }
    return listOrder;
}

void Configuration::hierarchyOrder(vector<int> branch,
        vector<regionsDep> regions, int level) {
    double l = level + 1;
    double sublevel = 0;

    for (auto regS = branch.begin() + 1; regS != branch.end(); ++regS) {
        if (*regS != this->getEnv()->getOperators().size() + 1) {

            bool bSave = true;
            for (auto ohier = this->getOrderedByHier().begin();
                    ohier != this->getOrderedByHier().end(); ++ohier) {
                if (*regS == ohier->operatorID) {
                    bSave = false;
                    break;
                }
            }

            if (branch[branch.size() - 1] == *regS) {
                l++;
            }

            if (bSave) {
                float ll = l + sublevel / 10;
                bool bSink = false;
                for (unsigned int sl = 0;
                        sl < this->getEnv()->getSinks().size(); ++sl) {
                    if (this->getEnv()->getSinks().at(sl)->getOperatorId()
                            == *regS) {
                        bSink = true;
                    }
                }

                if (!bSink) {
                    this->getOrderedByHier().push_back( { ll, *regS });
                } else {
                    this->getOrderedByHier().push_back(
                            { (double) 100, *regS });
                }

                //this->orderByHier.push_back({ll, task});
            }
            sublevel++;
        }
    }

    //Go through the hierarchy
    for (unsigned int it = 0; it < regions.size(); ++it) {
        if (branch == regions[it].from) {
            this->hierarchyOrder(regions[it].to, regions, l);
        }

    }

}

vector<int> Configuration::LowerBoundTaneja(vector<int> orderedList,
        bool isUseSlot) {

    //Estimate the number of input and output rates on each operator based on the sources
    vector<applicationMetrics> appMetrics = this->basicApplicationArrivalRates(
            orderedList);

    //Create the vector for the taneja's deployment
    vector<int> tempMap(this->getEnv()->getOperators().size());

    //Copy the resources to a local list to sort
    //    vector<ResourceCapability> hostCapacities;
    //    for (unsigned int i = 0;
    //            i < this->getEnv()->getResidualHostCapabilities().size(); i++) {
    //        if (this->getEnv()->getResources().at(i)->getType()
    //                != Patterns::DeviceType::Gateway) {
    //            hostCapacities.push_back(
    //                    this->getEnv()->getResidualHostCapabilities().at(i));
    //        }
    //    }

    vector<ResourceBase*> hostCapacities;
    for (unsigned int i = 0;
            i < this->getEnv()->getResidualHostCapabilities().size(); i++) {
        if (this->getEnv()->getResources().at(i)->getType()
                != Patterns::DeviceType::Gateway) {
            double btw = std::numeric_limits<double>::max();
            for (unsigned int j = 0;
                    j < this->getEnv()->getNetworkTopology().size(); j++) {
                if (this->getEnv()->getNetworkTopology().at(j)->getSourceId()
                        == this->getEnv()->getResidualHostCapabilities().at(i).getHostId()) {
                    if (this->getEnv()->getLinkCapabilities().at(
                            this->getEnv()->getNetworkTopology().at(j)->getLinkId()).getBandwidth()
                            < btw) {
                        btw =
                                this->getEnv()->getLinkCapabilities().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                j)->getLinkId()).getBandwidth();
                    }
                }

            }

            hostCapacities.push_back(
                    new ResourceBase(
                            this->getEnv()->getResidualHostCapabilities().at(i).getHostId(),
                            this->getEnv()->getResidualHostCapabilities().at(i).getCpu(),
                            this->getEnv()->getResidualHostCapabilities().at(i).getMemory(),
                            btw));
        }
    }

    sort(hostCapacities.begin(), hostCapacities.end(),
            [](ResourceBase *&left, ResourceBase *&right) {
                return (left->getCpu() < right->getCpu());
            });

    //Copy the operators to a local list to sort
    vector<OperatorData*> operators;
    for (unsigned int i = 0; i < this->getEnv()->getOperators().size(); i++) {
        operators.push_back(
                new OperatorData(this->getEnv()->getOperators().at(i)->getId(),
                        this->getEnv()->getOperators().at(i)->getFissionId(),
                        this->getEnv()->getOperators().at(i)->getSelectivity(),
                        this->getEnv()->getOperators().at(i)->getDataTransferRate(),
                        this->getEnv()->getOperators().at(i)->getStateRate(),
                        this->getEnv()->getOperators().at(i)->getType(),
                        this->getEnv()->getOperators().at(i)->getCPURequirement(),
                        this->getEnv()->getOperators().at(i)->getMemoryRequirement(),
                        this->getEnv()->getOperators().at(i)->getTimeWindow(),
                        this->getEnv()->getOperators().at(i)->isNextSplitted(),
                        this->getEnv()->getOperators().at(i)->getNextSplittedLength(),
                        this->getEnv()->getOperators().at(i)->getCharacteristicId(),
                        this->getEnv()->getOperators().at(i)->getStateSize()));
    }

    sort(operators.begin(), operators.end(),
            [](OperatorData *&left, OperatorData *&right) {
                return (left->getCPURequirement() < right->getCPURequirement());
            });

    int low = 0, high = hostCapacities.size() - 1;

    for (unsigned int start = 0; start < operators.size(); ++start) {
        cout << "Operator: " << operators[start]->getId() << " CPU req.: "
                << this->getEnv()->getOperators().at(operators[start]->getId())->getCPURequirement()
                << " Arrival: "
                << appMetrics.at(operators[start]->getId()).mNumberofMessages
                << endl;

        double cpuRequirement = this->getEnv()->getOperators().at(
                operators[start]->getId())->getCPURequirement()
                * appMetrics.at(operators[start]->getId()).mNumberofMessages;

        double btwReq = appMetrics.at(operators[start]->getId()).mSizeofMessages
                * 8;
        int iHost = -1;

        for (unsigned int ss = 0; ss < this->getEnv()->getSources().size();
                ss++) {
            if (this->getEnv()->getSources().at(ss)->getOperatorId()
                    == operators[start]->getId()) {
                iHost = this->getEnv()->getSources().at(ss)->getHostId();
                break;
            }
        }

        if (iHost == -1) {
            for (unsigned int ss = 0; ss < this->getEnv()->getSinks().size();
                    ss++) {
                if (this->getEnv()->getSinks().at(ss)->getOperatorId()
                        == operators[start]->getId()) {
                    iHost = this->getEnv()->getSinks().at(ss)->getHostId();
                    break;
                }
            }
        }

        if (iHost == -1) {
            int bound = this->lowerBound(hostCapacities, operators[start], low,
                    high, cpuRequirement, btwReq);

            if (bound != -1) {
                iHost = hostCapacities.at(bound)->getHostId();
                low = bound + 1;
                cout << "Good" << endl;
            } else {
                iHost =
                        hostCapacities.at(hostCapacities.size() - 1)->getHostId();
                cout << "Cloud" << endl;
            }
        }

        tempMap.at(operators[start]->getId()) = iHost;

        for (unsigned int iNode = 0; iNode < hostCapacities.size(); ++iNode) {
            if (hostCapacities.at(iNode)->getHostId() == iHost) {

                //                if (isUseSlot && hostCapacities.at(iNode).getSlotNumber() != -1)
                //                    hostCapacities.at(iNode).setSlotNumber(
                //                            hostCapacities.at(iNode).getSlotNumber() - 1);
                //
                //                if (hostCapacities.at(iNode).getSlotNumber() == 0) {
                //                    hostCapacities.at(iNode).setCpu(0);
                //
                //                } else {
                hostCapacities.at(iNode)->setCpu(
                        hostCapacities.at(iNode)->getCpu() - cpuRequirement);
                hostCapacities.at(iNode)->setBandwidth(
                        hostCapacities.at(iNode)->getBandwidth() - btwReq);
                //                }

                cout << "Operator: " << operators[start]->getId() << " Host: "
                        << iHost << " CPU: "
                        << hostCapacities.at(iNode)->getCpu() << " Req.: "
                        << cpuRequirement << endl;
                break;
            }
        }

        sort(hostCapacities.begin(), hostCapacities.end(),
                [](ResourceBase *&left, ResourceBase *&right) {
                    return (left->getCpu() < right->getCpu());
                });
    }

    for (unsigned int i = 0; i < operators.size(); i++) {
        delete operators.at(i);
    }
    operators.clear();

    for (unsigned int i = 0; i < hostCapacities.size(); i++) {
        delete hostCapacities.at(i);
    }
    hostCapacities.clear();

    return tempMap;
}

int Configuration::lowerBound(vector<ResourceBase*> nodes,
        OperatorData *tasksTemp, int &low, int high, double cpuRequirement,
        double btwReq) {
    int length = nodes.size(), mid = (low + high) / 2;
    ResourceBase *x;
    while (true) {
        x = nodes.at(mid);
        bool compare = (x->getCpu() >= cpuRequirement
                && x->getMemory() >= tasksTemp->getMemoryRequirement()
                && x->getBandwidth() >= btwReq);

        if (compare) {
            high = mid - 1;
            if (high < low) {
                return mid;
            }
        } else {
            low = mid + 1;
            if (mid < length - 1) {
                x = nodes.at(mid + 1);
                if (x->getCpu() >= cpuRequirement
                        && x->getMemory() >= tasksTemp->getMemoryRequirement()
                        && x->getBandwidth() >= btwReq) {
                    return mid + 1;

                } else {
                    return -1;
                }
            } else {
                return -1;
            }

            //            return ((mid < length - 1) ? mid + 1 : -1);
        }

        mid = (low + high) / 2;
    }
}

int Configuration::setupEnvironment(int strategy, bool isUseSlots,
        int configScaleApproach) {

    /**REGIONS PATTERNS TO ESTIMATE THE TIME IN PARALLEL AND CONCURRENT REGIONS
     * Currently, the algorithm only cares for the location of the sinks.
     * However, the procedure is coupled to the method...
     *
     * in the future, I intend to get the rules for this kind of estimation
     * and apply it to obtain the times following the application regions
     */

    this->setConfigScaleApproach(configScaleApproach);
    //Get regions from the graph
    vector<regionsDep> regions = this->getRegions();

    //Set up the order of the vertex by using BFS-Traversal
    //list<taskData> orderedList = this->orderGraph();
    //    vector<int> orderedList = this->orderByRegion(regions);

    vector<int> orderedList = this->OperatorOrdering();

    std::copy(this->getEnv()->getHostCapabilities().begin(),
            this->getEnv()->getHostCapabilities().end(),
            std::inserter(this->getEnv()->getResidualHostCapabilities(),
                    this->getEnv()->getResidualHostCapabilities().end()));

    //Update host capabilities according to the number of servers
    for (unsigned int i = 0;
            i < this->getEnv()->getResidualHostCapabilities().size(); i++) {
        this->getEnv()->getResidualHostCapabilities().at(i).setCpu(
                this->getEnv()->getResidualHostCapabilities().at(i).getCpu()
                        * this->getEnv()->getResidualHostCapabilities().at(i).getServers());
        this->getEnv()->getResidualHostCapabilities().at(i).setMemory(
                this->getEnv()->getResidualHostCapabilities().at(i).getMemory()
                        * this->getEnv()->getResidualHostCapabilities().at(i).getServers());
    }

    std::copy(this->getEnv()->getLinkCapabilities().begin(),
            this->getEnv()->getLinkCapabilities().end(),
            std::inserter(this->getEnv()->getResidualLinkCapabilities(),
                    this->getEnv()->getResidualLinkCapabilities().end()));

    vector<int> cloudIDs;
    vector<int> resourceList = this->defineDeployableHosts(cloudIDs);

    if (strategy == Patterns::BaseStrategy::CloudOnly) {
        this->ExecuteStrategies(orderedList, resourceList, regions, cloudIDs,
                Patterns::BaseStrategy::CloudOnly, isUseSlots); //onlyCloud
    } else if (strategy == Patterns::BaseStrategy::RTR) {
        this->ExecuteStrategies(orderedList, resourceList, regions, cloudIDs,
                Patterns::BaseStrategy::RTR, isUseSlots); //overloadRatioOnlyEdge
    } else if (strategy == Patterns::BaseStrategy::RTR_RP) {
        this->ExecuteStrategies(orderedList, resourceList, regions, cloudIDs,
                Patterns::BaseStrategy::RTR_RP, isUseSlots); //overloadRatioRT
    } else if (strategy == Patterns::BaseStrategy::Taneja) {
        this->ExecuteStrategies(orderedList, resourceList, regions, cloudIDs,
                Patterns::BaseStrategy::Taneja, isUseSlots);
    }

    return 0;
}

Env*& Configuration::getEnv() {
    return env;
}

void Configuration::setEnv(Env *&env) {
    this->env = env;
}

vector<orderListS>& Configuration::getOrderedByHier() {
    return mOrderedByHier;
}

vector<int> Configuration::defineDeployableHosts(vector<int> &cloudIDs) {
    //Set up the vector with available resources
    vector<ResourceCapability> edgeDevices;
    vector<ResourceCapability> gatewayDevices;
    vector<ResourceCapability> cloudDevices;

    for (unsigned int it = 0; it < this->getEnv()->getResources().size();
            ++it) {
        if (this->getEnv()->getResources().at(it)->getType()
                == Patterns::DeviceType::Sensor) {
            edgeDevices.push_back(this->getEnv()->getHostCapabilities().at(it));

        } else if (this->getEnv()->getResources().at(it)->getType()
                == Patterns::DeviceType::Gateway) {
            gatewayDevices.push_back(
                    this->getEnv()->getHostCapabilities().at(it));

        } else {
            cloudDevices.push_back(
                    this->getEnv()->getHostCapabilities().at(it));

        }
    }

    //Sort the vectors by memory
    sort(edgeDevices.begin(), edgeDevices.end(),
            [](ResourceCapability &left, ResourceCapability &right) {
                return (left.getMemory() > right.getMemory());
            });

    sort(gatewayDevices.begin(), gatewayDevices.end(),
            [](ResourceCapability &left, ResourceCapability &right) {
                return (left.getMemory() > right.getMemory());
            });

    sort(cloudDevices.begin(), cloudDevices.end(),
            [](ResourceCapability &left, ResourceCapability &right) {
                return (left.getMemory() > right.getMemory());
            });

    for (unsigned int i = 0; i < cloudDevices.size(); i++) {
        cloudIDs.push_back(cloudDevices.at(i).getHostId());
    }

    vector<int> resourceList;
    for (unsigned int edge = 0; edge < edgeDevices.size(); ++edge) {
        resourceList.push_back(edgeDevices.at(edge).getHostId());

    }

    for (unsigned int cloud = 0; cloud < cloudDevices.size(); ++cloud) {
        resourceList.push_back(cloudDevices.at(cloud).getHostId());

    }

    return resourceList;
}

void Configuration::determineOperatorPlacement(GoalRate *rate, int strategy,
        int operatorId, vector<int> tanejaDeployment, bool isUseSlot) {
    //If the current operator is a source, it means that there is no communication
    //  overhead
    for (unsigned int iSource = 0;
            iSource < this->getEnv()->getSources().size(); iSource++) {
        if (this->getEnv()->getSources().at(iSource)->getOperatorId()
                == operatorId) {
            return this->placementOverheads(rate, operatorId,
                    this->getEnv()->getSources().at(iSource)->getHostId(), true,
                    true);
        }
    }

    //If the current operator is a sink, it means that there is no communication
    //  overhead because the overhead will be in the previous operators
    for (unsigned int iSink = 0; iSink < this->getEnv()->getSinks().size();
            iSink++) {
        if (this->getEnv()->getSinks().at(iSink)->getOperatorId()
                == operatorId) {
            return this->placementOverheads(rate, operatorId,
                    this->getEnv()->getSinks().at(iSink)->getHostId(), false,
                    false);
        }
    }

    //if the strategy is cloud then the algorithm will define which cloud the operators
    // will be placed. The algorithm will direct the placement following the sink and sources placements
    double totalRate = std::numeric_limits<double>::max();
    if (strategy == Patterns::BaseStrategy::CloudOnly) {
        for (unsigned int i = 0; i < this->getEnv()->getCloudServers().size();
                i++) {

            this->isTimeExceeded();

            GoalRate *tempMap = new GoalRate();
            this->placementOverheads(tempMap, operatorId,
                    this->getEnv()->getCloudServers().at(i), false, false);

            if (!tempMap->isConstrained()
                    && tempMap->getTotalRate() < totalRate) {
                rate->clear();
                rate->cloneObject(tempMap);
                totalRate = rate->getTotalRate();
            }

            delete tempMap;
            tempMap = nullptr;
        }

    } else if (strategy == Patterns::BaseStrategy::RTR
            || strategy == Patterns::BaseStrategy::RTR_RP) {

        int iHostTypes = this->DeviceTypeStrategy(operatorId);

        //veith 2020/01/13 -- begin
        vector<int> availableResources;

        //New approach to determine the used devices when deploying operators
        if (this->getConfigScaleApproach()
                == Patterns::ConfigScaleApproach::NetworkLatency) {

            //Clear pointers begin
            //            if (iHostTypes != Patterns::DeviceType::Sensor) { //<-- check this todo
            this->scaleStrategySensors(availableResources, operatorId);
            //            }

            //Clear pointers end
        } else {
            for (unsigned int iHost = 0;
                    iHost < this->getEnv()->getDeployableHosts().size();
                    iHost++) {
                bool isSink = false;
                for (unsigned int iSink = 0;
                        iSink < this->getEnv()->getSinks().size(); iSink++) {
                    if (this->getEnv()->getSinks().at(iSink)->getHostId()
                            == this->getEnv()->getDeployableHosts().at(iHost)) {
                        isSink = true;
                        break;
                    }
                }

                if (!isSink) {
                    availableResources.push_back(
                            this->getEnv()->getDeployableHosts().at(iHost));
                }
            }
        } //veith 2020/01/13 -- end

        for (unsigned int iHost = 0; iHost < availableResources.size();
                iHost++) {

            this->isTimeExceeded();

            if (isUseSlot
                    and this->getEnv()->getResidualHostCapabilities().at(
                            availableResources.at(iHost)).getSlotNumber()
                            != -1) {

                if (this->getEnv()->getResidualHostCapabilities().at(
                        availableResources.at(iHost)).getSlotNumber() - 1 < 0)
                    continue;
            }
            //if the strategy considers only the rate then the algorithm will calculate the rate for all resources
            // otherwise it will follow the RP

            //            cout << "Resource type: " << this->getEnv()->getResources().at(
            //                    availableResources.at(iHost))->getType() << endl;
            if ((strategy == Patterns::BaseStrategy::RTR
                    && this->getEnv()->getResources().at(
                            availableResources.at(iHost))->getType()
                            == Patterns::DeviceType::Sensor)
                    || (strategy == Patterns::BaseStrategy::RTR_RP
                            && this->getEnv()->getResources().at(
                                    availableResources.at(iHost))->getType()
                                    == iHostTypes)) {
                GoalRate *tempMap = new GoalRate();
                this->placementOverheads(tempMap, operatorId,
                        availableResources.at(iHost), false, false);
                if (!tempMap->isConstrained()
                        && tempMap->getTotalRate() < totalRate) {
                    rate->cloneObject(tempMap);
                    totalRate = rate->getTotalRate();
                }

                delete tempMap;
                tempMap = nullptr;
            }
        }

        //If the algorithm didn't match a solution for the RP, it will try to place on cloud
        if ((strategy == Patterns::BaseStrategy::RTR
                && rate->getCurrentOperatorMapping() == nullptr)
                || (strategy == Patterns::BaseStrategy::RTR_RP
                        && rate->getCurrentOperatorMapping() == nullptr
                        && iHostTypes == Patterns::DeviceType::Sensor)) {

            //            for (auto it = this->getEnv()->getCloudServers().begin();
            //                    it != this->getEnv()->getCloudServers().end(); it++) {
            //
            for (unsigned int iHost = 0; iHost < availableResources.size();
                    iHost++) {

                this->isTimeExceeded();

                if (this->getEnv()->getResources().at(
                        availableResources.at(iHost))->getType()
                        == Patterns::DeviceType::Cloud) {

                    GoalRate *tempMap = new GoalRate();
                    this->placementOverheads(tempMap, operatorId,
                            availableResources.at(iHost), false, false);
                    if (!tempMap->isConstrained()
                            && tempMap->getTotalRate() < totalRate) {
                        rate->cloneObject(tempMap);
                        totalRate = rate->getTotalRate();
                    }

                    delete tempMap;
                    tempMap = nullptr;
                }
            }
        }

        availableResources.clear();

        //If the strategies didn't match a solution, it will raise an exception
        if (rate->getCurrentOperatorMapping() == nullptr) {
            throw cRuntimeError("Problems on deploying operator %d!",
                    operatorId);
        }
    } else {
        //Taneja
        return this->placementOverheads(rate, operatorId,
                tanejaDeployment[operatorId], false, false);
    }
}

void Configuration::placementOverheads(GoalRate *rate, int operatorId,
        int hostId, bool onlyComputation, bool isSource) {
    bool bConstrained = false;
    rate->setCurrentOperatorMapping(new OperatorMapping());
    rate->getCurrentOperatorMapping()->setOperatorId(operatorId);
    rate->getCurrentOperatorMapping()->setHostId(hostId);

    //First is estimated the communication costs since the operator is not a sink nor a source
    if (!onlyComputation) {
        bConstrained = this->estimateCommucation(operatorId, hostId,
                rate->getPreviousOperatorMapping(), rate->getLinkMapping());
        rate->setBtwConstraint(bConstrained);
    }

    if (!bConstrained) {
        //Determine the computation overhead
        bConstrained = this->estimateComputation(
                rate->getCurrentOperatorMapping(),
                rate->getPreviousOperatorMapping(), isSource);
        rate->setCpuConstraint(bConstrained);
    }

    //Summarize values
    double totalTime = 0;
    if (!bConstrained) {
        totalTime += rate->getCurrentOperatorMapping()->getCompTime();

        for (unsigned int i = 0; i < rate->getLinkMapping().size(); i++) {
            totalTime += rate->getLinkMapping().at(i)->getCommTime();
        }
    }
    rate->setConstrained(bConstrained);
    rate->setTotalRate(totalTime);
}

int Configuration::DeviceTypeStrategy(int operatorId) {
    //The RTR+RP algorithm considers a given area of the application
    //  to define which type of resource will host the operator
    for (auto dst = this->getEnv()->getDstBranches().begin();
            dst != this->getEnv()->getDstBranches().end(); ++dst) {
        for (unsigned int i = 0; i < dst->getApplicationBranch().size(); ++i) {
            if (dst->getApplicationBranch().at(i) == operatorId) {
                if (dst->getType() == 1) {
                    return Patterns::DeviceType::Sensor;
                }
            }
        }

    }
    return Patterns::DeviceType::Cloud;
}

bool Configuration::estimateCommucation(int OperatorId, int HostId,
        vector<OperatorMapping*> &commToPreviousOperators,
        vector<OperatorConnectionMapping*> &linkMap) {
    struct path_struc {
        int mSourceOperatorId;
        int mSourceHostId;
        double mArrivalRate;
        double mArrivalMsgSize;
        vector<int> mPath;
        vector<int> mLinks;
    };

    //This part will get all paths among the current node and the previous
    // if there is no constraint it will follow the flow and estimate the
    // communication overhead
    vector<path_struc> communicationPaths;
    int iPreviousOperators = 0;
    int iSameHost = 0;
    for (unsigned int iPrevious = 0;
            iPrevious < this->getEnv()->getApplicationTopology().size();
            iPrevious++) {

        //Identify the previous operators because can exist a union of data
        if (this->getEnv()->getApplicationTopology().at(iPrevious)->getToOperatorId()
                == OperatorId) {
            iPreviousOperators++;

            for (unsigned int iMap = 0;
                    iMap < this->getEnv()->getOperatorMapping().size();
                    iMap++) {
                if (this->getEnv()->getApplicationTopology().at(iPrevious)->getFromOperatorId()
                        == this->getEnv()->getOperatorMapping().at(iMap)->getOperatorId()) {

                    //If the mapping of the source and sink operators are the same host,
                    //  the communication will be neglected
                    if (this->getEnv()->getOperatorMapping().at(iMap)->getHostId()
                            == HostId) {
                        iSameHost++;
                        break;

                    } else {

                        //If hosts are different, first will be defined a valid path
                        vector<int> path;
                        path.clear();

                        vector<int> links;
                        links.clear();
                        if (this->getEnv()->getValidPath(
                                this->getEnv()->getOperatorMapping().at(iMap)->getHostId(),
                                HostId,
                                this->getEnv()->getOperatorMapping().at(iMap)->getOutputRate(),
                                this->getEnv()->getOperatorMapping().at(iMap)->getOutputMsgSize(),
                                path, links)) {
                            communicationPaths.push_back(
                                    { this->getEnv()->getOperatorMapping().at(
                                            iMap)->getOperatorId(),
                                            this->getEnv()->getOperatorMapping().at(
                                                    iMap)->getHostId(),
                                            this->getEnv()->getOperatorMapping().at(
                                                    iMap)->getOutputRate(),
                                            this->getEnv()->getOperatorMapping().at(
                                                    iMap)->getOutputMsgSize(),
                                            path, links });
                        } else {
                            return true;
                        }
                    }
                    break;
                }
            }
        }

    }

    if ((iPreviousOperators - iSameHost) != communicationPaths.size()) {
        return true;
    }

    for (unsigned int i = 0; i < communicationPaths.size(); i++) {

        double meanTimeinSystem = 0, meanTimeinQueue = 0,
                meanNumberofMsgsinSystem = 0, meanNumberinQueue = 0, rho = 0,
                firstMeanNumberSystem = 0, communicationTime = 0;

        double arrivalRate = communicationPaths.at(i).mArrivalRate;

        for (unsigned int j = 0; j < communicationPaths.at(i).mLinks.size();
                j++) {
            double serviceRate = this->getEnv()->getLinkCapabilities().at(
                    communicationPaths.at(i).mLinks.at(j)).getBandwidth()
                    / (communicationPaths.at(i).mArrivalMsgSize * 8);

            if (this->getEnv()->EstimateTimesandQueues(serviceRate, arrivalRate,
                    this->getEnv()->getLinks().at(
                            communicationPaths.at(i).mLinks.at(j))->getDelay(),
                    meanTimeinSystem, meanTimeinQueue, meanNumberofMsgsinSystem,
                    meanNumberinQueue, rho)) {

                communicationTime += meanTimeinSystem;

                if (firstMeanNumberSystem == 0) {
                    firstMeanNumberSystem = meanNumberofMsgsinSystem;
                }

                //Evaluate memory constraint
                if ((meanNumberofMsgsinSystem * 8) * arrivalRate
                        > this->getEnv()->getResidualHostCapabilities().at(
                                communicationPaths.at(i).mSourceHostId).getMemory()) {
                    return true;
                }

                arrivalRate = arrivalRate - meanNumberofMsgsinSystem;

                linkMap.push_back(
                        new OperatorConnectionMapping(
                                communicationPaths.at(i).mSourceOperatorId,
                                OperatorId,
                                communicationPaths.at(i).mLinks.at(j),
                                communicationPaths.at(i).mArrivalRate,
                                arrivalRate, meanNumberofMsgsinSystem,
                                meanTimeinSystem,
                                arrivalRate
                                        * communicationPaths.at(i).mArrivalMsgSize
                                        * 8));

            } else {
                return true;
            }

        }

        //Summarize the communication time considering the previous operator mapping
        for (unsigned int iPrevious = 0;
                iPrevious < this->getEnv()->getOperatorMapping().size();
                iPrevious++) {

            //Update previous operators considering the communication
            if (this->getEnv()->getOperatorMapping().at(iPrevious)->getOperatorId()
                    == communicationPaths.at(i).mSourceOperatorId) {

                commToPreviousOperators.push_back(
                        new OperatorMapping(
                                this->getEnv()->getOperatorMapping().at(
                                        iPrevious)));
                commToPreviousOperators.at(commToPreviousOperators.size() - 1)->setRequiredMemory(
                        firstMeanNumberSystem
                                * commToPreviousOperators.at(
                                        commToPreviousOperators.size() - 1)->getOutputMsgSize());
                break;
            }
        }

    }

    return false;
}

bool Configuration::estimateComputation(OperatorMapping *operatorMap,
        vector<OperatorMapping*> previousMappings, bool isSource) {

    double arrivalRate = 0, arrivalMsgSize = 0;

    if (!isSource) {
        //The arrival rate metrics are evaluated since the operator can have several
        //  upstream operators. If so, the algorithm summarizes all upstream operators.
        this->getEnv()->obtaingArrivalMetrics(operatorMap->getOperatorId(),
                arrivalRate, arrivalMsgSize);

    } else {
        for (unsigned int iSource = 0;
                iSource < this->getEnv()->getSources().size(); iSource++) {
            if (this->getEnv()->getSources().at(iSource)->getOperatorId()
                    == operatorMap->getOperatorId()) {

                arrivalRate =
                        this->getEnv()->getSources().at(iSource)->getArrivalRate();
                arrivalMsgSize =
                        this->getEnv()->getSources().at(iSource)->getArrivalMsgSize();
                break;
            }
        }

    }

    double meanTimeinSystem = 0, meanTimeinQueue = 0, meanNumberofMsgsinSystem =
            0, meanNumberinQueue = 0, rho = 0;

    //    cout << "Available: "
    //            << this->getEnv()->getHostCapabilities().at(
    //                    operatorMap->getHostId()).getCpu() << " Operator "
    //            << this->getEnv()->getOperators().at(operatorMap->getOperatorId())->getCPURequirement()
    //            << endl;

    double serviceRate =
            this->getEnv()->getHostCapabilities().at(operatorMap->getHostId()).getCpu()
                    / this->getEnv()->getOperators().at(
                            operatorMap->getOperatorId())->getCPURequirement();

    if (this->getEnv()->EstimateTimesandQueues(serviceRate, arrivalRate, 0,
            meanTimeinSystem, meanTimeinQueue, meanNumberofMsgsinSystem,
            meanNumberinQueue, rho)) {

        operatorMap->setInputRate(arrivalRate);
        operatorMap->setCompTime(meanTimeinSystem);
        operatorMap->setQueueSize(meanNumberofMsgsinSystem);
        if (arrivalRate
                * this->getEnv()->getOperators().at(
                        operatorMap->getOperatorId())->getCPURequirement()
                > this->getEnv()->getResidualHostCapabilities().at(
                        operatorMap->getHostId()).getCpu()) {
            return true;
        } else {
            operatorMap->setRequiredCpu(
                    arrivalRate
                            * this->getEnv()->getOperators().at(
                                    operatorMap->getOperatorId())->getCPURequirement());
        }

        if (meanNumberofMsgsinSystem * arrivalMsgSize
                + this->getEnv()->getOperators().at(
                        operatorMap->getOperatorId())->getMemoryRequirement()
                > this->getEnv()->getResidualHostCapabilities().at(
                        operatorMap->getHostId()).getMemory()) {
            return true;
        } else {
            operatorMap->setRequiredMemory(
                    meanNumberofMsgsinSystem * arrivalMsgSize
                            + this->getEnv()->getOperators().at(
                                    operatorMap->getOperatorId())->getMemoryRequirement());

        }

        operatorMap->setInputMsgSize(arrivalMsgSize);

        operatorMap->setOutputMsgSize(
                arrivalMsgSize
                        * this->getEnv()->getOperators().at(
                                operatorMap->getOperatorId())->getDataTransferRate());

        operatorMap->setOutputRate(
                (arrivalRate - meanNumberofMsgsinSystem)
                        * (1
                                - this->getEnv()->getOperators().at(
                                        operatorMap->getOperatorId())->getSelectivity()));

    } else {
        return true;
    }

    return false;
}

vector<applicationMetrics> Configuration::basicApplicationArrivalRates(
        vector<int> orderedList) {
    vector<applicationMetrics> metrics(this->getEnv()->getOperators().size());

    for (unsigned int iOperator = 0; iOperator < orderedList.size();
            iOperator++) {
        bool bSource = false;
        for (unsigned int iSource = 0;
                iSource < this->getEnv()->getSources().size(); iSource++) {
            if (this->getEnv()->getSources().at(iSource)->getOperatorId()
                    == orderedList.at(iOperator)) {

                metrics.at(orderedList.at(iOperator)).mNumberofMessages =
                        this->getEnv()->getSources().at(iSource)->getArrivalRate();

                metrics.at(orderedList.at(iOperator)).mSizeofMessages =
                        this->getEnv()->getSources().at(iSource)->getArrivalMsgSize();

                bSource = true;
                break;
            }
        }

        if (!bSource) {
            for (unsigned int iPrevious = 0;
                    iPrevious < this->getEnv()->getApplicationTopology().size();
                    iPrevious++) {
                if (this->getEnv()->getApplicationTopology().at(iPrevious)->getToOperatorId()
                        == orderedList.at(iOperator)) {
                    metrics.at(orderedList.at(iOperator)).mNumberofMessages +=
                            metrics.at(
                                    this->getEnv()->getApplicationTopology().at(
                                            iPrevious)->getFromOperatorId()).mNumberofMessages
                                    * (1
                                            - this->getEnv()->getOperators().at(
                                                    this->getEnv()->getApplicationTopology().at(
                                                            iPrevious)->getFromOperatorId())->getSelectivity());

                    metrics.at(orderedList.at(iOperator)).mSizeofMessages +=
                            metrics.at(
                                    this->getEnv()->getApplicationTopology().at(
                                            iPrevious)->getFromOperatorId()).mSizeofMessages
                                    * this->getEnv()->getOperators().at(
                                            this->getEnv()->getApplicationTopology().at(
                                                    iPrevious)->getFromOperatorId())->getDataTransferRate();
                }
            }
        }
    }

    return metrics;

}

vector<int> Configuration::OperatorOrdering() {
    vector<int> sources;
    for (unsigned int iSources = 0;
            iSources < this->getEnv()->getSources().size(); iSources++) {
        sources.push_back(
                this->getEnv()->getSources().at(iSources)->getOperatorId());
    }

    Graph *graphApp = new Graph(this->getEnv()->getOperators().size());

    for (unsigned int i = 0;
            i < this->getEnv()->getApplicationTopology().size(); ++i) {
        graphApp->addEdge(
                this->getEnv()->getApplicationTopology().at(i)->getFromOperatorId(),
                this->getEnv()->getApplicationTopology().at(i)->getToOperatorId());
    }

    vector<int> ordering = graphApp->DefineOperatorOrdering(sources);
    delete graphApp;
    graphApp = nullptr;
    return ordering;
}

void Configuration::setOrderedByHier(vector<orderListS> &orderedByHier) {
    mOrderedByHier = orderedByHier;
}

int Configuration::getConfigScaleApproach() const {
    return mConfigScaleApproach;
}

void Configuration::scaleStrategySensors(vector<int> &deployable,
        int operatorId) {

    vector<int> edgedevices;
    vector<int> clouds;

    //get current position of previous operators and  sink locations
    this->shorterListofDeployableDevices(operatorId, edgedevices, clouds);

    for (unsigned int i = 0; i < edgedevices.size(); i++) {
        deployable.push_back(edgedevices.at(i));
    }

    for (unsigned int i = 0; i < clouds.size(); i++) {
        deployable.push_back(clouds.at(i));
    }

    edgedevices.clear();
    clouds.clear();

}

void Configuration::shorterListofDeployableDevices(int operatorId,
        vector<int> &edgedevices, vector<int> &clouds) {
    double arrivalRate = 0, arrivalMsgSize = 0;

    //The arrival rate metrics are evaluated since the operator can have several
    //  upstream operators. If so, the algorithm summarizes all upstream operators.
    this->getEnv()->obtaingArrivalMetrics(operatorId, arrivalRate,
            arrivalMsgSize);

    double reqCPU =
            arrivalRate
                    * this->getEnv()->getOperators().at(operatorId)->getCPURequirement();

    double reqBdw = arrivalRate * arrivalMsgSize * 8;

    //Get details from previous operators
    for (unsigned int i = 0;
            i < this->getEnv()->getApplicationTopology().size(); ++i) {
        if (this->getEnv()->getApplicationTopology().at(i)->getToOperatorId()
                == operatorId) {

            for (unsigned int j = 0;
                    j < this->getEnv()->getOperatorMapping().size(); j++) {
                if (this->getEnv()->getOperatorMapping().at(j)->getOperatorId()
                        == this->getEnv()->getApplicationTopology().at(i)->getFromOperatorId()) {
                    if (this->getEnv()->getResources().at(
                            this->getEnv()->getOperatorMapping().at(j)->getHostId())->getGatewayId()
                            > 0) {

                        //in-situ
                        this->getBestInSituandInTransitDevices(
                                this->getEnv()->getResources().at(
                                        this->getEnv()->getOperatorMapping().at(
                                                j)->getHostId())->getGatewayId(),
                                edgedevices, clouds, true, reqCPU);

                        //the device with previous operators also can host the current deployment
                        if (this->getEnv()->getResidualHostCapabilities().at(
                                this->getEnv()->getOperatorMapping().at(j)->getHostId()).getCpu()
                                >= reqCPU) {
                            bool isSink = false;
                            for (unsigned int k = 0;
                                    k < this->getEnv()->getSinks().size();
                                    k++) {

                                if (this->getEnv()->getSinks().at(k)->getHostId()
                                        == this->getEnv()->getOperatorMapping().at(
                                                j)->getHostId()) {
                                    isSink = true;
                                    break;
                                }

                            }

                            if (!isSink) {
                                edgedevices.push_back(
                                        this->getEnv()->getOperatorMapping().at(
                                                j)->getHostId());
                            }
                        }
                    } else {
                        //in-situ
                        clouds.push_back(
                                this->getEnv()->getResources().at(
                                        this->getEnv()->getOperatorMapping().at(
                                                j)->getHostId())->getId());
                    }

                }
            }

        }
    }

    //get details from sinks and their placements --todo check rules
    for (unsigned int i = 0; i < this->getEnv()->getApplicationPaths().size();
            ++i) {
        for (unsigned int j = 0;
                j < this->getEnv()->getApplicationPaths().at(i).size(); ++j) {
            if (this->getEnv()->getApplicationPaths().at(i).at(j)
                    == operatorId) {

                for (unsigned int k = 0; k < this->getEnv()->getSinks().size();
                        k++) {

                    if (this->getEnv()->getSinks().at(k)->getOperatorId()
                            == this->getEnv()->getApplicationPaths().at(i).at(
                                    this->getEnv()->getApplicationPaths().at(i).size()
                                            - 1)) {

                        if (this->getEnv()->getResources().at(
                                this->getEnv()->getSinks().at(k)->getHostId())->getGatewayId()
                                > 0) {
                            //in-situ - Do I need to include in-transit resources for sinks?
                            this->getBestInSituandInTransitDevices(
                                    this->getEnv()->getResources().at(
                                            this->getEnv()->getSinks().at(k)->getHostId())->getGatewayId(),
                                    edgedevices, clouds, false, reqCPU);

                            //                            //the sink host is included to the list
                            //                            edgedevices.push_back(
                            //                                    this->getEnv()->getSinks().at(k)->getHostId());

                        } else {
                            clouds.push_back(
                                    this->getEnv()->getSinks().at(k)->getHostId());
                        }
                    }

                }

                break;
            }
        }

    }

    //Get unique values
    if (edgedevices.size() > 0) {
        sort(edgedevices.begin(), edgedevices.end());
        edgedevices.erase(unique(edgedevices.begin(), edgedevices.end()),
                edgedevices.end());
    }

    if (clouds.size() > 0) {
        sort(clouds.begin(), clouds.end());
        clouds.erase(unique(clouds.begin(), clouds.end()), clouds.end());
    }

    //check capabilities

}

void Configuration::getBestInSituandInTransitDevices(int gtw,
        vector<int> &edgedevices, vector<int> &clouds, bool bInTransit,
        double reqCPU) {
    double dLatSitu = std::numeric_limits<double>::max();
    double dCPUSitu = std::numeric_limits<double>::min();
    int iLatSitu = -1;
    int iCPUSitu = -1;

    double dLatTransit = std::numeric_limits<double>::max();
    double dCPUTransit = std::numeric_limits<double>::min();
    int iLatTransit = -1;
    int iCPUTransit = -1;

    double dLatCloud = std::numeric_limits<double>::max();
    double dCPUCloud = std::numeric_limits<double>::min();
    int iLatCloud = -1;
    int iCPUCloud = -1;

    for (unsigned int i = 0; i < this->getEnv()->getNetworkTopology().size();
            i++) {

        if (this->getEnv()->getNetworkTopology().at(i)->getSourceId() == gtw) {

            //In-situ
            if (this->getEnv()->getResources().at(
                    this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getType()
                    == Patterns::DeviceType::Sensor) {
                bool isSink = false;
                for (unsigned int k = 0; k < this->getEnv()->getSinks().size();
                        k++) {
                    if (this->getEnv()->getResources().at(
                            this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getId()
                            == this->getEnv()->getSinks().at(k)->getHostId()) {
                        isSink = true;
                        break;
                    }

                }

                //Lat Based
                if (!isSink) {
                    if (this->getEnv()->getResidualHostCapabilities().at(
                            this->getEnv()->getResources().at(
                                    this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getId()).getCpu()
                            >= reqCPU) {
                        if (dLatSitu
                                > this->getEnv()->getLinks().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                i)->getLinkId())->getDelay()) {
                            dLatSitu =
                                    this->getEnv()->getLinks().at(
                                            this->getEnv()->getNetworkTopology().at(
                                                    i)->getLinkId())->getDelay();
                            iLatSitu =
                                    this->getEnv()->getResources().at(
                                            this->getEnv()->getNetworkTopology().at(
                                                    i)->getDestinationId())->getId();
                        }
                        //                //CPU Based
                        //                if (dCPUSitu
                        //                        < this->getEnv()->getResidualHostCapabilities().at(
                        //                                this->getEnv()->getResources().at(
                        //                                        this->getEnv()->getNetworkTopology().at(
                        //                                                i)->getDestinationId())->getId()).getCpu()) {
                        //                    dCPUSitu =
                        //                            this->getEnv()->getResidualHostCapabilities().at(
                        //                                    this->getEnv()->getResources().at(
                        //                                            this->getEnv()->getNetworkTopology().at(
                        //                                                    i)->getDestinationId())->getId()).getCpu();
                        //                    iLatSitu =
                        //                            this->getEnv()->getResources().at(
                        //                                    this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getId();
                        //                }
                    }
                }
            }

            if (bInTransit) {
                //In-transit Lat based
                if (this->getEnv()->getResources().at(
                        this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getType()
                        != Patterns::DeviceType::Sensor) {
                    //Lat Based
                    if (dLatTransit
                            > this->getEnv()->getLinks().at(
                                    this->getEnv()->getNetworkTopology().at(i)->getLinkId())->getDelay()) {
                        dLatTransit =
                                this->getEnv()->getLinks().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                i)->getLinkId())->getDelay();
                        iLatTransit =
                                this->getEnv()->getResources().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                i)->getDestinationId())->getId();
                    }
                }

                if (this->getEnv()->getResources().at(
                        this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getType()
                        == Patterns::DeviceType::Cloud) {
                    //Lat Based
                    if (dLatCloud
                            > this->getEnv()->getLinks().at(
                                    this->getEnv()->getNetworkTopology().at(i)->getLinkId())->getDelay()) {
                        dLatCloud =
                                this->getEnv()->getLinks().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                i)->getLinkId())->getDelay();
                        iLatCloud =
                                this->getEnv()->getResources().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                i)->getDestinationId())->getId();
                    }
                }
            }
        }
    }

    if (iLatTransit > 0 and bInTransit) {
        double dLatTransit = std::numeric_limits<double>::max();
        int gtw2 = iLatTransit;
        for (unsigned int i = 0;
                i < this->getEnv()->getNetworkTopology().size(); i++) {
            if (this->getEnv()->getNetworkTopology().at(i)->getSourceId()
                    == gtw2) {
                //In-situ
                if (this->getEnv()->getResources().at(
                        this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getType()
                        == Patterns::DeviceType::Sensor
                        and dLatTransit
                                < this->getEnv()->getLinks().at(
                                        this->getEnv()->getNetworkTopology().at(
                                                i)->getLinkId())->getDelay()) {
                    if (this->getEnv()->getResidualHostCapabilities().at(
                            this->getEnv()->getResources().at(
                                    this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getId()).getCpu()
                            >= reqCPU) {
                        bool isSink = false;
                        for (unsigned int k = 0;
                                k < this->getEnv()->getSinks().size(); k++) {
                            if (this->getEnv()->getResources().at(
                                    this->getEnv()->getNetworkTopology().at(i)->getDestinationId())->getId()
                                    == this->getEnv()->getSinks().at(k)->getHostId()) {
                                isSink = true;
                                break;
                            }

                        }

                        if (!isSink) {
                            dLatTransit =
                                    this->getEnv()->getLinks().at(
                                            this->getEnv()->getNetworkTopology().at(
                                                    i)->getLinkId())->getDelay();
                            iLatTransit =
                                    this->getEnv()->getResources().at(
                                            this->getEnv()->getNetworkTopology().at(
                                                    i)->getDestinationId())->getId();
                        }
                    }

                }

            }
        }
    }

    if (iLatSitu > 0) {
        edgedevices.push_back(iLatSitu);
    }
    if (iLatTransit > 0) {
        edgedevices.push_back(iLatTransit);
    }
    if (iLatCloud > 0) {
        clouds.push_back(iLatCloud);
    }

}

void Configuration::setConfigScaleApproach(int configScaleApproach) {
    mConfigScaleApproach = configScaleApproach;
}

const high_resolution_clock::time_point& Configuration::getIntialTime() const {
    return mIntialTime;
}

void Configuration::setIntialTime(
        const high_resolution_clock::time_point &intialTime) {
    mIntialTime = intialTime;
}

double Configuration::getLimitTime() const {
    return mLimitTime;
}

void Configuration::isTimeExceeded() {
    duration<double> iteration_duration = high_resolution_clock::now()
            - this->getIntialTime();
    if (iteration_duration.count() > this->getLimitTime()) {
        throw cRuntimeError("Time exceeded!");
    }
}

void Configuration::setLimitTime(double limitTime) {
    mLimitTime = limitTime;
}

}


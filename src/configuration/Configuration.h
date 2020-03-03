#ifndef CONFIGURATION_CONFIGURATION_H_
#define CONFIGURATION_CONFIGURATION_H_
#include <omnetpp.h>
#include <unordered_map>
#include <cassert>
#include <map>
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <chrono>
#include "../util/graphS/Graph.h"
#include "goalrate/GoalRate.h"
#include "taneja/resource/ResourceBase.h"

#include "../env/Env.h"

/*
 #define IL_STD

 #include <ilcplex/ilocplex.h>
 #include <ilcp/cp.h>
 ILOSTLBEGIN
 */

using namespace std;
using namespace omnetpp;
using namespace chrono;

namespace fogstream {

struct regionsDep {
    vector<int> from;
    vector<int> to;
};

struct orderListS {
    double level;
    int operatorID;
};

struct applicationMetrics {
    double mNumberofMessages = 0;
    double mSizeofMessages = 0;
};

class Configuration {
private:
    //Functions and procedures shared by all strategies
    vector<regionsDep> getRegions();
    vector<int> orderByRegion(vector<regionsDep> regions);
    void ExecuteStrategies(vector<int> orderedList, vector<int> resourceList,
            vector<regionsDep> regions, vector<int> cloudDevices, int strategy,
            bool isUseSlot);

    void setDstBranches(vector<int> cloudDevices, vector<regionsDep> regions);

    vector<int> defineDeployableHosts(vector<int>& cloudIDs);

    void determineOperatorPlacement(GoalRate* rate, int strategy,
            int operatorId, vector<int> tanejaDeployment, bool isUseSlot);

    /***********************************************************************/
    vector<int> LowerBoundTaneja(vector<int> orderedList, bool isUseSlot);
    int lowerBound(vector<ResourceBase*> nodes, OperatorData* tasksTemp,
            int &low, int high, double cpuRequirement, double btwReq);

    vector<applicationMetrics> basicApplicationArrivalRates(
            vector<int> orderedList);
    //    void setupLinksSpecial(vector<int> orderedList);
    /***********************************************************************/

    void hierarchySetArea(vector<int> branch, vector<regionsDep> regions,
            int Cloud);
    //    void hierarchySearch(vector<int> branch, vector<regionsDep> regions,
    //            Graph graphApp);
    void hierarchyOrder(vector<int> branch, vector<regionsDep> regions,
            int level);

    void placementOverheads(GoalRate* rate, int operatorId, int hostId,
            bool onlyComputation, bool isSource);

    int DeviceTypeStrategy(int operatorId);

    bool estimateCommucation(int OperatorId, int HostId,
            vector<OperatorMapping*> &commToPreviousOperators,
            vector<OperatorConnectionMapping*> &linkMap);
    bool estimateComputation(OperatorMapping* operatorMap,
            vector<OperatorMapping*> previousMappings, bool isSource);

    /*********************************************************************/
    //Sort operator following upstream and downstream operators
    vector<DestinationBranches> RegionBranchesFromDataSoures(
            vector<regionsDep> regions);

    vector<int> OperatorOrdering();

    void scaleStrategySensors(vector<int> &deployable, int operatorId);
    void shorterListofDeployableDevices(int operatorId,
            vector<int>& edgedevices, vector<int>& clouds);
    void getBestInSituandInTransitDevices(int gtw, vector<int>& edgedevices,
            vector<int>& clouds, bool bInTransit, double reqCPU);

    void isTimeExceeded();
public:
    Configuration(Env* &env);
    virtual ~Configuration();

    //Functions and procedures shared by all strategies
    int setupEnvironment(int strategy, bool isUseSlots,
            int configScaleApproach);

    Env*& getEnv();
    void setEnv(Env*& env);
    vector<orderListS>& getOrderedByHier();
    void setOrderedByHier(vector<orderListS>& orderedByHier);
    int getConfigScaleApproach() const;
    void setConfigScaleApproach(int configScaleApproach);
    const high_resolution_clock::time_point& getIntialTime() const;
    void setIntialTime(const high_resolution_clock::time_point& intialTime);
    double getLimitTime() const;
    void setLimitTime(double limitTime);

protected:
    Env* env;
    vector<orderListS> mOrderedByHier;
    int mConfigScaleApproach;
    double mLimitTime = 60;
    high_resolution_clock::time_point mIntialTime;

};

} /* namespace fogstream */

#endif /* CONFIGURATION_CONFIGURATION_H_ */

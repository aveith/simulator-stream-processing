#ifndef CONFIGURATION_GOALRATE_GOALRATE_H_
#define CONFIGURATION_GOALRATE_GOALRATE_H_
#include <omnetpp.h>

#include "../../env/objects/applicationmapping/operatorconnectionmapping/OperatorConnectionMapping.h"
#include "../../env/objects/applicationmapping/operatormapping/OperatorMapping.h"

using namespace std;
using namespace omnetpp;

namespace fogstream {

class GoalRate {
public:
    GoalRate();
    GoalRate(OperatorMapping*& currentOperatorMapping,
            vector<OperatorMapping*>& previousOperatorMapping,
            vector<OperatorConnectionMapping*>& linkMapping, bool constrained,
            double totalRate);

    virtual ~GoalRate();
    void cloneObject(GoalRate* rate);

    bool isConstrained() const;
    void setConstrained(bool constrained);
    OperatorMapping*& getCurrentOperatorMapping();
    void setCurrentOperatorMapping(OperatorMapping* currentOperatorMapping);
    vector<OperatorMapping*>& getPreviousOperatorMapping();
    void setPreviousOperatorMapping(
            const vector<OperatorMapping*>& previousOperatorMapping);
    double getTotalRate() const;
    void setTotalRate(double totalRate);
    vector<OperatorConnectionMapping*>& getLinkMapping();
    void setLinkMapping(const vector<OperatorConnectionMapping*>& linkMapping);
    void clear();

protected:
    OperatorMapping* mCurrentOperatorMapping = nullptr; //The current operator is considered when estimating the computation
    vector<OperatorMapping*> mPreviousOperatorMapping; //The previous operators are considered when estimating the communication
    vector<OperatorConnectionMapping*> mLinkMapping;
    bool mConstrained = false;
    double mTotalRate = 0;
};

} /* namespace fogstream */

#endif /* CONFIGURATION_GOALRATE_GOALRATE_H_ */

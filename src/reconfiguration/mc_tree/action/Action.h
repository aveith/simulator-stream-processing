
#ifndef RECONFIGURATION_MC_TREE_ACTION_ACTION_H_
#define RECONFIGURATION_MC_TREE_ACTION_ACTION_H_
#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class Action {
public:
    Action();
    Action(int hostId, int originHostId);
    virtual ~Action();

    int getHostId() const;
    void setHostId(int hostId);
    int getOriginHostId() const;
    void setOriginHostId(int originHostId);
    bool isTaken() const;
    void setTaken(bool taken);
    string toString();

protected:
    int mHostID = -1;
    int mOriginHostID = -1;
    bool mTaken = false;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MC_TREE_ACTION_ACTION_H_ */

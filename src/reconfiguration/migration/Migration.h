#ifndef RECONFIGURATION_MIGRATION_MIGRATION_H_
#define RECONFIGURATION_MIGRATION_MIGRATION_H_
#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class MigrationPlan {
public:
    MigrationPlan(int linkId, int fromHostId, int toHostId, double requiredTime);
    virtual ~MigrationPlan();
    int getFromHostId() const;
    void setFromHostId(int fromHostId);
    int getLinkId() const;
    void setLinkId(int linkId);
    double getRequiredTime() const;
    void setRequiredTime(double requiredTime);
    int getToHostId() const;
    void setToHostId(int toHostId);

protected:
    int nLinkId;
    int nFromHostId;
    int nToHostId;
    double nRequiredTime;
};

class Migration {
public:
    Migration(int fromHostId, int toHostId, int operatorId);
    virtual ~Migration();

    int getFromHostId() const;
    void setFromHostId(int fromHostId);
    vector<MigrationPlan*>& getMigrationPlan();
    void setMigrationPlan(vector<MigrationPlan*>& migrationPlan);
    int getOperatorId() const;
    void setOperatorId(int operatorId);
    int getToHostId() const;
    void setToHostId(int toHostId);

protected:
    int mFromHostId;
    int mToHostId;
    int mOperatorId;
    vector<MigrationPlan*> mMigrationPlan;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MIGRATION_MIGRATION_H_ */

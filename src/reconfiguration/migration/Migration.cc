#include "Migration.h"

namespace fogstream {

Migration::Migration(int fromHostId, int toHostId, int operatorId) {
    this->setFromHostId(fromHostId);
    this->setToHostId(toHostId);
    this->setOperatorId(operatorId);

}

Migration::~Migration() {
    // TODO Auto-generated destructor stub
}

int Migration::getFromHostId() const {
    return mFromHostId;
}

void Migration::setFromHostId(int fromHostId) {
    mFromHostId = fromHostId;
}

vector<MigrationPlan*>& Migration::getMigrationPlan() {
    return mMigrationPlan;
}

void Migration::setMigrationPlan(vector<MigrationPlan*>& migrationPlan) {
    mMigrationPlan = migrationPlan;
}

int Migration::getOperatorId() const {
    return mOperatorId;
}

void Migration::setOperatorId(int operatorId) {
    mOperatorId = operatorId;
}

int Migration::getToHostId() const {
    return mToHostId;
}

void Migration::setToHostId(int toHostId) {
    mToHostId = toHostId;
}
/************************************************************************************/
/*                     MigrationPlan                                                */
/************************************************************************************/
MigrationPlan::MigrationPlan(int linkId, int fromHostId, int toHostId, double requiredTime) {
    this->setLinkId(linkId);
    this->setFromHostId(fromHostId);
    this->setToHostId(toHostId);
    this->setRequiredTime(requiredTime);
}

MigrationPlan::~MigrationPlan() {
}

int MigrationPlan::getFromHostId() const {
    return nFromHostId;
}

void MigrationPlan::setFromHostId(int fromHostId) {
    nFromHostId = fromHostId;
}

int MigrationPlan::getLinkId() const {
    return nLinkId;
}

void MigrationPlan::setLinkId(int linkId) {
    nLinkId = linkId;
}

double MigrationPlan::getRequiredTime() const {
    return nRequiredTime;
}

void MigrationPlan::setRequiredTime(double requiredTime) {
    nRequiredTime = requiredTime;
}

int MigrationPlan::getToHostId() const {
    return nToHostId;
}

void MigrationPlan::setToHostId(int toHostId) {
    nToHostId = toHostId;
}

} /* namespace fogstream */


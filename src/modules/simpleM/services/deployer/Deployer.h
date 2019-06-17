#ifndef __FOGSTREAM_DEPLOYER_H_
#define __FOGSTREAM_DEPLOYER_H_

#include <omnetpp.h>
#include <vector>

using namespace std;
using namespace omnetpp;

namespace fogstream {

/**
 * TODO - Generated class
 */
class Deployer : public cSimpleModule
{
public:
    vector<vector<int> >& getParticipatingPaths();
    void setParticipatingPaths(const vector<vector<int> >& participatingPaths);
    int getSentRegs() const;
    void setSentRegs(int sentRegs);

private:
    void initialize();
    void handleMessage(cMessage *msg);
    int createSpecificModule(const char* moduletypee, int path);
    int mSentRegs = 0;

protected:
    vector<vector<int>> mParticipatingPaths;

};

} //namespace

#endif

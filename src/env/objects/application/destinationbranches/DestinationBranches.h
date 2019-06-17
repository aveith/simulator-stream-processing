#ifndef ENV_OBJECTS_APPLICATION_DESTINATIONBRANCHES_DESTINATIONBRANCHES_H_
#define ENV_OBJECTS_APPLICATION_DESTINATIONBRANCHES_DESTINATIONBRANCHES_H_
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class DestinationBranches {
public:
    DestinationBranches(int type, vector<int>& applicationBranch);
    virtual ~DestinationBranches();
    const vector<int>& getApplicationBranch() const;
    void setApplicationBranch(const vector<int>& applicationBranch);
    int getType() const;
    void setType(int type);

protected:
    int mType;
    vector<int> mApplicationBranch;
};

} /* namespace fogstream */

#endif /* ENV_OBJECTS_APPLICATION_DESTINATIONBRANCHES_DESTINATIONBRANCHES_H_ */

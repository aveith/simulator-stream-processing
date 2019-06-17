#ifndef RECONFIGURATION_MAPPING_EDGE_MAPEDGE_H_
#define RECONFIGURATION_MAPPING_EDGE_MAPEDGE_H_

#include <vector>
#include <omnetpp.h>

using namespace std;
using namespace omnetpp;

namespace fogstream {

class MapEdge {
public:
    MapEdge();
    MapEdge(int fromOperatorId, int toOperatorId, int fromHostId, int toHostId, int linkId);

    virtual ~MapEdge();

    int getFromHostId() const;
    void setFromHostId(int fromHostId);
    int getFromOperatorId() const;
    void setFromOperatorId(int fromTaskId);
    int getLinkId() const;
    void setLinkId(int linkId);
    int getToHostId() const;
    void setToHostId(int toHostId);
    int getToOperatorId() const;
    void setToOperatorId(int toTaskId);
    string toString();

protected:
    int mFromOperatorID;
    int mToOperatorID;
    int mFromHostID;
    int mToHostID;
    int mLinkID;
};

} /* namespace fogstream */

#endif /* RECONFIGURATION_MAPPING_EDGE_MAPEDGE_H_ */

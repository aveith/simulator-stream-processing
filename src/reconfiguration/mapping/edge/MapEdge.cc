#include "MapEdge.h"

namespace fogstream {

MapEdge::MapEdge() {
    // TODO Auto-generated constructor stub

}
MapEdge::MapEdge(int fromOperatorId, int toOperatorId, int fromHostId,
        int toHostId, int linkId) {
    this->setFromOperatorId(fromOperatorId);
    this->setToOperatorId(toOperatorId);
    this->setFromHostId(fromHostId);
    this->setToHostId(toHostId);
    this->setLinkId(linkId);
}

MapEdge::~MapEdge() {
    // TODO Auto-generated destructor stub
}

int MapEdge::getFromHostId() const {
    return mFromHostID;
}

void MapEdge::setFromHostId(int fromHostId) {
    mFromHostID = fromHostId;
}

int MapEdge::getFromOperatorId() const {
    return mFromOperatorID;
}

void MapEdge::setFromOperatorId(int fromOperatorId) {
    mFromOperatorID = fromOperatorId;
}

int MapEdge::getLinkId() const {
    return mLinkID;
}

void MapEdge::setLinkId(int linkId) {
    mLinkID = linkId;
}

int MapEdge::getToHostId() const {
    return mToHostID;
}

void MapEdge::setToHostId(int toHostId) {
    mToHostID = toHostId;
}

int MapEdge::getToOperatorId() const {
    return mToOperatorID;
}

void MapEdge::setToOperatorId(int toOperatorId) {
    mToOperatorID = toOperatorId;
}

string MapEdge::toString() {
    return "MapEdge;" + to_string(this->getFromOperatorId()) + ";"
            + to_string(this->getFromHostId()) + ";"
            + to_string(this->getToOperatorId()) + ";"
            + to_string(this->getToHostId()) + ";"
            + to_string(this->getLinkId());
}

} /* namespace fogstream */


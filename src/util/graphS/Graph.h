#ifndef UTIL_GRAPHS_GRAPH_H_
#define UTIL_GRAPHS_GRAPH_H_
#include <omnetpp.h>
#include <algorithm>
#include <numeric>
#include <vector>

using namespace std;
using namespace omnetpp;

namespace fogstream {

struct operatorCharacteristics {
    int id;
    int characteristic;
};

class Graph {
private:
    int vertices;
    list<int>* adj;
    vector<vector<int>> paths;
    vector<operatorCharacteristics> operatorsCharac;

    vector<vector<int>> regions;
    void printAllPathsUtil(int, int, bool[], int[], int &);
    bool hasPendency(int vertex, vector<int>* ordering);

public:
    Graph(int V);
    virtual ~Graph();
    void addEdge(int V, int w);
    void printAllPaths(int s, int d);

    void printVector();
    void setVectorCharacteristic(int value, int characteristic);
    int getVectorCharacteristic(int value);

    vector<vector<int>> getPaths();

    vector<vector<int>> splitRegions();

    vector<int> DefineOperatorOrdering(vector<int> sources);
};

} /* namespace fogstream */

#endif /* UTIL_GRAPHS_GRAPH_H_ */

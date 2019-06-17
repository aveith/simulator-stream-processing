#include "Graph.h"
#include <iostream>
#include <list>
#include <algorithm>
#include <vector>

#include "../../configuration/Configuration.h"
#include "../Patterns.h"
namespace fogstream {

Graph::~Graph() {
    adj->clear();
    adj = NULL;
}

Graph::Graph(int V) {
    this->vertices = V;
    adj = new list<int> [V];

    for (int var = 0; var < V; ++var) {
        operatorsCharac.push_back( { var + 1, Patterns::Sequential });
    }
}

void Graph::addEdge(int V, int w) {
    adj[V].push_back(w);
}

// Prints all paths from 's' to 'd'
void Graph::printAllPaths(int s, int d) {
    this->paths.clear();

    // Mark all the vertices as not visited
    bool *visited = new bool[vertices];

    // Create an array to store paths
    int *path = new int[vertices];
    int path_index = 0; // Initialize path[] as empty

    // Initialize all vertices as not visited
    for (int i = 0; i < vertices; i++)
        visited[i] = false;

    // Call the recursive helper function to print all paths
    printAllPathsUtil(s, d, visited, path, path_index);

    delete visited;
    delete path;

}

// A recursive function to print all paths from 'u' to 'd'.
// visited[] keeps track of vertices in current path.
// path[] stores actual vertices and path_index is current
// index in path[]
void Graph::printAllPathsUtil(int u, int d, bool visited[], int path[],
        int &path_index) {

    // Mark the current node and store it in path[]
    visited[u] = true;
    path[path_index] = u;
    path_index++;

    // If current vertex is same as destination, then print
    // current path[]
    if (u == d) {
        vector<int> temp;
        for (unsigned int i = 0; i < path_index; i++) {
            temp.push_back(path[i]);
        }
        paths.push_back(temp);

    } else { // If current vertex is not destination
        // Recur for all the vertices adjacent to current vertex
        //list<int>::iterator i;
        for (auto i = adj[u].begin(); i != adj[u].end(); ++i) {
            if (!visited[*i]) {
                printAllPathsUtil(*i, d, visited, path, path_index);
            }
        }
    }

    // Remove current vertex from path[] and mark it as unvisited
    path_index--;
    visited[u] = false;
}

vector<vector<int>> Graph::getPaths() {
    return this->paths;
}

void Graph::printVector() {
    //To access values
    for (vector<vector<int> >::iterator it = paths.begin(); it != paths.end();
            ++it) {
        //it is now a pointer to a vector<int>
        for (vector<int>::iterator jt = it->begin(); jt != it->end(); ++jt) {
            // jt is now a pointer to an integer.
            cout << *jt << " ";
        }
        cout << endl;
    }
}

void Graph::setVectorCharacteristic(int value, int characteristic) {
    bool bExist = false;
    for (auto i = this->operatorsCharac.begin();
            i != this->operatorsCharac.end(); ++i) {
        if (i->id == value) {
            bExist = true;
            i->characteristic = characteristic;
            break;
        }
    }

    if (!bExist) {
        this->operatorsCharac.push_back( { value, characteristic });
    }

}

int Graph::getVectorCharacteristic(int value) {
    int characteristic;

    for (auto i = this->operatorsCharac.begin();
            i != this->operatorsCharac.end(); ++i) {
        if (i->id == value) {
            return i->characteristic;
        }
    }

    return characteristic;
}

vector<vector<int>> Graph::splitRegions() {
    vector<int> temp;

    vector<vector<int>> subRegions;

    for (unsigned int p = 0; p < this->paths.size(); ++p) {
        for (unsigned int v = 0; v < this->paths[p].size(); ++v) {
            if (this->getVectorCharacteristic(this->paths[p][v])
                    != Patterns::JokerSource) {
                temp.push_back(this->paths[p][v]);
            }

            if (this->getVectorCharacteristic(this->paths[p][v])
                    == Patterns::ForkBranch
                    || this->getVectorCharacteristic(this->paths[p][v])
                            == Patterns::ForkConcurrent
                    || this->getVectorCharacteristic(this->paths[p][v])
                            == Patterns::JoinBranch
                    || this->getVectorCharacteristic(this->paths[p][v])
                            == Patterns::Join
                    || this->getVectorCharacteristic(this->paths[p][v])
                            == Patterns::Joker
                    || this->getVectorCharacteristic(this->paths[p][v])
                            == Patterns::JokerSource
                    || this->getVectorCharacteristic(this->paths[p][v])
                            == Patterns::JoinConcurrent) {

                if (find(subRegions.begin(), subRegions.end(), temp)
                        == subRegions.end()) {
                    if (temp.size() > 0) {
                        subRegions.push_back(temp);
                    }
                }

                temp.clear();
                if (this->getVectorCharacteristic(this->paths[p][v])
                        != Patterns::Joker
                        && this->getVectorCharacteristic(this->paths[p][v])
                                != Patterns::JokerSource) {
                    temp.push_back(this->paths[p][v]);
                }
            }
        }

        if (temp.size() > 0) {
            subRegions.push_back(temp);
            temp.clear();
        }
    }

    return subRegions;
}

vector<int> Graph::DefineOperatorOrdering(vector<int> sources) {
    vector<int> ordering;

    list<int>* queue = new list<int>;
    for (unsigned int iSource = 0; iSource < sources.size(); iSource++) {
        for (auto i = adj[sources.at(iSource)].begin();
                i != adj[sources.at(iSource)].end(); ++i) {
            queue->push_back(*i);
        }
        ordering.push_back(sources.at(iSource));
    }

    while (!queue->empty()) {
        // Dequeue a vertex from queue
        int vertex = queue->front();
        queue->pop_front();

        if (!this->hasPendency(vertex, &ordering)) {
            bool exists = false;
            for (unsigned int iOpe = 0; iOpe < ordering.size(); iOpe++) {
                if (ordering.at(iOpe) == vertex) {
                    exists = true;
                    break;
                }
            }
            if (!exists){
                ordering.push_back(vertex);
                for (auto to = adj[vertex].begin(); to != adj[vertex].end(); to++) {
                    queue->push_back(*to);
                }
            }
        } else {
            queue->push_back(vertex);
        }
    }

    delete queue;
    return ordering;
}

bool Graph::hasPendency(int vertex, vector<int>* ordering) {
    for (int from = 0; from < this->vertices; from++) {
        for (auto to = adj[from].begin(); to != adj[from].end(); to++) {
            if (*to == vertex) {
                bool valid = false;
                for (unsigned int saved = 0; saved < ordering->size();
                        saved++) {

                    if ((int) from == ordering->at(saved)) {
                        valid = true;
                        break;
                    }

                }

                if (!valid)
                    return true;
            }
        }
    }
    return false;
}

} /* namespace fogstream */


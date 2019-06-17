//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Patterns.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include<sstream>

namespace fogstream {

std::string getDeploymentMsgName(int counter){
    std::string name;
    name += Patterns::BASE_DEPLOY;
    name += "-";
    name += std::to_string(counter);

    return name;
}

std::string getEventName(int counter, int source){
    std::string name;
    name += Patterns::BASE_TOPIC;
    name += "-";
    name += std::to_string(counter);
    name += ".";
    name += std::to_string(source);

    return name;
}

std::string getOperatorName(const char* appName, int operatorID, int fissionID){
    std::string name;

    name += Patterns::BASE_OPERATOR;
    name += "-";
    name += appName;
    name += "-";
    name += std::to_string(operatorID);
    name += "-";
    name += std::to_string(fissionID);

    return name;
}

std::string getQueueVertexHost(const char* hostName){
    std::string name;
    name += Patterns::MSG_VERTICES;
    name += "-";
    name += hostName;

    return name;
}

std::string getQueueVertexDep(const char* hostName){
    std::string name;
    name += Patterns::MSG_VERTICESDEP;
    name += "-";
    name += hostName;

    return name;
}

std::string getQueueEdgeHost(const char* hostName){
    std::string name;

    name += Patterns::MSG_EDGES;
    name += "-";
    name += hostName;

    return name;
}

void freeMemory(cModule* node, int memory){
    if (node->hasPar("avaliableMem")){
        double availableMem = node->par("avaliableMem");
        node->par("avaliableMem").setDoubleValue(availableMem + memory);
    }
}

void allocateMemory(cModule* node, int memory){
    if (node->hasPar("avaliableMem")){
        double availableMem = node->par("avaliableMem");
        node->par("avaliableMem").setDoubleValue(availableMem - memory);
    }
}

bool is_number(std::string &str){
    std::string::const_iterator it = str.begin();
        while (it != str.end() && std::isdigit(*it)) ++it;
        return !str.empty() && it == str.end();
}

string find_and_replace(string source, string find, string replace){
    string result = source;
    for(string::size_type i = 0; (i = result.find(find, i)) != string::npos;){
        result.replace(i, find.length(), replace);
        i += replace.length();
    }

    return result;
}


} /* namespace fogstream */

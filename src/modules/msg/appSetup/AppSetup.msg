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

namespace fogstream;

//
// TODO generated message class
//
message AppSetup {
    string dstModule;
    string moduleName;
    string appName;
    int operatorId;
    int fissionId;
    double selectivity;
    double dataReduction;
	int type;
	double stateIncreaseRate;	
	double memoryCost;
    double cpuCost;
}

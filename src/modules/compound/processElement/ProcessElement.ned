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

package fogstream.modules.compound.processElement;

import fogstream.modules.simpleM.operator.OperatorMsg;
import fogstream.modules.simpleM.services.MsgQueue;


module ProcessElement
{
    parameters:        
        volatile string appName = default("App-0");
        volatile int operatorId = default(0);
        volatile int fissionId = default(0);
        
        volatile double memoryCost = default(0);
        volatile double cpuCost = default(0);
        @display("i=block/transport_l;t=Mem:$memoryCost CPU:$cpuCost");
    
    gates:
        input Queue_port[];
        output PE_port[];
        
    submodules:
        msgQueue: MsgQueue {
            @display("p=50,67");
        }
        operatorMsg: OperatorMsg {
            @display("p=158,67");
        }
                    
    connections:
        msgQueue.outIn++ --> operatorMsg.queuePreviousIn++;
        operatorMsg.queuePreviousOut++ --> msgQueue.inIn++;        
        
        for i=0..sizeof(Queue_port)-1 {
            Queue_port[i] --> msgQueue.inIn++;
        }
          
                
}

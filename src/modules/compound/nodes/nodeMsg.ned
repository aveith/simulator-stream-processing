
package fogstream.modules.compound.nodes;


import fogstream.modules.simpleM.nodeManager.NodeManager;
import fogstream.modules.simpleM.nodeOutput.NodeOutput;


module NodeMsg
{
    parameters:        
        int id = default(0);
        volatile double avaliableCpu = default(10000000);
        volatile double avaliableMem = default(10000000); //bytes        
        
        double totalCpu = default(10000000);
        double totalMem = default(10000000);
        bool logData = default(false);
        
        //@display("i=abstract/router;t=Avail.:\nMem:$avaliableMem Bytes\nCPU:$avaliableCpu MIPS");
        @display("i=abstract/router");
        
    gates:
        input inExt[];
        output outExt[];
        
    submodules:
        nodeManager: NodeManager {
            @display("p=58,65");
        }
        nodeOutput: NodeOutput {
            @display("p=188,65");
        }


    connections:
        for i=0..sizeof(inExt)-1 {
            inExt[i] --> nodeManager.inEx++;
        }
        for i=0..sizeof(outExt)-1 {
            nodeOutput.outEx++ --> outExt[i];
        }
        nodeManager.outIn++ --> nodeOutput.inIn++;
}

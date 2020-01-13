package fogstream.modules.simpleM.services;

simple MsgQueue
{       
    parameters:
        @display("i=block/classifier;t=Rcv Msg:0");        
        volatile double serviceMin @unit(s) = default(0);
        volatile double serviceMax  @unit(s) = default(0);
        
        volatile double memoryCost = default(0);
        volatile double cpuCost = default(0);
        
        string topic = default("stream");    
        bool logData = default(false);    
                
    gates:
        input  inIn[];
        output outIn[];
}

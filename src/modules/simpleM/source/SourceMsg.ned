package fogstream.modules.simpleM.source;

simple SourceMsg
{
    parameters:
    	volatile int bytesMsg = default(1000);//bytes
        volatile int departureRate = default(1);
        volatile int state = default(1);
        volatile string app = default("APP-0");    
        
    	@display("i=block/source;t=Sent:0");
    	
    
    gates:
        input  inIn[];
        output outIn[];
        input inExt[];
        output outExt[];
}

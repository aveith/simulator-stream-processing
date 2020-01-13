package fogstream.modules.simpleM.nodeOutput;

//
// TODO auto-generated module
//
simple NodeOutput
{
    parameters:
    	@display("i=old/bwsink;t=Wait List:0");
    	volatile bool logData = default(false);  
    
    gates:        
        input inIn[];
        output outEx[];
        
}

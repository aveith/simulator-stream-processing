package fogstream.modules.simpleM.sink;

simple SinkMsg
{
    parameters:
    	@display("i=block/sink");
    	
    gates:
        input  inIn[];
        output outIn[];
        input inExt[];
        output outExt[];
}

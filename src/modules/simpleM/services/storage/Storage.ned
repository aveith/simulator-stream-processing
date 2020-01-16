package fogstream.modules.simpleM.services.storage;

simple Storage
{
    parameters:        
        @display("i=abstract/router");
        @signal[qlat](type=simtime_t); // optional
        @statistic[endtoendlatency](source=qlat; record=mean,vector);
        
    gates:
        input in[];
        output out[];
}

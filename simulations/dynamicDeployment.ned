import fogstream.modules.compound.nodes.NodeMsg;
import fogstream.modules.simpleM.scheduler.SchedulerMsg;
import fogstream.modules.simpleM.services.MsgQueue;
import fogstream.modules.simpleM.sink.SinkMsg;
import fogstream.modules.simpleM.source.SourceMsg;
import ned.DatarateChannel;

network DynamicDeployment
{
    submodules:

        scheduler: SchedulerMsg {
            @display("p=65,66");
        }
    connections:

}

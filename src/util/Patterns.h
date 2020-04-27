#ifndef UTIL_PATTERNS_H_
#define UTIL_PATTERNS_H_
#include <omnetpp.h>

//#define IL_STD
//#include <ilcplex/ilocplex.h>
//ILOSTLBEGIN

using namespace omnetpp;
using namespace std;

namespace fogstream {

class Patterns {

public:
    enum OperatorType {
        Stateless, PartionedStateful, Stateful
    };
    enum ConstraintType {
        CPU, Memory, Bandwidth
    };
    enum Situation {
        NoPending, Pending
    };
    enum OperatorCharacteristic {
        Sequential,
        ForkBranch,
        ForkConcurrent,
        Join,
        JoinBranch,
        JoinConcurrent,
        Joker,
        JokerSource
    };
    enum OperatorMeasurement {
        Event, Queue
    };
    enum StatisticType {
        Operator, Host, Link, Path, InternalState, LinkUsage
    };
    enum DeviceType {
        Sensor, Gateway, Cloud
    };
    enum ExecutionLogType {
        IterationLog
    };
    enum ReconfigurationStrategies {
        MC_UCT, MC_SARSA, QLearning, MC_EpsilonGreedy, MC_BasicUCB, MC_TunedUCB
    };
    enum InternalStateType {
        CountTrigger, EventTimeTrigger, ProcessingTimeTrigger
    };
    enum PricingType {
        Microsoft, AWS
    };
    enum BaseStrategy {
        RandomWalk, CloudOnly, RTR, RTR_RP, Taneja
    };
    enum MessageType {
        Topic,
        ApplicationDeployment,
        ReconfigurationScheduling,
        Statistics,
        IntercommunicationScheduling,
        SaveSatistics
    };

    enum XMLLocations {
        KeepXMLValues,
        AllRandom,
        CentralizedCloud,
        DescentralizedCloud,
        CentralizedEdge,
        DescentralizedEdge,
    };

    enum ActionHeuristics{
        None,
        CPUBased,
        LatencyAware,
    };

    enum ConfigScaleApproach{
        AllResources,
        NetworkLatency,
        CPUPower,
    };

    static constexpr double PERCENTAGE_EXTERNAL_HOSTS = .001;
    static constexpr bool PRINT_RECONFIGURATION_LOG = false;
    static constexpr double TIME_STATISTIC_EVALUATION = 1100;
    static constexpr double TIME_FOR_RECONFIGURATION = 1000;
    static constexpr int SINK_QUEUE_SIZE_RECONFIG = 200;
    static constexpr int BATCH_SAVE_LOG = 10000;
    static constexpr int STATISTIC_MAX_REGS = 10000;

    //TIME PATTERNS
    static constexpr double TIME_INITIALISATION = 1000;
    static constexpr double TIME_BETWEEN_SEND = 0; //1000;
    static constexpr double TIME_INTERNAL_SCHEDULING = 0; //.0000001;
    static constexpr double TIME_INTERNAL_MSG_QUEUE_MIN = 0; //.00000001;
    static constexpr double TIME_INTERNAL_MSG_QUEUE_MAX = 0; //.0000001;
    static constexpr double TIME_EXTERNAL_MSG_QUEUE_MIN = 0; //.0000001;
    static constexpr double TIME_EXTERNAL_MSG_QUEUE_MAX = 0; //.000001;
    static constexpr double TIME_OUTPUT_MSG_TRANSMISSION = 0;
    static constexpr double TIME_START_EVENTS = 0; //.001;

    //MEMORY
    static constexpr double MEM_INTERNAL_MSG_QUEUE = 0; //10;
    static constexpr double MEM_EXTERNAL_MSG_QUEUE = 0; //100;

    //CPU
    static constexpr double CPU_INTERNAL_MSG_QUEUE = 0; //10;
    static constexpr double CPU_EXTERNAL_MSG_QUEUE = 0; //100;

    //MODULE NAMES
    static constexpr const char* NAME_PROCESS_ELEMENT =
            "fogstream.modules.compound.processElement.ProcessElement";
    static constexpr const char* NAME_PROCESS_HOST =
            "fogstream.modules.compound.nodes.NodeMsg";
    static constexpr const char* NAME_PROCESS_SOURCE =
            "fogstream.modules.simpleM.source.SourceMsg";
    static constexpr const char* NAME_PROCESS_SINK =
            "fogstream.modules.simpleM.services.MsgQueue";
    static constexpr const char* NAME_STORAGE =
            "fogstream.modules.simpleM.services.storage.Storage";
    static constexpr const char* NAME_DEPLOYER =
            "fogstream.modules.simpleM.services.deployer.Deployer";

    //MSG NAME PATTERNS
    static constexpr const char* MSG_INITIALISATION = "Init";
    static constexpr const char* MSG_EXEC = "Exec";
    static constexpr const char* MSG_ROUTE = "Route";
    static constexpr const char* MSG_VERTICES = "Vertices";
    static constexpr const char* MSG_EDGES = "Edges";
    static constexpr const char* MSG_VERTICESDEP = "VertDep";
    static constexpr const char* MSG_PATHS = "Paths";

    //MSG NAME CONSTRUCTION
    static constexpr const char* BASE_TOPIC = "Topic";
    static constexpr const char* BASE_DEPLOY = "Deploy";
    static constexpr const char* BASE_REQUEST = "Request";
    static constexpr const char* BASE_OPERATOR = "Operator";
    static constexpr const char* BASE_SCHEDULING = "Scheduling";

    //QUEUE NAME
    static constexpr const char* QUEUE_DEPLOYMENT = "queueDeploy";
    static constexpr const char* QUEUE_TOPIC = "queue";

};

string getDeploymentMsgName(int counter);
string getEventName(int counter, int source);
string getOperatorName(const char* appName, int operatorID, int fissionID);
string getQueueVertexHost(const char* hostName);
string getQueueEdgeHost(const char* hostName);
string getQueueVertexDep(const char* hostName);

bool is_number(std::string &str);
string find_and_replace(string source, string find, string replace);

void freeMemory(cModule* node, int memory);
void allocateMemory(cModule* node, int memory);

} /* namespace fogstream */

#endif /* UTIL_PATTERNS_H_ */

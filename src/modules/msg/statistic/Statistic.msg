namespace fogstream;

packet Statistic {    
        simtime_t sTimestamp;
        int type;
        int Measurement;
        int PathID;
        int OperatorID;
        int HostID;
        double MsgSize;
        int QueueSize;
        simtime_t CompTime;
        simtime_t CommTime;
        simtime_t PathTime;
        bool Input;
        double StateSize;
        simtime_t RequiredTimeBuildWindow;
}

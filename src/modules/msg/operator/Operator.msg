namespace fogstream;

message Operator {               
    int operatorId;   
    int fissionId;
    
    bool nextSplitted;
    int nextSplitLength;
    
    double selectivity;
    double dataReduction;
	int type ;
	double stateIncreaseRate;	
	double memoryCost;
    double cpuCost;
    
    int nextState;
    int nextSubState;
    string appName;
    
    double availableCPU;
    double availableMem;
    
    double timeWindow;
}

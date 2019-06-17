package fogstream.modules.simpleM.operator;
simple OperatorMsg
{
	parameters:
    	@display("i=block/app2;t=Del:0 Prc:0 Window:%f");
    	volatile bool nextSplitted = default(false);
    	volatile int nextSplitLength = default(0);
    	double selectivity = default(-1); // value must be less or equal to 1, -1 no seletivity
    	volatile double dataReduction = default(-1); // value must be less or equal to 1, -1 no reduction
    	volatile int type = default(0); // 0 - stateless, 1 - partioned stateful, and 2 - stateful
    	volatile double stateIncreaseRate @unit(B) = default(0);
    	
    	volatile double memoryCost = default(1000);
        volatile double cpuCost = default(1000);
        
        volatile int curState = default(0);
        volatile int curSubState = default(0);
        volatile int nextState = default(0);
        volatile int nextSubState = default(0);
        
        volatile string appName = default("");  
        volatile double availableCPU = default(0);
        volatile double availableMem = default(0);
        
        volatile double timeWindow = default(0);        
    gates:        
        input  queueNextIn[];
        output queueNextOut[];
        input queuePreviousIn[];
        output queuePreviousOut[];
}
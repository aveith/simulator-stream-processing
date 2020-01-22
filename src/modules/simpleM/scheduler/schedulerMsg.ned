package fogstream.modules.simpleM.scheduler;

simple SchedulerMsg
{
    parameters:
        @display("i=old/telnet");
        string test_id;
        string networkxml;        
        bool save_traced_data_reconfig = default(false);
        bool read_from_traced_data_reconfig = default(false);
        bool finish_after_tracing = default(false);
        string applicationxml = default("");
        string applicationdot = default("");
        string dir_result = default("");
        string dir_trace_datasets = default("");
        double selectivity_min = default(1);
        double selectivity_max = default(1);
        double datarate_min = default(1);
        double datarate_max = default(1);
        double memory_min = default(1);
        double memory_max = default(1);
        double cpu_min = default(1);
        double cpu_max = default(1);
        double cpusrc_min = default(1);
        double cpusrc_max = default(1);
        
        double cpusink_min = default(1);
        double cpusink_max = default(1);
        
        double bytesmsg_min = default(1);
        double bytesmsg_max = default(1);
        double arrivalrate_min = default(1);
        double arrivalrate_max = default(1);
        double staterate_min = default(1);
        double staterate_max = default(1);
        double window_min = default(1);
        double window_max = default(1);
        
        double operators_with_state_perc = default(1); 
        
        double lanlat_min = default(0);
        double lanlat_max = default(0);
        
        double manlat_min = default(0);
        double manlat_max = default(0);
        
        double wanlat_min = default(0);
        double wanlat_max = default(0);
        
        int base_strategy = default(0);
        
        string vertex_positions = default("");
        
        double reconfig_constant = default(.9);
        double reconfig_alpha = default(.1);
        double reconfig_gamma = default(1);
        double reconfig_lambda = default(1);
        double reconfig_epsilon = default(.6);
        int reconfig_strategy = default(0);
        int reconfig_iterations = default(1000);
        
        bool reconfig_applying_rp = default(false);
        
        double latency_weight = default(0);
        double migration_weight = default(0);
        double costs_weight = default(0);
        double wan_traffic_weight = default(0);
        
        double latency_parameter = default(0);
        double migration_parameter = default(0);
        double costs_parameter = default(0);
        double wan_traffic_paramenter = default(0);
        
        int pricing_type = default(0); //0 - Microsft 1 - AWS 
        bool aggregate_cost_based_reward = default(false);
        
        int configuration_seeds  = default(0); 
        bool return_only_configuration_deployment = default(false);
        
        string file_operator_statistics = default("");
        string file_path_statistics = default("");
        int source_locations = default(0);
        int sink_locations = default(0);
        bool use_slots = default(false);
		int action_heuristic = default(0);
        int time_reconfiguration = default(-1);
        
        int config_scale_approach = default(0);
    gates:
        input in[];
        output out[];
}

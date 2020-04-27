#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)fogStream
#
# This file was generated with the command:
#  opp_makemake --make-so -f --deep -O out -I/usr/include/libxml2 -I. -I/opt/ibm/ILOG/CPLEX_Studio1271/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio1271/concert -I/opt/ibm/ILOG/CPLEX_Studio1271/cpoptimizer/include -I/usr/include/libxml2/libxml -I/usr/include/libxml2
#

# Name of target to be created (-o option)
TARGET = $(LIB_PREFIX)fogStream$(D)$(SHARED_LIB_SUFFIX)
TARGET_DIR = .

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I/usr/include/libxml2 \
    -I. \
    -I/opt/ibm/ILOG/CPLEX_Studio1271/cplex/include \
    -I/opt/ibm/ILOG/CPLEX_Studio1271/concert \
    -I/opt/ibm/ILOG/CPLEX_Studio1271/cpoptimizer/include \
    -I/usr/include/libxml2/libxml \
    -I/usr/include/libxml2

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/configuration/Configuration.o \
    $O/configuration/goalrate/GoalRate.o \
    $O/configuration/taneja/operator/OperatorBase.o \
    $O/configuration/taneja/resource/ResourceBase.o \
    $O/env/Env.o \
    $O/env/objects/application/destinationbranches/DestinationBranches.o \
    $O/env/objects/application/operatorconnection/OperatorConnection.o \
    $O/env/objects/application/operatordata/OperatorData.o \
    $O/env/objects/applicationmapping/operatorconnectionmapping/OperatorConnectionMapping.o \
    $O/env/objects/applicationmapping/operatormapping/OperatorMapping.o \
    $O/env/objects/datasink/DataSink.o \
    $O/env/objects/datasource/DataSource.o \
    $O/env/objects/networkconnection/NetworkConnection.o \
    $O/env/objects/networkconnection/linkdata/LinkData.o \
    $O/env/objects/networkconnection/linkdata/linkcapability/LinkCapability.o \
    $O/env/objects/networkconnection/resourcedata/ResourceData.o \
    $O/env/objects/networkconnection/resourcedata/resourcecapability/ResourceCapability.o \
    $O/modules/simpleM/nodeManager/NodeManager.o \
    $O/modules/simpleM/nodeOutput/NodeOutput.o \
    $O/modules/simpleM/operator/OperatorMsg.o \
    $O/modules/simpleM/operator/internalstate/InternalState.o \
    $O/modules/simpleM/scheduler/schedulerMsg.o \
    $O/modules/simpleM/scheduler/parameters/Parameters.o \
    $O/modules/simpleM/services/MsgQueue.o \
    $O/modules/simpleM/services/deployer/Deployer.o \
    $O/modules/simpleM/services/storage/Storage.o \
    $O/modules/simpleM/sink/SinkMsg.o \
    $O/modules/simpleM/source/SourceMsg.o \
    $O/reconfiguration/environment/Environment.o \
    $O/reconfiguration/mapping/Mapping.o \
    $O/reconfiguration/mapping/edge/MapEdge.o \
    $O/reconfiguration/mapping/operator/MapOperator.o \
    $O/reconfiguration/mc_tree/MCTree.o \
    $O/reconfiguration/mc_tree/action/Action.o \
    $O/reconfiguration/mc_tree/node/Node.o \
    $O/reconfiguration/mc_tree/state/State.o \
    $O/reconfiguration/mcts/MCTS.o \
    $O/reconfiguration/mcts/episode/Episode.o \
    $O/reconfiguration/migration/Migration.o \
    $O/reconfiguration/statistics/host/Host.o \
    $O/reconfiguration/statistics/link/Link.o \
    $O/reconfiguration/statistics/operator/OperatorStatistics.o \
    $O/reconfiguration/statisticsmcts/StatisticsMCTS.o \
    $O/reconfiguration/strategies/mc_basicucb/MC_BasicUCB.o \
    $O/reconfiguration/strategies/mc_epsilongreedy/MCEpsilonGreedy.o \
    $O/reconfiguration/strategies/mc_sarsa/MCSARSA.o \
    $O/reconfiguration/strategies/mc_tuneducb/MC_TunedUCB.o \
    $O/reconfiguration/strategies/mc_uct/MCUCT.o \
    $O/reconfiguration/strategies/qlearning/QLearning.o \
    $O/util/Patterns.o \
    $O/util/dag_Generator/DAGGenerator.o \
    $O/util/graphS/Graph.o \
    $O/modules/msg/appSetup/AppSetup_m.o \
    $O/modules/msg/edges/Edges_m.o \
    $O/modules/msg/operator/Operator_m.o \
    $O/modules/msg/operatorDep/OperatorDep_m.o \
    $O/modules/msg/routing/RoutingMsg_m.o \
    $O/modules/msg/statistic/Statistic_m.o \
    $O/modules/msg/topicEvent/TopicEvent_m.o

# Message files
MSGFILES = \
    modules/msg/appSetup/AppSetup.msg \
    modules/msg/edges/Edges.msg \
    modules/msg/operator/Operator.msg \
    modules/msg/operatorDep/OperatorDep.msg \
    modules/msg/routing/RoutingMsg.msg \
    modules/msg/statistic/Statistic.msg \
    modules/msg/topicEvent/TopicEvent.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) $(IMPORT_DEFINES)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(TARGET)

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)

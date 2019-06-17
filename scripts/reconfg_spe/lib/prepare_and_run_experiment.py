from time import sleep, time
from argparse import ArgumentParser
from util import Config
from lib.generate_infra_xml import main as generateNetwork
from lib.move_files import main as moveFiles
import os
from lib.run_experiments import main as runExperiments


def parse_options():
    """Parse the command line options for workload creation"""
    parser = ArgumentParser(description='Prepare and run the exepriments using the configuration file.')
    parser.add_argument('--config', dest='config', type=str, required=True,
                        help='the configuration file to use')

    args = parser.parse_args()
    return args


def prepare_network_xml(devices, cloudlets, clouds, app, pi3, output):
    generateNetwork(number_of_edge_devices_per_cloudlet=devices, number_of_cloudlets=cloudlets, number_of_clouds=clouds,
                    app=app, perc_of_pi_3=pi3, output_file_name=output)


def run_experiments(dir='', dir_trace_datasets='', network='', app='', reconfig_strategy=[], latency_weight=-1,
                    downtime_weight=-1, monetary_cost_weight=-1, wan_traffic_weight=-1, pricing_type=-1,
                    config_strategy=[], reconfig=False, config=False, source_location=0, sink_location=0):
    runExperiments(dir=dir, dir_trace_datasets=dir_trace_datasets,
                   network=network, app=app, reconfig_strategy=reconfig_strategy,
                   latency_weight=latency_weight,
                   downtime_weight=downtime_weight,
                   monetary_cost_weight=monetary_cost_weight,
                   wan_traffic_weight=wan_traffic_weight,
                   pricing_type=pricing_type,
                   config_strategy=config_strategy,
                   reconfig=reconfig,
                   config=config,
                   source_location=source_location, sink_location=sink_location)


def move_files(folder='', destination=''):
    moveFiles(folder=folder, destination=destination)


def validade_execution_type(configuration=False, reconfiguration=False):
    return configuration or reconfiguration


def validate_conf_file(conf=Config()):
    if not validade_execution_type(conf.getboolean('configuration', 'execute'),
                                   conf.getboolean('reconfiguration', 'execute')):
        print "You have to set TRUE for configuration and/or reconfiguration!"
        return False

    if not conf.getint('reconfiguration', 'reconfig_strategy') in [-1, 1, 2, 3, 4]:
        print "Invalid reconfiguration strategy [-1 - ALL, 1 - SARSA, 2 - Q-LEARNING, 3 - EPSILONGREEDY, 4 - MCTS-UCT]"
        return False

    if conf.getfloat('reconfiguration', 'latency_weight') + conf.getfloat('reconfiguration',
                                                                          'downtime_weight') + conf.getfloat(
        'reconfiguration', 'monetary_cost_weight') + conf.getfloat('reconfiguration', 'wan_traffic_weight') < .99:
        print "The sum of weights must be equal to 1"
        return False

    if not conf.getint('reconfiguration', 'pricing_type') in [0, 1]:
        print "Invalid pricing strategy [0 - MICROSOFT, 1 - AWS]"
        return False

    if not conf.getint('configuration', 'configuration_strategy') in [-1, 1, 2, 3, 4]:
        print "Invalid configuration strategy [-1 - ALL, 1 - ONLY CLOUD, 2 - RTR, 3 - RTR+RP, 4 - TANEJA]"
        return False

    if not conf.getint('app', 'sources') in [0, 1, 2, 3, 4, 5]:
        print "Invalid source location [0 - KEEP XML VALUES, 1 - ALL RANDOM (EDGE + CLOUD), 2 - CENTRALIZED CLOUD, 3 - DESCENTRALIZED CLOUD, 4 - CENTRALIZED EDGE, 5 - DESCENTRALIZED EDGE]"
        return False

    if not conf.getint('app', 'sinks') in [0, 1, 2, 3, 4, 5]:
        print "Invalid sink location [0 - KEEP XML VALUES, 1 - ALL RANDOM (EDGE + CLOUD), 2 - CENTRALIZED CLOUD, 3 - DESCENTRALIZED CLOUD, 4 - CENTRALIZED EDGE, 5 - DESCENTRALIZED EDGE]"
        return False

    if conf.getboolean('network', 'build_xml'):
        if conf.getint('network', 'number_of_edge_devices_per_cloudlet') < 1 or conf.getint('network',
                                                                                            'number_of_cloudlets') < 1 or conf.getint(
            'network', 'number_of_clouds') < 1:
            print "Invalid number of computational resources."
            return False

        if conf.getfloat('network', 'perc_of_pi_3') >= 1:
            print "Invalid percentage of RPis 3. The number must be lower than 1."
            return False
    else:
        if not os.path.exists(conf.get('general', 'basic_dir') + 'xml/' + conf.get('network', 'net_xml')):
            print "Invalid network XML. File does not exist."
            return False

    print conf.get('general', 'basic_dir') + 'xml/' + conf.get('app', 'app')
    if not os.path.exists(conf.get('general', 'basic_dir') + 'xml/' + conf.get('app', 'app')):
        print "Invalid application file. File does not exist."
        return False

    if conf.getboolean('configuration', 'save_statistics'):
        if not os.path.isdir(conf.get('configuration', 'save_statistics_path')):
            print "Folder for saving statistics does not exist."
            return False

    if not os.path.isdir(conf.get('general', 'output_directory')):
        print "Folder for saving simulation results does not exists."
        return False

    if conf.getboolean('reconfiguration', 'execute') and conf.getboolean('reconfiguration', 'load_statistics'):
        if not os.path.exists(conf.get('reconfiguration', 'file_path_statistics')):
            print "Invalid path statistics. File does not exist."
            return False
        if not os.path.exists(conf.get('reconfiguration', 'file_operator_statistics')):
            print "Invalid operator statistics. File does not exist."
            return False

    if not os.path.exists(conf.get('general', 'basic_dir')):
        print "Invalid basic directory. Path does not exist."
        return False

    if not os.path.exists(conf.get('general', 'destination_dir')):
        print "Invalid destination directory. Path does not exist."
        return False

    return True


def main():
    opts = parse_options()
    conf = Config(opts.config)

    if validate_conf_file(conf):
        if conf.getboolean('network', 'build_xml'):
            """Parse the command line options for preparing the network infrastructure"""
            app = conf.get('app', 'app')
            devices = conf.getint('network', 'number_of_edge_devices_per_cloudlet')
            cloudlets = conf.getint('network', 'number_of_cloudlets')
            clouds = conf.getint('network', 'number_of_clouds')
            pi3 = conf.getfloat('network', 'perc_of_pi_3')
            output = conf.get('network', 'output_file_name')
            prepare_network_xml(devices=devices, cloudlets=cloudlets, clouds=clouds, app=app, pi3=pi3,
                                output=conf.get('general', 'basic_dir') + 'xml/' + output)

        dir = conf.get('general', 'basic_dir')
        dir_trace_datasets = conf.get('configuration', 'save_statistics_path')

        if conf.getboolean('network', 'build_xml'):
            network = conf.get('network', 'output_file_name')
        else:
            network = conf.get('network', 'net_xml')

        if conf.getint('reconfiguration', 'reconfig_strategy') == -1:
            reconfiguration_strategies = [1, 4]
        else:
            reconfiguration_strategies = [conf.getint('reconfiguration', 'reconfig_strategy')]

        if conf.getint('configuration', 'configuration_strategy') == -1:
            configuration_strategies = [1, 2, 3, 4]
        else:
            configuration_strategies = [conf.getint('configuration', 'configuration_strategy')]

        run_experiments(dir=dir, dir_trace_datasets=dir_trace_datasets, network=network, app=conf.get('app', 'app'),
                        reconfig_strategy=reconfiguration_strategies,
                        latency_weight=conf.getfloat('reconfiguration', 'latency_weight'),
                        downtime_weight=conf.getfloat('reconfiguration', 'downtime_weight'),
                        monetary_cost_weight=conf.getfloat('reconfiguration', 'monetary_cost_weight'),
                        wan_traffic_weight=conf.getfloat('reconfiguration', 'wan_traffic_weight'),
                        pricing_type=conf.getint('reconfiguration', 'pricing_type'),
                        config_strategy=configuration_strategies,
                        reconfig=conf.getboolean('reconfiguration', 'execute'),
                        config=conf.getboolean('configuration', 'execute'),
                        source_location=conf.getint('app', 'sources'),
                        sink_location=conf.getint('app', 'sinks'))

        if conf.getboolean('reconfiguration', 'execute'):
            folder = conf.get('general', 'output_directory')
            destination = conf.get('general', 'destination_dir')
            move_files(folder=folder, destination=destination)
    else:
        print "Invalid configuration file!"


if __name__ == "__main__":
    main()

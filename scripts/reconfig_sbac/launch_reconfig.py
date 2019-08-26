from __builtin__ import str
from argparse import ArgumentParser
import os
import random
from multiprocessing import Pool
from shutil import copyfile
import logging
from datetime import datetime


class experiment_parameter:
    def __init__(self, ):
        self.app = ['a1', 'a2', 'a3', 'a4', 'a5', 'a10', 'a11', 'a12', 'a13', 'app3']
        self.reconfig_iterations = 10000
        self.reconfig_strategies = [0, 1, 2, 3, 4, 5]
        self.config_strategies = [1, 4]
        self.reconfig_constants = [.0001, .001, .05]
        self.reconfig_alpha = [.001, .1]  # no
        self.reconfig_gamma = [.005, .05]
        self.reconfig_lambda = [.005, .05]
        self.reconfig_epsilon = [.001, .1]  # no
        # self.reconfig_price_model = [0]
        # self.reconfig_scenario = [[1, 0, 0, 0], [.33, 0, .33, .33], [.25, .25, .25, .25], [.4, .2, .2, .2],
        #                           [.7, .1, .1, .1]]
        self.network = ['n3.xml']
        self.file_operator_stat = ""
        self.file_path_stat = ""
        self.reconfig_applying_rp = ['true', 'false']
        self.action_heuristic = [0]


class experiment_configuration:
    def __init__(self, id=0, directory='',
                 reconfig_strategy=0, reconfig_iterations=10000, reconfig_constant=.9, reconfig_alpha=.1,
                 reconfig_gamma=1, reconfig_lambda=1, reconfig_epsilon=.6, read_from_traced_data_reconfig='false',
                 save_traced_data_reconfig='false', dir_trace_datasets='', finish_after_tracing='false',
                 app='a1', seeds=[], configuration=0, tracing=False, config_strategy=-1, network='n3.xml',
                 file_path_stat="", file_operator_stat="", reconfig_applying_rp='false', action_heuristic=0):
        self.id = id
        self.directory = directory
        self.reconfig_strategy = reconfig_strategy
        self.reconfig_iterations = reconfig_iterations
        self.reconfig_constant = reconfig_constant
        self.reconfig_alpha = reconfig_alpha
        self.reconfig_gamma = reconfig_gamma
        self.reconfig_lambda = reconfig_lambda
        self.reconfig_epsilon = reconfig_epsilon

        self.read_from_traced_data_reconfig = read_from_traced_data_reconfig
        self.save_traced_data_reconfig = save_traced_data_reconfig
        self.dir_trace_datasets = dir_trace_datasets
        self.finish_after_tracing = finish_after_tracing
        self.app = app
        # self.reconfig_price_model = reconfig_price_model
        # self.latency_weight = latency_weight
        # self.migration_weight = migration_weight
        # self.costs_weight = costs_weight
        # self.wan_traffic_weight = wan_traffic_weight
        self.seeds = seeds
        self.configuration = configuration
        self.tracing = tracing
        self.config_strategy = config_strategy
        self.network = network
        self.file_path_stat = file_path_stat
        self.file_operator_stat = file_operator_stat
        self.reconfig_applying_rp = reconfig_applying_rp
        self.action_heuristic = action_heuristic


def parse_options():
    """Parse the command line options for workload creation"""
    parser = ArgumentParser(description='Prepare and run the exepriments using the configuration file.')

    parser.add_argument('-d', '--directory', type=str, required=True,
                        help='directory to save the simulations')

    parser.add_argument('-p', '--process', type=int, required=True,
                        help='number of simultaneous executions(system processes)')

    parser.add_argument('-l', '--traceddatasetlocation', type=str, required=True,
                        help='directory for saving or loading the traced data')

    parser.add_argument('-s', '--seed', type=int, required=True,
                        help='number base for random simulation configuration (seed)')

    parser.add_argument('-c', '--configurations', type=int, required=True,
                        help='number of configurations of infrastructure and application')
    args = parser.parse_args()
    return args


def build_tracing_scenarios(dir='', dir_trace_datasets='', seed=3454326, configurations=1,
                            parameters=experiment_parameter()):
    _scenarios = []
    id = 0

    random.seed(seed)

    # This initial part create the application statistics (MAPE)
    for c in range(0, int(configurations)):
        seeds = []
        for r in range(56):
            seeds.append(random.randint(10000, 99999999))

        for st in range(len(parameters.config_strategies)):
            for a in range(len(parameters.app)):
                for n in range(len(parameters.network)):
                    exp = experiment_configuration()
                    exp.id = id
                    exp.directory = dir
                    exp.read_from_traced_data_reconfig = 'false'
                    exp.save_traced_data_reconfig = 'true'
                    exp.dir_trace_datasets = dir_trace_datasets
                    exp.finish_after_tracing = 'true'
                    exp.app = parameters.app[a]
                    exp.tracing = True
                    exp.configuration = c
                    exp.seeds = seeds
                    exp.config_strategy = parameters.config_strategies[st]
                    exp.network = parameters.network[n]
                    exp.file_path_stat = parameters.file_path_stat
                    exp.file_operator_stat = parameters.file_operator_stat
                    exp.network = parameters.network[n]
                    logging.warning('id\t%d\tapp\t%s\tconf_number\t%d\tstrategy\t%d', id, parameters.app[a], c,
                                    parameters.config_strategies[st])

                    id = id + 1
                    _scenarios.append(exp)
    return _scenarios


def build_simulation_scenarios(traced_scenarios=[], parameters=experiment_parameter()):
    _scenarios = []
    id = 0

    for tr in range(len(traced_scenarios)):
        for st in range(len(parameters.reconfig_strategies)):
            constants = [1]
            alpha = [1]
            gamma = [1]
            _lambda = [1]
            epsilon = [1]

            if parameters.reconfig_strategies[st] in [0, 1, 3, 4, 5]:
                constants = parameters.reconfig_constants

            if parameters.reconfig_strategies[st] in [1, 2]:
                gamma = parameters.reconfig_gamma

            if parameters.reconfig_strategies[st] == 2:
                alpha = parameters.reconfig_alpha

            if parameters.reconfig_strategies[st] == 3:
                epsilon = parameters.reconfig_epsilon

            if parameters.reconfig_strategies[st] == 1:
                _lambda = parameters.reconfig_lambda

            for ct in range(len(constants)):
                for al in range(len(alpha)):
                    for gm in range(len(gamma)):
                        for lb in range(len(_lambda)):
                            for ep in range(len(epsilon)):
                                for ah in range(len(parameters.reconfig_applying_rp)):
                                    for ac in (range(len(parameters.action_heuristic))):
                                        # for pm in range(len(parameters.reconfig_price_model)):
                                        #     for sc in range(len(parameters.reconfig_scenario)):
                                        exp = experiment_configuration()
                                        exp.id = id
                                        exp.directory = traced_scenarios[tr].directory

                                        # if parameters.reconfig_strategies[st] == 1 and traced_scenarios[
                                        #     tr].config_strategy == 1 and parameters.reconfig_price_model[pm] == 0 and \
                                        #         parameters.reconfig_scenario[sc][0] == .25 and traced_scenarios[
                                        #     tr].app == 'a1':
                                        #     ttes = 0
                                        #     ttes = ttes + 1

                                        exp.reconfig_iterations = parameters.reconfig_iterations
                                        exp.reconfig_strategy = parameters.reconfig_strategies[st]
                                        exp.reconfig_constant = constants[ct]
                                        exp.reconfig_alpha = alpha[al]
                                        exp.reconfig_gamma = gamma[gm]
                                        exp.reconfig_lambda = _lambda[lb]
                                        exp.reconfig_epsilon = epsilon[ep]
                                        exp.read_from_traced_data_reconfig = 'true'
                                        exp.save_traced_data_reconfig = 'false'
                                        exp.dir_trace_datasets = traced_scenarios[tr].dir_trace_datasets
                                        exp.finish_after_tracing = 'false'
                                        exp.app = traced_scenarios[tr].app

                                        # exp.reconfig_price_model = parameters.reconfig_price_model[pm]
                                        # exp.latency_weight = parameters.reconfig_scenario[sc][0]
                                        # exp.migration_weight = parameters.reconfig_scenario[sc][1]
                                        # exp.costs_weight = parameters.reconfig_scenario[sc][2]
                                        # exp.wan_traffic_weight = parameters.reconfig_scenario[sc][3]

                                        exp.configuration = traced_scenarios[tr].configuration
                                        exp.config_strategy = traced_scenarios[tr].config_strategy
                                        exp.network = traced_scenarios[tr].network
                                        exp.file_path_stat = traced_scenarios[tr].file_path_stat
                                        exp.file_operator_stat = traced_scenarios[tr].file_operator_stat
                                        exp.reconfig_applying_rp = parameters.reconfig_applying_rp[ah]
                                        exp.action_heuristic = parameters.action_heuristic[ac]

                                        exp.seeds = traced_scenarios[tr].seeds

                                        # logging.warning(
                                        #     'id\t%d\tapp\t%s\tconf_number\t%d\tstrategy\t%d\treconfig_strategy\t%d\tprice\t%d\tlatencyweight\t%d\tmigrationweight\t%d\tcostweight\t%d\twanweight\t%d\tconstant\t%d\talpha\t%d\tgamma\t%d\tlambda\t%d\tepsilon\t%d',
                                        #     id,
                                        #     traced_scenarios[tr].app, traced_scenarios[tr].configuration,
                                        #     traced_scenarios[tr].config_strategy, parameters.reconfig_strategies[st],
                                        #     # parameters.reconfig_price_model[pm], parameters.reconfig_scenario[sc][0],
                                        #     # parameters.reconfig_scenario[sc][1], parameters.reconfig_scenario[sc][2],
                                        #     # parameters.reconfig_scenario[sc][3],
                                        #     constants[ct], alpha[al], gamma[gm],
                                        #     _lambda[lb], epsilon[ep])
                                        id = id + 1
                                        _scenarios.append(exp)
    return _scenarios


def build_scenarios(dir='', dir_trace_datasets='',
                    tracing=False,
                    seed=3454326,
                    traced_scenarios=[],
                    configurations=1,
                    parameters=experiment_parameter()):
    try:
        if tracing:
            return build_tracing_scenarios(dir=dir, dir_trace_datasets=dir_trace_datasets, seed=seed,
                                           configurations=configurations, parameters=parameters)

        else:
            return build_simulation_scenarios(traced_scenarios=traced_scenarios, parameters=parameters)

    except Exception as e:
        logging.error("Exception occurred", exc_info=True)


def build_ini_file(_scenario=experiment_configuration(), _dir=''):
    _name = 'id_' + str(
        _scenario.id) + '_' + str(_scenario.configuration)

    _name = _name.replace('"', "")

    _file = _name + '.ini'
    copyfile('sbac.ini', _dir + _file)

    _search = ["lb_scenario_0",
               "..\/results\/",
               "a5.dot",
               "reconfig_iterations = 10000",
               "reconfig_constant = .9",
               "reconfig_alpha = .1",
               "reconfig_gamma = 1",
               "reconfig_lambda = 1",
               "reconfig_epsilon = .6",
               "reconfig_strategy = 2",
               "read_from_traced_data_reconfig = false",
               "save_traced_data_reconfig = false",
               "finish_after_tracing = false",
               "\/home\/veith\/tracedatasets\/",
               "configuration_seeds = 0",
               "app_app5",
               "base_strategy = 1",
               "n3.xml",
               "&",
               "@",
               "reconfig_applying_rp = false",
               "action_heuristic = 0"
               ]

    _replace = [_name,
                _dir.replace("/", "\/"),
                _scenario.app + '.dot',
                "reconfig_iterations = " + str(_scenario.reconfig_iterations),
                "reconfig_constant = " + str(_scenario.reconfig_constant),
                "reconfig_alpha = " + str(_scenario.reconfig_alpha),
                "reconfig_gamma = " + str(_scenario.reconfig_gamma),
                "reconfig_lambda = " + str(_scenario.reconfig_lambda),
                "reconfig_epsilon = " + str(_scenario.reconfig_epsilon),
                "reconfig_strategy = " + str(_scenario.reconfig_strategy),
                "read_from_traced_data_reconfig = " + str(_scenario.read_from_traced_data_reconfig),
                "save_traced_data_reconfig = " + str(_scenario.save_traced_data_reconfig),
                "finish_after_tracing = " + str(_scenario.finish_after_tracing),
                _scenario.dir_trace_datasets.replace("/", "\/"),
                "configuration_seeds = " + str(_scenario.configuration),
                str(_scenario.id),
                "base_strategy = " + str(_scenario.config_strategy),
                str(_scenario.network),
                str(_scenario.file_operator_stat),
                str(_scenario.file_path_stat),
                "reconfig_applying_rp = " + str(_scenario.reconfig_applying_rp),
                "action_heuristic = " + str(_scenario.action_heuristic)
                ]
    _base = 3454326
    for s in range(56):
        _search.append("seed-" + str(s) + "-lcg32 = " + str(_base + s))
        _replace.append("seed-" + str(s) + "-lcg32 = " + str(_scenario.seeds[s]))

    for r in range(len(_search)):
        print 'sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file
        os.system('sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file)

    return _file, _name


def execute_simulation(_scenario=experiment_configuration()):
    os.chdir(_scenario.directory)

    logging.warning('Simulating scenario\t%d', _scenario.id)

    _folder_name = "app_" + str(_scenario.app) + "_tracing_" + str(_scenario.tracing) + "_configuration_" + str(
        _scenario.configuration) + "_rp_" + str(_scenario.reconfig_applying_rp)

    os.system("mkdir ../results")
    os.system("mkdir ../results/" + _folder_name)
    _root_dir_exp = "../results/" + _folder_name
    _working_dir = "../results/" + _folder_name + "/"
    os.system("mkdir " + _working_dir)

    _file_name, _name_config = build_ini_file(_scenario=_scenario, _dir=_working_dir)
    now = datetime.now()
    # print "../simulator-stream-processing  -u Cmdenv --cmdenv-redirect-output=true --cmdenv-express-mode=true -l ../src/fogStream -n ../src:. -f " + _working_dir + _file_name + " -c " + _name_config + ">" + _working_dir + str(
    #         _scenario.id) + ".txt"
    os.system(
        "../simulator-stream-processing  -u Cmdenv --cmdenv-redirect-output=true --cmdenv-express-mode=true -l ../src/fogStream -n ../src:. -f " + _working_dir + _file_name + " -c " + _name_config + ">" + _working_dir + str(
            _scenario.id) + ".txt")
    check_simulation_execution(id=_scenario.id, app=_scenario.app, dir=_working_dir + 'results/', filename=_file_name)
    later = datetime.now()
    difference = (later - now).total_seconds()
    if _scenario.tracing:
        logging.warning('Finished tracing scenario\t%d time(seconds)\t%f', _scenario.id, difference)
    else:
        logging.warning('Finished simulated scenario\t%d time(seconds)\t%f ', _scenario.id, difference)


def check_simulation_execution(id=-1, app='', dir='', filename=''):
    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith('.out'):
                if filename.replace(".ini", "") in file:
                    if 'Error' in open(root + file).read():
                        with open(root + file, 'r') as fin:
                            logging.warning('An error occurred on the application %s! id %d \t error \t %s', app, id,
                                            fin.readlines())


def main(dir="", dir_trace_datasets="", configurations=1, seed=3454326, processes=1):
    logging.basicConfig(filename='simulation.stat', filemode='w', format='%(asctime)s - %(message)s')

    logging.warning('Starting simulations')

    parameters = experiment_parameter()

    logging.warning('Building the basic scenarios')
    tracing_scenarios = build_scenarios(dir=dir,
                                        dir_trace_datasets=dir_trace_datasets,
                                        configurations=configurations,
                                        seed=seed,
                                        tracing=True, parameters=parameters)
    logging.warning('Total of basic scenarios: %d', tracing_scenarios.__len__())

    logging.warning('Building the simulation based on the basic scenarios')
    scenarios = build_scenarios(tracing=False,
                                traced_scenarios=tracing_scenarios,
                                parameters=parameters)
    logging.warning('Total of simulation scenarios: %d', scenarios.__len__())

    # This "for" corresponds to the process for building the traces (MAPE)
    # execute_simulation(tracing_scenarios[1])
    # execute_simulation(tracing_scenarios[100])
    # execute_simulation(scenarios[0])
    if tracing_scenarios.__len__() > 0:
        execute_simulation(tracing_scenarios[1])
    #     pool = Pool(processes=processes)
    #     pool.map(execute_simulation, tracing_scenarios)
    #     pool.close()
    #     pool.join()
    #
    # if scenarios.__len__() > 0:
    #     # execute_simulation(scenarios[0])
    #     pool = Pool(processes=processes)
    #     pool.map(execute_simulation, scenarios)
    #     pool.close()
    #     pool.join()

    logging.warning('Simulations finished!')


if __name__ == '__main__':
    opts = parse_options()

    main(dir=opts.directory, dir_trace_datasets=opts.traceddatasetlocation,
         configurations=opts.configurations, seed=opts.seed, processes=opts.process)

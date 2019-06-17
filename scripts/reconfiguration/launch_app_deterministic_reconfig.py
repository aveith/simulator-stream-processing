import argparse
import os
import random
from multiprocessing import Pool
from shutil import copyfile
import time
import fileinput
import itertools


class experiment_configuration:
    def __init__(self, net='', app='', slt=[], time=0, id=0, directory='', experiment=0, arrival_rate=1000,
                 byte_size=100, data_rate=.1, selectivity=.1, cpu=1000, cpu_src=10, memory=7500, placements="",
                 reconfig_strategy=0, reconfig_iterations=10000, reconfig_constant=.9, reconfig_alpha=.1,
                 reconfig_gamma=1, reconfig_lambda=1, reconfig_epilson=.6, read_from_traced_data_reconfig='false',
                 save_traced_data_reconfig='false', dir_trace_datasets='', finish_after_tracing='false'):

        self.net = net
        self.app = app
        self.time = time
        self.id = id
        self.directory = directory
        self.slt = slt
        self.experiment = experiment
        self.arrival_rate = arrival_rate
        self.byte_size = byte_size
        self.data_rate = data_rate
        self.selectivity = selectivity
        self.cpu = cpu
        self.memory = memory
        self.cpu_src = cpu_src
        self.placements = placements
        self.reconfig_strategy = reconfig_strategy
        self.reconfig_iterations = reconfig_iterations
        self.reconfig_constant = reconfig_constant
        self.reconfig_alpha = reconfig_alpha
        self.reconfig_gamma = reconfig_gamma
        self.reconfig_lambda = reconfig_lambda
        self.reconfig_epilson = reconfig_epilson

        self.read_from_traced_data_reconfig = read_from_traced_data_reconfig
        self.save_traced_data_reconfig = save_traced_data_reconfig
        self.dir_trace_datasets = dir_trace_datasets
        self.finish_after_tracing = finish_after_tracing


def build_scenarios(_slt=[], _net=[], _app=[], _dir='', _time=0, _initial_id=0,
                    _arrival_rate=[], _bytes_size=[], _data_rate=[], _selectivity=[], _sink_placement=[],
                    _source_placement=[], _sinks_app=[], _cpu_ips=[], _reconfig_strategies=[],
                    _reconfig_iterations=[], _reconfig_constants=[], _reconfig_alpha=[], _reconfig_gamma=[],
                    _reconfig_lambda=[], _reconfig_epilson=[], read_from_traced_data_reconfig='false',
                 save_traced_data_reconfig='false', dir_trace_datasets='', finish_after_tracing='false'):
    _scenarios = []
    id = _initial_id
    _exp = _initial_id

    for n in range(len(_net)):
        for a in range(len(_app)):
            for b in range(len(_bytes_size)):
                for ar in range(len(_arrival_rate[a][b])):
                    for d in range(len(_data_rate)):
                        for s in range(len(_selectivity)):
                            for it in range(len(_reconfig_iterations)):
                                for st in range(len(_reconfig_strategies)):
                                    for ct in range(len(_reconfig_constants[st])):
                                        for al in range(len(_reconfig_alpha[st])):
                                            for gm in range(len(_reconfig_gamma[st])):
                                                for lb in range(len(_reconfig_lambda[st])):
                                                    for ep in range(len(_reconfig_epilson[st])):
                                                        lists = []
                                                        for i in range(len(_sinks_app[a])):
                                                            lists.append(list(itertools.product(_sinks_app[a][i],
                                                                                                _sink_placement[a][i])))

                                                        for l in range(len(lists[0])):
                                                            base = "0-" + str(_source_placement[0]) + " " + str(
                                                                lists[0][l][0]) + "-" + str(
                                                                lists[0][l][1])
                                                            if lists.__len__() > 1:
                                                                for m in range(len(lists[1])):
                                                                    placements = base + " " + str(
                                                                        lists[1][m][0]) + "-" + str(lists[1][m][1])
                                                                    # print placements
                                                                    exp = experiment_configuration()
                                                                    exp.net = _net[n]
                                                                    exp.app = _app[a]
                                                                    exp.time = _time
                                                                    exp.id = id
                                                                    exp.directory = _dir
                                                                    exp.experiment = _exp
                                                                    exp.slt = _slt
                                                                    exp.arrival_rate = _arrival_rate[a][b][ar]
                                                                    exp.byte_size = _bytes_size[b]
                                                                    exp.data_rate = _data_rate[d]
                                                                    exp.selectivity = _selectivity[s]
                                                                    exp.cpu = _cpu_ips[b]
                                                                    exp.memory = 7500
                                                                    exp.cpu_src = _cpu_ips[b]
                                                                    exp.placements = placements

                                                                    exp.reconfig_iterations = _reconfig_iterations[it]
                                                                    exp.reconfig_strategy = _reconfig_strategies[st]
                                                                    exp.reconfig_constant = _reconfig_constants[st][ct]
                                                                    exp.reconfig_alpha = _reconfig_alpha[st][al]
                                                                    exp.reconfig_gamma = _reconfig_gamma[st][gm]
                                                                    exp.reconfig_lambda = _reconfig_lambda[st][lb]
                                                                    exp.reconfig_epilson = _reconfig_epilson[st][ep]
                                                                    exp.read_from_traced_data_reconfig = read_from_traced_data_reconfig
                                                                    exp.save_traced_data_reconfig = save_traced_data_reconfig
                                                                    exp.dir_trace_datasets = dir_trace_datasets
                                                                    exp.finish_after_tracing = finish_after_tracing

                                                                    id = id + 1
                                                                    _scenarios.append(exp)
                                                            else:
                                                                # print base
                                                                exp = experiment_configuration()
                                                                exp.net = _net[n]
                                                                exp.app = _app[a]
                                                                exp.time = _time
                                                                exp.id = id
                                                                exp.directory = _dir
                                                                exp.experiment = _exp
                                                                exp.slt = _slt
                                                                exp.arrival_rate = _arrival_rate[a][b][ar]
                                                                exp.byte_size = _bytes_size[b]
                                                                exp.data_rate = _data_rate[d]
                                                                exp.selectivity = _selectivity[s]
                                                                exp.cpu = _cpu_ips[b]
                                                                exp.memory = 7500
                                                                exp.cpu_src = _cpu_ips[b]
                                                                exp.placements = base

                                                                exp.reconfig_iterations = _reconfig_iterations[it]
                                                                exp.reconfig_strategy = _reconfig_strategies[st]
                                                                exp.reconfig_constant = _reconfig_constants[st][ct]
                                                                exp.reconfig_alpha = _reconfig_alpha[st][al]
                                                                exp.reconfig_gamma = _reconfig_gamma[st][gm]
                                                                exp.reconfig_lambda = _reconfig_lambda[st][lb]
                                                                exp.reconfig_epilson = _reconfig_epilson[st][ep]

                                                                exp.read_from_traced_data_reconfig = read_from_traced_data_reconfig
                                                                exp.save_traced_data_reconfig = save_traced_data_reconfig
                                                                exp.dir_trace_datasets = dir_trace_datasets
                                                                exp.finish_after_tracing = finish_after_tracing

                                                                id = id + 1
                                                                _scenarios.append(exp)
    return _scenarios


def build_ini_file(_scenario=experiment_configuration(), _slt='', _index=0, _dir=''):
    _name = _slt + '_id_' + str(
        _scenario.id) + '_net_' + _scenario.net + '_app_' + _scenario.app + '_st_' + str(
        _scenario.reconfig_strategy)

    _name = _name.replace('"', "")

    _file = _name + '.ini'
    copyfile('general_d.ini', _dir + _file)

    _search = ["lb_scenario_0",
               "1010s",
               "a1.dot",
               "n1.xml",
               "strategy = 0",
               "..\/results\/",
               "selectivity_min = .1",
               "selectivity_max = 1",
               "datarate_min = .1",
               "datarate_max = 1",
               "cpu_min = 10",
               "cpu_max = 2500",
               "cpusrc_min = 1",
               "cpusrc_max = 100",
               "memory_min = 100",
               "memory_max = 7500",
               "arrivalrate_min = 1000",
               "arrivalrate_max = 15000",
               "bytesmsg_min = 100",
               "bytesmsg_max = 2500",
               "1-13 6-6",
               "reconfig_iterations = 10000",
               "reconfig_constant = .9",
               "reconfig_alpha = .1",
               "reconfig_gamma = 1",
               "reconfig_lambda = 1",
               "reconfig_epilson = .6",
               "reconfig_strategy = 2",
               "read_from_traced_data_reconfig = false",
               "save_traced_data_reconfig = false",
               "finish_after_tracing = false"
               ]


    _replace = [_name,
                str(1000 + int(_scenario.time)) + 's',
                _scenario.app + '.dot',
                _scenario.net + '.xml',
                'strategy = ' + str(_index + 1),
                _dir.replace("/", "\/"),
                "selectivity_min = " + str(_scenario.selectivity),
                "selectivity_max = " + str(_scenario.selectivity),
                "datarate_min = " + str(_scenario.data_rate),
                "datarate_max = " + str(_scenario.data_rate),
                "cpu_min = " + str(_scenario.cpu),
                "cpu_max = " + str(_scenario.cpu),
                "cpusrc_min = " + str(_scenario.cpu_src),
                "cpusrc_max = " + str(_scenario.cpu_src),
                "memory_min = " + str(_scenario.memory),
                "memory_max = " + str(_scenario.memory),
                "arrivalrate_min = " + str(_scenario.arrival_rate),
                "arrivalrate_max = " + str(_scenario.arrival_rate),
                "bytesmsg_min = " + str(_scenario.byte_size),
                "bytesmsg_max = " + str(_scenario.byte_size),
                _scenario.placements,
                "reconfig_iterations = " + str(_scenario.reconfig_iterations),
                "reconfig_constant = " + str(_scenario.reconfig_constant),
                "reconfig_alpha = " + str(_scenario.reconfig_alpha),
                "reconfig_gamma = " + str(_scenario.reconfig_gamma),
                "reconfig_lambda = " + str(_scenario.reconfig_lambda),
                "reconfig_epilson = " + str(_scenario.reconfig_epilson),
                "reconfig_strategy = " + str(_scenario.reconfig_strategy),
                "read_from_traced_data_reconfig = " + str(_scenario.read_from_traced_data_reconfig),
                "save_traced_data_reconfig = " + str(_scenario.save_traced_data_reconfig),
                "finish_after_tracing = " + str(_scenario.finish_after_tracing)
                ]

    # _base = 3454326
    # for s in range(56):
    #     _search.append("seed-" + str(s) + "-lcg32 = " + str(_base + s))
    #     _replace.append("seed-" + str(s) + "-lcg32 = " + str(_scenario.seeds[s]))

    for r in range(len(_search)):
        print 'sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file
        os.system('sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file)

    return _file, _name


#
def execute_simulation(_scenario=experiment_configuration()):
    os.chdir(_scenario.directory)

    _folder_name = 'net_' + _scenario.net + '_app_' + _scenario.app

    _sfolder_name = "arr_" + str(_scenario.arrival_rate) + '_bytes_' + str(
        _scenario.byte_size) + '_rate_' + str(_scenario.data_rate) + '_selec_' + str(
        _scenario.selectivity) + '_plac_' + _scenario.placements.replace('-', "").replace(" ", "_")

    _sfolder_name = _sfolder_name.replace(".", "")

    os.system("mkdir ../results")
    os.system("mkdir ../results/" + _folder_name)
    _root_dir_exp = "../results/" + _folder_name
    _working_dir = "../results/" + _folder_name + "/" + _sfolder_name + "/"
    os.system("mkdir " + _working_dir)
    for s in range(len(_scenario.slt)):
        if (_scenario.slt[s] == "lb"):
            _file_name, _name_config = build_ini_file(_scenario=_scenario, _slt=_scenario.slt[s], _index=s,
                                                      _dir=_working_dir)
            os.system(
                "../fogStream -u Cmdenv --cmdenv-redirect-output=true --cmdenv-express-mode=true -l ../src/fogStream -n ../src:. -f " + _working_dir + _file_name + " -c " + _name_config)
            #
            # os.system(
            #     "rm " + _working_dir + "results/*.vci " + _working_dir + "results/*.sca " + _working_dir + "results/*.vec ")  # + _working_dir + "results/*.out " + _working_dir + "/*.out"

            # os.system("mv " + _working_dir + _name_config + ".cfg " + _working_dir + _scenario.slt[s] + \
            #           '_st_' + str(_scenario.reconfig_strategy) + ".cfg")
            #
            # os.system("mv " + _working_dir + _name_config + ".cc " + _working_dir + _scenario.slt[s] + \
            #           '_st_' + str(_scenario.reconfig_strategy) + ".cc")
            #
            # os.system("mv " + _working_dir + _name_config + ".ss " + _working_dir + _scenario.slt[s] + \
            #           '_st_' + str(_scenario.reconfig_strategy) + ".ss")

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='base directory')

    parser.add_argument('-t', '--time',
                        help='time for the experiment')

    parser.add_argument('-p', '--process',
                        help='number of simultaneous processes')

    parser.add_argument('-i', '--id',
                        help='initial id for the experiment set')

    parser.add_argument('-r', '--readfromtraceddata',
                        help='read operator statistics from traced data')

    parser.add_argument('-s', '--savetraceddata',
                        help='save operator statistics to traced data')

    parser.add_argument('-f', '--finishaftertracing',
                        help='run the scenarios for collecting the operator statistics')

    parser.add_argument('-l', '--traceddatasetlocation',
                        help='directory of the operator statistics')

    args = parser.parse_args()

    if args.id:
        if args.directory:
            if args.process:
                # if args.experiments:
                # if args.seed:
                if args.time:
                    if not args.readfromtraceddata:
                        args.readfromtraceddata = 'false'

                    if not args.savetraceddata:
                        args.savetraceddata = 'false'

                    if not args.finishaftertracing:
                        args.finishaftertracing = 'false'

                    if (args.readfromtraceddata or args.savetraceddata and args.traceddatasetlocation) or (
                            not args.readfromtraceddata and not args.savetraceddata):
                        slt = ["cloud", "rv", "rvrs", "lb"]

                        net = ["n3"]
                        app = ["app0"]  # , "app3"]

                        # bytes_size = [10, 50000, 200000]
                        bytes_size = [10, 200000]
                        arrival_rate = [[[1249999], [62]]]  # ,
                        # arrival_rate = [[[124999, 624999, 1249999], [24, 124, 249], [6, 31, 62]]]#,
                        # [[124999, 374999, 624999], [24, 74, 124], [6, 19, 31]],
                        # [[124999, 218749, 300000], [24, 43, 62], [6, 10, 15]],
                        # [[124999, 137499, 150000], [24, 27, 30], [6, 7, 8]]]
                        # cpu_ips = [3.7952, 18976, 75000]
                        cpu_ips = [3.7952, 75000]

                        # data_rate = [1, .25]
                        data_rate = [1, .25]
                        selectivity = [1, .25]

                        sinks_app = [[[5]]]  # ,
                        # [[9], [10]],
                        # [[8], [9]],
                        # [[7], [8]]]

                        sink_placement = [[[14, 24, 10]]]  # ,
                        # [[9, 24, 10], [15, 25, 10]],
                        # [[9, 24, 10], [15, 25, 10]],
                        # [[9, 24, 10], [15, 25, 10]]]

                        # net = ["n3"]
                        # app = ["app0"]
                        # arrival_rate = [[[1]]]
                        # bytes_size = [1000]
                        # data_rate = [1]
                        # selectivity = [1]
                        # sinks_app = [[[5]]]
                        # sink_placement = [[[14]]]

                        source_placement = [3]

                        reconfig_iterations = [10000]

                        # reconfig_strategies = [0, 1, 2, 3, 4, 5]

                        # reconfig_constants = [[.005, .05, .1, .9, 1], [.005, .05, .1, .9, 1], [1],
                        #                       [.005, .05, .1, .9, 1]]
                        #
                        # reconfig_alpha = [[1], [.005, .05, .1, .9, 1], [.005, .05, .1, .9, 1], [1]]
                        #
                        # reconfig_gamma = [[1], [.005, .05, .1, .9, 1], [.005, .05, .1, .9, 1], [1]]
                        #
                        # reconfig_lambda = [[1], [.005, .05, .1, .9, 1], [1], [1]]
                        #
                        # reconfig_epilson = [[1], [1], [.005, .05, .1, .9, 1], [.005, .05, .1, .9, 1]]

                        reconfig_strategies = [0]

                        reconfig_constants = [[.005, .05, .1, .9, 1, 2]]

                        reconfig_alpha = [[1]]

                        reconfig_gamma = [[1]]

                        reconfig_lambda = [[1]]

                        reconfig_epilson = [[1]]

                        scenarios = build_scenarios(_net=net,
                                                    _app=app,
                                                    _slt=slt,
                                                    _dir=args.directory,
                                                    _time=args.time,
                                                    _initial_id=int(args.id),
                                                    _arrival_rate=arrival_rate,
                                                    _bytes_size=bytes_size,
                                                    _data_rate=data_rate,
                                                    _selectivity=selectivity,
                                                    _sink_placement=sink_placement,
                                                    _source_placement=source_placement,
                                                    _sinks_app=sinks_app,
                                                    _cpu_ips=cpu_ips,
                                                    _reconfig_strategies=reconfig_strategies,
                                                    _reconfig_iterations=reconfig_iterations,
                                                    _reconfig_constants=reconfig_constants,
                                                    _reconfig_alpha=reconfig_alpha,
                                                    _reconfig_gamma=reconfig_gamma,
                                                    _reconfig_lambda=reconfig_lambda,
                                                    _reconfig_epilson=reconfig_epilson,
                                                    read_from_traced_data_reconfig=args.readfromtraceddata,
                                                    save_traced_data_reconfig=args.savetraceddata,
                                                    dir_trace_datasets=args.traceddatasetlocation,
                                                    finish_after_tracing=args.finishaftertracing
                                                    )

                        if scenarios.__len__() > 0:
                            # pool = Pool(processes=int(args.process))
                            # pool.map(execute_simulation, scenarios)
                            execute_simulation(scenarios[5])
                            # execute_simulation(scenarios[21])
                            # execute_simulation(scenarios[47])
                            # execute_simulation(scenarios[250])
                        else:
                            print 'Erro to create the execution scenarios!'
                else:
                    print 'Argument -t (simulation time) must be filled!'
            # else:
            #     print 'Argument -s (base seed) must be filled!'
            # else:
            #     print 'Argument -e (number of experiments) must be filled!'
            else:
                print 'Argument -p (number of parallel processes) must be filled!'
        else:
            print 'Argument -d (directory of simulations) must be filled!'
    else:
        print 'Argument -i (id for experiment set) must be filled!'

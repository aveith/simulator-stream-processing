import argparse
import os
import random
from multiprocessing import Pool
from shutil import copyfile
import time
import fileinput
import itertools


class experiment_configuration:
    def __init__(self, id=0, directory='', experiment=0,
                 reconfig_strategy=0, reconfig_iterations=10000, reconfig_constant=.9, reconfig_alpha=.1,
                 reconfig_gamma=1, reconfig_lambda=1, reconfig_epilson=.6, read_from_traced_data_reconfig='false',
                 save_traced_data_reconfig='false', dir_trace_datasets='', finish_after_tracing='false',
                 reconfig_applying_rp='false'):
        self.id = id
        self.directory = directory
        self.experiment = experiment
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
        self.reconfig_applying_rp = reconfig_applying_rp


def build_scenarios(_dir='', _initial_id=0,
                    _reconfig_strategies=[],
                    _reconfig_iterations=[], _reconfig_constants=[], _reconfig_alpha=[], _reconfig_gamma=[],
                    _reconfig_lambda=[], _reconfig_epilson=[], read_from_traced_data_reconfig='false',
                    save_traced_data_reconfig='false', dir_trace_datasets='', finish_after_tracing='false',
                    reconfig_applying_rp=[]):
    _scenarios = []
    id = _initial_id
    _exp = _initial_id

    for it in range(len(_reconfig_iterations)):
        for st in range(len(_reconfig_strategies)):
            for ct in range(len(_reconfig_constants[st])):
                for al in range(len(_reconfig_alpha[st])):
                    for gm in range(len(_reconfig_gamma[st])):
                        for lb in range(len(_reconfig_lambda[st])):
                            for ep in range(len(_reconfig_epilson[st])):
                                for ap in range(len(reconfig_applying_rp)):
                                    exp = experiment_configuration()
                                    exp.id = id
                                    exp.directory = _dir
                                    exp.experiment = _exp

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
                                    exp.reconfig_applying_rp = reconfig_applying_rp[ap]

                                    id = id + 1
                                    _scenarios.append(exp)


    return _scenarios


def build_ini_file(_scenario=experiment_configuration(), _dir=''):
    _name = 'id_' + str(
        _scenario.id) + '_' + _scenario.reconfig_applying_rp

    _name = _name.replace('"', "")

    _file = _name + '.ini'
    copyfile('general_d.ini', _dir + _file)

    _search = ["lb_scenario_0",
               "..\/results\/",
               "reconfig_iterations = 10000",
               "reconfig_constant = .9",
               "reconfig_alpha = .1",
               "reconfig_gamma = 1",
               "reconfig_lambda = 1",
               "reconfig_epilson = .6",
               "reconfig_strategy = 2",
               "read_from_traced_data_reconfig = false",
               "save_traced_data_reconfig = false",
               "finish_after_tracing = false",
               "reconfig_applying_rp = true"
               ]

    _replace = [_name,
                _dir.replace("/", "\/"),
                "reconfig_iterations = " + str(_scenario.reconfig_iterations),
                "reconfig_constant = " + str(_scenario.reconfig_constant),
                "reconfig_alpha = " + str(_scenario.reconfig_alpha),
                "reconfig_gamma = " + str(_scenario.reconfig_gamma),
                "reconfig_lambda = " + str(_scenario.reconfig_lambda),
                "reconfig_epilson = " + str(_scenario.reconfig_epilson),
                "reconfig_strategy = " + str(_scenario.reconfig_strategy),
                "read_from_traced_data_reconfig = " + str(_scenario.read_from_traced_data_reconfig),
                "save_traced_data_reconfig = " + str(_scenario.save_traced_data_reconfig),
                "finish_after_tracing = " + str(_scenario.finish_after_tracing),
                "reconfig_applying_rp = " + str(_scenario.reconfig_applying_rp)
                ]

    for r in range(len(_search)):
        print 'sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file
        os.system('sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file)

    return _file, _name


#
def execute_simulation(_scenario=experiment_configuration()):
    os.chdir(_scenario.directory)

    _folder_name = 'net_3_app_1'

    # _sfolder_name = "arr_" + str(_scenario.arrival_rate) + '_bytes_' + str(
    #     _scenario.byte_size) + '_rate_' + str(_scenario.data_rate) + '_selec_' + str(
    #     _scenario.selectivity) + '_plac_' + _scenario.placements.replace('-', "").replace(" ", "_")
    #
    # _sfolder_name = _sfolder_name.replace(".", "")

    os.system("mkdir ../results")
    os.system("mkdir ../results/" + _folder_name)
    _root_dir_exp = "../results/" + _folder_name
    _working_dir = "../results/" + _folder_name + "/"  # + _sfolder_name + "/"
    os.system("mkdir " + _working_dir)
    # for s in range(len(_scenario.slt)):
    # if (_scenario.slt[s] == "lb"):
    _file_name, _name_config = build_ini_file(_scenario=_scenario, _dir=_working_dir)
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

                if not args.readfromtraceddata:
                    args.readfromtraceddata = 'false'

                if not args.savetraceddata:
                    args.savetraceddata = 'false'

                if not args.finishaftertracing:
                    args.finishaftertracing = 'false'

                if (args.readfromtraceddata or args.savetraceddata and args.traceddatasetlocation) or (
                        not args.readfromtraceddata and not args.savetraceddata):

                    reconfig_iterations = [10000]

                    reconfig_rp = ['true', 'false']

                    reconfig_strategies = [2]

                    reconfig_constants = [[1]]

                    reconfig_alpha = [[.001, .01, .1, .9, 2]]

                    reconfig_gamma = [[.005, .05, .1, .9, 1, 2]]

                    reconfig_lambda = [[1]]

                    reconfig_epilson = [[1]]

                    scenarios = build_scenarios(_dir=args.directory,
                                                _initial_id=int(args.id),
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
                                                finish_after_tracing=args.finishaftertracing,
                                                reconfig_applying_rp=reconfig_rp
                                                )

                    if scenarios.__len__() > 0:
                        pool = Pool(processes=int(args.process))
                        pool.map(execute_simulation, scenarios)
                        # execute_simulation(scenarios[5])
                        # execute_simulation(scenarios[0])
                        # execute_simulation(scenarios[47])
                        # execute_simulation(scenarios[250])
                    else:
                        print 'Erro to create the execution scenarios!'

            else:
                print 'Argument -p (number of parallel processes) must be filled!'
        else:
            print 'Argument -d (directory of simulations) must be filled!'
    else:
        print 'Argument -i (id for experiment set) must be filled!'

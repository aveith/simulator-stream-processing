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
                 byte_size=100, data_rate=.1, selectivity=.1, cpu=1000, cpu_src=10, memory=7500, placements=""):
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


def build_scenarios(_slt=[], _net=[], _app=[], _dir='', _time=0, _initial_id=0,
                    _arrival_rate=[], _bytes_size=[], _data_rate=[], _selectivity=[], _sink_placement=[],
                    _source_placement=[], _sinks_app=[[]], _cpu_ips=[]):
    _scenarios = []
    id = _initial_id
    _exp = _initial_id

    for n in range(len(_net)):
        for a in range(len(_app)):
            for b in range(len(_bytes_size)):
                for ar in range(len(_arrival_rate[b])):
                    for d in range(len(_data_rate)):
                        for s in range(len(_selectivity)):
                            lists = []
                            for i in range(len(_sinks_app[a])):
                                lists.append(list(itertools.product(_sinks_app[a][i], _sink_placement[a][i])))

                            for l in range(len(lists[0])):
                                base = "0-" + str(_source_placement[0]) + " " + str(lists[0][l][0]) + "-" + str(
                                    lists[0][l][1])
                                if lists.__len__() > 1:
                                    for m in range(len(lists[1])):
                                        placements = base + " " + str(lists[1][m][0]) + "-" + str(lists[1][m][1])
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

                                    id = id + 1
                                    _scenarios.append(exp)
    return _scenarios


def build_ini_file(_scenario=experiment_configuration(), _slt='', _index=0, _dir=''):
    _name = _slt + '_id_' + str(
        _scenario.id) + '_net_' + _scenario.net + '_app_' + _scenario.app

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
               "1-13 6-6"]

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
                _scenario.placements]

    # _base = 3454326
    # for s in range(56):
    #     _search.append("seed-" + str(s) + "-lcg32 = " + str(_base + s))
    #     _replace.append("seed-" + str(s) + "-lcg32 = " + str(_scenario.seeds[s]))

    for r in range(len(_search)):
        print 'sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file
        os.system('sed -i "s/' + _search[r] + '/' + _replace[r] + '/g" ' + _dir + _file)

    return _file, _name


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
        _file_name, _name_config = build_ini_file(_scenario=_scenario, _slt=_scenario.slt[s], _index=s,
                                                  _dir=_working_dir)
        os.system(
            "../src/fogStream -u Cmdenv --cmdenv-redirect-output=true --cmdenv-express-mode=false -n ../src:. " + _working_dir + _file_name + " -c " + _name_config)
        time.sleep(1)
        os.system(
            "mv " + _working_dir + "results/" + _name_config + "*.out " + _working_dir + _scenario.slt[s] + ".out")
        os.system(
            "mv " + _working_dir + "results/" + _name_config + "*.sca " + _working_dir + _scenario.slt[s] + ".sca")
        os.system(
            "mv " + _working_dir + "results/" + _name_config + "*.vec " + _working_dir + _scenario.slt[s] + ".vec")
        os.system(
            "mv " + _working_dir + "results/" + _name_config + "*.vci " + _working_dir + _scenario.slt[s] + ".vci")
        os.system("mv " + _working_dir + _name_config + ".ini " + _working_dir + _scenario.slt[s] + ".ini")
        os.system("mv " + _working_dir + _name_config + ".cfg " + _working_dir + _scenario.slt[s] + ".cfg")
        os.system("mv " + _working_dir + _name_config + ".cc " + _working_dir + _scenario.slt[s] + ".cc")
        os.system("mv " + _working_dir + _name_config + ".ss " + _working_dir + _scenario.slt[s] + ".ss")
        time.sleep(1)
        os.system("scavetool x " + _working_dir + _scenario.slt[
            s] + ".sca -o " + _working_dir + _scenario.slt[s] + ".csv")
        os.system("scavetool x " + _working_dir + _scenario.slt[
            s] + ".vec -o " + _working_dir + _scenario.slt[s] + "_vec.csv")

    time.sleep(1)
    if len([f for f in os.listdir(_working_dir)
            if f.endswith('.csv') and os.path.isfile(os.path.join(_working_dir, f))]) < 4:
        os.system("mkdir " + _root_dir_exp + "/discarded")
        os.system("mv " + _working_dir + " " +_root_dir_exp + "/discarded")


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='base directory')

    parser.add_argument('-t', '--time',
                        help='time for the experiment')

    parser.add_argument('-p', '--process',
                        help='number of simultaneous processes')

    # parser.add_argument('-e', '--experiments',
    #                     help='number of experiments')

    parser.add_argument('-i', '--id',
                        help='initial id for the experiment set')

    args = parser.parse_args()

    if args.id:
        if args.directory:
            if args.process:
                # if args.experiments:
                # if args.seed:
                if args.time:
                    slt = ["cloud", "rv", "rvrs", "lb"]
                    # net = ["n1", "n2"]
                    net = ["n3"]
                    app = ["app0", "app1", "app2", "app3"]
                    # arrival_rate = [1, 10, 100, 1000, 10000, 100000]
                    # bytes_size = [1000, 10000, 100000, 1000000, 10000000]

                    bytes_size = [10, 50000, 200000]
                    arrival_rate = [[[124999, 624999, 1249999], [24, 124, 249], [6, 31, 62]],
                                    [[124999, 374999, 624999], [24, 74, 124], [6, 19, 31]],
                                    [[124999, 218749, 300000], [24, 43, 62], [6, 10, 15]],
                                    [[124999, 137499, 150000], [24, 27, 30], [6, 7, 8]]]
                    cpu_ips = [3.7952, 18976, 75904]

                    data_rate = [1, .75, .50, .25]
                    selectivity = [1, .75, .50, .25]

                    # net = ["n2"]
                    # app = ["app0"]
                    # arrival_rate = [1]
                    # bytes_size = [1000]
                    # data_rate = [1]
                    # selectivity = [1]

                    sinks_app = [[[5]],
                                 [[9], [10]],
                                 [[8], [9]],
                                 [[7], [8]]]

                    sink_placement = [[[14, 24, 10]],
                                      [[9, 24, 10], [15, 25, 10]],
                                      [[9, 24, 10], [15, 25, 10]],
                                      [[9, 24, 10], [15, 25, 10]]]

                    source_placement = [3]


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
                                                _cpu_ips = cpu_ips)

                    if scenarios.__len__() > 0:
                        pool = Pool(processes=int(args.process))
                        pool.map(execute_simulation, scenarios)
                        # execute_simulation(scenarios[5])
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

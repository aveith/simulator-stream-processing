import argparse
import os
import random
from multiprocessing import Pool
from shutil import copyfile
import time
import fileinput


class experiment_configuration:
    def __init__(self, seeds=[], net='', app='', slt=[], time=0, id=0, directory='', experiment=0):
        self.seeds = seeds
        self.net = net
        self.app = app
        self.time = time
        self.id = id
        self.directory = directory
        self.slt = slt
        self.experiment = experiment


def build_scenarios(_experiments=0, _seed=0, _slt=[], _net=[], _app=[], _dir='', _time=0, _initia_id=0):
    random.seed(_seed)
    _scenarios = []
    id = _initia_id
    _exp = _initia_id
    for e in range(_experiments):
        seeds = []
        for r in range(56):
            seeds.append(random.randint(10000, 99999999))

        for n in range(len(_net)):
            for a in range(len(_app)):
                exp = experiment_configuration()
                exp.seeds = seeds
                exp.net = _net[n]
                exp.app = _app[a]
                exp.time = _time
                exp.id = id
                exp.directory = _dir
                exp.experiment = _exp
                exp.slt = _slt
                id = id + 1
                _scenarios.append(exp)
        _exp = _exp + 1

    return _scenarios


def build_ini_file(_scenario=experiment_configuration(), _slt='', _index=0, _dir=''):
    _name = _slt + '_id_' + str(_scenario.id) + '_net_' + _scenario.net + '_app_' + _scenario.app

    _file = _name + '.ini'
    copyfile('general.ini', _dir + _file)

    _search = ["lb_scenario_0",
               "1010s",
               "a1.dot",
               "n1.xml",
               "strategy = 0",
               "..\/results\/"]

    _replace = [_name,
                str(1000 + int(_scenario.time)) + 's',
                _scenario.app + '.dot',
                _scenario.net + '.xml',
                'strategy = ' + str(_index + 1),
                _dir.replace("/", "\/")]

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

    _folder_name = "results_" + str(
        _scenario.id) + '_exp_' + str(_scenario.experiment) + '_net_' + _scenario.net + '_app_' + _scenario.app

    os.system("mkdir ../results")
    os.system("mkdir ../results/experiment_" + str(_scenario.experiment))
    _root_dir_exp = "../results/experiment_" + str(_scenario.experiment)
    _working_dir = "../results/experiment_" + str(_scenario.experiment) + "/" + _folder_name + "/"
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
        os.system("mv " + _working_dir + _name_config + ".ini " + _working_dir + _scenario.slt[s] + ".ini")
        os.system("mv " + _working_dir + _name_config + ".cfg " + _working_dir + _scenario.slt[s] + ".cfg")
        os.system("mv " + _working_dir + _name_config + ".cc " + _working_dir + _scenario.slt[s] + ".cc")
        os.system("mv " + _working_dir + _name_config + ".ss " + _working_dir + _scenario.slt[s] + ".ss")
        time.sleep(1)
        os.system("scavetool x " + _working_dir + _scenario.slt[
            s] + ".sca -o " + _working_dir + _scenario.slt[s] + ".csv")

    time.sleep(1)
    if len([f for f in os.listdir(_working_dir)
            if f.endswith('.csv') and os.path.isfile(os.path.join(_working_dir, f))]) < 4:
        # os.system("rm -rf " + _working_dir)
        os.system("mkdir " + _root_dir_exp + "/discarded")
        os.system("mv " + _working_dir + " " + _root_dir_exp + "/discarded")


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='base directory')

    parser.add_argument('-t', '--time',
                        help='time for the experiment')

    parser.add_argument('-p', '--process',
                        help='number of simultaneous processes')

    parser.add_argument('-e', '--experiments',
                        help='number of experiments')

    parser.add_argument('-s', '--seed',
                        help='number of the base seed')

    parser.add_argument('-i', '--id',
                        help='initial id for the experiment set')

    args = parser.parse_args()

    if args.id:
        if args.directory:
            if args.process:
                if args.experiments:
                    if args.seed:
                        if args.time:
                            slt = ["cloud", "rv", "rvrs", "lb"]
                            net = ["n3"]
                            app = ["a1", "a2", "a3", "a4"]

                            # net = ["n1"]
                            # app = ["a1"]

                            scenarios = build_scenarios(_experiments=int(args.experiments),
                                                        _seed=int(args.seed),
                                                        _net=net,
                                                        _app=app,
                                                        _slt=slt,
                                                        _dir=args.directory,
                                                        _time=args.time,
                                                        _initia_id=int(args.id))

                            if scenarios.__len__() > 0:
                                pool = Pool(processes=int(args.process))
                                pool.map(execute_simulation, scenarios)
                                # execute_simulation(scenarios[2])
                            else:
                                print 'Erro to create the execution scenarios!'
                        else:
                            print 'Argument -t (simulation time) must be filled!'
                    else:
                        print 'Argument -s (base seed) must be filled!'
                else:
                    print 'Argument -e (number of experiments) must be filled!'
            else:
                print 'Argument -p (number of parallel processes) must be filled!'
        else:
            print 'Argument -d (directory of simulations) must be filled!'
    else:
        print 'Argument -i (id for experiment set) must be filled!'

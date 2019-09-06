import argparse
import os
from multiprocessing import Pool
import numpy as np
import pandas as pd


class dataset_configuration:
    def __init__(self, strategy=-1, reconfig_applying_rp=0, app='', configurations=-1, output_file='', dir='',
                 config_strategy=-1, action_heuristic=-1):
        self.strategy = strategy
        self.app = app
        self.configurations = configurations
        self.dir = dir
        self.output_file = output_file
        self.config_strategy = config_strategy
        self.reconfig_applying_rp = reconfig_applying_rp
        self.action_heuristic = action_heuristic


def build_scenarios(reconfig_strategies=[],
                    reconfig_applying_rp=[],
                    reconfig_app=[],
                    reconfig_configurations=0,
                    files_dir='',
                    output_directory='',
                    config_strategies=[],
                    action_heuristic=[]):
    _scenarios = []

    for c in range(len(config_strategies)):
        for r in range(len(reconfig_strategies)):
            for ac in range(len(reconfig_applying_rp)):
                for a in range(len(reconfig_app)):
                    for ah in range(len(action_heuristic)):
                        exp = dataset_configuration()
                        exp.strategy = reconfig_strategies[r]
                        exp.reconfig_applying_rp = reconfig_applying_rp[ac]
                        exp.app = reconfig_app[a]
                        exp.configurations = reconfig_configurations
                        exp.dir = files_dir
                        exp.config_strategy = config_strategies[c]
                        exp.action_heuristic = action_heuristic[ah]
                        exp.output_file = output_directory + 'st_' + str(exp.config_strategy) + '_rst_' + str(
                            exp.strategy) + '_rp_' + str(
                            exp.reconfig_applying_rp) + '_app_' + exp.app + '_ar_' + str(exp.action_heuristic) + '.csv'
                        _scenarios.append(exp)

    return _scenarios


def validate_scenario(reconfig_strategy=-1, config_strategy=-1, action_heuristic=-1, log_sample=pd.DataFrame()):
    if not log_sample['strategy'].max() == config_strategy or not log_sample[
                                                                      'reconfig'].max() == reconfig_strategy or not \
            log_sample['action_heuristic'].max() == action_heuristic:
        return False

    return True


def create_datasets(dataset=dataset_configuration()):
    dataset_pd = pd.DataFrame()
    dataset_pd.empty

    for c in range(dataset.configurations):
        merged_files = pd.DataFrame()
        merged_files.empty

        index = 0
        for root, dirs, files in os.walk(args.directory):
            for file in files:
                if file.endswith('.rlog'):
                    if 'app_' + str(dataset.app) + '_' in root and 'configuration_' + str(c) + '_' in root and 'rp_' + str(
                            dataset.reconfig_applying_rp) in root:
                        if validate_scenario(reconfig_strategy=dataset.strategy,
                                             config_strategy=dataset.config_strategy,
                                             action_heuristic=dataset.action_heuristic,
                                             log_sample=pd.read_csv(root + '/' + file,
                                                                    sep=';',
                                                                    index_col=None,
                                                                    nrows=5)):
                            # print 'READ FROM -> ' + root + '/' + file
                            tmp = pd.read_csv(root + '/' + file,
                                              sep=';')
                            iteration = tmp['iteration'].max()
                            tmp = tmp[(tmp['iteration'] == iteration)]

                            merged_files = merged_files.append(tmp, ignore_index=False)
                            # print merged_files

        if merged_files.__len__() > 0:
            min_value = merged_files['best_latency_time'].min()
            filtered_data = merged_files[
                (merged_files['best_latency_time'] <= min_value)]

            min_index = filtered_data['test_id'].min()

            filtered_data = filtered_data[
                (filtered_data['test_id'] <= min_index)]
            dataset_pd = dataset_pd.append(filtered_data, ignore_index=False)
            # print dataset_pd

    if dataset_pd.__len__() > 0:
        dataset_pd.to_csv(dataset.output_file, sep='\t')
        print 'SAVED TO -> ' + dataset.output_file
    else:
        print 'NO DATA -> ' + dataset.output_file


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='base directory')

    parser.add_argument('-o', '--output',
                        help='output directory')

    parser.add_argument('-p', '--process',
                        help='number of simultaneous processes')

    parser.add_argument('-c', '--configurations',
                        help='number of application configurations')

    args = parser.parse_args()

    if args.configurations:
        if args.directory:
            if args.process:
                if args.output:
                    config_strategies = [1, 4]
                    app = ['a1', 'a5']
                    reconfig_strategies = [0, 1, 2, 4]
                    reconfig_constants = [.0001, .001, .05]
                    reconfig_alpha = [.001, .1]  # no
                    reconfig_gamma = [.005, .05]
                    reconfig_lambda = [.005, .05]
                    reconfig_epsilon = [.001, .1]
                    reconfig_applying_rp = ['true', 'false']
                    action_heuristic = [0]

                    scenarios = build_scenarios(reconfig_strategies=reconfig_strategies,
                                                reconfig_app=app,
                                                reconfig_configurations=int(args.configurations),
                                                reconfig_applying_rp=reconfig_applying_rp,
                                                files_dir=args.directory,
                                                output_directory=args.output,
                                                config_strategies=config_strategies,
                                                action_heuristic=action_heuristic)
                    if scenarios.__len__() > 0:
                        # create_datasets(scenarios[0])
                        pool = Pool(processes=int(args.process))
                        pool.map(create_datasets, scenarios)
                        pool.close()
                        pool.join()
                else:
                    print 'Argument -o (output directory) must be filled!'
            else:
                print 'Argument -p (number of parallel processes) must be filled!'
        else:
            print 'Argument -d (directory of simulations) must be filled!'
    else:
        print 'Argument -c (number of application configurations) must be filled!'

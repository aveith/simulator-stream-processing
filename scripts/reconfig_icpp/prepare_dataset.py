import argparse
import os
from multiprocessing import Pool
import numpy as np
import pandas as pd


class dataset_configuration:
    def __init__(self, strategy=-1, price_model=-1, scenario=-1, app='', configurations=-1, output_file='', dir='',
                 config_strategy=-1):
        self.strategy = strategy
        self.price_model = price_model
        self.scenario = scenario
        self.app = app
        self.configurations = configurations
        self.dir = dir
        self.output_file = output_file
        self.config_strategy = config_strategy


def build_scenarios(reconfig_strategies=[],
                    reconfig_price_model=[],
                    reconfig_scenario=[],
                    reconfig_app=[],
                    reconfig_configurations=0,
                    files_dir='',
                    output_directory='',
                    config_strategies=[]):
    _scenarios = []

    for c in range(len(config_strategies)):
        for r in range(len(reconfig_strategies)):
            for p in range(len(reconfig_price_model)):
                for s in range(len(reconfig_scenario)):
                    for a in range(len(reconfig_app)):
                        exp = dataset_configuration()
                        exp.strategy = reconfig_strategies[r]
                        exp.price_model = reconfig_price_model[p]
                        exp.scenario = reconfig_scenario[s]
                        exp.app = reconfig_app[a]
                        exp.configurations = reconfig_configurations
                        exp.dir = files_dir
                        exp.config_strategy = config_strategies[c]
                        exp.output_file = output_directory + 'st_' + str(exp.config_strategy) + '_rst_' + str(exp.strategy) + '_pm_' + str(
                            exp.price_model) + '_sc_' + str(exp.scenario).replace('[', '').replace(']', '').replace(',',
                                                                                                                    '_').replace(
                            ' ', '').replace('.','') + '_app_' + exp.app + '.csv'
                        _scenarios.append(exp)

    return _scenarios


def validate_scenario(reconfig_strategy=-1, config_strategy=-1, scenario=[], price_model=-1, log_sample=pd.DataFrame()):
    field = ['latency_weight', 'migration_weight', 'costs_weight', 'wan_traffic_weight']
    if not log_sample['strategy'].max() == config_strategy or not log_sample[
                                                                      'reconfig'].max() == reconfig_strategy or not \
            log_sample['pricing_type'].max() == price_model:
        return False

    for s in range(len(scenario)):
        value = log_sample[field[s]].max()
        if scenario[s] != value:
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
                    if 'app_' + str(dataset.app) + '_' in root and 'configuration_' + str(c) in root:
                        if validate_scenario(reconfig_strategy=dataset.strategy,
                                             config_strategy=dataset.config_strategy, scenario=dataset.scenario,
                                             price_model=dataset.price_model, log_sample=pd.read_csv(root + '/' + file,
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
            min_value = merged_files['best_aggregate_cost'].min()
            filtered_data = merged_files[
                (merged_files['best_aggregate_cost'] <= min_value)]

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

                    app = ['a1', 'a2', 'a3', 'a4', 'a5', 'a10', 'a11', 'a12', 'a13', 'a14', 'app3']

                    reconfig_strategies = [1, 4]

                    reconfig_price_model = [0]

                    reconfig_scenario = [[1, 0, 0, 0], [.33, 0, .33, .33], [.25, .25, .25, .25], [.4, .2, .2, .2], [.7, .1, .1, .1]]

                    scenarios = build_scenarios(reconfig_strategies=reconfig_strategies,
                                                reconfig_price_model=reconfig_price_model,
                                                reconfig_scenario=reconfig_scenario,
                                                reconfig_app=app,
                                                reconfig_configurations=int(args.configurations),
                                                files_dir=args.directory,
                                                output_directory=args.output,
                                                config_strategies=config_strategies)
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

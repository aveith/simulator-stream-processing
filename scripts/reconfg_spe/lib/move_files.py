import numpy as np
import pandas as pd
import argparse
import os
from shutil import copyfile


def read_simulation_files(folder='', config=-1):
    merged_files = pd.DataFrame()

    index = 0
    for root, dirs, files in os.walk(folder):
        for file in files:
            if file.endswith('.rlog'):
                tmp = pd.read_csv(root + '/' + file,
                                  sep=';')
                configuraton = tmp['strategy'].max()
                if config == configuraton:
                    print 'READ FROM -> ' + root + '/' + file
                    iteration = tmp['iteration'].max()
                    tmp = tmp[(tmp['iteration'] == iteration)]

                    merged_files = merged_files.append(tmp, ignore_index=False)

    return merged_files


def select_best_results(results=pd.DataFrame()):
    reconfiguration_strategies = results.loc[:, ['reconfig']].drop_duplicates().values.flatten().tolist()
    best_values = []
    for s in range(len(reconfiguration_strategies)):
        base_data = results[(results['reconfig'] == reconfiguration_strategies[s])]
        max_value = base_data['best_gain_rate'].max()
        filtered_data = base_data[(base_data['best_gain_rate'] >= max_value)]
        filtered_data = filtered_data.iloc[0]
        best_values.append(filtered_data['test_id'].max())

    return best_values, reconfiguration_strategies


def move(configurations=[], folder='', destination='', reconfig_strategies=[], config=-1):
    pos = 0
    for c in range(len(configurations)):
        for root, dirs, files in os.walk(folder):
            for file in files:
                if file.endswith('.rfg') and str(configurations[c]) + '.rfg' == file:
                    copyfile(root + '/' + file, destination + 'config_' + str(config) + '_reconfig_' + str(
                        reconfig_strategies[pos]) + '.rfg')
                    pos += 1


def move_files(folder='', destination=''):
    configuration_strategies = [1, 2, 3, 4]
    for c in range(len(configuration_strategies)):
        read_results = read_simulation_files(folder=folder, config=configuration_strategies[c])

        if read_results.__len__() > 0:
            best_results, reconfig = select_best_results(results=read_results)

            if best_results.__len__() > 0:
                move(configurations=best_results, folder=folder, destination=destination, reconfig_strategies=reconfig,
                     config=configuration_strategies[c])


def main(folder='', destination=''):
    move_files(folder=folder, destination=destination)

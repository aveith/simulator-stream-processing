import argparse
import os

from mpl_toolkits.axes_grid.inset_locator import (inset_axes, InsetPosition,
                                                  mark_inset)

import pandas as pd
from mpl_toolkits.mplot3d import Axes3D
from multiprocessing import Pool
import argparse

from networkx import fiedler_vector
from numpy import _globals
from pytest import collect
from scipy.interpolate import griddata
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib import cm
import numpy as np
import pandas as pd
from math import pi
import matplotlib.patches as mpatches
from scipy._lib._ccallback_c import plus1b_t

plt.rcParams.update({'font.size': 20})


class stategy_results:
    def __init__(self, data=pd.DataFrame(), index=-1):
        self.data = data
        self.strategy_index = index


class configurations:
    def __init__(self):
        self.applications = ['a1', 'a2', 'a3', 'a4', 'a5', 'a10', 'a11', 'a12', 'a13', 'a14', 'app3']
        self.reconfig_strategies = [1, 4]
        self.config_strategies = [1, 4]
        self.action_heuristic = [0, 1, 2]
        self.config_st_names = ['Cloud-Only', 'Taneja']
        self.reconfig_strategies_name = ['TDTS-SARSA(' + r'$ \lambda $)', 'MCTS-UCT']


def load_files(dir=''):
    merged_files = pd.DataFrame()
    merged_files.empty

    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith('.csv'):
                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                merged_files = merged_files.append(tmp, ignore_index=False)

    return merged_files


def get_scenario_description(scenario=[], conf=configurations()):
    description = ''
    for s in range(len(scenario)):
        if scenario[s] > 0:
            description = description + conf.reconfig_scenarios_base[s] + ': ' + str(scenario[s]) + ' '
    return description.rstrip()


def validate_data(conf=configurations(), data=pd.DataFrame()):
    removed_data = pd.DataFrame()

    valid_data = pd.DataFrame()
    for strategy in range(len(conf.config_strategies)):
        for a in range(len(conf.applications)):
            for ac in range(len(conf.action_heuristic)):
                base_data = data[(data['action_heuristic'] == conf.action_heuristic[ac]) & (
                        data['strategy'] == conf.config_strategies[strategy]) & (
                                         data['app'] == '/dot/' + conf.applications[a] + '.dot')]

                configuration_values = base_data['seed_configuration'].unique()

                for c in range(len(configuration_values)):
                    conf_filter = base_data[(base_data['seed_configuration'] == configuration_values[c])]

                    print 'Filtered data: ' + str(conf_filter.__len__()) + ' Strategies: ' + str(
                        len(conf.reconfig_strategies) - 1)

                    if conf_filter.__len__() == (len(conf.reconfig_strategies)):
                        print 'Configuration - ' + str(configuration_values[c]) + ' - Valid data: App-' + \
                              conf.applications[a]

                        valid_data = valid_data.append(conf_filter, ignore_index=False)
                    else:
                        print 'Configuration - ' + str(configuration_values[c]) + ' - Invalid data: App-' + \
                              conf.applications[a]
                        removed_data = removed_data.append(conf_filter, ignore_index=False)

    return valid_data


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    args = parser.parse_args()

    if args.directory:
        data = load_files(args.directory)

        if data.__len__() > 0:
            conf = configurations()

            valid_data = validate_data(conf=conf, data=data)

            # valid_data.to_csv('data.csv', sep='\t')

            # Diagnostics
            combination = valid_data.loc[:,
                                 ['strategy', 'reconfig', 'app', 'action_heuristic', 'best_latency_time']]

            combination = combination.drop_duplicates()
            print combination.groupby(['strategy', 'reconfig', 'action_heuristic']).mean()



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
        self.applications = ['a1', 'a2', 'a3', 'a4', 'a5', 'a10', 'a11', 'a12', 'a13', 'app3']
        self.reconfig_strategies = [0, 1, 2, 3, 4, 5]
        self.config_strategies = [1, 4]
        self.action_heuristic = [0, 1, 2]
        self.reconfig_applying_rp = [0, 1]

        self.config_st_names = ['Cloud-Only', 'Taneja']
        self.reconfig_strategies_name = ['TDTS-SARSA(' + r'$ \lambda $)', 'MCTS-UCT']


def load_files(dir=''):
    merged_files = pd.DataFrame()
    merged_files.empty

    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith('.csv'):
                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                tmp['rp'] = 0 if 'rp_false' in file else 1
                merged_files = merged_files.append(tmp, ignore_index=False)

    return merged_files


def get_scenario_description(scenario=[], conf=configurations()):
    description = ''
    for s in range(len(scenario)):
        if scenario[s] > 0:
            description = description + conf.reconfig_scenarios_base[s] + ': ' + str(scenario[s]) + ' '
    return description.rstrip()


def create_dataset(conf=configurations(), data=pd.DataFrame()):
    dtt = pd.DataFrame()

    for a in range(len(conf.applications)):
        tmp_app = data[(data['app'] == '/dot/' + conf.applications[a] + '.dot')]

        for strategy in range(len(conf.config_strategies)):
            tmp_st= tmp_app[(tmp_app['strategy'] == conf.config_strategies[strategy])]

            for rcf in range(len(conf.reconfig_strategies)):
                tmp_rcf = tmp_st[(tmp_st['reconfig'] == conf.reconfig_strategies[rcf])]

                for rp in range(len(conf.reconfig_applying_rp)):
                    tmp_rp = tmp_rcf[(tmp_rcf['rp'] == conf.reconfig_applying_rp[rp])]

                    for ac in range(len(conf.action_heuristic)):
                        tmp_ac = tmp_rp[(tmp_rp['action_heuristic'] == conf.action_heuristic[ac])]

                        if tmp_ac.__len__() > 0:
                            min_value = tmp_ac['best_aggregate_cost'].min()
                            filtered_data = tmp_ac[
                                (tmp_ac['best_aggregate_cost'] <= min_value)]

                            min_index = filtered_data['test_id'].min()

                            filtered_data = filtered_data[
                                (filtered_data['test_id'] <= min_index)]
                            dtt = dtt.append(filtered_data, ignore_index=False)

    return dtt


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    args = parser.parse_args()

    if args.directory:
        data = load_files(args.directory)

        if data.__len__() > 0:
            conf = configurations()

            # valid_data = create_dataset(conf=conf, data=data)
            #
            # print 'From ' + str(data.__len__()) + ' to ' + str(valid_data.__len__())

            # valid_data.to_csv('data.csv', sep='\t')

            # Diagnostics
            combination = data.loc[:,
                          ['strategy', 'reconfig', 'app', 'action_heuristic', 'best_latency_time', 'rp']]

            combination.to_csv('data.csv', sep='\t')

            combination = combination.drop_duplicates()
            print combination.groupby(['strategy', 'reconfig', 'action_heuristic']).mean()

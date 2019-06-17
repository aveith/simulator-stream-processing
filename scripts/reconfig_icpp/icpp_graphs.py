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
        self.reconfig_price_models = [0]
        self.config_strategies = [1, 4]

        self.config_st_names = ['Cloud-Only', 'Taneja']
        self.reconfig_scenarios = [[1, 0, 0, 0], [.33, 0, .33, .33], [.25, .25, .25, .25], [.4, .2, .2, .2],
                                   [.7, .1, .1, .1]]
        self.reconfig_scenarios_base = ['Latency', 'Reconf. Overhead', '$', 'WAN',
                                        'Migration Number']
        self.reconfig_strategies_name = ['TDTS-SARSA(' + r'$ \lambda $)', 'MCTS-UCT']
        self.reconfig_scenarios_field = ['Latency', 'Reconf. Overhead', '$', '$',
                                         'WAN', 'Number of Migrations']


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


def weighted_radar_firstpart(conf=configurations(), data=pd.DataFrame()):
    conf.reconfig_scenarios = [[1, 0, 0, 0], [.33, 0, .33, .33]]
    for scenario in range(len(conf.reconfig_scenarios)):
        for pricing_type in range(len(conf.reconfig_price_models)):
            plt.cla()
            plt.clf()
            plt.gcf().set_size_inches(10.5, 5.5)
            iaxes = 0
            for strategy in range(len(conf.config_strategies)):
                categories = ['End-to-end\nLatency', 'Monetary\nCost', 'WAN\nTraffic']
                price = ''
                if conf.reconfig_price_models[pricing_type] == 0:
                    fields = ['real_number_msgs_app', 'best_latency_time', 'best_microsoft_pricing',
                              'best_wan_traffic']

                    root_fields = ['real_number_msgs_app', 'root_latency_time', 'root_microsoft_price',
                                   'root_wan_traffic']

                    root_base = ['app', 'real_number_msgs_app', 'root_latency_time', 'root_microsoft_price',
                                 'root_wan_traffic']
                    price = 'Microsoft'

                else:
                    fields = ['real_number_msgs_app', 'best_latency_time', 'best_aws_pricing',
                              'best_wan_traffic']

                    root_fields = ['real_number_msgs_app', 'root_latency_time', 'root_aws_price',
                                   'root_wan_traffic']

                    root_base = ['real_number_msgs_app', 'app', 'root_latency_time', 'root_aws_price',
                                 'root_wan_traffic']
                    price = 'AWS'

                base_data = data[(data['strategy'] == conf.config_strategies[strategy]) & (
                        data['pricing_type'] == conf.reconfig_price_models[pricing_type]) & (
                                         data['latency_weight'] == conf.reconfig_scenarios[scenario][0]) & (
                                         data['migration_weight'] == conf.reconfig_scenarios[scenario][1]) & (
                                         data['costs_weight'] == conf.reconfig_scenarios[scenario][2]) & (
                                         data['wan_traffic_weight'] == conf.reconfig_scenarios[scenario][3])]

                values = []
                for reconfig in range(len(conf.reconfig_strategies)):
                    rec_data = base_data[(base_data['reconfig'] == conf.reconfig_strategies[reconfig])]
                    local = []
                    for field in range(len(fields)):
                        # print fields[field]
                        if fields[field] != 'real_number_msgs_app':
                            local.append(((rec_data[fields[field]] / rec_data[fields[field]].sum()) * rec_data[
                                fields[field]]).sum())
                            # local.append(((rec_data['real_number_msgs_app'] * rec_data[
                            #     fields[field]]).sum() / rec_data['real_number_msgs_app']).sum())
                    values.append(local)

                root_data = base_data.loc[:, root_base]
                root_data = root_data.drop_duplicates()

                local = []
                for field in range(len(root_fields)):
                    # print root_fields[field]
                    if root_fields[field] != 'real_number_msgs_app':
                        local.append(((root_data[root_fields[field]] / root_data[root_fields[field]].sum()) * root_data[
                            root_fields[field]]).sum())
                        # local.append(((root_data['real_number_msgs_app'] * root_data[
                        #     root_fields[field]]).sum() / root_data['real_number_msgs_app']).sum())
                values.append(local)

                maximum = np.max(values, axis=0)
                # print maximum
                result = []
                for v in range(len(values)):
                    local = []
                    for i in range(len(values[v])):
                        local.append(((values[v][i]) / (maximum[i])) * 1)
                    result.append(local)

                labels = []
                for it in range(len(conf.reconfig_strategies_name)):
                    labels.append(conf.reconfig_strategies_name[it])

                radar_plot_general(data=result, labels=labels, categories=categories, iax=iaxes,
                                   strategy=conf.config_strategies[strategy])
                iaxes += 1

            plt.tight_layout()
            plt.savefig('Base: ' + conf.config_st_names[
                strategy] + ' Price: ' + price + ' Scenario: ' + str(conf.reconfig_scenarios[scenario]) + '.pdf')
            # plt.show()


def weighted_radar_thirdpart(conf=configurations(), data=pd.DataFrame()):
    values_maximum = []
    for scenario in range(len(conf.reconfig_scenarios)):
        for pricing_type in range(len(conf.reconfig_price_models)):
            for strategy in range(len(conf.config_strategies)):
                if conf.reconfig_price_models[pricing_type] == 0:
                    fields = ['real_number_msgs_app', 'best_latency_time', 'best_microsoft_pricing',
                              'best_migration_costs',
                              'best_wan_traffic']

                    root_fields = ['real_number_msgs_app', 'root_latency_time', 'root_microsoft_price',
                                   'root_wan_traffic']

                    root_base = ['app', 'real_number_msgs_app', 'root_latency_time', 'root_microsoft_price',
                                 'root_wan_traffic']

                    base_data = data[(data['strategy'] == conf.config_strategies[strategy]) & (
                            data['pricing_type'] == conf.reconfig_price_models[pricing_type]) & (
                                             data['latency_weight'] == conf.reconfig_scenarios[scenario][0]) & (
                                             data['migration_weight'] == conf.reconfig_scenarios[scenario][1]) & (
                                             data['costs_weight'] == conf.reconfig_scenarios[scenario][2]) & (
                                             data['wan_traffic_weight'] == conf.reconfig_scenarios[scenario][3])]

                    for reconfig in range(len(conf.reconfig_strategies)):
                        rec_data = base_data[(base_data['reconfig'] == conf.reconfig_strategies[reconfig])]
                        local = []
                        for field in range(len(fields)):
                            # print fields[field]
                            if fields[field] != 'real_number_msgs_app':
                                local.append(rec_data[fields[field]].max())
                                # local.append(((rec_data[fields[field]] / rec_data[fields[field]].sum()) * rec_data[
                                #     fields[field]]).sum())
                                # local.append((rec_data['real_number_msgs_app'] * rec_data[
                                #     fields[field]]).sum() / rec_data['real_number_msgs_app'].sum())
                        values_maximum.append(local)

                        root_data = base_data.loc[:, root_base]
                        root_data = root_data.drop_duplicates()
                        local = []
                        for field in range(len(root_fields)):
                            # print root_fields[field]
                            if root_fields[field] != 'real_number_msgs_app':
                                local.append(root_data[root_fields[field]].max())
                                # local.append(
                                #     ((root_data[root_fields[field]] / root_data[root_fields[field]].sum()) * root_data[
                                #         root_fields[field]]).sum())
                                # local.append((root_data['real_number_msgs_app'] * root_data[
                                #     root_fields[field]]).sum() / root_data['real_number_msgs_app'].sum())
                            if root_fields[field] == 'root_microsoft_price':
                                local.append(0)
                        values_maximum.append(local)

    conf.reconfig_scenarios = [[.25, .25, .25, .25], [.4, .2, .2, .2], [.7, .1, .1, .1]]
    for scenario in range(len(conf.reconfig_scenarios)):
        for pricing_type in range(len(conf.reconfig_price_models)):
            plt.cla()
            plt.clf()
            plt.gcf().set_size_inches(13, 6)
            iaxes = 0
            for strategy in range(len(conf.config_strategies)):
                categories = ['End-to-end\nLatency', 'Monetary\nCost', 'Reconfiguration\nOverhead', 'WAN\nTraffic']
                price = ''
                if conf.reconfig_price_models[pricing_type] == 0:
                    fields = ['real_number_msgs_app', 'best_latency_time', 'best_microsoft_pricing',
                              'best_migration_costs',
                              'best_wan_traffic']

                    price = 'Microsoft'

                    base_data = data[(data['strategy'] == conf.config_strategies[strategy]) & (
                            data['pricing_type'] == conf.reconfig_price_models[pricing_type]) & (
                                             data['latency_weight'] == conf.reconfig_scenarios[scenario][0]) & (
                                             data['migration_weight'] == conf.reconfig_scenarios[scenario][1]) & (
                                             data['costs_weight'] == conf.reconfig_scenarios[scenario][2]) & (
                                             data['wan_traffic_weight'] == conf.reconfig_scenarios[scenario][3])]

                    values = []
                    for reconfig in range(len(conf.reconfig_strategies)):
                        rec_data = base_data[(base_data['reconfig'] == conf.reconfig_strategies[reconfig])]
                        local = []
                        for field in range(len(fields)):
                            # print fields[field]
                            if fields[field] != 'real_number_msgs_app':
                                local.append(((rec_data[fields[field]] / rec_data[fields[field]].sum()) * rec_data[
                                    fields[field]]).sum())
                                # local.append((rec_data['real_number_msgs_app'] * rec_data[
                                #     fields[field]]).sum() / rec_data['real_number_msgs_app'].sum())
                        values.append(local)

                    maximum = np.max(values_maximum, axis=0)
                    # print maximum
                    result = []
                    for v in range(len(values)):
                        local = []
                        for i in range(len(values[v])):
                            local.append(((values[v][i]) / (maximum[i])) * 1)
                        result.append(local)

                    labels = []
                    for it in range(len(conf.reconfig_strategies_name)):
                        labels.append(conf.reconfig_strategies_name[it])

                    radar_plot_thirdpart(data=result, labels=labels, categories=categories, iax=iaxes,
                                         strategy=conf.config_strategies[strategy])
                    iaxes += 1

            plt.subplots_adjust(wspace=0, hspace=0)
            plt.tight_layout()
            plt.savefig('Base: ' + conf.config_st_names[
                strategy] + ' Price: ' + price + ' Scenario: ' + str(conf.reconfig_scenarios[scenario]) + '.pdf')
            # plt.show()


def radar_plot_thirdpart(data=[], labels=[], categories=[], iax=0, strategy=0):
    colors = ['b', 'r', 'g', 'darkgrey', 'y', 'm', 'k']
    hatch = ['-', '\\', '//', 'o', 'O', '.', 'x', '+', '*']
    # ------- PART 1: Create background
    # number of variable
    categories = list(categories)

    N = len(categories)

    # What will be the angle of each axis in the plot? (we divide the plot / number of variable)
    angles = [n / float(N) * 2 * pi for n in range(N)]
    angles += angles[:1]

    # Initialise the spider plot
    ax = plt.subplot(121 + iax, polar=True)

    # If you want the first axis to be on top:
    ax.set_theta_offset(pi / 2)
    ax.set_theta_direction(-1)

    # Draw one axe per variable + add labels labels yet
    plt.xticks(angles, categories)

    # Draw ylabels
    ax.set_rlabel_position(0)
    plt.yticks([0, .10, .20, .30, .40, .50, .60, .70, .80, .90, 1],
               ["0", ".1", ".2", ".3", ".4", ".5", ".6", ".7", ".8", ".9", "1"], color="grey", size=13)
    plt.ylim(0, 1)

    # ------- PART 2: Add plots
    # Plot each individual = each line of the data
    # I don't do a loop, because plotting more than 3 groups makes the chart unreadable
    ax.legend().set_visible(False)
    leg = []
    for d in range(len(data)):
        values = data[d]
        values += values[:1]

        ax.plot(angles, values, linewidth=3, linestyle='solid', color=colors[d],
                label=labels[d])
        leg.append(
            mpatches.Patch(facecolor=colors[d], alpha=1, label=labels[d]))

        # if data.__len__() - 1 == d:
        #     ax.fill(angles, values, colors[d], alpha=.6, fill=True)
        #     leg.append(
        #         mpatches.Patch(facecolor=colors[d], alpha=1,
        #                        label=labels[d]))
        # else:
        # ax.fill(angles, values, colors[d], alpha=1, fill=False,
        #         edgecolor=colors[d], hatch=hatch[d])
        # leg.append(
        #     mpatches.Patch(facecolor=colors[d], alpha=1, hatch=hatch[d],
        #                        label=labels[d]))

    #
    # plt.title(title + '\n' + subtitle)
    if iax == 1:
        # ax.legend(handles=leg, loc='lower center', fontsize=16, bbox_to_anchor=(0.5, -0.45)).set_visible(True)
        ax.legend(handles=leg, fontsize=16).set_visible(True)


def radar_plot_general(data=[], labels=[], categories=[], iax=0, strategy=0):
    colors = ['b', 'r', 'g', 'darkgrey', 'y', 'm', 'k']
    hatch = ['-', '\\', '//', 'o', 'O', '.', 'x', '+', '*']
    # ------- PART 1: Create background
    # number of variable
    categories = list(categories)

    N = len(categories)

    # What will be the angle of each axis in the plot? (we divide the plot / number of variable)
    angles = [n / float(N) * 2 * pi for n in range(N)]
    angles += angles[:1]

    # Initialise the spider plot
    ax = plt.subplot(121 + iax, polar=True)

    # If you want the first axis to be on top:
    ax.set_theta_offset(pi / 2)
    ax.set_theta_direction(-1)

    # Draw one axe per variable + add labels labels yet
    plt.xticks(angles, categories)

    # Draw ylabels
    ax.set_rlabel_position(0)
    plt.yticks([0, .10, .20, .30, .40, .50, .60, .70, .80, .90, 1],
               ["0", ".1", ".2", ".3", ".4", ".5", ".6", ".7", ".8", ".9", "1"], color="grey", size=7)
    plt.ylim(0, 1)

    # ------- PART 2: Add plots
    # Plot each individual = each line of the data
    # I don't do a loop, because plotting more than 3 groups makes the chart unreadable
    ax.legend().set_visible(False)
    leg = []
    for d in range(len(data)):
        values = data[d]
        values += values[:1]

        if d == data.__len__() - 1 and strategy == 1:
            ax.plot(angles, values, linewidth=3, linestyle='solid', color=colors[d],
                    label='Cloud-Only')
            leg.append(
                mpatches.Patch(facecolor=colors[d], alpha=1, label='Cloud-Only'))

        elif d == data.__len__() - 1 and strategy == 4:
            ax.plot(angles, values, linewidth=3, linestyle='solid', color=colors[3],
                    label='Taneja')
            leg.append(
                mpatches.Patch(facecolor=colors[3], alpha=1, label='Taneja'))
            leg.append(
                mpatches.Patch(facecolor=colors[2], alpha=1, label='Cloud-Only'))

        else:
            ax.plot(angles, values, linewidth=3, linestyle='solid', color=colors[d],
                    label=labels[d])
            leg.append(
                mpatches.Patch(facecolor=colors[d], alpha=1, label=labels[d]))

        # if data.__len__() - 1 == d:
        #     ax.fill(angles, values, colors[d], alpha=.6, fill=True)
        #     leg.append(
        #         mpatches.Patch(facecolor=colors[d], alpha=1,
        #                        label=labels[d]))
        # else:
        # ax.fill(angles, values, colors[d], alpha=1, fill=False,
        #         edgecolor=colors[d], hatch=hatch[d])
        # leg.append(
        #     mpatches.Patch(facecolor=colors[d], alpha=1, hatch=hatch[d],
        #                        label=labels[d]))

    #
    # plt.title(title + '\n' + subtitle)
    if iax == 1:
        ax.legend(handles=leg, loc='lower center', fontsize=16, bbox_to_anchor=(0.5, -0.17)).set_visible(True)
        # ax.legend(handles=leg, fontsize=16).set_visible(True)


def bar_plot_rl_2(conf=configurations(), data=pd.DataFrame()):
    pos = [1, 2]
    width = 0.25
    dist = 0.15
    colors = ['b', 'r', 'g', 'darkgrey', 'y', 'm', 'k']
    categories = ['Cloud-only', 'TCEP']
    algorithm = []
    pricing_type = 0
    hatches = ['-', '+', 'x', '\\', '*', 'o', 'O', '.']

    fig, ax = plt.subplots(1, 2, figsize=(10.5, 5.5), sharey="row", sharex=True)
    _ax = None

    for scenario in range(len(conf.reconfig_scenarios)):
        if scenario in [0, 1]:
            r1 = [p - dist for p in pos]
            r2 = [p + dist for p in pos]
            for strategy in range(len(conf.config_strategies)):
                down_time = {}
                num_migrations = {}
                for reconfig in range(len(conf.reconfig_strategies)):
                    base_data = data[(data['strategy'] == conf.config_strategies[strategy]) & (
                            data['pricing_type'] == conf.reconfig_price_models[pricing_type]) & (
                                             data['latency_weight'] == conf.reconfig_scenarios[scenario][0]) & (
                                             data['migration_weight'] == conf.reconfig_scenarios[scenario][1]) & (
                                             data['costs_weight'] == conf.reconfig_scenarios[scenario][2]) & (
                                             data['wan_traffic_weight'] == conf.reconfig_scenarios[scenario][3]) & (
                                             data['reconfig'] == conf.reconfig_strategies[reconfig])]
                    down_time[reconfig] = np.mean(base_data['best_migration_costs'])

                bar = ax[scenario].bar([r1[strategy], r2[strategy]], down_time.values(), width, color=colors, ecolor='black')
                ax[scenario].set_ylim([0, 0.15])
                for barz, pattern in zip(bar, ('\\', 'o', 'x', '\\', '*', 'o', 'O', '.')):
                    barz.set_hatch(pattern)
                # ax[scenario].legend().set_visible(False)

                # ax[scenario].set_xticklabels([r for r in pos], categories)
            if scenario == 1:
                ax[scenario].legend(bar, conf.reconfig_strategies_name, loc='lower center', fontsize=16)
            else:
                ax[scenario].set_ylabel('Reconfiguration Overhead (s)', fontsize=20)
            plt.xticks([r for r in pos], categories)

            # ax[scenario].tick_params(labelsize=20)
    plt.tight_layout()
    # plt.show()
    fig_name = "reconfiguration-overhead"
    plt.savefig(fig_name + '.png')


def bar_plot_rl(conf=configurations(), data=pd.DataFrame()):
    pos = [1, 2]
    width = 0.25
    dist = 0.15
    colors = ['b', 'r', 'g', 'darkgrey', 'y', 'm', 'k']
    categories = ['Reconfiguration\nOverhead', 'Migration\nNumber']
    algorithm = []
    pricing_type = 0

    for scenario in range(len(conf.reconfig_scenarios)):
        if scenario == 0 or scenario == 1:
            fig, ax1 = plt.subplots(1, 2, figsize=(10.5, 5.5), sharey=True)
            _ax = None

            for strategy in range(len(conf.config_strategies)):
                down_time = {}
                num_migrations = {}
                for reconfig in range(len(conf.reconfig_strategies)):
                    base_data = data[(data['strategy'] == conf.config_strategies[strategy]) & (
                            data['pricing_type'] == conf.reconfig_price_models[pricing_type]) & (
                                             data['latency_weight'] == conf.reconfig_scenarios[scenario][0]) & (
                                             data['migration_weight'] == conf.reconfig_scenarios[scenario][1]) & (
                                             data['costs_weight'] == conf.reconfig_scenarios[scenario][2]) & (
                                             data['wan_traffic_weight'] == conf.reconfig_scenarios[scenario][3]) & (
                                             data['reconfig'] == conf.reconfig_strategies[reconfig])]

                    down_time[reconfig] = np.mean(base_data['best_migration_costs'])
                    num_migrations[reconfig] = np.mean(base_data['best_migrations_number_LT'])
                r1 = [p - dist for p in pos]
                r2 = [p + dist for p in pos]
                sarsa = [down_time[0], num_migrations[0]]
                mcts = [down_time[1], num_migrations[1]]
                bar = ax1[strategy].bar([r1[0], r2[0]], down_time.values(), width, color=colors)
                ax1[strategy].set_ylim([0, 0.15])
                ax2 = ax1[strategy].twinx()
                if strategy == 1:
                    _ax.get_shared_y_axes().join(_ax, ax2)

                ax2.bar([r1[1], r2[1]], num_migrations.values(), width, color=colors)
                ax2.set_ylim([0, 13])
                # plt.bar([r1[0], r2[0]], num_migrations, width, color=colors[1], label=conf.reconfig_strategies_name)
                if strategy == 0:
                    ax1[strategy].set_ylabel('Reconfiguration Overhead (s)')
                    ax2.axes.yaxis.set_ticklabels([])
                else:
                    ax2.set_ylabel('Migration Number')
                plt.xticks([r for r in pos], categories)
                if strategy == 0:
                    _ax = ax2
                else:
                    ax2.legend(bar, conf.reconfig_strategies_name, loc='lower center', fontsize=16)

            plt.tight_layout()
            # plt.show()
            fig_name = "lat-" + str(conf.reconfig_scenarios[scenario][0])
            plt.savefig(fig_name + '.pdf')


def validate_data(conf=configurations(), data=pd.DataFrame()):
    removed_data = pd.DataFrame()

    valid_data = pd.DataFrame()
    for strategy in range(len(conf.config_strategies)):
        for a in range(len(conf.applications)):
            for p in range(len(conf.reconfig_price_models)):
                for s in range(len(conf.reconfig_scenarios)):
                    base_data = data[(data['pricing_type'] == conf.reconfig_price_models[p]) & (
                            data['strategy'] == conf.config_strategies[strategy]) & (
                                             data['app'] == '/dot/' + conf.applications[a] + '.dot') & (
                                             data['latency_weight'] == conf.reconfig_scenarios[s][0]) & (
                                             data['migration_weight'] == conf.reconfig_scenarios[s][1]) & (
                                             data['costs_weight'] == conf.reconfig_scenarios[s][2]) & (
                                             data['wan_traffic_weight'] == conf.reconfig_scenarios[s][3])]

                    configuration_values = base_data['seed_configuration'].unique()

                    for c in range(len(configuration_values)):
                        conf_filter = base_data[(base_data['seed_configuration'] == configuration_values[c])]

                        print 'Filtered data: ' + str(conf_filter.__len__()) + ' Strategies: ' + str(
                            len(conf.reconfig_strategies) - 1)

                        if conf_filter.__len__() == (len(conf.reconfig_strategies)):
                            print 'Configuration - ' + str(configuration_values[c]) + ' - Valid data: App-' + \
                                  conf.applications[a] + ' Price: ' + str(
                                conf.reconfig_price_models[p]) + ' Scenario: ' + \
                                  str(conf.reconfig_scenarios[s])

                            valid_data = valid_data.append(conf_filter, ignore_index=False)
                        else:
                            print 'Configuration - ' + str(configuration_values[c]) + ' - Invalid data: App-' + \
                                  conf.applications[a] + ' Price: ' + str(
                                conf.reconfig_price_models[p]) + ' Scenario: ' + \
                                  str(conf.reconfig_scenarios[s])
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
            weight_combination = valid_data.loc[:,
                                 ['strategy', 'reconfig', 'app', 'seed_configuration', 'pricing_type', 'latency_weight',
                                  'migration_weight', 'costs_weight', 'wan_traffic_weight', 'constant', 'alpha',
                                  'gamma', 'lambda', 'epsilon']]

            weight_combination = weight_combination.drop_duplicates()

            # Kepp working in the main rules
            if valid_data.__len__() > 0:
                # basic_cdf(conf=conf, data=valid_data)
                # weighted_radar_firstpart(conf=conf, data=valid_data)
                # weighted_radar_thirdpart(conf=conf, data=valid_data)
                bar_plot_rl_2(conf=conf, data=valid_data)
                # basic_radar(conf=conf, data=valid_data)

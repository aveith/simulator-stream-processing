import argparse
import os
from matplotlib.ticker import MaxNLocator
from mpl_toolkits.axes_grid.inset_locator import (inset_axes, InsetPosition,
                                                  mark_inset)

import pandas as pd
from mpl_toolkits.mplot3d import Axes3D
from multiprocessing import Pool
import argparse

from numpy import _globals
from pytest import collect
from scipy.interpolate import griddata
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib import cm
import numpy as np
import pandas as pd
from scipy._lib._ccallback_c import plus1b_t

plt.rcParams.update({'font.size': 16})
# plt.rcParams["figure.figsize"] = (9, 7)

def plot_cdf_app1_app2(metric='', label_x='', label_y='', solutions=[], solutions_label=[], rp=[], markers=[],
                       linestyles=[]):
    for r in range(len(rp)):
        plt.cla()
        plt.clf()
        fig = plt.figure()
        axes = fig.gca()
        _global = [[] for j in range(len(solutions))]
        for s in range(len(solutions)):
            bfound = False
            for root, dirs, files in os.walk(args.comparison_directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()
                                filtered_data = tmp[
                                    (tmp['best_gain_rate'] >= max_value)]
                                filtered_data = filtered_data.iloc[0]

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                _global[s] = plot_data.loc[:, [metric]].values
                                bfound = True
                                break
                if bfound:
                    break

            n, bins, patches = axes.hist(_global[s], 10000, density=1, histtype='step',
                                         cumulative=True, label=solutions_label[s], linestyle=linestyles[s],
                                         linewidth=2.5)
        axes.legend()
        plt.xlabel(label_x)
        plt.ylabel(label_y)
        plt.show()


def plot_line(metric='', label_x='', label_y='', solutions=[], solutions_label=[], rp=[], markers=[], linestyles=[],
              app=1):
    for r in range(len(rp)):
        plt.cla()
        plt.clf()
        fig = plt.figure()
        axes = fig.gca()

        for s in range(len(solutions)):

            bfound = False
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()

                                print 'Max value = ' + str(max_value)

                                filtered_data = tmp[
                                    (tmp['best_gain_rate'] >= max_value)]
                                filtered_data = filtered_data.iloc[0]

                                print 'Latency = ' + str(tmp['best_latency_time'].min())

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                sub_set = plot_data.loc[:, ['iteration', metric]]

                                # print str(tmp['best_gain_rate'].max())

                                axis_x = sub_set['iteration'].values

                                if metric == 'best_gain_rate':
                                    axis_y = (sub_set[metric] * 100).values
                                else:
                                    axis_y = sub_set[metric].values

                                axes.plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                          markersize=1, alpha=.8, linestyle=linestyles[s])

                                axes.grid(linestyle='dotted')
                                axes.legend()
                                axes.set_xlabel(label_x)
                                axes.set_ylabel(label_y)

                                if metric == 'best_gain_rate':
                                    axes.set_ylim([0, 100])
                                elif metric == 'best_migrations_number_LT':
                                    axes.set_ylim([0, 20])
                                elif metric == 'best_latency_time':
                                    axes.set_ylim([0, 5])

                                if app == 1:
                                    if metric in ['best_gain_rate', 'best_latency_time']:
                                        ax2 = plt.axes([0, 0, 1, 1])
                                        ip = InsetPosition(axes, [.1, .55, 0.4, 0.4])
                                        ax2.set_axes_locator(ip)

                                        sub_set = plot_data.loc[:, ['iteration', metric]]

                                        if metric == 'best_gain_rate' and rp[r] == 0:
                                            mark_inset(axes, ax2, loc1=1, loc2=4, fc="none", ec='0.5')
                                            sub_set = sub_set[
                                                (sub_set['iteration'] > 8100) & (sub_set['iteration'] < 8200)]

                                        elif metric == 'best_gain_rate' and rp[r] == 1:
                                            mark_inset(axes, ax2, loc1=3, loc2=4, fc="none", ec='0.5')
                                            sub_set = sub_set[
                                                (sub_set['iteration'] > 1500) & (sub_set['iteration'] < 2000)]

                                        elif metric == 'best_latency_time' and rp[r] == 0:
                                            mark_inset(axes, ax2, loc1=1, loc2=4, fc="none", ec='0.5')
                                            sub_set = sub_set[
                                                (sub_set['iteration'] > 8100) & (sub_set['iteration'] < 8200)]

                                        elif metric == 'best_latency_time' and rp[r] == 1:
                                            mark_inset(axes, ax2, loc1=3, loc2=4, fc="none", ec='0.5')
                                            sub_set = sub_set[
                                                (sub_set['iteration'] > 1500) & (sub_set['iteration'] < 2000)]

                                        axis_x = sub_set['iteration'].values

                                        if metric == 'best_gain_rate':
                                            axis_y = (sub_set[metric] * 100).values
                                        else:
                                            axis_y = sub_set[metric].values

                                        ax2.plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                                 markersize=1, alpha=.8, linestyle=linestyles[s])

                                        if metric == 'best_gain_rate':
                                            ax2.set_ylim(35, 37)

                                        elif metric == 'best_latency_time':
                                            ax2.set_ylim(1.15, 1.25)

                                bfound = True
                                break
                if bfound:
                    break

        # plt.show()
        plt.tight_layout()
        plt.savefig('app' + str(app) + '_' + metric + '_rp_' + str(rp[r]) + '.png')


def plot_line_sidebyside(solutions=[], solutions_label=[], rp=[], markers=[], linestyles=[],
              app=1):
    for r in range(len(rp)):
        plt.cla()
        plt.clf()
        fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(14, 5))
        for s in range(len(solutions)):
            bfound = False
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                # print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()

                                # print 'Max value = ' + str(max_value)

                                filtered_data = tmp[
                                    (tmp['best_gain_rate'] >= max_value)]
                                filtered_data = filtered_data.iloc[0]

                                # print 'Latency = ' + str(tmp['best_latency_time'].min())

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate']]

                                axis_x = sub_set['iteration'].values
                                axis_y = (sub_set['best_gain_rate'] * 100).values


                                axes[0].plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                          markersize=1, alpha=.8, linestyle=linestyles[s])

                                axes[0].grid(linestyle='dotted')
                                axes[0].legend().set_visible(True)
                                axes[0].set_xlabel('Number of Simulations')
                                axes[0].set_ylabel('Latency Improvement(%)')
                                axes[0].set_ylim([0, 100])

                                if(app == 2 and r == 1):
                                    gg = sub_set[
                                        (sub_set['iteration'] == 9999)]
                                    print 'Latency index ' + solutions_label[s]
                                    print gg


                                # axes[0].set_title('Without DH')

                                if app == 1:
                                    ax2 = plt.axes([0, 0, 1, 1])
                                    ip = InsetPosition(axes[0], [.1, .55, 0.4, 0.4])
                                    ax2.set_axes_locator(ip)
                                    sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate']]

                                    if  rp[r] == 0:
                                        mark_inset(axes[0], ax2, loc1=1, loc2=4, fc="none", ec='0.5')
                                        sub_set = sub_set[
                                            (sub_set['iteration'] > 8100) & (sub_set['iteration'] < 8200)]

                                    elif rp[r] == 1:
                                        mark_inset(axes[0], ax2, loc1=3, loc2=4, fc="none", ec='0.5')
                                        sub_set = sub_set[
                                            (sub_set['iteration'] > 1500) & (sub_set['iteration'] < 2000)]


                                    axis_x = sub_set['iteration'].values
                                    axis_y = (sub_set['best_gain_rate'] * 100).values

                                    ax2.plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                             markersize=1, alpha=.8, linestyle=linestyles[s])
                                    ax2.set_ylim(35, 37)


                                sub_set = plot_data.loc[:, ['iteration', 'best_migrations_number_LT']]

                                # print str(tmp['best_gain_rate'].max())

                                axis_x = sub_set['iteration'].values
                                axis_y = sub_set['best_migrations_number_LT'].values

                                axes[1].plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                          markersize=1, alpha=.8, linestyle=linestyles[s])

                                axes[1].grid(linestyle='dotted')
                                axes[1].legend().set_visible(False)
                                axes[1].set_xlabel('Number of Simulations')
                                axes[1].set_ylabel('Number of Operator Migrations')

                                if (app == 2 and r == 1):
                                    gg = sub_set[
                                        (sub_set['iteration'] == 9999)]
                                    print 'Migration index ' + solutions_label[s]
                                    print gg
                                # axes[1].set_title('With DH')

                                bfound = True
                                break
                if bfound:
                    break

        # plt.show()
        plt.tight_layout()
        plt.savefig('app' + str(app) + '_general_rp_' + str(rp[r]) + '.png')


def plot_line_sidebyside_bar(solutions=[], solutions_label=[], rp=[], markers=[], linestyles=[],
              app=1):
    for r in range(len(rp)):
        plt.cla()
        plt.clf()
        fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(20, 7))
        _global = pd.DataFrame()
        for s in range(len(solutions)):
            bfound = False
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                # print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()

                                # print 'Max value = ' + str(max_value)

                                filtered_data = tmp[
                                    (tmp['best_gain_rate'] >= max_value)]
                                filtered_data = filtered_data.iloc[0]

                                print 'Latency ' + solutions_label[s] + ' = ' + str(tmp['best_latency_time'].min())

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate']]

                                axis_x = sub_set['iteration'].values
                                axis_y = (sub_set['best_gain_rate'] * 100).values


                                axes[0].plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                          markersize=1, alpha=.8, linestyle=linestyles[s])

                                axes[0].grid(linestyle='dotted')
                                axes[0].legend().set_visible(True)
                                axes[0].set_xlabel('Number of Simulations')
                                axes[0].set_ylabel('Latency Improvement(%)')
                                axes[0].set_ylim([0, 100])

                                if(app == 2 and r == 1):
                                    gg = sub_set[
                                        (sub_set['iteration'] == 9999)]
                                    # print 'Latency index ' + solutions_label[s]
                                    # print gg

                                if app == 1:
                                    ax2 = plt.axes([0, 0, 1, 1])
                                    ip = InsetPosition(axes[0], [.1, .55, 0.4, 0.4])
                                    ax2.set_axes_locator(ip)
                                    sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate']]

                                    if  rp[r] == 0:
                                        mark_inset(axes[0], ax2, loc1=1, loc2=4, fc="none", ec='0.5')
                                        sub_set = sub_set[
                                            (sub_set['iteration'] > 8100) & (sub_set['iteration'] < 8200)]

                                    elif rp[r] == 1:
                                        mark_inset(axes[0], ax2, loc1=3, loc2=4, fc="none", ec='0.5')
                                        sub_set = sub_set[
                                            (sub_set['iteration'] > 1500) & (sub_set['iteration'] < 2000)]


                                    axis_x = sub_set['iteration'].values
                                    axis_y = (sub_set['best_gain_rate'] * 100).values

                                    ax2.plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                             markersize=1, alpha=.8, linestyle=linestyles[s])
                                    ax2.set_ylim(35, 37)

                                if app == 2 :
                                    ax2 = plt.axes([0, 0, 1, 1])
                                    ip = InsetPosition(axes[0], [.57, .1, 0.3, 0.3])
                                    ax2.set_axes_locator(ip)
                                    sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate']]
                                    mark_inset(axes[0], ax2, loc1=3, loc2=4, fc="none", ec='0.5')
                                    sub_set = sub_set[
                                        (sub_set['iteration'] > 9000) & (sub_set['iteration'] < 10000)]
                                    axis_x = sub_set['iteration'].values
                                    axis_y = (sub_set['best_gain_rate'] * 100).values

                                    ax2.plot(axis_x, axis_y, label=solutions_label[s], marker=markers[s],
                                             markersize=1, alpha=.8, linestyle=linestyles[s])



                                sub_set = plot_data.loc[:, ['iteration', 'best_migrations_number_LT']]
                                gg = sub_set[
                                    (sub_set['iteration'] == 9999)]
                                gg = gg.loc[:, ['iteration', 'best_migrations_number_LT']]
                                gg.columns = ['iteration', solutions_label[s]]

                                gg = gg.set_index(['iteration'])

                                if _global.__len__() > 0:
                                    _global = pd.concat([_global, gg], axis=1)
                                else:
                                    _global = gg

                                bfound = True
                                break
                if bfound:
                    break
        _global = _global.reset_index()
        # print _global
        _global = _global.drop(['iteration'], axis=1).T
        _global.plot(kind='bar', ylim=(0, 20), ax=axes[1], alpha=.6)
        bars = axes[1].patches
        axes[1].legend().set_visible(False)

        for bar, color in zip(bars, ('#1f77b4', '#ff7f0e', '#2ca02c', '#d62728')):#
            bar.set_facecolor(color)

        for bar, pattern in zip(bars, ('-', '+', 'x', '\\', '*', 'o', 'O', '.')):
            bar.set_hatch(pattern)

        axes[1].set_xticklabels(solutions_label, rotation=0)
        axes[1].grid(linestyle='dotted')
        axes[1].yaxis.set_major_locator(MaxNLocator(integer=True))

        axes[1].legend().set_visible(False)
        axes[1].set_xlabel('')
        axes[1].set_ylabel('Number of Operator Migrations')
        # plt.show()
        plt.tight_layout()
        plt.savefig('app' + str(app) + '_general_rp_' + str(rp[r]) + '.png')

def fun(x, axis_z=[]):
    return axis_z[x]


def plot_3d(metric1='', metric2='', label_x='', label_y='', solutions=[], solutions_label=[], rp=[], markers=[],
            linestyles=[]):
    colors = ['Greys', 'Purples', 'Blues', 'Greens', 'Oranges', 'Reds']
    for r in range(len(rp)):

        for s in range(len(solutions)):

            bfound = False
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                plt.cla()
                                plt.clf()
                                fig = plt.figure()
                                axes = fig.gca(projection='3d')

                                print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()
                                filtered_data = tmp[
                                    (tmp['best_gain_rate'] >= max_value)]
                                filtered_data = filtered_data.iloc[0]

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate', metric1, metric2]]
                                axis_x = sub_set['best_gain_rate'].values
                                axis_y = sub_set[metric1].values
                                axis_z = sub_set[metric2].values
                                axes.plot_trisurf(axis_x, axis_y, axis_z, linewidth=0.2, antialiased=True)
                                plt.xlabel(label_x)
                                plt.ylabel(label_y)

                                axes.set_xlabel(label_x)
                                axes.set_ylabel(label_y)
                                axes.set_zlabel('Number of Migrations')
                                for i in range(0, 360, 45):
                                    axes.view_init(None, i)
                                    plt.show()
                                bfound = True
                                break
                if bfound:
                    break


def merge_files(solutions=[], solutions_label=[], rp=[]):
    for s in range(len(solutions)):
        for r in range(len(rp)):
            merged_files = pd.DataFrame()
            merged_files.empty
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.log'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file,
                                                  sep=';',
                                                  index_col=None)
                                if merged_files.__len__() < 1:
                                    merged_files = pd.read_csv(root + '/' + file,
                                                               sep=';',
                                                               index_col=None)
                                else:
                                    merged_files = merged_files.append(tmp, ignore_index=False)

            merged_files.to_csv('reconfig_' + str(solutions[s]) + '_RP-' + str(rp[r]) + '.csv', sep='\t')
            print 'SAVED TO -> reconfig_' + str(solutions[s]) + '_RP-' + str(rp[r]) + '.csv'


def iteration_behavior(solutions=[], solutions_label=[], rp=[]):
    for r in range(len(rp)):
        for s in range(len(solutions)):
            plt.cla()
            plt.clf()
            fig = plt.figure()
            axes = fig.gca()
            constant = -1
            alpha = -1
            gamma = -1
            _lambda = -1
            epsilon = -1
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()
                                filtered_data = tmp[
                                    (tmp.best_gain_rate >= max_value)]
                                filtered_data = filtered_data.iloc[0]
                                constant = filtered_data['constant'].max()
                                alpha = filtered_data['alpha'].max()
                                gamma = filtered_data['gamma'].max()
                                _lambda = filtered_data['lambda'].max()
                                epsilon = filtered_data['epsilon'].max()

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                sub_set = plot_data.loc[:, ['iteration', 'latency_time']]
                                axis_x = sub_set['iteration'].values
                                axis_y = sub_set['latency_time'].values
                                axes.plot(axis_x, axis_y, label=solutions_label[s], alpha=.5)
            plt.xlabel('Iteration')
            plt.ylabel('Total Latency')
            label = ""
            if solutions[s] in [0, 4, 5]:
                label = label + ' c:' + str(constant)

            if solutions[s] == 1:
                label = label + ' c:' + str(constant) + r'$ \gamma $:' + str(gamma) + r'$ \lambda $:' + str(_lambda)

            if solutions[s] == 2:
                label = label + r'$ \gamma $:' + str(gamma) + r'$ \alpha $:' + str(alpha)

            if solutions[s] == 3:
                label = label + ' c:' + str(constant) + r'$ \epsilon $:' + str(
                    epsilon)

            plt.title('Based on Cloud Deployment (Latency evolution) -\n Approach: ' + solutions_label[
                s] + '(' + label + ') RP active = ' + str(rp[r]))
            plt.show()


def bar_plot_latency():
    solutions = ['MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-LEARNING',
                 'MCTS-UCT', 'CLOUD-ONLY', 'TANEJA', 'RTR', 'RTR-RP']
    # a1 = [1.192613, 1.195742, 1.205434, 1.204456, 1.871786, 2.01241, 1.70671, 1.70519]
    # a2 = [1.200919, 1.197348, 1.206433, 1.205249, 2.041068, 2.10121, 1.19978, 1.19489]
    #
    # a1_rp = [1.192616, 1.1946709999999998, 1.202805, 1.202065, 1.871786, 2.01241, 1.70671, 1.70519]
    # a2_rp = [1.194779, 1.1944, 1.195417, 1.195229, 2.041068, 2.10121, 1.19978, 1.19489]
    plt.cla()
    plt.clf()
    fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(20, 7), sharey=True)

    df = pd.DataFrame(
        {'MCTS-Best-UCT': [1.192613, 0.682711], 'TDTS-SARSA(' + r'$ \lambda $)': [1.195742, 0.761952], 'Q-LEARNING': [1.205434, 0.745434], 'MCTS-UCT': [1.204456, 0.891867],
         'CLOUD-ONLY': [1.871786, 1.750612], 'TANEJA': [1.81241, 1.60121], 'RTR': [1.70671, 0.90978]})
    df.plot(kind='bar', ylim=(0, 3), ax=axes[0], alpha=.6)
    axes[0].legend().set_visible(False)
    for container, hatch in zip(axes[0].containers, ('-', '+', 'x', '\\', '*', 'o', 'O', '.')):
        for patch in container.patches:
            patch.set_hatch(hatch)

    axes[0].set_title('Without DH')
    axes[0].grid(linestyle='dotted')
    axes[0].set_xticklabels(('AppA', 'AppB'), rotation=0)
    axes[0].set_ylabel('Aggregate End-to-End Latency (ms)')
    axes[0].legend(loc='upper center', bbox_to_anchor=(0.5, 1), ncol=3)


    # plt.show()
    # plt.tight_layout()
    # plt.savefig('latency_rp_0.png')
    #
    # plt.cla()
    # plt.clf()
    # fig = plt.figure()
    # axes = fig.gca()
    rp_df = pd.DataFrame({'MCTS-Best-UCT': [1.192616, 0.586958], 'TDTS-SARSA(' + r'$ \lambda $)': [1.1946709999999998, 0.676658], 'Q-LEARNING': [1.202805, 0.579789],
                             'MCTS-UCT': [1.202065, 0.742886], 'RTR-RP': [1.70519, 0.677958]})
    print rp_df

    rp_df.plot(kind='bar', ylim=(0, 3), ax=axes[1], alpha=.6)
    axes[1].legend().set_visible(False)
    for container, hatch in zip(axes[1].containers, ('-', '+', 'x', '\\', '*', 'o', 'O', '.')):
        for patch in container.patches:
            patch.set_hatch(hatch)

    # ax2 = plt.axes([0, 0, 1, 1])
    # ip = InsetPosition(axes[1], [.57, .33, 0.4, 0.4])
    # ax2.set_axes_locator(ip)
    # ax2.grid(linestyle='dotted')

    # sub_set = plot_data.loc[:, ['iteration', metric]]
    # mark_inset(axes, ax2, loc1=1, loc2=4, fc="none", ec='0.5')
    #
    # rp_df2 = pd.DataFrame(
    #     {'MCTS-Best-UCT': [1.194779], 'TDTS-SARSA(' + r'$ \lambda $)': [1.1944],
    #      'Q-LEARNING': [1.195417],
    #      'MCTS-UCT': [1.195229], 'RTR-RP': [1.19489]})
    #
    # rp_df2.plot(kind='bar', ylim=(0, 1.196), ax=ax2, alpha=.6)
    # ax2.legend().set_visible(False)
    # for container, hatch in zip(ax2.containers, ('-', '+', 'x', '\\', '*', 'o', 'O', '.')):
    #     for patch in container.patches:
    #         patch.set_hatch(hatch)
    #
    #
    # ax2.set_xticklabels(['AppB'])
    axes[1].set_xticklabels(('AppA', 'AppB'), rotation=0)
    axes[1].grid(linestyle='dotted')
    axes[1].set_ylabel('Aggregate End-to-End Latency (ms)')
    axes[1].set_title('With DH')
    axes[1].legend(loc='upper center', bbox_to_anchor=(0.5, 1), ncol=3)
    plt.tight_layout()
    plt.savefig('latency_rp_1.png')


def plot_violin(metric='', label_x='', label_y='', solutions=[], solutions_label=[], rp=[], markers=[], linestyles=[],
              app=1):
    for r in range(len(rp)):
        plt.cla()
        plt.clf()
        fig = plt.figure()
        axes = fig.gca()

        # _global_values =pd.DataFrame()
        for s in range(len(solutions)):
            bfound = False
            for root, dirs, files in os.walk(args.directory):
                for file in files:
                    if file.endswith('.csv'):
                        if 'reconfig_' + str(solutions[s]) in file:
                            if 'RP-' + str(rp[r]) in file:
                                print 'READ FROM -> ' + root + '/' + file
                                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                                max_value = tmp['best_gain_rate'].max()

                                filtered_data = tmp[
                                    (tmp['best_gain_rate'] >= max_value)]
                                filtered_data = filtered_data.iloc[0]

                                print 'Latency = ' + str(tmp['best_latency_time'].min())

                                plot_data = tmp[
                                    (tmp['constant'] == filtered_data['constant']) & (
                                            tmp['alpha'] == filtered_data['alpha']) & (
                                            tmp['gamma'] == filtered_data['gamma']) & (
                                            tmp['lambda'] == filtered_data['lambda']) & (
                                            tmp['epsilon'] == filtered_data['epsilon'])]
                                sub_set = plot_data.loc[:, ['iteration', 'best_gain_rate']]
                                axis_x = sub_set['iteration'].values

                                if metric == 'best_gain_rate':
                                    axis_y = (sub_set[metric] * 100).values
                                else:
                                    axis_y = sub_set[metric].values
                                axes.scatter(x=axis_x, y=axis_y)

                                # _global_values = _global_values.append(sub_set,ignore_index=True)


                                bfound = True
                                break
                if bfound:
                    break

        # print _global_values
        # axes.violinplot(dataset=[_global_values[_global_values['reconfig'] == 0]["best_gain_rate"].values,
        #                          _global_values[_global_values['reconfig'] == 1]["best_gain_rate"].values,
        #                          _global_values[_global_values['reconfig'] == 2]["best_gain_rate"].values,
        #                          _global_values[_global_values['reconfig'] == 4]["best_gain_rate"].values])
        # # axes.set_title('')
        # axes.yaxis.grid(True)
        # axes.set_xlabel('Solutions')
        # axes.set_ylabel('Latency Improvemente(%)')

        plt.show()
        # break

    # print _global_values

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    parser.add_argument('-c', '--comparison_directory',
                        help='directory to read the files')

    args = parser.parse_args()

    if args.directory:
        markers = ['.', 'o', 'v', '^', '>', '<', 's', 'd', 'v', '+', '*']
        linestyles = ['-', '--', '-.', ':', '-', '--', '-.', ':']

        solutions = [0, 1, 2, 4]
        solutions_label = ['MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-LEARNING',
                           'MCTS-UCT']
        rp = [0, 1]

        # merge_files(solutions=solutions, solutions_label=solutions_label, rp=rp)

        # plot_3d(metric1='best_latency_time', metric2='best_migrations_number_LT', label_x='Iteration',
        #         label_y='Latency(ms)', solutions=solutions,
        #         solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles)
        #
        # plot_cdf_app1_app2(metric='best_gain_rate', label_x='Iteration', label_y='Gain Rate(%)', solutions=solutions,
        #           solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles)

        # plot_line(metric='best_gain_rate', label_x='Number of Simulations', label_y='Latency Improvement (ms)', solutions=solutions,
        #           solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles, app=1)
        #
        # plot_line(metric='best_migrations_number_LT', label_x='Number of Simulations',
        #           label_y='Number of Operator Migrations',
        #           solutions=solutions,
        #           solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles, app=1)

        # plot_line(metric='best_latency_time', label_x='Number of Simulations', label_y='Latency(ms)',
        #           solutions=solutions,
        #           solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles, app=2)
        # plot_line_sidebyside(solutions=solutions,
        #            solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles, app=1)

        # plot_line_sidebyside_bar(solutions=solutions,
        #            solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles, app=2)

        # plot_violin(metric='best_gain_rate', label_x='Number of Simulations', label_y='Latency(ms)',
        #           solutions=solutions,
        #           solutions_label=solutions_label, rp=rp, markers=markers, linestyles=linestyles, app=2)
        bar_plot_latency()

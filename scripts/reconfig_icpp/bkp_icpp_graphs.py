import argparse
import os

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
from math import pi
from scipy._lib._ccallback_c import plus1b_t

plt.rcParams.update({'font.size': 15})


# plt.rcParams["figure.figsize"] = (9, 7)

def radar_chart(df=pd.DataFrame(), df1=pd.DataFrame(), df2=pd.DataFrame(), df3=pd.DataFrame()):
    # ------- PART 1: Create background

    # number of variable
    categories = list(df.columns.values)

    N = len(categories)

    # What will be the angle of each axis in the plot? (we divide the plot / number of variable)
    angles = [n / float(N) * 2 * pi for n in range(N)]
    # angles += angles[:1]

    # Initialise the spider plot
    ax = plt.subplot(111, polar=True)

    # If you want the first axis to be on top:
    ax.set_theta_offset(pi / 2)
    ax.set_theta_direction(-1)

    # Draw one axe per variable + add labels labels yet
    plt.xticks(angles, categories)

    # Draw ylabels
    ax.set_rlabel_position(0)
    plt.yticks([-10, 0, 10, 20, 30], ["-10", "0", "10", "20", "30"], color="grey", size=7)
    plt.ylim(-10, 40)

    # ------- PART 2: Add plots

    # Plot each individual = each line of the data
    # I don't do a loop, because plotting more than 3 groups makes the chart unreadable

    # Ind1
    values = df.values.flatten().tolist()
    # values += values[:1]
    print values
    ax.plot(angles, values, linewidth=1, linestyle='solid', label="group A")
    ax.fill(angles, values, 'b', alpha=0.1)

    # Ind2
    values = df1.values.flatten().tolist()
    print values
    ax.plot(angles, values, linewidth=1, linestyle='solid', label="group B")
    ax.fill(angles, values, 'r', alpha=0.1)

    # Ind2
    values = df2.values.flatten().tolist()
    print values
    ax.plot(angles, values, linewidth=1, linestyle='solid', label="group C")
    ax.fill(angles, values, 'r', alpha=0.1)

    # Ind2
    values = df2.values.flatten().tolist()
    print values
    ax.plot(angles, values, linewidth=1, linestyle='solid', label="group D")
    ax.fill(angles, values, 'r', alpha=0.1)
    # Add legend
    plt.legend(loc='upper right', bbox_to_anchor=(0.1, 0.1))
    plt.show()


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    args = parser.parse_args()

    if args.directory:
        markers = ['.', 'o', 'v', '^', '>', '<', 's', 'd', 'v', '+', '*']
        linestyles = ['-', '--', '-.', ':', '-', '--', '-.', ':']

        solutions = [0, 1, 2, 4]
        solutions_label = ['MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-LEARNING',
                           'MCTS-UCT']
        rp = [0, 1]

        # Scenario:
        #          0 - .85: Latency .05: Migration .05: Costs .05: WAN
        #          1 - .25: Latency .25: Migration .25: Costs .25: WAN
        #          2 - 1.0: Latency   0: Migration   0: Costs   0: WAN
        #          3 -   0: Latency   1: Migration   0: Costs   0: WAN
        #          4 -   0: Latency   0: Migration   1: Costs   0: WAN
        #          5 -   0: Latency   0: Migration   0: Costs   1: WAN

        # Scenario 0
        tmp = pd.read_csv(
            args.directory + 'cloud_id_504_net_n3_app_app1_reconfig_1_A-1000000_C-0005000_E-1000000_G-1000000_L-1000000_LW-0850000_MW-0050000_CW-0050000_WW-0050000_PT-0_BRW-1_RP-0.log',
            sep=';', index_col=None)
        plot_data = tmp[
            (tmp['iteration'] == 5)]

        sc_0 = plot_data.loc[:,
               ['best_wan_traffic', 'best_migration_costs', 'best_microsoft_pricing', 'best_latency_time']]
        # sc_0['scenario'] = 0

        # Scenario 1
        tmp = pd.read_csv(
            args.directory + 'cloud_id_504_net_n3_app_app1_reconfig_1_A-1000000_C-0005000_E-1000000_G-1000000_L-1000000_LW-0250000_MW-0250000_CW-0250000_WW-0250000_PT-0_BRW-1_RP-0.log',
            sep=';', index_col=None)
        plot_data = tmp[
            (tmp['iteration'] == 5)]

        sc_1 = plot_data.loc[:,
               ['best_wan_traffic', 'best_migration_costs', 'best_microsoft_pricing', 'best_latency_time']]
        # sc_1['scenario'] = 1

        # Scenario 2
        tmp = pd.read_csv(
            args.directory + 'cloud_id_504_net_n3_app_app1_reconfig_1_A-1000000_C-0005000_E-1000000_G-1000000_L-1000000_LW-1000000_PT-0_BRW-1_RP-0.log',
            sep=';', index_col=None)
        plot_data = tmp[
            (tmp['iteration'] == 5)]

        sc_2 = plot_data.loc[:,
               ['best_wan_traffic', 'best_migration_costs', 'best_microsoft_pricing', 'best_latency_time']]
        # sc_2['scenario'] = 2

        # Scenario 3
        tmp = pd.read_csv(
            args.directory + 'cloud_id_504_net_n3_app_app1_reconfig_1_A-1000000_C-0005000_E-1000000_G-1000000_L-1000000_MW-1000000_PT-0_BRW-1_RP-0.log',
            sep=';', index_col=None)
        plot_data = tmp[
            (tmp['iteration'] == 5)]

        sc_3 = plot_data.loc[:,
               ['best_wan_traffic', 'best_migration_costs', 'best_microsoft_pricing', 'best_latency_time']]
        # sc_3['scenario'] = 3

        # Scenario 4
        tmp = pd.read_csv(
            args.directory + 'cloud_id_504_net_n3_app_app1_reconfig_1_A-1000000_C-0005000_E-1000000_G-1000000_L-1000000_CW-1000000_PT-0_BRW-1_RP-0.log',
            sep=';', index_col=None)
        plot_data = tmp[
            (tmp['iteration'] == 5)]

        sc_4 = plot_data.loc[:,
               ['best_wan_traffic', 'best_migration_costs', 'best_microsoft_pricing', 'best_latency_time']]
        # sc_4['scenario'] = 4

        # Scenario 5
        tmp = pd.read_csv(
            args.directory + 'cloud_id_504_net_n3_app_app1_reconfig_1_A-1000000_C-0005000_E-1000000_G-1000000_L-1000000_WW-1000000_PT-0_BRW-1_RP-0.log',
            sep=';', index_col=None)
        plot_data = tmp[
            (tmp['iteration'] == 5)]

        sc_5 = plot_data.loc[:,
               ['best_wan_traffic', 'best_migration_costs', 'best_microsoft_pricing', 'best_latency_time']]
        # sc_5['scenario'] = 5

        merged_files = sc_0

        merged_files = merged_files.append(sc_1, ignore_index=False)
        merged_files = merged_files.append(sc_2, ignore_index=False)
        merged_files = merged_files.append(sc_3, ignore_index=False)
        merged_files = merged_files.append(sc_4, ignore_index=False)
        merged_files = merged_files.append(sc_5, ignore_index=False)
        print merged_files
        # merged_files = merged_files.set_index(['scenario'])

        print 'Value : ' + str(sc_0['best_wan_traffic'].max())
        print 'Mean : ' + str(merged_files['best_wan_traffic'].mean())
        print 'Max : ' + str(merged_files['best_wan_traffic'].max())
        print 'Min : ' + str(merged_files['best_wan_traffic'].min())

        sc_0['best_wan_traffic'] = (sc_0['best_wan_traffic'] - merged_files['best_wan_traffic'].mean()) / (
                    merged_files['best_wan_traffic'].max() - merged_files['best_wan_traffic'].min())
        sc_0['best_migration_costs'] = (sc_0['best_migration_costs'] - merged_files['best_migration_costs'].mean()) / (
                merged_files['best_migration_costs'].max() - merged_files['best_migration_costs'].min())
        # sc_0['best_microsoft_pricing'] = (sc_0['best_microsoft_pricing'] - merged_files['best_microsoft_pricing'].mean()) / (
        #         merged_files['best_microsoft_pricing'].max() - merged_files['best_microsoft_pricing'].min())
        sc_0['best_latency_time'] = (sc_0['best_latency_time'] - merged_files['best_latency_time'].mean()) / (
                merged_files['best_latency_time'].max() - merged_files['best_latency_time'].min())
        print sc_0

        sc_1['best_wan_traffic'] = (sc_1['best_wan_traffic'] - merged_files['best_wan_traffic'].mean()) / (
                merged_files['best_wan_traffic'].max() - merged_files['best_wan_traffic'].min())
        sc_1['best_migration_costs'] = (sc_1['best_migration_costs'] - merged_files['best_migration_costs'].mean()) / (
                merged_files['best_migration_costs'].max() - merged_files['best_migration_costs'].min())
        # sc_1['best_microsoft_pricing'] = (sc_1['best_microsoft_pricing'] - merged_files['best_microsoft_pricing'].mean()) / (
        #         merged_files['best_microsoft_pricing'].max() - merged_files['best_microsoft_pricing'].min())
        sc_1['best_latency_time'] = (sc_1['best_latency_time'] - merged_files['best_latency_time'].mean()) / (
                merged_files['best_latency_time'].max() - merged_files['best_latency_time'].min())
        print sc_1

        sc_2['best_wan_traffic'] = (sc_2['best_wan_traffic'] - merged_files['best_wan_traffic'].mean()) / (
                merged_files['best_wan_traffic'].max() - merged_files['best_wan_traffic'].min())
        sc_2['best_migration_costs'] = (sc_2['best_migration_costs'] - merged_files['best_migration_costs'].mean()) / (
                merged_files['best_migration_costs'].max() - merged_files['best_migration_costs'].min())
        # sc_2['best_microsoft_pricing'] = (sc_2['best_microsoft_pricing'] - merged_files['best_microsoft_pricing'].mean()) / (
        #         merged_files['best_microsoft_pricing'].max() - merged_files['best_microsoft_pricing'].min())
        sc_2['best_latency_time'] = (sc_2['best_latency_time'] - merged_files['best_latency_time'].mean()) / (
                merged_files['best_latency_time'].max() - merged_files['best_latency_time'].min())
        print sc_2

        sc_3['best_wan_traffic'] = (sc_3['best_wan_traffic'] - merged_files['best_wan_traffic'].mean()) / (
                merged_files['best_wan_traffic'].max() - merged_files['best_wan_traffic'].min())
        sc_3['best_migration_costs'] = (sc_3['best_migration_costs'] - merged_files['best_migration_costs'].mean()) / (
                merged_files['best_migration_costs'].max() - merged_files['best_migration_costs'].min())
        # sc_3['best_microsoft_pricing'] = (sc_3['best_microsoft_pricing'] - merged_files['best_microsoft_pricing'].mean()) / (
        #         merged_files['best_microsoft_pricing'].max() - merged_files['best_microsoft_pricing'].min())
        sc_3['best_latency_time'] = (sc_3['best_latency_time'] - merged_files['best_latency_time'].mean()) / (
                merged_files['best_latency_time'].max() - merged_files['best_latency_time'].min())
        print sc_3
        radar_chart(sc_0, sc_1, sc_2, sc_3)

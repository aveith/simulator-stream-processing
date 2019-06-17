from mpl_toolkits.mplot3d import Axes3D
from multiprocessing import Pool
import argparse
from scipy.interpolate import griddata
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib import cm
import numpy as np
import pandas as pd
from scipy._lib._ccallback_c import plus1b_t

from launch_reconfig import experiment_configuration


class experiment_configuration:
    def __init__(self, reconfig_placement=0):
        self.reconfig_placement = reconfig_placement


def plot3d_by_strategy(reconfig_strategy=0, data_strategy=pd.DataFrame()):
    markers = ['.', 'o', 'v', '^', '>', '<', 's', 'd', 'v', '+', '*']
    linestyles = ['-', '--', '-.', ':', '-', '--', '-.', ':']
    filtered_data = data_strategy[(data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
            data_strategy.reconfig == reconfig_strategy) & (data_strategy.selectivity == 1)
                                  & (data_strategy.datarate == 1) & (data_strategy.placement == '03_510') & (
                                          data_strategy.arrival == 1249999)]
    # & (data_strategy.alpha == .005)& (data_strategy.alpha == .005) & (data_strategy.bytes == 10)]
    # ]& (data_strategy.placement == '03_514') & & (data_strategy.constant == 2)

    # filtered_data = filtered_data.set_index(
    #     ['strategy', 'net', 'app', 'reconfig', 'ite', 'constant', 'alpha', 'gamma', 'lambda', 'epilson'])

    unique_combination = filtered_data[
        ['placement', 'datarate', 'selectivity', 'bytes', 'arrival', 'constant', 'alpha', 'gamma', '_lambda',
         'epilson']].drop_duplicates()

    unique_combination = unique_combination.sort_values(
        by=['placement', 'datarate', 'selectivity', 'bytes', 'arrival', 'constant', 'alpha', 'gamma', '_lambda',
            'epilson'])

    print unique_combination

    if unique_combination.__len__() > 0:
        _global = pd.DataFrame()

        plt.cla()
        plt.clf()
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        counter = 0
        for index, row in unique_combination.iterrows():
            sub_set = filtered_data[(filtered_data.constant == row.constant) & (
                    filtered_data.alpha == row.alpha) & (
                                            filtered_data.gamma == row.gamma) & (
                                            filtered_data._lambda == row._lambda) & (
                                            filtered_data.epilson == row.epilson) & (
                                            filtered_data.datarate == row.datarate) & (
                                            filtered_data.selectivity == row.selectivity) & (
                                            filtered_data.bytes == row.bytes) & (
                                            filtered_data.arrival == row.arrival) & (
                                            filtered_data.placement == row.placement)]

            label = ''
            if (row.constant > 0):
                label = label + 'c:' + str(row.constant)

            if (row.alpha > 0):
                label = label + ' ' + r'$ \alpha $:' + str(row.alpha)

            if (row.gamma > 0):
                label = label + ' ' + r'$ \gamma $:' + str(row.gamma)

            if (row._lambda > 0):
                label = label + ' ' + r'$ \lambda $:' + str(row._lambda)

            if (row.epilson > 0):
                label = label + ' ' + r'$ \epsilon $:' + str(row.epilson)

            title = ''
            if (row.datarate > 0):
                title = title + ' ' + 'datarate:' + str(row.datarate)

            if (row.selectivity > 0):
                title = title + ' ' + 'selectivity:' + str(row.selectivity)

            if (row.bytes > 0):
                title = title + ' ' + 'bytes:' + str(row.bytes)

            if (row.arrival > 0):
                title = title + ' ' + 'arrival:' + str(row.arrival)

            if (not row.placement == ''):
                title = title + ' ' + 'plac:' + str(row.placement)

            sub_set = sub_set.loc[:, ['ite', 'time', 'gain']]
            sub_set.to_csv(label + '.csv', sep='\t')

            axis_x = sub_set.ite.values
            axis_y = sub_set.gain.values
            axis_z = sub_set.time.values

            # fig = plt.figure()
            # ax = fig.gca(projection='3d')
            #
            # ax.scatter(axis_x, axis_y, axis_z)
            #
            # ax.set_xlabel('Iteration')
            # ax.set_ylabel('Gain Rate')
            # ax.set_zlabel('Elapsed Time')
            #
            # for i in range(0, 360, 45):
            #     ax.view_init(None, i)
            #     plt.show()
            #
            # plt.cla()
            # plt.clf()
            # fig = plt.figure()
            # ax = fig.gca(projection='3d')
            #
            ax.plot_trisurf(axis_x, axis_y, axis_z, cmap='viridis', edgecolor='none', label=label)
            # ax.set_xlabel('Iteration')
            # ax.set_ylabel('Gain Rate')
            # ax.set_zlabel('Elapsed Time')
            # ax.view_init(-35, 80)
            # for i in range(0, 360, 180):
            #     ax.view_init(None, i)
            #     plt.show()

            # ax.plot(axis_x, axis_y, axis_z,marker=markers[counter], markersize=1, alpha=.8,
            #         linestyle=linestyles[counter], label=label)

            #
            # plt.cla()
            # plt.clf()
            # fig = plt.figure()
            # ax = fig.gca(projection='3d')
            #
            # xi = np.linspace(axis_x.min(), axis_x.max(), 10000)
            # yi = np.linspace(axis_y.min(), axis_y.max(), 10000)
            # # VERY IMPORTANT, to tell matplotlib how is your data organized
            # zi = griddata((axis_x, axis_y), axis_z, (xi[None, :], yi[:, None]), method='cubic')
            #
            # CS = plt.contour(xi, yi, zi, 15, linewidths=0.5, color='k')
            #
            # xig, yig = np.meshgrid(xi, yi)
            #
            # surf = ax.plot_surface(xig, yig, zi, cmap='afmhot',
            #                        linewidth=0)
            # ax.set_xlabel('Iteration')
            # ax.set_ylabel('Gain Rate')
            # ax.set_zlabel('Elapsed Time')
            # for i in range(0, 360, 45):
            #     ax.view_init(None, i)
            #     plt.show()
            #
            # plt.cla()
            # plt.clf()
            # fig = plt.figure()
            # ax = fig.gca(projection='3d')
            #
            #
            # ax.plot_wireframe(xi, yi, zi, rstride=10, cstride=10)
            # ax.set_xlabel('Iteration')
            # ax.set_ylabel('Gain Rate')
            # ax.set_zlabel('Elapsed Time')
            # plt.show()
            # break
            counter += 1
        ax.set_xlabel('Iteration')
        ax.set_ylabel('Gain Rate')
        ax.set_zlabel('Elapsed Time')
        plt.title(title)

        # ax.legend(loc=3)
        ax.view_init(-100, 0)
        # for i in range(0, 360, 45):
        #     ax.view_init(None, i)
        #     plt.show()
        plt.show()


def save_csv_by_strategy(reconfig_strategy=0):
    data_strategy = readDataSet(reconfig_strategy)
    placement = ['03_510', '03_514', '03_524']
    for p in range(len(placement)):
        filtered_data = data_strategy[(data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
                data_strategy.reconfig == reconfig_strategy) & (data_strategy.placement == placement[p])]

        unique_combination = filtered_data[
            ['placement', 'datarate', 'selectivity', 'bytes', 'arrival', 'constant', 'alpha', 'gamma', '_lambda',
             'epilson']].drop_duplicates()

        unique_combination = unique_combination.sort_values(
            by=['placement', 'datarate', 'selectivity', 'bytes', 'arrival', 'constant', 'alpha', 'gamma', '_lambda',
                'epilson'])

        print unique_combination

        if unique_combination.__len__() > 0:
            _global = pd.DataFrame()
            _global_errors = pd.DataFrame()
            for index, row in unique_combination.iterrows():
                sub_set = filtered_data[(filtered_data.constant == row.constant) & (
                        filtered_data.alpha == row.alpha) & (
                                                filtered_data.gamma == row.gamma) & (
                                                filtered_data._lambda == row._lambda) & (
                                                filtered_data.epilson == row.epilson) & (
                                                filtered_data.datarate == row.datarate) & (
                                                filtered_data.selectivity == row.selectivity) & (
                                                filtered_data.bytes == row.bytes) & (
                                                filtered_data.arrival == row.arrival) & (
                                                filtered_data.placement == row.placement)]

                sub_set = sub_set.loc[:, ['ite', 'gain']]
                sub_set_error = sub_set.groupby(['ite']).agg(np.std)
                sub_set = sub_set.groupby(['ite']).mean()

                label = ''
                if (row.constant > 0):
                    label = label + 'c:' + str(row.constant)

                if (row.alpha > 0):
                    label = label + ' ' + r'$ \alpha $:' + str(row.alpha)

                if (row.gamma > 0):
                    label = label + ' ' + r'$ \gamma $:' + str(row.gamma)

                if (row._lambda > 0):
                    label = label + ' ' + r'$ \lambda $:' + str(row._lambda)

                if (row.epilson > 0):
                    label = label + ' ' + r'$ \epsilon $:' + str(row.epilson)

                if (row.datarate > 0):
                    label = label + ' ' + 'datarate:' + str(row.datarate)

                if (row.selectivity > 0):
                    label = label + ' ' + 'selectivity:' + str(row.selectivity)

                if (row.bytes > 0):
                    label = label + ' ' + 'bytes:' + str(row.bytes)

                if (row.arrival > 0):
                    label = label + ' ' + 'arrival:' + str(row.arrival)

                if (not row.placement == ''):
                    label = label + ' ' + 'plac:' + str(row.placement)

                sub_set.columns = [label]
                sub_set_error.columns = [label]

                if _global.__len__() > 0:
                    _global = pd.merge(_global, sub_set, left_index=True, right_index=True)
                else:
                    _global = sub_set

                if _global_errors.__len__() > 0:
                    _global_errors = pd.merge(_global_errors, sub_set_error, left_index=True, right_index=True)
                else:
                    _global_errors = sub_set_error

            _global.to_csv('reconfig_' + str(reconfig_strategy) + '_plac_' + str(placement[p]) + '.csv', sep='\t')


def plot_by_strategy(reconfig_strategy=0, data_strategy=pd.DataFrame()):
    markers = ['.', 'o', 'v', '^', '>', '<', 's', 'd', 'v', '+', '*']
    placement = ['03_510', '03_514', '03_510']
    filtered_data = data_strategy[(data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
            data_strategy.reconfig == reconfig_strategy) & (data_strategy.selectivity == 1)
                                  & (data_strategy.datarate == 1) & (data_strategy.placement == '03_524')]
    # & (data_strategy.alpha == .005)& (data_strategy.alpha == .005) & (data_strategy.bytes == 10)]
    # ]& (data_strategy.placement == '03_514') & & (data_strategy.constant == 2)
    #
    print filtered_data.__len__()
    print filtered_data

    # filtered_data = filtered_data.set_index(
    #     ['strategy', 'net', 'app', 'reconfig', 'ite', 'constant', 'alpha', 'gamma', 'lambda', 'epilson'])

    unique_combination = filtered_data[
        ['placement', 'datarate', 'selectivity', 'bytes', 'arrival', 'constant', 'alpha', 'gamma', '_lambda',
         'epilson']].drop_duplicates()

    unique_combination = unique_combination.sort_values(
        by=['placement', 'datarate', 'selectivity', 'bytes', 'arrival', 'constant', 'alpha', 'gamma', '_lambda',
            'epilson'])

    print unique_combination

    if unique_combination.__len__() > 0:
        _global = pd.DataFrame()
        _global_errors = pd.DataFrame()
        counter = 0
        for index, row in unique_combination.iterrows():
            sub_set = filtered_data[(filtered_data.constant == row.constant) & (
                    filtered_data.alpha == row.alpha) & (
                                            filtered_data.gamma == row.gamma) & (
                                            filtered_data._lambda == row._lambda) & (
                                            filtered_data.epilson == row.epilson) & (
                                            filtered_data.datarate == row.datarate) & (
                                            filtered_data.selectivity == row.selectivity) & (
                                            filtered_data.bytes == row.bytes) & (
                                            filtered_data.arrival == row.arrival) & (
                                            filtered_data.placement == row.placement)]

            sub_set = sub_set.loc[:, ['ite', 'gain']]
            sub_set_error = sub_set.groupby(['ite']).agg(np.std)
            sub_set = sub_set.groupby(['ite']).mean()

            label = ''
            if (row.constant > 0):
                label = label + 'c:' + str(row.constant)

            if (row.alpha > 0):
                label = label + ' ' + r'$ \alpha $:' + str(row.alpha)

            if (row.gamma > 0):
                label = label + ' ' + r'$ \gamma $:' + str(row.gamma)

            if (row._lambda > 0):
                label = label + ' ' + r'$ \lambda $:' + str(row._lambda)

            if (row.epilson > 0):
                label = label + ' ' + r'$ \epsilon $:' + str(row.epilson)

            if (row.datarate > 0):
                label = label + ' ' + 'datarate:' + str(row.datarate)

            if (row.selectivity > 0):
                label = label + ' ' + 'selectivity:' + str(row.selectivity)

            if (row.bytes > 0):
                label = label + ' ' + 'bytes:' + str(row.bytes)

            if (row.arrival > 0):
                label = label + ' ' + 'arrival:' + str(row.arrival)

            if (not row.placement == ''):
                label = label + ' ' + 'plac:' + str(row.placement)

            sub_set.columns = [label]
            sub_set_error.columns = [label]

            if _global.__len__() > 0:
                _global = pd.merge(_global, sub_set, left_index=True, right_index=True)
            else:
                _global = sub_set

            if _global_errors.__len__() > 0:
                _global_errors = pd.merge(_global_errors, sub_set_error, left_index=True, right_index=True)
            else:
                _global_errors = sub_set_error

        _global.to_csv('base.csv', sep='\t')

        if (_global.__len__() > 0):

            plt.cla()
            plt.clf()

            axes = plt.gca()

            values_plot = []
            erros_plot = []
            for index, row in unique_combination.iterrows():
                label = ''
                if (row.constant > 0):
                    label = label + 'c:' + str(row.constant)

                if (row.alpha > 0):
                    label = label + ' ' + r'$ \alpha $:' + str(row.alpha)

                if (row.gamma > 0):
                    label = label + ' ' + r'$ \gamma $:' + str(row.gamma)

                if (row._lambda > 0):
                    label = label + ' ' + r'$ \lambda $:' + str(row._lambda)

                if (row.epilson > 0):
                    label = label + ' ' + r'$ \epsilon $:' + str(row.epilson)

                if (row.datarate > 0):
                    label = label + ' ' + 'datarate:' + str(row.datarate)

                if (row.selectivity > 0):
                    label = label + ' ' + 'selectivity:' + str(row.selectivity)

                if (row.bytes > 0):
                    label = label + ' ' + 'bytes:' + str(row.bytes)

                if (row.arrival > 0):
                    label = label + ' ' + 'arrival:' + str(row.arrival)

                if (not row.placement == ''):
                    label = label + ' ' + 'plac:' + str(row.placement)

                _aux = _global.loc[:, [label]]
                values_plot.append(_aux.values)

                _aux_error = _global_errors.loc[:, [label]]
                erros_plot.append(_aux_error.values)

                axes.errorbar(_global.index, values_plot[values_plot.__len__() - 1],
                              yerr=erros_plot[erros_plot.__len__() - 1], ls='', marker=markers[counter], markersize=3,
                              capsize=2, alpha=.5, capthick=1, label=label)
                counter = counter + 1

                # break

            axes.set_xlim([-10, 10000])
            axes.set_ylim([0, 0.007])
            axes.legend(loc='best')
            plt.ylabel('Gain Rate (%)')
            plt.xlabel('Iteration')
            plt.title('Pipeline with base scheduling Taneja, \nplacement Cloudlet1(Source)-Cloudlet1(Sink)')

            plt.show()


def fun(x, y=[]):
    return y[x]


def plotbest(reconfigure_placement=experiment_configuration()):
    data_strategy = readDataSet()

    markers = ['.', 'o', 'v', '^', '>', '<', 's', 'd', 'v', '+', '*', 'd', 'v', '+', '*', 'd', 'v', '+', '*']
    linestyles = ['-', '--', '-.', ':', '-', '--', '-.', ':']
    colors = ['Greys', 'Purples', 'Blues', 'Greens', 'Oranges', 'Reds']
    strategies = [0, 4, 5]
    placements = ['03_510', '03_514', '03_524']
    title_plac = ['Source(Cloudlet1)-Sink(Cloud)', 'Source(Cloudlet1)-Sink(Cloudlet1)',
                  'Source(Cloudlet1)-Sink(Cloudlet2)']
    strategy_desc = ['MCTS-TunedUCB', 'MCTS-UCB', 'MCTS-FirstUCB']
    arrivals = [62, 1249999]
    constants = [[[.05, .005], [.05, .005], [.05, .05]],  # strategy 0
                 [[.005, .005], [.005, .05], [.005, .005]],  # strategy 4
                 [[.005, .005], [.005, .05], [.005, .005]]]  # strategy 5

    for placement in range(len(placements)):
        if reconfigure_placement.reconfig_placement > -1:
            if reconfigure_placement.reconfig_placement != placement:
                continue
        plt.cla()
        plt.clf()
        fig = plt.figure()
        axes = fig.gca(projection='3d')

        counter = 0
        for strategy in range(len(strategies)):
            for arrival in range(len(arrivals)):
                filtered_data = data_strategy[
                    (data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
                            data_strategy.reconfig == strategies[strategy]) & (
                            data_strategy.placement == str(placements[placement])) & (
                            data_strategy.constant == constants[strategy][placement][arrival]) & (
                            data_strategy.arrival == arrivals[arrival])]
                # base_data = filtered_data.loc[:, ['ite', 'gain', 'time']]
                # df = base_data.groupby('gain').agg({'ite':'min', 'time':'min'})[['ite','time']].reset_index()
                # print df

                sub_set = filtered_data.loc[:, ['ite', 'gain']]
                sub_set_z = filtered_data.loc[:, ['ite', 'time']]
                sub_set = sub_set.groupby(['ite']).mean()
                sub_set_z = sub_set_z.groupby(['ite']).mean()

                # print sub_set

                label = str(strategy_desc[strategy])
                if (constants[strategy][placement][arrival] > 0):
                    label = label + ' c:' + str(constants[strategy][placement][arrival])

                if (arrivals[arrival] > 0):
                    label = label + ' ' + ' arrival:' + str(arrivals[arrival])

                axis_x = sub_set.index
                axis_y = sub_set.gain.values
                axis_z = sub_set_z.time.values
                #
                # axes.plot(axis_x, axis_y, axis_z,marker=markers[counter], markersize=1, alpha=.8,
                #         linestyle=linestyles[counter], label=label)

                # axes.plot_trisurf(axis_x, axis_y, axis_z, cmap=colors[counter], linewidth=0.5,
                #                   antialiased=True, alpha=0.6, label=label)
                axes.plot(axis_x, axis_y, axis_z, marker=markers[counter], markersize=1, alpha=.8,
                          linestyle=linestyles[counter], label=label)

                # X, Y = np.meshgrid(axis_x, axis_y)
                # zs = np.array([fun(x, axis_z) for x, y in zip(np.ravel(X), np.ravel(Y))])
                # Z = zs.reshape(X.shape)
                # axes.plot_surface(X, Y, Z, cmap=colors[counter], rstride=1, cstride=1, linewidth=0.5, antialiased=True,
                #                   alpha=0.8, label=label)

                counter = counter + 1
            #     if counter == 1:
            #         break
            # break

        axes.set_xlim([0, 10000])
        axes.set_ylim([0, 1])
        axes.legend(loc=0)
        # fake2Dline = mpl.lines.Line2D([3], [0], linestyle="none", c='grey', marker='o')
        # axes.legend([fake2Dline], ['MCTS-TunedUCB c:0.05 arrival:62'], numpoints=1)
        #
        # fake2Dline1 = mpl.lines.Line2D([1], [0], linestyle="none", c='purple', marker='o')
        # axes.legend([fake2Dline1], ['MCTS-TunedUCB c:0.005 arrival:1249999'], numpoints=1)
        #
        # fake2Dline2 = mpl.lines.Line2D([0], [0], linestyle="none", c='blue', marker='o')
        # axes.legend([fake2Dline2], ['MCTS-UCB c:0.005 arrival:62'], numpoints=1)
        #
        # fake2Dline3 = mpl.lines.Line2D([0], [0], linestyle="none", c='g', marker='o')
        # axes.legend([fake2Dline3], ['MCTS-UCB c:0.005 arrival:1249999'], numpoints=1)

        plt.ylabel('Gain Rate (%)')
        plt.xlabel('Iteration')
        plt.ylabel('Elapsed Time (ms)')
        plt.title('Taneja Deployment -\n' + title_plac[placement])
        # axes.view_init(None, 75)
        for i in range(0, 360, 45):
            axes.view_init(None, i)
        plt.show()
        # plt.tight_layout()
        # plt.savefig('3d_' + title_plac[placement] + '.pdf')

        # break


def plot_cdf_best():
    data_strategy = readDataSet()
    linestyles = ['-', '--', ':', '--', '-.']
    strategies = [0, 2, 3, 4, 5]
    placements = ['03_510', '03_514', '03_524']
    title_plac = ['Source(Cloudlet1)-Sink(Cloud)', 'Source(Cloudlet1)-Sink(Cloudlet1)',
                  'Source(Cloudlet1)-Sink(Cloudlet2)']
    strategy_desc = ['MCTS-TunedUCB', 'Q-Learning', 'MCTS-GreedUCB', 'MCTS-UCB', 'MCTS-FirstUCB']
    arrivals = [62, 1249999]
    constants = [[[.05, .005], [.05, .005], [.05, .05]],  # strategy 0
                 [[]],
                 [[]],
                 [[.005, .005], [.005, .05], [.005, .005]],  # strategy 4
                 [[.005, .1], [.005, .05], [.05, .05]]]  # strategy 5

    strategy_3 = [[[]],
                    [[]],
                    [[[.9, .9], [.005, .9]], [[.05, .9], [.1, .9]], [[.9, 1], [1, .1]]]]


    for placement in range(len(placements)):

        for arrival in range(len(arrivals)):
            plt.cla()
            plt.clf()
            fig = plt.figure()
            axes = fig.gca()
            _global = [[] for j in range(len(strategies) * len(arrivals))]
            counter = 0
            for strategy in range(len(strategies)):

                filtered_data=pd.DataFrame()

                label = str(strategy_desc[strategy])
                if strategies[strategy] in [0, 4, 5]:
                    label = label + ' c:' + str(constants[strategy][placement][arrival])
                    label = label + ' ' + ' arrival:' + str(arrivals[arrival])

                    filtered_data = filtered_data = data_strategy[
                        (data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
                                data_strategy.reconfig == strategies[strategy]) & (
                                data_strategy.placement == str(placements[placement])) & (
                                data_strategy.constant == constants[strategy][placement][arrival]) & (
                                data_strategy.arrival == arrivals[arrival])]

                if strategies[strategy] == 2:

                    label = label + ' ' + r'$ \gamma $:' + str(.005)
                    label = label + ' ' + r'$ \lambda $:' + str(.005)
                    label = label + ' ' + ' arrival:' + str(arrivals[arrival])

                    filtered_data = filtered_data = data_strategy[
                        (data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
                                data_strategy.reconfig == strategies[strategy]) & (
                                data_strategy.placement == str(placements[placement])) & (
                                data_strategy.alpha == .005) & (
                                data_strategy.gamma == .005) & (
                                data_strategy.arrival == arrivals[arrival])]

                if strategies[strategy] == 3:
                    label = label + ' c:' + str(strategy_3[strategy][placement][arrival][0])

                    label = label + ' ' + ' arrival:' + str(arrivals[arrival])
                    label = label + ' ' + r'$ \epsilon $:' + str(strategy_3[strategy][placement][arrival][1])
                    filtered_data = filtered_data = data_strategy[
                        (data_strategy.strategy == 4) & (data_strategy.app == '/dot/app0.dot') & (
                                data_strategy.reconfig == strategies[strategy]) & (
                                data_strategy.placement == str(placements[placement])) & (
                                data_strategy.constant == strategy_3[strategy][placement][arrival][0]) & (
                                data_strategy.epilson == strategy_3[strategy][placement][arrival][1]) & (
                                data_strategy.arrival == arrivals[arrival])]

                sub_set = filtered_data.loc[:, ['ite', 'gain', 'time']]
                sub_set = sub_set.groupby(['ite']).mean()
                # print sub_set

                sub_set['value'] = sub_set['gain'] / sub_set['time']
                # sub_set.to_csv(
                #     'cdf_data_' + title_plac[placement] + '_' + str(strategies[strategy]) + '-' + str(
                #         arrivals[arrival]) + '.csv', sep='\t')
                _aux = sub_set.loc[:, ['value']]
                _global[counter] = _aux.values

                n, bins, patches = axes.hist(_global[counter], 10000, density=1, histtype='step',
                                             cumulative=True, label=label, linestyle=linestyles[arrival],
                                             linewidth=2.5)
                counter += 1

            axes.grid(linestyle='dotted')
            axes.set_title('Taneja Deployment -\n' + title_plac[arrival] + ' Arrival Rate: ' + str(arrivals[arrival]))
            axes.set_xscale('log')
            axes.legend(loc=4)
            plt.ylabel('CDF')
            plt.xlabel('Gain Rate/ Elapsed Time')
            plt.tight_layout()
            # plt.show()
            plt.savefig('cdf_' + title_plac[placement] + ' Arrival Rate_' + str(arrivals[arrival]) + '.pdf')


def readDataSet(strategy=-1):
    if (strategy == -1):
        data = pd.read_csv(args.directory + '/reconfig_0/net_n3_app_app0_reconfig_0/reconfig_0.csv', sep='\t',
                           index_col=None)

        tmp = pd.read_csv(args.directory + '/reconfig_2/net_n3_app_app0_reconfig_2/reconfig_2.csv', sep='\t',
                          index_col=None)
        data = data.append(tmp, ignore_index=False)

        tmp = pd.read_csv(args.directory + '/reconfig_3/net_n3_app_app0_reconfig_3/reconfig_3.csv', sep='\t',
                          index_col=None)
        data = data.append(tmp, ignore_index=False)
        #
        tmp = pd.read_csv(args.directory + '/reconfig_4/net_n3_app_app0_reconfig_4/reconfig_4.csv', sep='\t',
                          index_col=None)
        data = data.append(tmp, ignore_index=False)

        tmp = pd.read_csv(args.directory + '/reconfig_5/net_n3_app_app0_reconfig_5/reconfig_5.csv', sep='\t',
                          index_col=None)
        data = data.append(tmp, ignore_index=False)
    else:
        data = pd.read_csv(
            args.directory + '/reconfig_' + str(strategy) + '/net_n3_app_app0_reconfig_' + str(
                strategy) + '/reconfig_' + str(strategy) + '.csv',
            sep='\t',
            index_col=None)

    data.columns = ['index', 'strategy', 'net', 'app', 'bytes', 'arrival', 'datarate', 'selectivity', 'iterations',
                    'constant', 'alpha', 'gamma', '_lambda', 'epilson', 'reconfig', 'ite', 'time',
                    'reward', 'root', 'best', 'gain', 'placement']

    # print data
    print data.__len__()
    return data


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    parser.add_argument('-p', '--placement',
                        help='placement parallel executions')

    args = parser.parse_args()

    if args.directory:
        # save_csv_by_strategy(reconfig_strategy=1)


        if args.placement:
            scenarios = []
            for i in range(0, 3):
                exp = experiment_configuration()
                exp.reconfig_placement = i
                scenarios.append(exp)

            # pool = Pool(processes=int(args.process))
            # pool.map(plotbest, scenarios)
            plotbest(scenarios[0])
        else:
            plot_cdf_best()

    # save_csv_by_strategy(0, data)

    # plot3d_by_strategy(4, data)

    else:
        print 'The directory was not configured'

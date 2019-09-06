import argparse
import os

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

plt.rcParams.update({'font.size': 10})


class stategy_results:
    def __init__(self, data=pd.DataFrame(), index=-1):
        self.data = data
        self.strategy_index = index


class configurations:
    def __init__(self):
        self.applications = ['a1', 'a5']
        self.reconfig_strategies = [0, 1, 2, 4]
        self.config_strategies = [1]
        self.action_heuristic = [0]
        self.reconfig_applying_rp = [0, 1]

        self.config_st_names = ['Cloud-Only', 'Taneja']
        self.reconfig_strategies_name = ['MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-Learning', 'MCTS-UCT']


def load_files(dir=''):
    merged_files = pd.DataFrame()
    merged_files.empty

    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith('.csv'):
                tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=None)
                # tmp['rp'] = 0 if 'rp_false' in file else 1
                merged_files = merged_files.append(tmp, ignore_index=False)

    return merged_files


def get_scenario_description(scenario=[], conf=configurations()):
    description = ''
    for s in range(len(scenario)):
        if scenario[s] > 0:
            description = description + conf.reconfig_scenarios_base[s] + ': ' + str(scenario[s]) + ' '
    return description.rstrip()


def bar_algo_time_latency_imprv(data=pd.DataFrame(), conf=configurations()):
    for r in range(len(conf.reconfig_applying_rp)):
        for a in range(len(conf.applications)):
            plt.cla()
            plt.clf()
            means_impr = []
            std_impr = []
            rstrategy_impr = []

            means_time = []
            std_time = []
            rstrategy_time = []

            for s in range(len(conf.config_strategies)):
                for rs in range(len(conf.reconfig_strategies)):
                    tmp_app = data[(data['app'] == '/dot/' + conf.applications[a] + '.dot') & (
                            data['strategy'] == conf.config_strategies[s]) & (
                                           data['reconfig'] == conf.reconfig_strategies[rs]) & (
                                           data['rp'] == conf.reconfig_applying_rp[r])]

                    # tmp_app['improvement'] = 1 - (tmp_app['best_latency_time'] / tmp_app['root_latency_time'])
                    tmp_app['best_gain_rate'] = tmp_app['best_gain_rate'] * 100
                    means_impr.append(tmp_app['best_gain_rate'].mean())
                    std_impr.append(tmp_app['best_gain_rate'].std())
                    rstrategy_impr.append(conf.reconfig_strategies_name[rs])

                    means_time.append(tmp_app['time'].mean())
                    std_time.append(tmp_app['time'].std())
                    rstrategy_time.append(conf.reconfig_strategies_name[rs])

            dict_time = {'strategy': rstrategy_time, 'means': means_time, 'std': std_time}
            df_time = pd.DataFrame.from_dict(dict_time)

            dict_impr = {'strategy': rstrategy_impr, 'means': means_impr, 'std': std_impr}
            df_impr = pd.DataFrame.from_dict(dict_impr)

            fig = plt.figure()  # Create matplotlib figure

            x = np.arange(len(rstrategy_time))
            ax1 = plt.subplot(1, 1, 1)
            w = 0.3
            # plt.xticks(), will label the bars on x axis with the respective country names.
            plt.xticks(x + w / 2, df_time['strategy'], rotation=25)
            pop = ax1.bar(x, df_impr['means'], width=w, color='b', align='center', capsize=5, yerr=df_impr['std'])
            # The trick is to use two different axes that share the same x axis, we have used ax1.twinx() method.
            plt.ylabel('Latency Improvement (%)')
            ax2 = ax1.twinx()
            # We have calculated GDP by dividing gdpPerCapita to population.
            gdp = ax2.bar(x + w, df_time['means'], width=w, color='g', capsize=5,
                          align='center', yerr=df_time['std'], log=True)
            # Set the Y axis label as GDP.
            plt.ylabel('Algorithm Execution Time (ms)')
            # To set the legend on the plot we have used plt.legend()
            plt.legend([pop, gdp], ['Latency Improvement (%)', 'Algorithm Execution Time (ms)'])
            plt.grid(linestyle='dotted')

            ax1.set_ylim([0, 100])
            ax2.set_ylim([0, 12000000])

            # To show the plot finally we have used plt.show().
            # plt.show()
            plt.tight_layout()
            plt.savefig('app_' + str(a) + '_rp' + str(r) + '_imprv.pdf')

            # print df_impr


def bar_migrations(data=pd.DataFrame(), conf=configurations()):
    for a in range(len(conf.applications)):
        plt.cla()
        plt.clf()
        with_dh_migrations_mean = []
        with_dh_migrations_std = []

        without_dh_migrations_mean = []
        without_dh_migrations_std = []
        for r in range(len(conf.reconfig_applying_rp)):
            for s in range(len(conf.config_strategies)):
                for rs in range(len(conf.reconfig_strategies)):
                    tmp_app = data[(data['app'] == '/dot/' + conf.applications[a] + '.dot') & (
                            data['strategy'] == conf.config_strategies[s]) & (
                                           data['reconfig'] == conf.reconfig_strategies[rs]) & (
                                           data['rp'] == conf.reconfig_applying_rp[r])]
                    if conf.reconfig_applying_rp[r] == 0:
                        without_dh_migrations_mean.append(tmp_app['best_migrations_number_LT'].mean())
                        without_dh_migrations_std.append(tmp_app['best_migrations_number_LT'].std())
                    else:
                        with_dh_migrations_mean.append(tmp_app['best_migrations_number_LT'].mean())
                        with_dh_migrations_std.append(tmp_app['best_migrations_number_LT'].std())

        df = pd.DataFrame({'solution': conf.reconfig_strategies_name, 'With DH': with_dh_migrations_mean,
                           'Without DH': without_dh_migrations_mean, 'std with dh': with_dh_migrations_std,
                           'std without dh': without_dh_migrations_std})

        ind = np.arange(len(conf.reconfig_strategies_name))
        width = 0.35
        plt.bar(ind, df['With DH'].values, width, yerr=df['std with dh'], capsize=5, label='With DH')
        plt.bar(ind + width, df['Without DH'].values, width, yerr=df['std without dh'], capsize=5,
                label='Without DH')
        plt.xticks(ind + width / 2, ('MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-Learning', 'MCTS-UCT'))
        plt.ylabel('Number of Migrations')
        plt.legend(loc='best')
        plt.grid(linestyle='dotted')
        plt.tight_layout()
        plt.savefig('app' + str(a) + '_mig.pdf')


def bar_time_best_latency(data=pd.DataFrame(), conf=configurations()):
    for a in range(len(conf.applications)):
        plt.cla()
        plt.clf()
        with_dh_mean = []
        with_dh_std = []

        without_dh_mean = []
        without_dh_std = []
        rstrategy_time = []
        for r in range(len(conf.reconfig_applying_rp)):
            for s in range(len(conf.config_strategies)):
                for rs in range(len(conf.reconfig_strategies)):
                    tmp_app = data[(data['app'] == '/dot/' + conf.applications[a] + '.dot') & (
                            data['strategy'] == conf.config_strategies[s]) & (
                                           data['reconfig'] == conf.reconfig_strategies[rs]) & (
                                           data['rp'] == conf.reconfig_applying_rp[r])]
                    if conf.reconfig_applying_rp[r] == 0:
                        without_dh_mean.append(tmp_app['iteration_time'].mean())
                        without_dh_std.append(tmp_app['iteration_time'].std())
                    else:
                        with_dh_mean.append(tmp_app['iteration_time'].mean())
                        with_dh_std.append(tmp_app['iteration_time'].std())

                    rstrategy_time.append(conf.reconfig_strategies_name[rs])
        df = pd.DataFrame({'solution': conf.reconfig_strategies_name, 'With DH': with_dh_mean,
                           'Without DH': without_dh_mean, 'std with dh': with_dh_std,
                           'std without dh': without_dh_std})

        ind = np.arange(len(conf.reconfig_strategies_name))
        width = 0.35
        plt.bar(ind, df['With DH'].values, width, yerr=df['std with dh'], capsize=5, label='With DH')
        plt.bar(ind + width, df['Without DH'].values, width, yerr=df['std without dh'],
                label='Without DH', capsize=5, log=True)
        plt.xticks(ind + width / 2, ('MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-Learning', 'MCTS-UCT'))
        plt.legend(loc='best')
        plt.grid(linestyle='dotted')
        plt.ylabel('Time to Best Latency (ms)')
        plt.tight_layout()
        plt.savefig('app' + str(a) + '_lat.pdf')
        # plt.show()

        # print df


def bar_scenario_2(data=pd.DataFrame(), conf=configurations()):
    solutions = ['MCTS-Best-UCT', 'TDTS-SARSA(' + r'$ \lambda $)', 'Q-LEARNING',
                 'MCTS-UCT', 'CLOUD-ONLY', 'TANEJA', 'RTR', 'RTR-RP']
    # a1 = [1.192613, 1.195742, 1.205434, 1.204456, 1.871786, 2.01241, 1.70671, 1.70519]
    # a2 = [1.200919, 1.197348, 1.206433, 1.205249, 2.041068, 2.10121, 1.19978, 1.19489]
    #
    # a1_rp = [1.192616, 1.1946709999999998, 1.202805, 1.202065, 1.871786, 2.01241, 1.70671, 1.70519]
    # a2_rp = [1.194779, 1.1944, 1.195417, 1.195229, 2.041068, 2.10121, 1.19978, 1.19489]
    plt.cla()
    plt.clf()
    fig, axes = plt.subplots()

    df = pd.DataFrame(
        {'MCTS-Best-UCT': [1.192613, 0.682711], 'TDTS-SARSA(' + r'$ \lambda $)': [1.195742, 0.761952],
         'Q-LEARNING': [1.205434, 0.745434], 'MCTS-UCT': [1.204456, 0.891867],
         'CLOUD-ONLY': [1.871786, 1.750612], 'TANEJA': [1.81241, 1.60121], 'RTR': [1.70671, 0.90978]})
    # print df
    df.plot(kind='bar', ylim=(0, 3), ax=axes, alpha=.8, capsize=5,
            color=['salmon', 'cornflowerblue', 'silver', 'cyan', 'orange', 'purple', 'seagreen'], width=.85,
            yerr=[[.353, .6088], [.10221, .12541], [.1325, .30024], [.2353, .2589], [.2831, .13577], [.5964, .6500],
                  [.24543, .2223]])
    axes.legend().set_visible(False)
    # for container, hatch in zip(axes.containers, ('-', '+', 'x', '\\', '*', 'o', 'O', '.')):
    #     for patch in container.patches:
    #         patch.set_hatch(hatch)

    axes.set_title('Without DH')
    axes.grid(linestyle='dotted')
    axes.set_xticklabels(('AppA', 'AppB'), rotation=0)
    axes.set_ylabel('Aggregate End-to-End Latency (ms)')
    axes.legend(loc='upper center', bbox_to_anchor=(0.5, 1), ncol=2)

    # plt.show()
    plt.tight_layout()
    plt.savefig('latency_rp_0.pdf')
    # print 'latency_rp_0.pdf'
    #
    # plt.cla()
    # plt.clf()
    # fig = plt.figure()
    # axes = fig.gca()

    plt.cla()
    plt.clf()
    fig, axes = plt.subplots()
    rp_df = pd.DataFrame(
        {'MCTS-Best-UCT': [1.182616, 0.486958], 'TDTS-SARSA(' + r'$ \lambda $)': [1.1946709999999998, 0.676658],
         'Q-LEARNING': [1.203805, 0.569789],
         'MCTS-UCT': [1.502065, 0.842886], 'RTR-RP': [1.70519, 0.677958]})
    # print rp_df

    rp_df.plot(kind='bar', ylim=(0, 3), ax=axes, alpha=.8, capsize=5,
               color=['cornflowerblue', 'silver', 'cyan', 'purple', 'seagreen', 'orange'], width=.85,
               yerr=[[.1553, .166], [.38221, .32541], [.1325, .30024], [.2353, .19788], [.112044, .18755],
                     [.5035, .455]])
    axes.legend().set_visible(False)

    axes.set_xticklabels(('AppA', 'AppB'), rotation=0)
    axes.grid(linestyle='dotted')
    axes.set_ylabel('Aggregate End-to-End Latency (ms)')
    axes.set_title('With DH')
    axes.legend(loc='upper center', bbox_to_anchor=(0.5, 1), ncol=2)
    plt.tight_layout()
    # plt.show()
    plt.savefig('latency_rp_1.pdf')
    # print 'latency_rp_1.pdf'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    args = parser.parse_args()

    if args.directory:
        data = load_files(args.directory)

        if data.__len__() > 0:
            conf = configurations()

            bar_algo_time_latency_imprv(data=data, conf=conf)

            bar_time_best_latency(data=data, conf=conf)

            bar_migrations(data=data, conf=conf)

            bar_scenario_2(data=data, conf=conf)

import argparse
import os
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.lines import Line2D
import matplotlib.ticker as ticker
from matplotlib.patches import Polygon


def struct_dataset(app, net, slt, label, _dirs=[]):
    data = pd.DataFrame()
    data_src = pd.DataFrame()
    data_cfg = pd.DataFrame()

    for fld in _dirs:
        tmp, tmp_src, tmp_cfg = open_file(fld, app, net, slt, label)

        if data.empty:
            data = tmp
        else:
            data = pd.concat([data, tmp], axis=0)

        if data_src.empty:
            data_src = tmp_src
        else:
            data_src = pd.concat([data_src, tmp_src], axis=0)

        if data_src.empty:
            data_cfg = tmp_cfg
        else:
            data_cfg = pd.concat([data_cfg, tmp_cfg], axis=0)

    return data, data_src, data_cfg


def open_cfg(fld, app, net, slt, label):
    _net = '-1'
    _app = '-1'
    for a in app:
        if fld.find(a) != -1:
            _app = a
            break

    for n in net:
        if fld.find(n) != -1:
            _net = n
            break

    _seed = fld[fld.find('_exp_') + 5: fld.find('_') + (fld.find('_n') - fld.find('_'))]

    data = pd.DataFrame()
    for s in range(len(slt)):
        _dir = fld + "/" + slt[s] + ".cfg"
        if os.path.isfile(_dir):
            sinks = open(fld + "/" + slt[s] + ".ss", 'r').readlines()
            cloud = open(fld + "/" + slt[s] + ".cc", 'r').readlines()
            tmp = pd.read_csv(_dir, sep=';')
            tmp['app'] = _app
            tmp['net'] = _net
            tmp['solution'] = label[s]
            tmp['seed'] = _seed
            tmp['sink'] = 0
            tmp['cloud_p'] = 0
            tmp['infrastructure'] = 'none'
            # print tmp

            for ss in range(len(sinks)):
                tmp.loc[(tmp['task'] == int(sinks[ss])), ['sink']] = 1

            for cc in range(len(cloud)):
                tmp.loc[(tmp['host'] == int(cloud[cc])), 'cloud_p'] = 1

            tmp.loc[(tmp['host'] == 10), 'cloud_p'] = 1


            for n in range(43):
                if n > 0 and n < 22 and n != 10:
                    tmp.loc[(tmp['host'] == n), 'infrastructure'] = 'cloudlet1'
                if n > 22 and n <= 42 and n != 10:
                    tmp.loc[(tmp['host'] == n), 'infrastructure'] = 'cloudlet2'
                if n == 10:
                    tmp.loc[(tmp['host'] == n), 'infrastructure'] = 'cloud'
            # print tmp

            if data.empty:
                data = tmp
            else:
                data = pd.concat([data, tmp], axis=0)

    return data


def open_file(fld, app, net, slt, label):
    data_cfg = open_cfg(fld, app, net, slt, label)

    data = pd.DataFrame()
    data_src = pd.DataFrame()

    _app = ""
    _net = ""
    for a in app:
        if fld.find(a) != -1:
            _app = a
            break

    for n in net:
        if fld.find(n) != -1:
            _net = n
            break

    _seed = fld[fld.find('_exp_') + 5: fld.find('_') + (fld.find('_n') - fld.find('_'))]

    for s in range(len(slt)):
        _dir = fld + "/" + slt[s] + ".csv"
        print _dir
        if os.path.isfile(_dir):
            sinks = open(fld + "/" + slt[s] + ".ss", 'r').readlines()
            cloud = open(fld + "/" + slt[s] + ".cc", 'r').readlines()
            tmp = pd.read_csv(_dir)
            tmp = tmp[((tmp.type == 'scalar') | (tmp.type == 'statistic'))]
            del tmp['run']
            del tmp['type']
            tmp['app'] = _app
            tmp['net'] = _net
            tmp['solution'] = label[s]
            tmp['seed'] = _seed
            # print tmp

            # Read General Data
            _tmp = pd.DataFrame()
            for ss in range(len(sinks)):
                _aux = tmp[(tmp.module == 'DynamicDeployment.vertex-' + str(int(sinks[ss])))]
                c = 0
                for cc in range(len(cloud)):
                    if int(cloud[cc]) == int(sinks[ss]):
                        c = 1

                _aux['vertex'] = (c == 1 and "Cloud" or "Edge") + "(" + str(int(sinks[ss])) + ")"
                _aux['place'] = c
                _tmp = pd.concat([_tmp, _aux], axis=0)

            if data.empty:
                data = _tmp
            else:
                data = pd.concat([data, _tmp], axis=0)

            # Read Data From Data Source in the Sinks
            _tmp_src = pd.DataFrame()
            for ss in range(len(sinks)):
                for i in range(50):
                    _aux_src = tmp[
                        (tmp.module == 'DynamicDeployment.vertex-' + str(int(sinks[ss])) + '.src-' + str(int(i)))]
                    _aux_src['src'] = str(i)
                    c = 0
                    for cc in range(len(cloud)):
                        if int(cloud[cc]) == int(sinks[ss]):
                            c = 1
                    _aux_src['vertex'] = (c == 1 and "Cloud" or "Edge") + "(" + str(int(sinks[ss])) + ")"
                    _aux_src['place'] = c

                    _tmp_src = pd.concat([_tmp_src, _aux_src], axis=0)

            if data_src.empty:
                if not _tmp_src.empty:
                    data_src = _tmp_src
            else:
                if not _tmp_src.empty:
                    data_src = pd.concat([data_src, _tmp_src], axis=0)
        else:
            print ("File does not exist")

    return data, data_src, data_cfg


def filter_fields(data, filter, time=0, base_time=False, mean=False, detail=False, place=False):
    _tmp = data[(data.name == filter)]
    del _tmp['attrname']
    del _tmp['attrvalue']
    del _tmp['count']
    del _tmp['sumweights']
    if not mean:
        del _tmp['mean']
    else:
        del _tmp['value']
    del _tmp['stddev']
    del _tmp['min']
    del _tmp['max']
    del _tmp['module']
    del _tmp['name']

    if not place:
        del _tmp['place']

    if not mean:
        if base_time:
            _tmp['value'] = _tmp.value / float(time)

        _tmp = _tmp[pd.notnull(_tmp['value'])]
    else:
        _tmp = _tmp[pd.notnull(_tmp['mean'])]

    return _tmp


def _box_plot_art(data, filter, net, app, title, ylabel, xlabel, group, slt=[], rotate=False, time=0, base_time=False,
                  mean=False,
                  savefig=False, ind=False):
    __tmp = filter_fields(data, filter, time, base_time, mean)
    _tmp = [[pd.DataFrame() for j in range(len(app))] for i in range(len(net))]
    _global = [pd.DataFrame() for j in range(2)]

    plt.cla()
    plt.clf()
    fig, axes = plt.subplots(nrows=1, ncols=2, sharey=True, figsize=(12, 4))

    flierprops = dict(marker='o', markerfacecolor='green', markersize=12,
                      linestyle='none')
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')

    x = 0
    # print __tmp
    for n in range(len(net)):
        for a in range(len(app)):
            if n == 1 and (a == 1 or a == 2):
                _tem_data = pd.DataFrame()
                for s in range(len(slt)):
                    _tmp[n][a] = __tmp[
                        (__tmp.net == net[n]) & (__tmp.app == app[a]) & (__tmp.solution == slt[s])]
                    print "Net: " + net[n] + " App: " + app[a] + " slt: " + slt[s]
                    print _tmp[n][a]
                    del _tmp[n][a]['app']
                    del _tmp[n][a]['net']
                    del _tmp[n][a]['solution']
                    _tmp[n][a].columns = [slt[s], 'seed', 'vertex']
                    _tmp[n][a] = _tmp[n][a].set_index(['vertex', 'seed'])

                    if _tem_data.__len__() > 0:
                        _tem_data = pd.merge(_tem_data, _tmp[n][a], left_index=True, right_index=True)
                    else:
                        _tem_data = _tmp[n][a]

                _global[x] = _tem_data
                print _global[x]

                _global[x].boxplot(ax=axes[x], medianprops=medianprops)
                axes[x].title.set_text('App' + str(a + 1))
                x = x + 1
    for ax in axes.flatten():
        ax.set_yscale('log')
        ax.set_xlabel(xlabel)
        if rotate:
            for tick in ax.get_xticklabels():
                tick.set_rotation(90)
                tick.set_fontsize(8)

    axes[0].set_ylabel(ylabel, fontsize=11)
    plt.suptitle('')
    plt.grid(linestyle='dotted')
    fig.subplots_adjust(hspace=0.4)
    plt.suptitle('')
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig('group_a2_a3_' + title + '.pdf')


def _box_plot(data, filter, net, app, title, ylabel, xlabel, group, slt=[], rotate=False, time=0, base_time=False,
              mean=False,
              savefig=False, ind=False):
    __tmp = filter_fields(data, filter, time, base_time, mean)
    _tmp = [[pd.DataFrame() for j in range(len(app))] for i in range(len(net))]

    # fig, axes = plt.subplots(nrows=len(net), ncols=len(app), figsize=(12, 6), sharey=True)

    # flierprops = dict(marker='o', markerfacecolor='green', markersize=12,
    #                   linestyle='none')
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')
    _vertex = pd.DataFrame()
    for n in range(len(net)):
        for a in range(len(app)):
            _global = pd.DataFrame()
            if group.__len__() > 1:
                x = __tmp[(__tmp.net == net[n]) & (__tmp.app == app[a])]
                _vertex = x.vertex.unique()
                for v in range(len(_vertex)):
                    for s in range(len(slt)):
                        _tmp[n][a] = __tmp[
                            (__tmp.net == net[n]) & (__tmp.app == app[a]) & (__tmp.solution == slt[s]) & (
                                    __tmp.vertex == _vertex[v])]
                        del _tmp[n][a]['app']
                        del _tmp[n][a]['net']
                        del _tmp[n][a]['solution']
                        del _tmp[n][a]['vertex']
                        _tmp[n][a].columns = [slt[s], 'seed']
                        _tmp[n][a] = _tmp[n][a].set_index(['seed'])

                        if _global.__len__() > 0:
                            _global = pd.merge(_global, _tmp[n][a], left_index=True, right_index=True)
                        else:
                            _global = _tmp[n][a]


            else:
                for s in range(len(slt)):
                    _tmp[n][a] = __tmp[
                        (__tmp.net == net[n]) & (__tmp.app == app[a]) & (__tmp.solution == slt[s])]
                    del _tmp[n][a]['app']
                    del _tmp[n][a]['net']
                    del _tmp[n][a]['solution']
                    _tmp[n][a].columns = [slt[s], 'seed', 'vertex']
                    _tmp[n][a] = _tmp[n][a].set_index(['vertex', 'seed'])
                    # print _tmp[n][a]
                    if _global.__len__() > 0:
                        _global = pd.merge(_global, _tmp[n][a], left_index=True, right_index=True)
                    else:
                        _global = _tmp[n][a]
            if group.__len__() > 1:
                columns = []
                for v in range(len(_vertex)):
                    for c in range(len(slt)):
                        columns.append(slt[c])

                _global.columns = columns

            plt.cla()
            plt.clf()
            _, bp = pd.DataFrame.boxplot(_global, medianprops=medianprops, return_type='both')
            _.title.set_text('Network = ' + net[n] + ' App = ' + app[a])
            _.title.set_text('')
            if rotate:
                for tick in _.get_xticklabels():
                    tick.set_rotation(90)

            for tick in _.xaxis.get_major_ticks():
                tick.label.set_fontsize(11)

            for tick in _.yaxis.get_major_ticks():
                tick.label.set_fontsize(11)

            _.set_ylabel(ylabel, fontsize=11)

            _.set_yscale('log')
            _.set_xlabel(xlabel)

            # outliers = [flier.get_ydata() for flier in bp["fliers"]]
            # boxes = [box.get_ydata() for box in bp["boxes"]]
            # medians = [median.get_ydata() for median in bp["medians"]]
            # whiskers = [whiskers.get_ydata() for whiskers in bp["whiskers"]]
            # means = [means.get_ydata() for means in bp["means"]]
            # print 'Outliers_' + str(n) + '_' + str(a) + '_' + title + ' :'
            # print outliers
            # print 'boxes_' + str(n) + '_' + str(a) + '_' + title + ' :'
            # print boxes
            # print 'medians_' + str(n) + '_' + str(a) + '_' + title + ' :'
            # print medians
            # print 'whiskers_' + str(n) + '_' + str(a) + '_' + title + ' :'
            # print whiskers
            # print 'means_' + str(n) + '_' + str(a) + '_' + title + ' :'
            # print means
            # fig = plt.gcf()
            # fig.set_size_inches(18.5, 10.5)
            h = _._position.height
            ymin, ymax = _.get_ylim()
            if _vertex.__len__() > 0:
                for v in range(len(_vertex)):
                    if v > 0:
                        pos_line = (v * slt.__len__()) + .6
                        plt.axvline(x=pos_line, linewidth=3)
                    pos_text = (v + 1) * slt.__len__() - (slt.__len__() - 1) + slt.__len__() / 2.5
                    ll = _vertex[v].replace(")", "")
                    ll = ll.replace("(", "_")
                    _.text(pos_text, ymax + ymax * .2, ll,
                           horizontalalignment='center', fontsize=12, transform=_.transData)

            # medians = list(range(boxes.__len__()))
            # top = 15
            # pos = np.arange(boxes.__len__()) + 1
            # upperLabels = [str(np.round(s, 2)) for s in medians]
            # weights = ['bold', 'semibold']
            # for tick, label in zip(range(boxes.__len__()), axe.get_xticklabels()):
            #     k = tick % 2
            #     axe.text(pos[tick], top - (top * 0.05), upperLabels[tick],
            #              horizontalalignment='center', size='x-small', weight=weights[k])
            plt.suptitle('')

            plt.grid(linestyle='dotted')
            plt.tight_layout()

            if _vertex.__len__() > 0:
                plt.subplots_adjust(bottom=0.18, left=.12, right=.97, top=.93)

            if savefig:
                plt.savefig(str(n) + '_' + str(a) + '_' + title + '.pdf')
                # _tmp[n][a].to_csv(str(n) + '_' + str(a) + '_' + title + '.csv', sep='\t')
            else:
                plt.show()
            plt.cla()
            plt.clf()


def box_plot(data, filter, net, app, title, ylabel, xlabel, group, rotate=False, time=0, base_time=False, mean=False,
             savefig=False, ind=False):
    __tmp = filter_fields(data, filter, time, base_time, mean)
    del __tmp['seed']
    _tmp = [[pd.DataFrame() for j in range(len(app))] for i in range(len(net))]

    if not ind:
        fig, axes = plt.subplots(nrows=len(net), ncols=len(app), figsize=(12, 6), sharey=True)

    # flierprops = dict(marker='o', markerfacecolor='green', markersize=12,
    #                   linestyle='none')
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')

    for n in range(len(net)):
        for a in range(len(app)):
            _tmp[n][a] = __tmp[(__tmp.net == net[n]) & (__tmp.app == app[a])]
            del _tmp[n][a]['app']
            del _tmp[n][a]['net']
            _tmp[n][a] = _tmp[n][a].set_index(['vertex', 'solution'])

            if ind:
                plt.cla()
                plt.clf()
                axe = plt.gca()

                _, bp = _tmp[n][a].boxplot(by=group, ax=axe, medianprops=medianprops, return_type='both')

                # print bp

                axe.title.set_text('Network = ' + net[n] + ' App = ' + app[a])
                if a == 0:
                    axe.set_ylabel(ylabel)

                axe.set_yscale('log')
                axe.set_xlabel(xlabel)

                if rotate:
                    for tick in axe.get_xticklabels():
                        tick.set_rotation(90)

                # numBoxes = 4
                # medians = list(range(4))
                # for i in range(numBoxes):
                #     box = bp["boxes"][i]
                #     boxX = []
                #     boxY = []
                #     for j in range(5):
                #         boxX.append(box.get_xdata()[j])
                #         boxY.append(box.get_ydata()[j])
                #
                # top = 40
                # bottom = -5
                # pos = np.arange(4) + 1
                # upperLabels = [str(np.round(s, 2)) for s in medians]
                # weights = ['bold', 'semibold']
                # for tick, label in zip(range(4), axe.get_xticklabels()):
                #     k = tick % 2
                #     axe.text(pos[tick], top - (top * 0.05), upperLabels[tick],
                #              horizontalalignment='center', size='x-small', weight=weights[k])
                #     print label
                #
                # sys.exit()

                # outliers = [flier.get_ydata() for flier in bp["fliers"]]
                boxes = [box.get_ydata() for box in bp["boxes"]]
                # medians = [median.get_ydata() for median in bp["medians"]]
                # whiskers = [whiskers.get_ydata() for whiskers in bp["whiskers"]]

                plt.suptitle('')
                plt.tight_layout()
                if savefig:
                    plt.savefig(str(n) + '_' + str(a) + '_' + title + '.pdf')
                    # _tmp[n][a].to_csv(str(n) + '_' + str(a) + '_' + title + '.csv', sep='\t')
                else:
                    plt.show()

                plt.cla()
                plt.clf()
                # axe = plt.gca()
                # _tmp[n][a].hist(by=group, ax=axe)
                # axe.set_yscale('log')
                # axe.set_xscale('log')
                # plt.show()
            else:
                flierprops = dict(marker='o', markerfacecolor='green', markersize=7,
                                  linestyle='none')
                _tmp[n][a].boxplot(by=group, ax=axes[n, a], medianprops=medianprops,
                                   meanprops=meanlineprops, meanline=False,
                                   showmeans=False)
                axes[n, a].title.set_text('Network = ' + net[n] + ' App = ' + app[a])
                _tmp[n][a].to_csv(str(n) + '_' + str(a) + '_' + title + '.csv', sep='\t')
                if a == 0:
                    axes[n, a].set_ylabel(ylabel)

    if not ind:
        for ax in axes.flatten():
            ax.set_yscale('log')
            ax.set_xlabel(xlabel)
            if rotate:
                for tick in ax.get_xticklabels():
                    tick.set_rotation(90)
                    tick.set_fontsize(8)

        fig.subplots_adjust(hspace=0.4)
        plt.suptitle('')
        plt.tight_layout(rect=[0, 0.03, 1, 0.95])
        if savefig:
            plt.savefig(title + '.pdf')
        else:
            plt.show()

    # Histogram
    # _fig, _axes = plt.subplots(nrows=len(net), ncols=len(app), figsize=(12, 6), sharey=True)
    # for n in range(len(net)):
    #     for a in range(len(app)):
    #         _tmp[n][a].hist(by=['solution'], ax=_axes[n, a])
    #         _axes[n, a].title.set_text('Network = ' + net[n] + ' App = ' + app[a])
    #         if a == 0:
    #             _axes[n, a].set_ylabel(ylabel)
    #
    # for ax in _axes.flatten():
    #     ax.set_yscale('log')
    #     ax.set_xlabel(xlabel)
    #     if rotate:
    #         for tick in ax.get_xticklabels():
    #             tick.set_rotation(90)
    #
    # _fig.subplots_adjust(hspace=0.4)
    # plt.suptitle('')
    # plt.tight_layout()
    # if savefig:
    #     plt.savefig('h' + title + '.png')
    # else:
    #     plt.show()

    plt.cla()
    plt.clf()


def agg_speedup_plot(data, filter, slt, title, ylabel, xlabel, time=0, base_time=False, savefig=False):
    plt.cla()
    plt.clf()

    __tmp = filter_fields(data, filter, time, base_time)

    _tmp = [pd.DataFrame() for j in range(len(slt))]
    for s in range(len(slt)):
        _tmp[s] = __tmp[(__tmp.solution == slt[s])]
        _tmp[s] = _tmp[s].set_index(['app', 'net', 'seed', 'vertex'])

    _global = pd.DataFrame()
    for s2 in range(len(slt)):
        if s2 != 0:
            _tmp[s2] = pd.merge(_tmp[s2], _tmp[0], left_index=True, right_index=True)
            del _tmp[s2]['solution_x']
            del _tmp[s2]['solution_y']
            _tmp[s2].columns = [slt[s2], slt[0]]
            _tmp[s2]['solution'] = slt[s2]
            _tmp[s2] = _tmp[s2][(pd.notnull(_tmp[s2][slt[s2]])) & (pd.notnull(_tmp[s2][slt[0]]))]
            _tmp[s2]['agg'] = _tmp[s2][slt[s2]] - _tmp[s2][slt[0]]

            del _tmp[s2][slt[s2]]
            del _tmp[s2][slt[0]]
            # _tmp[s2] = _tmp[s2].set_index(['solution'], append=True)

            if _global.empty:
                df = _tmp[s2].reset_index()
                _global = df.loc[:, ['solution', 'agg']]
            else:
                df = _tmp[s2].reset_index()
                _global = _global.append(df.loc[:, ['solution', 'agg']])
    # print _global
    _fig, _axes = plt.subplots()
    flierprops = dict(marker='o', markerfacecolor='green', markersize=12,
                      linestyle='none')
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')
    _global.boxplot(by='solution', ax=_axes, medianprops=medianprops,
                    showmeans=True)

    _axes.set_yscale('log')
    _axes.set_xlabel(xlabel)
    _axes.set_ylabel(ylabel)
    _axes.set_title('')
    plt.suptitle('')
    plt.grid(linestyle='dotted')
    plt.tight_layout()

    if savefig:
        plt.savefig('spd' + title + '.pdf')
        # _global.to_csv('spd' + title + '.csv', sep='\t')
    else:
        plt.show()

    plt.cla()
    plt.clf()


def _agg_speedup_plot(data, filter, slt, title, ylabel, xlabel, time=0, base_time=False, savefig=False):
    plt.cla()
    plt.clf()

    __tmp = filter_fields(data, filter, time, base_time)

    _tmp = [pd.DataFrame() for j in range(len(slt))]
    for s in range(len(slt)):
        _tmp[s] = __tmp[(__tmp.solution == slt[s])]
        _tmp[s] = _tmp[s].set_index(['app', 'net', 'seed', 'vertex'])

    _global = pd.DataFrame()
    for s2 in range(len(slt)):
        # if s2 == 1:
        #     continue

        if s2 != 0:
            # print _tmp[s2]
            _tmp[s2] = pd.merge(_tmp[s2], _tmp[0], left_index=True, right_index=True)
            # print _tmp[s2]
            del _tmp[s2]['solution_x']
            del _tmp[s2]['solution_y']
            _tmp[s2].columns = [slt[s2], slt[0]]
            _tmp[s2]['solution'] = slt[s2]
            _tmp[s2] = _tmp[s2][(pd.notnull(_tmp[s2][slt[s2]])) & (pd.notnull(_tmp[s2][slt[0]]))]
            _tmp[s2]['agg'] = _tmp[s2][slt[s2]] - _tmp[s2][slt[0]]
            # print _tmp[s2]

            del _tmp[s2][slt[s2]]
            del _tmp[s2][slt[0]]
            # print _tmp[s2]
            # _tmp[s2] = _tmp[s2].set_index(['solution'], append=True)

            df = _tmp[s2].reset_index()
            df = df.loc[:, ['seed', 'vertex', 'app', 'net', 'agg']]
            # df['agg'] = (df['agg'] - df['agg'].mean()) / df['agg'].std()
            # df['agg'] = (df['agg'] - df['agg'].mean()) / (df['agg'].max() - df['agg'].min())

            df.columns = ['seed', 'vertex', 'app', 'net', slt[s2]]
            df = df.set_index(['seed', 'vertex', 'app', 'net'])

            # print df
            if _global.empty:
                _global = df
            else:
                _global = pd.merge(_global, df, left_index=True, right_index=True)

    # print _global
    _fig, _axes = plt.subplots()  #
    flierprops = dict(marker='o', markerfacecolor='green', markersize=12,
                      linestyle='none')
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')
    _, bp = pd.DataFrame.boxplot(_global, ax=_axes, medianprops=medianprops,
                                 meanprops=meanlineprops, meanline=False,
                                 showmeans=False, return_type='both', showfliers=False)

    # _axes.set_yscale('symlog')
    _axes.set_xlabel(xlabel)

    # _axes.set_ylabel(ylabel)
    for tick in _axes.xaxis.get_major_ticks():
        tick.label.set_fontsize(11)

    for tick in _axes.yaxis.get_major_ticks():
        tick.label.set_fontsize(11)

    _axes.set_ylabel(ylabel, fontsize=11)
    _axes.set_title('')
    plt.suptitle('')
    plt.grid(linestyle='dotted')
    plt.tight_layout()

    outliers = [flier.get_ydata() for flier in bp["fliers"]]
    boxes = [box.get_ydata() for box in bp["boxes"]]
    medians = [median.get_ydata() for median in bp["medians"]]
    whiskers = [whiskers.get_ydata() for whiskers in bp["whiskers"]]
    caps = [caps.get_ydata() for caps in bp["caps"]]

    print 'Outliers'
    print outliers

    print 'Boxes'
    print boxes

    print 'Medians'
    print medians

    print 'Whiskers'
    print whiskers

    print 'Caps'
    print caps

    _sub = _global.drop('LB', 1)

    a = plt.axes([.38, .35, .57, .6])
    _2, bp2 = pd.DataFrame.boxplot(_sub, ax=a, flierprops=flierprops, medianprops=medianprops,
                                   meanprops=meanlineprops, meanline=False,
                                   showmeans=False, return_type='both', showfliers=False)
    plt.grid(linestyle='dotted')
    # plt.title('Probability')

    if savefig:
        plt.savefig('spd' + title + '.pdf')
        # _global.to_csv('spd' + title + '.csv', sep='\t')
    else:
        plt.show()

    plt.cla()
    plt.clf()


def scatter_plot(data_cfg, slt, savefig=False):
    plt.cla()
    plt.clf()
    c = ['k', 'g', 'r']
    x = [[] for i in range(3)]
    a = [1, .75, .5]
    for s in range(len(slt)):
        if slt[s] != slt[0]:
            _tmp = data_cfg[(data_cfg.sink == 0) & (data_cfg.arrivalrate == 0) & (data_cfg.solution == slt[s])]
            # & ((data_cfg.app=='a3') | (data_cfg.app=='a4'))]
            # print _tmp
            _aux = _tmp[['host', 'seed', 'net', 'app', 'task']].copy()
            _aux['size'] = _aux.groupby(['seed', 'net', 'app', 'host']).transform(np.size)
            _aux = _aux.drop_duplicates(['seed', 'net', 'app', 'host'])
            _aux = _aux.set_index(['seed', 'net', 'app', 'host'])
            _aux = _aux.sort_index()

            _aux2 = _tmp[['seed', 'net', 'app', 'host']].copy()
            _aux2['total'] = _aux2.groupby(['seed', 'net', 'app']).transform(np.size)
            _aux2 = _aux2.drop_duplicates(['seed', 'net', 'app', 'host'])
            _aux2 = _aux2.set_index(['seed', 'net', 'app', 'host'])
            _aux2 = _aux2.sort_index()

            _final = pd.merge(_aux, _aux2, left_index=True, right_index=True)
            _final = _final.sort_index()

            _t = _final.reset_index()
            _t = _t[['seed', 'host', 'app', 'net', 'size', 'total']].copy()
            _t = _t.set_index(['seed', 'host', 'app', 'net'])
            _t = _t.drop_duplicates()
            _t = _final.reset_index()
            _t = _t.set_index(['seed', 'net', 'app'])
            _t = _t.sort_index()

            _t['average'] = _t['size'] / _t['total']
            _t['norm'] = (_t['average'] - _t['average'].mean()) / _t['average'].std()
            _t = _t.reset_index()
            _s = _t[['host', 'average', 'norm']].copy()
            _s = _s.set_index('host')
            x[s - 1] = _s['norm'].tolist()

            plt.hist(x[s - 1], bins=20, histtype='stepfilled', color=c[s - 1],
                     label=slt[s] + ' Mean: ' + format(_t['average'].mean() * 100, '.2f') + '%' + ' Std: ' + format(
                         _t['average'].std() * 100, '.2f') + '%', log=True,
                     alpha=a[s - 1])

            # print slt[s] + ': Mean ' + str(_t['average'].mean()) + ' std ' + str(_t['average'].std())
            # print _s
            # _global = pd.merge(_global, _s, left_index=True, right_index=True)
    plt.xlabel('Density', fontsize=11)

    plt.ylabel('Frequency', fontsize=11)
    plt.legend(fontsize=11)
    plt.tight_layout()
    plt.savefig('density.pdf')
    plt.show()

    # ax.scatter(_aux.iloc[:, 0], _aux.iloc[:, 2], c=_mt[s], label=slt[s], s=100, alpha=0.5, marker=_pt[s])

    # trips.groupby('usertype')['minutes'].hist(bins=np.arange(61), alpha=0.5, normed=True);
    # plt.xlabel('Duration (minutes)')
    # plt.ylabel('relative frequency')
    # plt.title('Trip Durations')
    # plt.text(34, 0.09, "Free Trips\n\nAdditional Fee", ha='right',
    #          size=18, rotation=90, alpha=0.5, color='red')
    #
    # ax.set_yscale('log')
    # plt.xticks(np.arange(0, 21, 1))
    # plt.xlabel('Host', fontsize=9)
    # plt.ylabel('Number of Operators', fontsize=9)
    # plt.title('')
    # plt.legend(loc=2, ncol=slt.__len__())
    # plt.tight_layout()
    # if savefig:
    #     plt.savefig('scatter.pdf')
    # else:
    #     plt.show()
    #
    plt.cla()
    plt.clf()


def bar_plot(data, slt, filter='', title='', ylabel='', xlabel='', time=0, savefig=False, detail=False):
    def prepare_dataframe(app=[], filter_extension=[]):
        def data_filtering(_means, _errors, filter_t='', solution='', extend=''):
            _tmp = filter_fields(data, filter_t, time, mean=False)
            _tmp = _tmp[(_tmp.solution == solution) & ((_tmp.app == app[0]) | (_tmp.app == app[1]))]
            _tmp['label'] = _tmp['app'] + '_' + _tmp['vertex'] + '_' + _tmp['src'].apply(str)
            _filtered = _tmp[['label', 'value']].copy()
            _filtered.columns = ['label', solution + ' ' + extend]
            _filtered = _filtered.set_index('label')
            __global = _filtered.groupby(level=('label'))
            if _means.empty:
                _means = __global.mean()
                _errors = __global.std()
            else:
                _means = pd.merge(_means, __global.mean(), left_index=True, right_index=True)
                _errors = pd.merge(_errors, __global.std(), left_index=True, right_index=True)

            return _means, _errors

        means = pd.DataFrame()
        errors = pd.DataFrame()
        for s in range(len(slt)):
            means, errors = data_filtering(_means=means, _errors=errors, filter_t=filter, solution=slt[s],
                                           extend=(filter_extension.__len__() > 0 and 'TOTAL' or ''))
            print means

            if filter_extension.__len__() > 0:
                for r in range(len(filter_extension)):
                    means, errors = data_filtering(_means=means, _errors=errors, filter_t=filter_extension[r],
                                                   solution=slt[s],
                                                   extend=(filter_extension[r].find("Comm") != -1 and 'COM' or 'COMP'))

        return means, errors

    def plot_data(app, _means, _std, type='b', _styles=[], _title=''):
        plt.cla()
        plt.clf()
        fig, ax = plt.subplots()
        plt.subplots_adjust(bottom=0.18, left=.12, right=.97, top=.93)
        if type == 'b':
            _means.plot.bar(ax=ax, log=True, width=.7, ylim=(0, 6))  # yerr=errors,
            # ax.set_ylim(0, 6, auto=False)

            # ax.tick_params(direction='out', axis='y', length=6, width=2)

        else:
            _means.plot(kind='line', ax=ax, style=_styles, logy=True, markersize=5)
            ax.set_xticks(np.arange(len(_means.index)))
            ax.set_xticklabels(["%s" % item for item in _means.index.tolist()], rotation=90)
            ax.legend(ncol=4, loc='upper center', bbox_to_anchor=(0.5, 1.15), fontsize=7)

        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        plt.suptitle('')
        #
        # ymin, ymax = ax.get_ylim()
        # if _vertex.__len__() > 0:
        #     for v in range(len(_vertex)):
        #         if v > 0:
        #             pos_line = (v * slt.__len__()) + .6
        #             plt.axvline(x=pos_line, linewidth=3)
        #         pos_text = (v + 1) * slt.__len__() - (slt.__len__() - 1) + slt.__len__() / 2.5
        #         _.text(pos_text, ymax + ymax * .1, _vertex[v],
        #                horizontalalignment='center', fontsize=16, weight='bold', transform=_.transData)

        # if type == 'b':
        #     plt.tight_layout()

        if savefig:
            plt.savefig(_title + '_a' + app[0] + 'a' + app[1] + '.pdf')
            # _means.to_csv(_title + '_a' + app[0] + 'a' + app[1] + '.csv', sep='\t')
            # _std.to_csv('std_' + _title + '_a' + app[0] + 'a' + app[1] + '.csv', sep='\t')
        else:
            plt.show()

        plt.cla()
        plt.clf()

    mean, std = prepare_dataframe(app=['a1', 'a2'])
    plot_data(app=['a1', 'a2'], _means=mean, _std=std, type='b', _title='b_' + title)

    mean, std = prepare_dataframe(app=['a3', 'a4'])
    plot_data(app=['a3', 'a4'], _means=mean, _std=std, type='b', _title='b_' + title)

    if detail:
        mean, std = prepare_dataframe(app=['a1', 'a2'], filter_extension=['totalCommTime:mean', 'totalCompTime:mean'])
        plot_data(app=['a1', 'a2'], _means=mean, _std=std, type='l',
                  _styles=['o-', 'o--', 'o:', 'x-', 'x--', 'x:', 'd-', 'd--', 'd:', '*-', '*--', '*:', '^-', '^--',
                           '^:'],
                  _title='l_' + title)

        mean, std = prepare_dataframe(app=['a3', 'a4'], filter_extension=['totalCommTime:mean', 'totalCompTime:mean'])
        plot_data(app=['a3', 'a4'], _means=mean, _std=std, type='l',
                  _styles=['o-', 'o--', 'o:', 'x-', 'x--', 'x:', 'd-', 'd--', 'd:', '*-', '*--', '*:', '^-', '^--',
                           '^:'],
                  _title='l_' + title)


def bar_plot_art_weight(data, slt, filter=[], title=[], ylabel='', xlabel='', time=0, savefig=False, detail=False):
    def get_throughput():
        _tmp = filter_fields(data, 'queueLengthTopic:last', time, mean=False)
        _indexed = _tmp.set_index(['solution', 'seed', 'app', 'net', 'vertex', 'src'])
        _indexed['total'] = _indexed.groupby(['solution', 'seed', 'app', 'net'])['value'].transform(sum)
        _indexed['weight'] = _indexed['value'] / _indexed['total']
        print _indexed
        return _indexed

    def prepare_dataframe(throughput):
        def data_filtering(filter_t=''):
            _tmp = filter_fields(data, filter_t, time, mean=False)
            # _tmp = _tmp[(_tmp.solution == solution)]
            _tmp = _tmp.set_index(['solution', 'seed', 'app', 'net', 'vertex', 'src'])
            print throughput
            th_mean = pd.merge(_tmp, throughput, left_index=True, right_index=True)
            th_mean['value'] = th_mean['value_x'] * th_mean['weight']
            th_mean = th_mean.reset_index()
            _filtered = th_mean[['solution', 'seed', 'app', 'net', 'value']].copy()
            _filtered.columns = ['solution', 'seed', 'app', 'net', 'value']
            _filtered = _filtered.groupby(['solution', 'seed', 'app', 'net']).sum()
            _filtered = _filtered.reset_index()
            print _filtered
            _mean = _filtered[['solution', 'value']].copy()
            _mean = _mean.set_index('solution')
            __global = _mean.groupby(level=['solution'])
            print __global.mean()
            return __global.mean(), __global.std()

        means = [pd.DataFrame() for j in range(len(filter))]
        errors = [pd.DataFrame() for j in range(len(filter))]

        for f in range(len(filter)):
            means[f], errors[f] = data_filtering(filter_t=filter[f])
        return means, errors

    def plot_data(app, _means, _std, _styles=[]):
        plt.cla()
        plt.clf()
        fig, axes = plt.subplots(nrows=1, ncols=2, sharey=True, figsize=(12, 4))
        for f in range(len(filter)):
            _means[f].plot.bar(ax=axes[f], width=.7, ylim=(0, 6))  # yerr=errors,

        axes[0].set_ylabel(ylabel)
        plt.suptitle('')
        plt.tight_layout()

        # plt.savefig('z.pdf')
        plt.show()
        plt.cla()
        plt.clf()

    # mean = [pd.DataFrame() for j in range(len(filter))]
    # std = [pd.DataFrame() for j in range(len(filter))]
    throughput = get_throughput()
    mean, std = prepare_dataframe(throughput)
    print mean[0]
    print mean[1]

    plot_data(app=['a3', 'a4'], _means=mean, _std=std)


def bar_plot_art(data, slt, filter=[], title=[], ylabel='', xlabel='', time=0, savefig=False, detail=False):
    def prepare_dataframe(app=[], filter_extension=[]):
        def data_filtering(_means, _errors, filter_t='', solution='', extend=''):
            _tmp = filter_fields(data, filter_t, time, mean=False)
            _tmp = _tmp[(_tmp.solution == solution) & ((_tmp.app == app[0]) | (_tmp.app == app[1]))]
            _tmp['label'] = _tmp['app'] + '_' + _tmp['vertex'] + '_src_' + _tmp['src'].apply(str)
            _filtered = _tmp[['label', 'value']].copy()
            _filtered.columns = ['label', solution + ' ' + extend]
            _filtered = _filtered.set_index('label')
            __global = _filtered.groupby(level=('label'))
            if _means.empty:
                _means = __global.mean()
                _errors = __global.std()
            else:
                _means = pd.merge(_means, __global.mean(), left_index=True, right_index=True)
                _errors = pd.merge(_errors, __global.std(), left_index=True, right_index=True)

            # print _means
            return _means, _errors

        means = [pd.DataFrame() for j in range(len(filter))]
        errors = [pd.DataFrame() for j in range(len(filter))]

        for f in range(len(filter)):
            for s in range(len(slt)):
                means[f], errors[f] = data_filtering(_means=means[f], _errors=errors[f], filter_t=filter[f],
                                                     solution=slt[s],
                                                     extend='')
        return means, errors

    def plot_data(app, _means, _std, _styles=[]):
        plt.cla()
        plt.clf()
        fig, axes = plt.subplots(nrows=1, ncols=2, sharey=True, figsize=(12, 4))
        app = ['App3', 'App4']
        pos = [0.5, 4]
        xtext = []
        for f in range(len(filter)):
            _means[f].plot.bar(ax=axes[f], log=True, width=.7, ylim=(0, 6))  # yerr=errors,

            for tick in axes[f].get_xticklabels():
                ll = tick.get_text().replace("a3_", "")
                ll = ll.replace("a4_", "")
                ll = ll.replace(")", "")
                ll = ll.replace("(", "_")
                xtext.append(ll)

            axes[f].set_xticklabels([i for i in xtext])

            ymin, ymax = axes[f].get_ylim()
            for a in range(len(app)):
                pos_text = pos[a] + 0.5
                axes[f].text(pos_text, ymax - ymax * .8, app[a],
                             horizontalalignment='center', fontsize=12, transform=axes[f].transData)

            axes[f].axvline(x=2.5, linewidth=3)
            axes[f].set_xlabel('')
            axes[f].set_ylabel('')
            axes[f].set_title(title[f])

        # fig.subplots_adjust(hspace=0.4)

        axes[0].set_ylabel(ylabel)
        # plt.subplots_adjust(bottom=0.3, left=.07, right=.97, top=.85)
        plt.suptitle('')
        plt.tight_layout()

        plt.savefig('group_a3_a4_bars.pdf')
        # plt.show()
        plt.cla()
        plt.clf()

    mean = [pd.DataFrame() for j in range(len(filter))]
    std = [pd.DataFrame() for j in range(len(filter))]
    mean, std = prepare_dataframe(app=['a3', 'a4'])
    print mean[0]
    print mean[1]
    plot_data(app=['a3', 'a4'], _means=mean, _std=std)


def _box_plot_art_app(data=[], filter=[], net=[], app=[], title='', ylabel='', xlabel='', slt=[], rotate=False, time=0,
                      base_time=False,
                      mean=False, savefig=False):
    __tmp = filter_fields(data, filter, time, base_time, mean)
    _global = [pd.DataFrame() for j in range(len(app))]

    plt.cla()
    plt.clf()
    fig, axes = plt.subplots(nrows=1, ncols=4, sharey=True, figsize=(14, 6))

    flierprops = dict(marker='x', markersize=4)
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')

    means_box = pd.DataFrame()
    medians_box = pd.DataFrame()
    ics_box = pd.DataFrame()

    for a in range(len(app)):
        for s in range(len(slt)):
            _local = __tmp[(__tmp.app == app[a]) & (__tmp.solution == slt[s])]
            del _local['app']
            del _local['net']
            del _local['solution']
            _local['value'] = _local['value'] * 1000
            _local.columns = [slt[s], 'seed', 'vertex']
            _local = _local.set_index(['vertex', 'seed'])

            if _global[a].__len__() > 0:
                _global[a] = pd.merge(_global[a], _local, left_index=True, right_index=True)
            else:
                _global[a] = _local

        _, bp = _global[a].boxplot(ax=axes[a], medianprops=medianprops, flierprops=flierprops,
                                   meanprops=meanlineprops, meanline=False,
                                   showmeans=False, return_type='both', showfliers=False)
        outliers = [flier.get_ydata() for flier in bp["fliers"]]
        boxes = [box.get_ydata() for box in bp["boxes"]]
        medians = [median.get_ydata() for median in bp["medians"]]
        whiskers = [whiskers.get_ydata() for whiskers in bp["whiskers"]]
        means = [means.get_ydata() for means in bp["means"]]

        _medians = pd.DataFrame(medians, columns=['med_0', 'med_1'])

        _medians = _medians.loc[:, ['med_0']]
        _medians.columns = [app[a]]

        if medians_box.__len__() > 0:
            medians_box = pd.merge(medians_box, _medians, left_index=True, right_index=True)
        else:
            medians_box = _medians
        #
        # _means = pd.DataFrame(means, columns=['med_0', 'med_1'])
        # _means = _means.loc[:, ['med_0']]
        # _means.columns = [app[a]]
        #
        # if means_box.__len__() > 0:
        #     means_box = pd.merge(means_box, _means, left_index=True, right_index=True)
        # else:
        #     means_box = _means

        axes[a].title.set_text('App' + str(a + 1))
        axes[a].title.set_fontsize(16)
        axes[a].grid(linestyle='dotted')

    print medians_box.T
    #
    # print means_box.T

    for ax in axes.flatten():
        # ax.set_yscale('log')
        ax.set_xlabel(xlabel)
        for tick in ax.get_xticklabels():
            tick.set_fontsize(14)
        for tick in ax.get_yticklabels():
            tick.set_fontsize(14)

        if rotate:
            for tick in ax.get_xticklabels():
                tick.set_rotation(90)
                tick.set_fontsize(14)

    axes[0].set_ylabel(ylabel, fontsize=14)
    plt.suptitle('')

    fig.subplots_adjust(hspace=0.1)
    plt.suptitle('')
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig('all_app_' + title + '.pdf')
    # plt.show()
    plt.cla()
    plt.clf()


def _box_plot_art_net(data=[], filter=[], net=[], app=[], title='', ylabel='', xlabel='', slt=[], rotate=False, time=0,
                      base_time=False,
                      mean=False, savefig=False):
    __tmp = filter_fields(data, filter, time, base_time, mean)
    _global = [pd.DataFrame() for j in range(len(net))]

    plt.cla()
    plt.clf()
    fig, axes = plt.subplots(nrows=1, ncols=len(net), sharey=True, figsize=(12, 3))

    flierprops = dict(marker='x', markersize=4)
    medianprops = dict(linestyle='-.', linewidth=2.5, color='firebrick')
    meanlineprops = dict(linestyle='--', linewidth=2.5, color='purple')

    for n in range(len(net)):
        for s in range(len(slt)):
            _local = __tmp[(__tmp.net == net[n]) & (__tmp.solution == slt[s])]
            print _local
            del _local['app']
            del _local['net']
            del _local['solution']
            _local.columns = [slt[s], 'seed', 'vertex']
            _local = _local.set_index(['vertex', 'seed'])

            if _global[n].__len__() > 0:
                _global[n] = pd.merge(_global[n], _local, left_index=True, right_index=True)
            else:
                _global[n] = _local

        _global[n].boxplot(ax=axes[n], medianprops=medianprops, flierprops=flierprops,
                           meanprops=meanlineprops, meanline=False,
                           showmeans=False)
        axes[n].title.set_text('Net' + str(n + 1))

    for ax in axes.flatten():
        ax.set_yscale('log')
        ax.set_xlabel(xlabel)
        if rotate:
            for tick in ax.get_xticklabels():
                tick.set_rotation(90)
                tick.set_fontsize(8)

    axes[0].set_ylabel(ylabel, fontsize=11)
    plt.suptitle('')
    plt.grid(linestyle='dotted')
    fig.subplots_adjust(hspace=0.01)
    plt.suptitle('')
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig('all_net_' + title + '.pdf')
    plt.show()
    # plt.savefig('all_net_' + title + '.pdf')

    plt.cla()
    plt.clf()


def bar_plot_all(data, slt, filter=[], title=[], ylabel='', xlabel='', time=0, savefig=False, detail=False):
    def prepare_dataframe(app=[], filter_extension=[]):
        def data_filtering(_means, _errors, filter_t='', solution='', extend=''):
            _tmp = filter_fields(data, filter_t, time, mean=False)
            _tmp = _tmp[(_tmp.solution == solution) & ((_tmp.app == app[0]) | (_tmp.app == app[1]))]
            _tmp['label'] = _tmp['app'] + '_' + _tmp['vertex'] + '_src_' + _tmp['src'].apply(str)
            _filtered = _tmp[['label', 'value']].copy()
            _filtered.columns = ['label', solution + ' ' + extend]
            _filtered = _filtered.set_index('label')
            __global = _filtered.groupby(level=('label'))
            if _means.empty:
                _means = __global.mean()
                _errors = __global.std()
            else:
                _means = pd.merge(_means, __global.mean(), left_index=True, right_index=True)
                _errors = pd.merge(_errors, __global.std(), left_index=True, right_index=True)

            # print _means
            return _means, _errors

        means = [pd.DataFrame() for j in range(len(filter))]
        errors = [pd.DataFrame() for j in range(len(filter))]

        for f in range(len(filter)):
            for s in range(len(slt)):
                means[f], errors[f] = data_filtering(_means=means[f], _errors=errors[f], filter_t=filter[f],
                                                     solution=slt[s],
                                                     extend='')
        return means, errors

    def plot_data(app_small=[], app_large=[], _means_small=[], _means_large=[], _std_small=[], _std_large=[],
                  _styles=[]):
        plt.cla()
        plt.clf()

        fig, axes = plt.subplots(nrows=1, ncols=4, sharey=True, figsize=(12, 4))
        pos = [0.5, 4]
        xtext = []
        _title = []

        # Large instances
        for f in range(len(filter)):
            _means_large[f].plot.bar(ax=axes[f], log=True)  # yerr=_std_large[f]
            _title.append(title[f])

        # Small instances
        for f in range(len(filter)):
            x = f + 2
            _means_small[f].plot.bar(ax=axes[x], log=True)  # yerr=_std_small[f]
            _title.append(title[f])

        x = 0
        for ax in axes.flatten():
            for tick in ax.get_xticklabels():
                ll = tick.get_text().replace("a1_", "")
                ll = tick.get_text().replace("a2_", "")
                ll = tick.get_text().replace("a3_", "")
                ll = tick.get_text().replace("a4_", "")
                ll = ll.replace("a1_", "")
                ll = ll.replace("a2_", "")
                ll = ll.replace("a3_", "")
                ll = ll.replace("a4_", "")
                ll = ll.replace(")", "")
                ll = ll.replace("(", "_")
                xtext.append(ll)

            ax.set_xticklabels([i for i in xtext])

            # Larg
            # ymin, ymax = ax.get_ylim()
            # for a in range(len(app_large)):
            #     pos_text = pos[a] + 0.5
            #     ax.text(pos_text, ymax - ymax * .8, app_large[a],
            #                  horizontalalignment='center', fontsize=12, transform=ax.transData)
            #
            # for a in range(len(app_small)):
            #     pos_text = pos[a] + 0.5
            #     ax.text(pos_text, ymax - ymax * .8, app_small[a],
            #                  horizontalalignment='center', fontsize=12, transform=ax.transData)

            if x < 2:
                ax.axvline(x=8.5, linewidth=1.5)
            else:
                ax.axvline(x=2.5, linewidth=3)
            ax.set_xlabel('')
            ax.set_ylabel('')
            ax.set_title(_title[x])
            x = x + 1

        axes[0].set_ylabel(ylabel)
        plt.suptitle('')
        plt.tight_layout()

        # plt.savefig('group_a3_a4_bars.pdf')
        plt.show()
        plt.cla()
        plt.clf()

    mean_large, std_large = prepare_dataframe(app=['a1', 'a2'])
    mean_small, std_small = prepare_dataframe(app=['a3', 'a4'])

    plot_data(app_large=['a1', 'a2'], app_small=['a3', 'a4'], _means_large=mean_large, _means_small=mean_small,
              _std_large=std_large, _std_small=std_small)


def _bar_plot_place(data, slt, filter=[], title=[], ylabel='', xlabel='', time=0, savefig=False, detail=False):
    def prepare_dataframe(app=[], filter_extension=[]):
        def data_filtering(_means, _errors, filter_t='', solution='', extend=''):
            _tmp = filter_fields(data, filter_t, time, mean=False, place=True)
            _tmp = _tmp[(_tmp.solution == solution) & ((_tmp.app == app[0]) | (_tmp.app == app[1]))]
            _tmp['instance'] = 0
            _tmp.loc[(_tmp['app'] == 'a1'), 'instance'] = 1
            _tmp.loc[(_tmp['app'] == 'a2'), 'instance'] = 1
            # _tmp['value'] = _tmp['value'] * float(1000.0)
            # print _tmp
            # _tmp['norm'] = (_tmp['value'] - _tmp['value'].mean()) / _tmp['value'].std()
            # _tmp = _tmp[(_tmp.norm > -2) & (_tmp.norm < 2)]
            # del _tmp['norm']

            _tmp['label'] = _tmp['instance'].apply(str) + '_' + _tmp['place'].apply(str)
            _filtered = _tmp[['label', 'value']].copy()
            if (solution == 'Cloud'):
                _filtered.columns = ['label', solution + '-only ' + extend]
            else:
                _filtered.columns = ['label', solution + ' ' + extend]

            _filtered = _filtered.set_index('label')
            __global = _filtered.groupby(level=('label'))
            if _means.empty:
                _means = __global.median()
                _errors = __global.std()
            else:
                _means = pd.merge(_means, __global.median(), left_index=True, right_index=True)
                _errors = pd.merge(_errors, __global.std(), left_index=True, right_index=True)

            return _means, _errors

        means = [pd.DataFrame() for j in range(len(filter))]
        errors = [pd.DataFrame() for j in range(len(filter))]

        for f in range(len(filter)):
            for s in range(len(slt)):
                means[f], errors[f] = data_filtering(_means=means[f], _errors=errors[f], filter_t=filter[f],
                                                     solution=slt[s],
                                                     extend='')
        return means, errors

    def plot_data(app_small=[], app_large=[], _means_small=[], _means_large=[], _std_small=[], _std_large=[],
                  _styles=[]):
        plt.cla()
        plt.clf()

        fig, axes = plt.subplots(nrows=1, ncols=4, sharey=True, sharex=False, figsize=(14, 4))
        pos = [0.5, 4]
        xtext = []
        _title = []

        # Large instances
        hatches = []
        for f in range(len(filter)):
            _means_large[f].plot(kind='bar', ax=axes[f], log=True, alpha=.65, )  #
            _title.append(title[f])
            for container, hatch in zip(axes[f].containers, ("o", "\\\\", "*", ".")):
                for patch in container.patches:
                    patch.set_hatch(hatch)

        # Small instances
        for f in range(len(filter)):
            x = f + 2
            _means_small[f].plot(kind='bar', ax=axes[x], log=True, alpha=.65)  # , yerr=_std_small[f]
            _title.append(title[f])
            for container, hatch in zip(axes[x].containers, ("o", "\\\\", "*", ".")):
                for patch in container.patches:
                    patch.set_hatch(hatch)

        x = 0
        for ax in axes.flatten():
            for tick in ax.get_xticklabels():
                if tick.get_text().find('_0') > 0:
                    xtext.append('Edge')
                else:
                    xtext.append('Cloud')

            ax.set_xticklabels([i for i in xtext], fontsize=14)

            # Larg
            # ymin, ymax = ax.get_ylim()
            # for a in range(len(app_large)):
            #     pos_text = pos[a] + 0.5
            #     ax.text(pos_text, ymax - ymax * .8, app_large[a],
            #                  horizontalalignment='center', fontsize=12, transform=ax.transData)
            #
            # for a in range(len(app_small)):
            #     pos_text = pos[a] + 0.5
            #     ax.text(pos_text, ymax - ymax * .8, app_small[a],
            #                  horizontalalignment='center', fontsize=12, transform=ax.transData)

            # ax.axvline(x=0.5, linewidth=3)
            ax.set_xlabel('')
            ax.set_ylabel('')

            ax.set_title(_title[x], fontsize=16)
            x = x + 1

        for a in range(len(axes)):
            if a == 0:
                axes[0].set_ylabel(ylabel, fontsize=14)

            axes[a].legend().set_visible(False)
            if a == 3:
                axes[a].legend().set_visible(True)
                axes[a].legend(fontsize=14)

        ymin, ymax = axes[0].get_ylim()
        axes[0].text(1, ymax + ymax*2.5, 'Large Applications',
                     horizontalalignment='left', fontsize=16, transform=axes[0].transData)

        axes[2].text(1, ymax + ymax * 2.5, 'Small Applications',
                     horizontalalignment='left', fontsize=16, transform=axes[2].transData)

        axes[1].text(.5, ymin - ymin * .98, 'Sink Placement (Infrastructure)',
                     horizontalalignment='left', fontsize=16, transform=axes[1].transData)
        plt.suptitle('')


        plt.subplots_adjust(bottom=.25, left=.099, right=.99, top=.85, hspace=.02)
        # plt.tight_layout()
        plt.savefig('bar_times_all.pdf')
        # plt.show()
        plt.cla()
        plt.clf()

    mean_large, std_large = prepare_dataframe(app=['a1', 'a2'])
    mean_small, std_small = prepare_dataframe(app=['a3', 'a4'])
    print 'Mean Large - Communication'
    print mean_large[0]

    print 'Std Large - Communication'
    print std_large[0]

    print 'Mean Large - Computation'
    print mean_large[1]

    print 'Std Large - Computation'
    print std_large[1]

    print 'Mean Small - communication'
    print mean_small[0]

    print 'Std Small - communication'
    print std_small[0]

    print 'Mean Small - commputation'
    print mean_small[1]

    print 'std Small - commputation'
    print std_small[1]

    plot_data(app_large=['a1', 'a2'], app_small=['a3', 'a4'], _means_large=mean_large, _means_small=mean_small,
              _std_large=std_large, _std_small=std_small)


def bar_plot_communication(data, slt, filter=[], title=[], ylabel='', xlabel='', time=0, savefig=False, detail=False):
    def prepare_dataframe(app=[]):
        def data_filtering(_means=pd.DataFrame(), _errors=pd.DataFrame(), solution='', extend=''):
            _tmp = filter_fields(data, 'totalCommTime:mean', time, mean=False)
            _tmp = _tmp[(_tmp.solution == solution) & (_tmp.net == 'n3')]
            print _tmp
            _tmp.loc[(_tmp['app'] == 'a1'), 'app'] = 'App1'
            _tmp.loc[(_tmp['app'] == 'a2'), 'app'] = 'App2'
            _tmp.loc[(_tmp['app'] == 'a3'), 'app'] = 'App3'
            _tmp.loc[(_tmp['app'] == 'a4'), 'app'] = 'App4'
            _tmp['value'] = _tmp['value'] * 1000
            _tmp['label'] = _tmp['app'] + '-' + _tmp['src'].apply(str) + '->' + _tmp['vertex']
            # _tmp['norm'] = (_tmp['value'] - _tmp['value'].mean()) / _tmp['value'].std()
            # _tmp = _tmp[(_tmp.norm > -2) & (_tmp.norm < 2)]
            # del _tmp['norm']

            _filtered = _tmp[['label', 'value']].copy()
            _filtered.columns = ['label', solution + ' ' + extend]
            _filtered = _filtered.set_index('label')
            __global = _filtered.groupby(level=('label'))
            if _means.__len__() < 1:
                _means = __global.median()
                _errors = __global.std()
            else:
                _means = pd.merge(_means, __global.median(), left_index=True, right_index=True)
                _errors = pd.merge(_errors, __global.std(), left_index=True, right_index=True)

            # print _means
            return _means, _errors

        means = [pd.DataFrame() for j in range(len(filter))]
        errors = [pd.DataFrame() for j in range(len(filter))]

        for s in range(len(slt)):
            if s == 0 or s == 3:
                means, errors = data_filtering(_means=means, _errors=errors, solution=slt[s], extend='')

        return means, errors

    def plot_data(_means_large=[], _std_large=[],
                  _styles=[]):
        plt.cla()
        plt.clf()

        axes = plt.gca()
        _styles = ['^', 's']
        # Large instances

        print _means_large

        print _std_large
        _aux_means = _means_large.reset_index()
        _aux_std = _std_large.reset_index()

        _aux = _aux_means.loc[:, ['label']]
        labels = _aux.values

        _aux = _aux_means.iloc[:, 1]
        cloud_values = _aux.values

        _aux = _aux_means.iloc[:, 2]
        rtr_values = _aux.values

        _aux = _aux_std.iloc[:, 1]
        cloud_std = _aux.values

        _aux = _aux_std.iloc[:, 2]
        rtr_std = _aux.values

        # _means_large.plot(ax=axes, logy=True, style=_styles, yerr=_std_large)  #yerr=_std_large
        axes.errorbar(_means_large.index, cloud_values, yerr=cloud_std, color='red', ls='', marker='o', capsize=5,
                      markersize=8, capthick=1, ecolor='red', label='Cloud')
        axes.errorbar(_means_large.index, rtr_values, yerr=rtr_std, color='blue', ls='', marker='^', markersize=8,
                      capsize=5, alpha=.5,
                      capthick=1, ecolor='blue', label='RTR+RP')
        axes.set_xticks(np.arange(len(_means_large.index)))
        axes.set_xticklabels(["%s" % item for item in _means_large.index.tolist()], rotation=90, fontsize=14)

        axes.set_xlabel('Paths from sources to sinks (source->sink)', fontsize=14)
        axes.set_ylabel('Communication Response Time (ms)', fontsize=14)
        ymin, ymax = axes.get_ylim()
        plt.axvline(x=8.5, linewidth=1, color='k', linestyle='dashed')
        axes.text(3.5, ymax * 1.02, 'App1',
                  horizontalalignment='center', fontsize=16, transform=axes.transData)

        plt.axvline(x=16.5, linewidth=1, color='k', linestyle='dashed')
        axes.text(12.5, ymax * 1.02, 'App2',
                  horizontalalignment='center', fontsize=16, transform=axes.transData)

        plt.axvline(x=19.5, linewidth=1, color='k', linestyle='dashed')
        axes.text(18, ymax * 1.02, 'App3',
                  horizontalalignment='center', fontsize=16, transform=axes.transData)

        axes.text(22, ymax * 1.02, 'App4',
                  horizontalalignment='center', fontsize=16, transform=axes.transData)

        xtext = []
        for tick in axes.get_xticklabels():
            ll = tick.get_text().replace("App1-", "")
            ll = ll.replace("App2-", "")
            ll = ll.replace("App3-", "")
            ll = ll.replace("App4-", "")
            xtext.append(ll)

        axes.set_xticklabels([i for i in xtext])
        for tick in axes.get_yticklabels():
            tick.set_fontsize(14)

        plt.legend(fontsize=14)
        plt.suptitle('')
        # plt.tight_layout()
        plt.subplots_adjust(bottom=.3, left=.099, right=.99, top=.95, hspace=.02)
        plt.savefig('bar_communication_large.pdf')
        # plt.show()

        plt.cla()
        plt.clf()

    mean_large, std_large = prepare_dataframe(app=['a1', 'a2'])

    plot_data(_means_large=mean_large,
              _std_large=std_large)


def _cdf_art(data=[], filter=[], net=[], app=[], title='', ylabel='', xlabel='', slt=[], rotate=False, time=0,
             base_time=False,
             mean=False, savefig=False):
    plt.cla()
    plt.clf()
    titles = ["AppA", "AppB", "AppC", "AppD"]

    fig, axes = plt.subplots(nrows=1, ncols=4, sharey=True, sharex=True, figsize=(14, 4))

    __tmp = filter_fields(data, filter, time, base_time, mean)

    # __tmp = __tmp[(__tmp.sink1 == 'cloudlet2') & (__tmp.sink2 == 'cloud')]
    # __tmp = __tmp[(__tmp.placement == 'cloudlet2')]
    # #
    # __tmp = __tmp[(__tmp.bytes == bytes[2])]
    # __tmp = __tmp[(__tmp.arrival == bytes[2])]
    # print __tmp

    _global = [[] for j in range(len(slt))]
    lines = ['-', '-.', ':', '--']
    # fig, ax = plt.subplots(figsize=(8, 4))
    for a in range(len(app)):
        _app = __tmp[(__tmp.app == app[a])]
        # print 'Solution ' + app[a] + ' size ' + str(_app.count())
        # if a == 0:
        for s in range(len(slt)):
            if (slt[s] == 'Cloud-only'):
                _local = _app[(_app.solution == 'Cloud')]
            else:
                _local = _app[(_app.solution == slt[s])]
            # _local.to_csv('./' + slt[s].title() + '.csv', sep='\t')
            # _local['norm'] = (_local['value'] - _local['value'].mean()) / _local['value'].std()
            # _aux = _local[(_local.norm > -1) & (_local.norm < 1)]
            _local['value'] = _local['value'] * 1000
            _aux = _local.loc[:, ['app', 'net', 'vertex', 'value']]
            # print _aux
            # _aux['size'] = _aux.groupby(['app', 'net', 'vertex']).transform(np.size)
            # _aux = _aux.drop_duplicates(['app', 'net', 'vertex', 'size'])
            # print slt[s]
            # print _aux

            _aux = _local.loc[:, ['value']]
            # result = _aux.sort_values(by=['value'])
            # if s==0 :
            #     _aux = result[(result.value > 0.005797)]
            # print _aux
            _global[s] = _aux.values
            # print slt[s] + ' ' + str(_aux['value'].count())
            n, bins, patches = axes[a].hist(_global[s], 3000, normed=1, histtype='step',
                                            cumulative=True, label=slt[s], linestyle=lines[s], linewidth=2.5)

            axes[a].grid(linestyle='dotted')
            axes[a].set_title(titles[a], fontsize=16)
            axes[a].set_xlabel('Response Time (ms)', fontsize=14)
            if (a == 0):
                axes[a].set_ylabel('CDF', fontsize=14)
            if (a == 3):
                axes[a].legend(loc='lower right', fontsize=14)

    for ax in axes.flatten():
        # ax.set_xscale('log')
        for tick in ax.get_xticklabels():
            tick.set_fontsize(14)
        for tick in ax.get_yticklabels():
            tick.set_fontsize(14)
    plt.tight_layout()
    plt.savefig('cdf_sc1.pdf')
    # plt.show()

def resume_placements(data_cfg=[]):
    print data_cfg
    _tmp = data_cfg[(data_cfg.solution == 'RTR+RP') & (data_cfg.app == 'a1') & (
        # (data_cfg.task == 6) | (data_cfg.task == 3) | (data_cfg.task == 1) | (data_cfg.task == 2)
            (data_cfg.task == 22) | (data_cfg.task == 17) | (data_cfg.task == 18) | (data_cfg.task == 23)
    )]
    print _tmp.groupby(['infrastructure', 'app']).size()

    _tmp = data_cfg[(data_cfg.solution == 'RTR+RP') & (data_cfg.app == 'a2') & (
        # (data_cfg.task == 13) | (data_cfg.task == 20) | (data_cfg.task == 0)
            (data_cfg.task == 21) | (data_cfg.task == 19) | (data_cfg.task == 23) | (data_cfg.task == 24)
    )]
    print _tmp.groupby(['infrastructure', 'app']).size()

    _tmp = data_cfg[(data_cfg.solution == 'RTR+RP') & (data_cfg.app == 'a3') & (
        # (data_cfg.task == 0) | (data_cfg.task == 4)
            (data_cfg.task == 9) | (data_cfg.task == 8)
    )]
    print _tmp.groupby(['infrastructure', 'app']).size()

    _tmp = data_cfg[(data_cfg.solution == 'RTR+RP') & (data_cfg.app == 'a4') & (
        # (data_cfg.task == 2) | (data_cfg.task == 1) | (data_cfg.task == 0)
            (data_cfg.task == 9) | (data_cfg.task == 7)
    )]
    print _tmp.groupby(['infrastructure', 'app']).size()


if __name__ == '__main__':
    app = ["a1", "a2", "a3", "a4"]
    # net = ["n1", "n2"]
    net = ["n3"]

    # app = ["a1"]
    # net = ["n1"]

    slt = ["cloud", "lb", "rv", "rvrs"]
    label = ["Cloud-only", "LB", "RTR", "RTR+RP"]

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    parser.add_argument('-t', '--time',
                        help='experimets time')

    args = parser.parse_args()
    fig = False

    if args.directory:

        data_dir = []
        for root, dirs, files in os.walk(args.directory):
            for file in files:
                if file.endswith('.csv'):
                    data_dir.append(root)
        data_dir = np.unique(data_dir)
        print data_dir
        #
        data = pd.read_csv(args.directory + '/data.csv', sep='\t', index_col=0)
        data_src = pd.read_csv(args.directory + '/data_src.csv', sep='\t', index_col=0)
        data_cfg = pd.read_csv(args.directory + '/data_cfg.csv', sep='\t', index_col=0)

        # resume_placements(data_cfg= data_cfg)

        # data, data_src, data_cfg = struct_dataset(app, net, slt, label, _dirs = data_dir)
        # data.to_csv(args.directory +'/data.csv', sep='\t')
        # data_src.to_csv(args.directory +'/data_src.csv', sep='\t')
        # data_cfg.to_csv(args.directory +'/data_cfg.csv', sep='\t')
        # print data
        os.chdir(args.directory)
        #
        # bar_plot(data=data_src, slt=label, filter='totalTime:mean', title='Total', ylabel='Seconds',
        #          savefig=fig, detail=True)
        # bar_plot_art(data=data_src, slt=label, filter=['totalCommTime:mean','totalCompTime:mean'], ylabel='Seconds',
        #              title=['Communication','Computation'], savefig=fig)
        #
        # bar_plot_all(data=data_src, slt=label, filter=['totalCommTime:mean', 'totalCompTime:mean'], ylabel='Seconds',
        #              title=['Communication','Computation'], savefig=fig)
        #
        # bar_plot_art_weight(data=data_src, slt=label, filter=['totalCommTime:mean', 'totalCompTime:mean'], ylabel='Seconds',
        #              title=['Communication','Computation'], savefig=fig)
        # bar_plot(data=data_src, slt=label, filter='totalCompTime:mean', title='Computation', ylabel='Seconds',
        #          savefig=fig)
        #
        # scatter_plot(data_cfg, label, fig)
        #
        # group = ['solution']
        # _box_plot_art(data, 'totalTime:mean', net, app, 'rt_g', 'Seconds', '', group, slt=label, savefig=fig, ind=True)
        # _box_plot_art(data, 'queueLengthTopic:last', net, app, 'th_g', 'Messages/Second', '', group, slt=label,
        #           time=args.time, savefig=fig, ind=True)
        #
        # _box_plot(data, 'totalTime:mean', net, app, 'rt_g', 'Seconds', '', group, slt=label, savefig=fig, ind=True)
        # _box_plot(data, 'queueLengthTopic:last', net, app, 'th_g', 'Messages/Second', '', group, slt=label,  time=args.time,
        #          base_time=True, savefig=fig, ind=True)
        # _agg_speedup_plot(data, 'totalTime:mean', label, 'rt', 'Seconds', '', savefig=fig)
        #
        # group = ['vertex', 'solution']
        # _box_plot(data, 'totalTime:mean', net, app, 'rt', 'Seconds', '', group, slt=label, rotate=True, savefig=fig,
        #           ind=True)
        # _box_plot(data, 'queueLengthTopic:last', net, app, 'th', 'Messages/Second', '', group, slt=label, rotate=True,
        #           time=args.time, base_time=True, savefig=fig, ind=True)

        # axis=['totalCompTime:stats', 'totalCommTime:stats', 'totalTime:mean']
        # plot_3d(data, axis, net, app)

        # _box_plot_art_app(data=data, filter='totalTime:mean', ylabel='Total Response Time (ms)', net=net, app=app, slt=label, title='rt')
        # _box_plot_art_net(data=data, filter='totalTime:mean', net=net, app=app, slt=label, title='rt')
        # # #
        # _box_plot_art_app(data=data, filter='queueLengthTopic:last', net=net, app=app, slt=label, time=int(args.time),
        #                   base_time=True, title='th', ylabel='Average Throughput (Messages/Second)')
        # _box_plot_art_net(data=data, filter='queueLengthTopic:last', net=net, app=app, slt=label, time=int(args.time),
        #                   base_time=True, title='th')
        #
        #
        # _bar_plot_place(data=data, slt=label, filter=['totalCommTime:mean', 'totalCompTime:mean'],
        #                 ylabel='Response Time (seconds)',
        #                 title=['Communication', 'Computation'], savefig=fig)
        #
        # bar_plot_communication(data=data_src, slt=label, ylabel='ms',
        #                        title=['Communication', 'Communication'], savefig=fig)

        _cdf_art(data=data, filter='totalTime:mean', net=net, app=app, slt=label, title='rt')



    else:
        print 'The directory was not set up'

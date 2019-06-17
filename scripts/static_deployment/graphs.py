import inline as inline
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import math
import os
import argparse


def parse_if_number(s):
    try:
        return float(s)
    except:
        return True if s == "true" else False if s == "false" else s if s else None


def parse_ndarray(s):
    return np.fromstring(s, sep=' ') if s else None


def open_file(s):
    tmp = pd.read_csv(s)
    tmp.head()
    tmp.module.describe()
    tmp.module.unique()
    tmp.value.describe()
    tmp.describe()
    tmp.dtypes
    del tmp['attrname']
    del tmp['attrvalue']
    del tmp['run']
    return tmp


def print_view(s, file):
    tmp = s[(s.type == 'scalar')
            & (s.name == 'queueLengthTopic:last')]
    tmp.head()
    tmp.to_csv(file, sep='\t')
    # print(tmp)


def save_overview(cloud, ovg, rw, ovrs, lb):
    tmp_cloud = cloud[(cloud.type == 'scalar')]
    tmp_cloud.head()

    tmp_ovg = ovg[(ovg.type == 'scalar')]
    tmp_ovg.head()

    tmp_rw = rw[(rw.type == 'scalar')]
    tmp_rw.head()

    tmp_ovrs = ovrs[(ovrs.type == 'scalar')]
    tmp_ovrs.head()

    tmp_lb = lb[(lb.type == 'scalar')]
    tmp_lb.head()

    result = prepare_result(tmp_cloud, tmp_ovg, tmp_rw, tmp_ovrs, tmp_lb)
    result.to_csv('general_view.csv', sep='\t')


def prepare_file(sinks, s, timeEv, filter):
    general = s[(s.type == 'scalar')
                & (s.name == filter)]

    tmp = general[(general.module == 'DynamicDeployment.vertex-' + str(int(sinks[0])))]

    if len(sinks) > 1:
        for ss in range(len(sinks)):
            if ss > 0:
                tmp2 = general[(general.module == 'DynamicDeployment.vertex-' + str(int(sinks[ss])))]
                tmp = pd.concat([tmp, tmp2], axis=0)

    if filter == 'queueLengthTopic:last':
        tmp['value'] = tmp.value / float(timeEv)

    tmp = tmp[pd.notnull(tmp['value'])]
    return tmp


def prepare_result(cloud, ovg, rw, ovrs, lb):
    tmp = pd.merge(cloud, ovg, how='left', on=['type', 'module', 'name'])
    tmp = pd.merge(tmp, rw, how='left', on=['type', 'module', 'name'])
    tmp = pd.merge(tmp, ovrs, how='left', on=['type', 'module', 'name'])
    tmp = pd.merge(tmp, lb, how='left', on=['type', 'module', 'name'])
    tmp.columns = ['type', 'module', 'name',
                   'cloud_value',
                   'cloud_count',
                   'cloud_sumweights',
                   'cloud_mean',
                   'cloud_stddev',
                   'cloud_min',
                   'cloud_max',
                   'ovg_value',
                   'ovg_count',
                   'ovg_sumweights',
                   'ovg_mean',
                   'ovg_stddev',
                   'ovg_min',
                   'ovg_max',
                   'rw_value',
                   'rw_count',
                   'rw_sumweights',
                   'rw_mean',
                   'rw_stddev',
                   'rw_min',
                   'rw_max',
                   'ovrs_value',
                   'ovrs_count',
                   'ovrs_sumweights',
                   'ovrs_mean',
                   'ovrs_stddev',
                   'ovrs_min',
                   'ovrs_max',
                   'lb_value',
                   'lb_count',
                   'lb_sumweights',
                   'lb_mean',
                   'lb_stddev',
                   'lb_min',
                   'lb_max']
    tmp = tmp.set_index('module')
    return tmp


def print_graph(time, sinks, cloud, ovg, rw, ovrs, lb):
    group = ['totalTime:mean', 'queueLengthTopic:last']
    _tmp_cloud = [pd.DataFrame() for j in range(len(group))]
    _tmp_ovg = [pd.DataFrame() for j in range(len(group))]
    _tmp_rw = [pd.DataFrame() for j in range(len(group))]
    _tmp_ovrs = [pd.DataFrame() for j in range(len(group))]
    _tmp_lb = [pd.DataFrame() for j in range(len(group))]
    _result = [pd.DataFrame() for j in range(len(group))]

    for gp in range(len(group)):
        _tmp_cloud[gp] = prepare_file(sinks, cloud, time, group[gp])
        _tmp_ovg[gp] = prepare_file(sinks, ovg, time, group[gp])
        _tmp_rw[gp] = prepare_file(sinks, rw, time, group[gp])
        _tmp_ovrs[gp] = prepare_file(sinks, ovrs, time, group[gp])
        _tmp_lb[gp] = prepare_file(sinks, lb, time, group[gp])
        _result[gp] = prepare_result(_tmp_cloud[gp], _tmp_ovg[gp], _tmp_rw[gp], _tmp_ovrs[gp], _tmp_lb[gp])
        _result[gp].to_csv(group[gp] + '_view.csv', sep='\t')
        del _result[gp]['cloud_count']
        del _result[gp]['cloud_sumweights']
        del _result[gp]['cloud_mean']
        del _result[gp]['cloud_stddev']
        del _result[gp]['cloud_min']
        del _result[gp]['cloud_max']

        del _result[gp]['rw_count']
        del _result[gp]['rw_sumweights']
        del _result[gp]['rw_mean']
        del _result[gp]['rw_stddev']
        del _result[gp]['rw_min']
        del _result[gp]['rw_max']

        del _result[gp]['ovg_count']
        del _result[gp]['ovg_sumweights']
        del _result[gp]['ovg_mean']
        del _result[gp]['ovg_stddev']
        del _result[gp]['ovg_min']
        del _result[gp]['ovg_max']

        del _result[gp]['ovrs_count']
        del _result[gp]['ovrs_sumweights']
        del _result[gp]['ovrs_mean']
        del _result[gp]['ovrs_stddev']
        del _result[gp]['ovrs_min']
        del _result[gp]['ovrs_max']

        del _result[gp]['lb_count']
        del _result[gp]['lb_sumweights']
        del _result[gp]['lb_mean']
        del _result[gp]['lb_stddev']
        del _result[gp]['lb_min']
        del _result[gp]['lb_max']

        _result[gp][['cloud_value', 'ovg_value', 'rw_value', 'ovrs_value', 'lb_value']] = _result[gp][
            ['cloud_value', 'ovg_value', 'rw_value', 'ovrs_value', 'lb_value']].astype(float)
        # print(_result[gp])

    fig = plt.figure()  # Create matplotlib figure

    ax = fig.add_subplot(111)  # Create matplotlib axes
    ax2 = ax.twinx()  # Create another axes that shares the same x-axis as ax.

    width = 0.4

    _result[0].plot(kind='bar', ax=ax, position=1, color=['darkblue', 'mediumblue', 'royalblue', 'steelblue', 'skyblue'])
    _result[1].plot(kind='bar', ax=ax2, position=0, color=['#9f6060', '#e61919', '#ff4d4d', '#ff8080', '#ffcccc'])

    ax.set_ylabel('Seconds')
    ax2.set_ylabel('Messages/seconds')

    ax.set_yscale('symlog', linthreshy=0.015)
    ax.set_xlabel('')
    ax2.set_yscale('symlog', linthreshy=0.015)
    ax2.set_xlabel('')
    plt.title('Response Time and Throughput')

    plt.savefig('rtxth.png')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    parser.add_argument('-s', '--sinks',
                        help='file with sinks')

    parser.add_argument('-t', '--time',
                        help='time evaluated in seconds')

    args = parser.parse_args()

    if args.directory:
        os.chdir(args.directory)
        cloud = open_file('./cloud.csv')
        ovg = open_file('./rv.csv')
        rw = open_file('./rw.csv')
        ovrs = open_file('./rvrs.csv')
        lb = open_file('./lb.csv')

        file = open(args.sinks, 'r')
        sink_list = file.readlines()

        print_graph(args.time,
                    sink_list,
                    cloud,
                    ovg,
                    rw,
                    ovrs,
                    lb)
    else:
        print 'The directory was not set up'

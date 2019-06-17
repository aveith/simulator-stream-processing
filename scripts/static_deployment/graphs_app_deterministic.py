import argparse
import os
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def struct_dataset(app, net, slt, label, _dirs=[], bytes=[], arrival=[], rates=[]):
    data = pd.DataFrame()
    data_src = pd.DataFrame()
    data_cfg = pd.DataFrame()

    for fld in _dirs:
        tmp, tmp_src, tmp_cfg = open_file(fld, app, net, slt, label, bytes=bytes, arrival=arrival, rates=rates)

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

def define_parameters(fld, app=[], net=[], bytes=[], arrival=[], rates=[]):
    _net = 'n3'
    _app = '-1'
    _bytes = '-1'
    _arrival = '-1'
    _select = '-1'
    _data_rate = '-1'
    _sink1 = ''
    _sink2 = ''
    _sinks = []

    sinks_1 = [5, 9, 8, 7]
    sinks_2 = [10, 9, 8]

    for a in app:
        if fld.find(a) != -1:
            _app = a
            break

    for n in net:
        if fld.find(n) != -1:
            _net = n
            break

    for b in bytes:
        if fld.find('bytes_' + str(b)) != -1:
            _bytes = b
            break

    for a in arrival:
        if fld.find('arr_' + str(a)) != -1:
            _arrival = a
            break

    for r in rates:
        if fld.find('rate_' + r) != -1:
            _data_rate = r
        if fld.find('selec_' + r) != -1:
            _select = r

    for s in sinks_1:
        if _app == app[0]:
            pattern = ''
        else:
            pattern = '_'

        if fld.find('_' + str(s) + '9' + pattern) != -1:
            _sinks.append([s,'cloudlet1'])
            _sink1 = 'cloudlet1'

        if fld.find('_' + str(s) + '14' + pattern) != -1:
            _sinks.append([s, 'cloudlet1'])
            _sink1 = 'cloudlet1'

        if fld.find('_' + str(s) + '15' + pattern) != -1:
            _sinks.append([s, 'cloudlet1'])
            _sink1 = 'cloudlet1'

        if fld.find('_' + str(s) + '24' + pattern) != -1:
            _sinks.append([s, 'cloudlet2'])
            _sink1 = 'cloudlet2'

        if fld.find('_' + str(s) + '25' + pattern) != -1:
            _sinks.append([s, 'cloudlet2'])
            _sink1 = 'cloudlet2'

        if fld.find('_' + str(s) + '10' + pattern) != -1:
            _sinks.append([s, 'cloud'])
            _sink1 = 'cloud'

    if _app != app[0]:
        for s in sinks_2:
            if fld.find('_' + str(s) + '9', len(fld)-5) != -1:
                _sinks.append([s, 'cloudlet1'])
                _sink2 = 'cloudlet1'

            if fld.find('_' + str(s) + '14', len(fld)-5) != -1:
                _sinks.append([s, 'cloudlet1'])
                _sink2 = 'cloudlet1'

            if fld.find('_' + str(s) + '15', len(fld)-5) != -1:
                _sinks.append([s, 'cloudlet1'])
                _sink2 = 'cloudlet1'

            if fld.find('_' + str(s) + '24', len(fld)-5) != -1:
                _sinks.append([s, 'cloudlet2'])
                _sink2 = 'cloudlet2'

            if fld.find('_' + str(s) + '25', len(fld)-5) != -1:
                _sinks.append([s, 'cloudlet2'])
                _sink2 = 'cloudlet2'

            if fld.find('_' + str(s) + '10', len(fld)-5) != -1:
                _sinks.append([s, 'cloud'])
                _sink2 = 'cloud'

    _seed = fld[fld.find('_exp_') + 5: fld.find('_') + (fld.find('_n') - fld.find('_'))]

    return _net, _app, _bytes, _arrival, _select, _data_rate, _sink1, _sink2, _seed, _sinks

def open_cfg(fld, app, net, slt, label, bytes=[], arrival=[], rates=[]):
    _net, _app, _bytes, _arrival, _select, _data_rate, _sink1, _sink2, _seed, _sinks = define_parameters(fld, app=app,
                                                                                                        net=net,
                                                                                                 bytes=bytes,
                                                                                                 arrival=arrival,
                                                                                                 rates=rates)

    data = pd.DataFrame()

    for s in range(len(slt)):
        _dir = fld + "/" + slt[s] + ".cfg"
        if os.path.isfile(_dir):
            sinks = open(fld + "/" + slt[s] + ".ss", 'r').readlines()
            cloud = open(fld + "/" + slt[s] + ".cc", 'r').readlines()

            tmp = pd.read_csv(_dir, sep=';')
            tmp['app'] = _app
            tmp['net'] = _net
            tmp['bytes'] = _bytes
            tmp['arrival'] = _arrival
            tmp['data_rate'] = _data_rate
            tmp['selectivity'] = _select
            tmp['sink1'] = _sink1
            tmp['sink2'] = _sink2
            tmp['file'] = fld
            tmp['solution'] = label[s]
            tmp['seed'] = _seed
            tmp['sink'] = 0
            tmp['cloud_p'] = 0

            for ss in range(len(sinks)):
                tmp.loc[(tmp['task'] == int(sinks[ss])), ['sink']] = 1

            for cc in range(len(cloud)):
                tmp.loc[(tmp['host'] == int(cloud[cc])), 'cloud_p'] = 1

            tmp.loc[(tmp['host'] == 10), 'cloud_p'] = 1

            if data.empty:
                data = tmp
            else:
                data = pd.concat([data, tmp], axis=0)

    return data


def open_file(fld, app, net, slt, label, bytes=[], arrival=[], rates=[]):
    data_cfg = open_cfg(fld, app, net, slt, label, bytes=bytes, arrival=arrival, rates=rates)

    data = pd.DataFrame()
    data_src = pd.DataFrame()

    _net, _app, _bytes, _arrival, _select, _data_rate, _sink1, _sink2, _seed, _sinks = define_parameters(fld, app=app,
                                                                                                         net=net,
                                                                                                 bytes=bytes,
                                                                                                 arrival=arrival,
                                                                                                 rates=rates)

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
            tmp['bytes'] = _bytes
            tmp['arrival'] = _arrival
            tmp['data_rate'] = _data_rate
            tmp['selectivity'] = _select
            tmp['sink1'] = _sink1
            tmp['sink2'] = _sink2
            tmp['file'] = fld
            tmp['solution'] = label[s]
            tmp['seed'] = _seed

            # Read General Data
            _tmp = pd.DataFrame()
            for ss in range(len(sinks)):
                _aux = tmp[(tmp.module == 'DynamicDeployment.vertex-' + str(int(sinks[ss])))]
                c = 0
                for cc in range(len(cloud)):
                    if int(cloud[cc]) == int(sinks[ss]):
                        c = 1

                for sink in _sinks:
                    if int(sinks[ss]) == sink[0]:
                        position = sink[1]

                _aux['placement'] = position

                _aux['vertex'] = int(sinks[ss])
                _aux['place'] = c
                # print _aux
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

                    if int(sinks[ss]) == sink[0]:
                        position = sink[1]

                    _aux['placement'] = position

                    _aux_src['vertex'] = int(sinks[ss])
                    _aux_src['place'] = c
                    # print _aux_src

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


def _cdf_art(data=[], filter=[], net=[], app=[], title='', ylabel='', xlabel='', slt=[], rotate=False, time=0,
             base_time=False,
             mean=False, savefig=False, bytes=[]):
    plt.cla()
    plt.clf()
    titles = ["app1", "app2", "app3", "app4"]

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
            n, bins, patches = axes[a].hist(_global[s],  3000, normed=1, histtype='step',
                                       cumulative=True, label=slt[s], linestyle=lines[s], linewidth=2.5)

            axes[a].grid(linestyle='dotted')
            axes[a].set_title(titles[a].title(), fontsize=16)
            axes[a].set_xlabel('Response Time (ms)', fontsize=14)
            if (a==0):
                axes[a].set_ylabel('CDF', fontsize=14)
            if (a==3):
                axes[a].legend(loc='lower right', fontsize=14)

    for ax in axes.flatten():
        # ax.set_xscale('log')
        for tick in ax.get_xticklabels():
            tick.set_fontsize(14)
        for tick in ax.get_yticklabels():
            tick.set_fontsize(14)
    plt.tight_layout()
    plt.savefig('cdf_sc0.pdf')
    # plt.show()


if __name__ == '__main__':
    app = ["app0", "app1", "app2", "app3"]
    net = ["n3"]

    slt = ["cloud", "lb", "rv", "rvrs"]
    label = ["Cloud-only", "LB", "RTR", "RTR+RP"]
    bytes = [10, 50000, 200000]

    arrival_rate = [6, 7, 8, 10, 15,  19, 24, 27, 30, 31, 43, 62, 74, 124, 249, 124999, 137499, 150000, 218749,
                    300000, 374999, 624999, 1249999]

    rates = ["1", "075", "05", "025"]

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

        data = pd.read_csv(args.directory + '/data.csv', sep='\t', index_col=0)
        data_src = pd.read_csv(args.directory + '/data_src.csv', sep='\t', index_col=0)
        data_cfg = pd.read_csv(args.directory + '/data_cfg.csv', sep='\t', index_col=0)

        # data, data_src, data_cfg = struct_dataset(app, net, slt, label, _dirs=data_dir, bytes=bytes,
        #                                           arrival=arrival_rate, rates=rates)
        # data.to_csv(args.directory + '/data.csv', sep='\t')
        # data_src.to_csv(args.directory + '/data_src.csv', sep='\t')
        # data_cfg.to_csv(args.directory + '/data_cfg.csv', sep='\t')

        print data

        os.chdir(args.directory)

        _cdf_art(data=data, filter='totalTime:mean', net=net, app=app, slt=label, title='rt', bytes=bytes)

    else:
        print 'The directory was not configured'

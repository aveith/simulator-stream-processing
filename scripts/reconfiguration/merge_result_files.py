import argparse
import os
import pandas as pd
from multiprocessing import Pool
from shutil import copyfile
import itertools

from pandas import merge


class file_list:
    def __init__(self, files_list=[], id=0, dir='', reconfig_strategy=0):
        self.file_list = files_list
        self.id = id
        self.dir = dir
        self.reconfig_strategy = reconfig_strategy


def split_files(files_list=[], processes=1, dir='', reconfig_strategy=0):
    _process_list = []
    id = 0
    number_files = abs(files_list.__len__() / processes)

    counter = 0
    files = []
    for file in range(len(files_list)):
        add = 0
        if (counter >= number_files - 1):
            if (_process_list.__len__() < processes - 1):
                files.append(files_list[file][0] + '/' + files_list[file][1])
                exp = file_list()
                exp.file_list = files
                exp.id = id
                exp.dir = dir
                exp.reconfig_strategy = reconfig_strategy
                id = id + 1
                counter = 0
                files = []
                _process_list.append(exp)
                add = 1

        if (add == 0):
            files.append(files_list[file][0] + '/' + files_list[file][1])

        counter = counter + 1

    if (files.__len__() > 0):
        exp = file_list()
        exp.file_list = files
        exp.id = id
        exp.dir = dir
        exp.reconfig_strategy = reconfig_strategy
        _process_list.append(exp)

    return _process_list


def merge_files(merge_list=file_list()):
    data = pd.DataFrame()

    for _file in range(len(merge_list.file_list)):
        tmp = pd.read_csv(merge_list.file_list[_file], sep=';', index_col=False)
        direc = merge_list.file_list[_file]
        plac = direc[direc.find('plac_', 0)+5:direc.find('/', direc.find('plac_', 0)+5)]
        tmp['placement'] = plac
        data = data.append(tmp, ignore_index=False)

    data.to_csv(merge_list.dir + '/file_' + str(merge_list.reconfig_strategy) + '-'
                + str(merge_list.id) + '.pp', sep='\t')


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='directory to read the files')

    parser.add_argument('-p', '--processes',
                        help='number of processes')

    parser.add_argument('-e', '--extension',
                        help='extension of result files')

    parser.add_argument('-o', '--output',
                        help='output file name')

    parser.add_argument('-r', '--reconfig_strategy',
                        help='reconfiguration strategy code')
    args = parser.parse_args()

    if args.directory:
        if args.extension:
            if args.reconfig_strategy:
                if args.output:
                    files_dir = []
                    for root, dirs, files in os.walk(args.directory):
                        for file in files:
                            if file.endswith('.' + args.extension) \
                                    and 'reconfig_st_' + str(args.reconfig_strategy) in file:
                                files_dir.append([root, file])
                                print file

                    if (args.processes):
                        processes = int(args.processes)

                    process_files = split_files(files_list=files_dir, processes=processes, dir=args.directory,
                                                reconfig_strategy=args.reconfig_strategy)

                    if process_files.__len__() > 0:
                        # merge_files(process_files[0])
                        pool = Pool(processes=int(args.processes))
                        pool.map(merge_files, process_files)
                        pool.close()
                        pool.join()

                        data = pd.DataFrame()
                        for root, dirs, files in os.walk(args.directory):
                            for file in files:
                                if file.endswith('.pp') and str(args.reconfig_strategy) + '-' in file:
                                    tmp = pd.read_csv(root + '/' + file, sep='\t', index_col=0)
                                    data = data.append(tmp, ignore_index=False)

                        data.to_csv(
                            args.directory + '/' + args.output,
                            sep='\t')

                    else:
                        print 'Error to create the merged files!'
                else:
                    print 'The output file name was not configured'
            else:
                print 'The reconfiguration strategy code was not configured'
        else:
            print 'Extension of results files was not configured'
    else:
        print 'The directory was not configured'

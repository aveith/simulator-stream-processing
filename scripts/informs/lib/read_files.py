from argparse import ArgumentParser
import os
import random
import xml.etree.ElementTree as ET
import numpy as np
from util import Config


def App(file='', seed=854741, conf=Config()):
    random.seed(a=seed)
    with open(file, 'r') as file:
        data = file.read().replace('\n', '')

    data = data.replace('digraph graphname {', '')
    data = data.replace('}', '')
    data = data.split(";")
    streams = []
    operators = []
    for r in range(len(data)):
        values = data[r].split(" -> ")
        src = int(values[0])
        dst = int(values[1])
        streams.append([src, dst])
        operators.append(src)
        operators.append(dst)

    vert = np.unique(operators)
    vertices = []
    for i in range(len(vert)):
        selectivity = random.uniform(conf.getfloat('general', 'selectivity_min'),
                                     conf.getfloat('general', 'selectivity_max'))
        data_pattern = random.uniform(conf.getfloat('general', 'data_pattern_min'),
                                      conf.getfloat('general', 'data_pattern_max'))
        cpu = int(random.uniform(conf.getint('general', 'cpu_min'),
                                 conf.getint('general', 'cpu_max')))
        mem = int(random.uniform(conf.getint('general', 'mem_min'),
                                 conf.getint('general', 'mem_max')))

        vertices.append([vert[i], cpu, mem, selectivity, data_pattern])

    return streams, vertices


def Net(file=''):
    tree = ET.parse(file)
    root = tree.getroot()
    hosts = []
    links = []
    hosts_capac = []
    links_capac = []
    hosts_general = []
    links_general = []
    for child in root:
        if child.tag == 'host':
            hosts_general.append(
                [int(child.get('id')), int(child.get('cpu')), int(child.get('mem')), int(child.get('type'))])
            if int(child.get('type')) != 1:
                hosts.append([int(child.get('id'))])
                hosts_capac.append([int(child.get('cpu')), int(child.get('mem'))])

        if child.tag == 'link':
            links_general.append(
                [int(child.get('src_id')), int(child.get('dst_id')), float(child.get('latency')),
                 int(child.get('bandwidth'))])
            links.append([int(child.get('src_id')), int(child.get('dst_id'))])
            links_capac.append([int(child.get('bandwidth'))])

    return hosts, hosts_capac, hosts_general, links, links_capac, links_general

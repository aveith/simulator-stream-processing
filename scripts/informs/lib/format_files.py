from argparse import ArgumentParser

from util import Config
from lib.read_files import App
from lib.read_files import Net
from lib.save_file import ilp_file


def parse_options():
    """Parse the command line options for workload creation"""
    parser = ArgumentParser(description='Format files to launch ILP.')

    parser.add_argument('-n', '--network', type=str, required=True,
                        help='network file (xml)')

    parser.add_argument('-a', '--application', type=str, required=True,
                        help='application file (dot)')

    parser.add_argument('-p', '--parameter', type=str, required=True,
                        help='application parameters')

    parser.add_argument('-o', '--directory', type=str, required=True,
                        help='output directory')

    parser.add_argument('-s', '--seed', type=int, required=True,
                        help='number base for random simulation configuration (seed)')
    args = parser.parse_args()
    return args


def main():
    opts = parse_options()
    conf = Config(opts.parameter)

    streams, vertices = App(file=opts.application, seed=opts.seed, conf=conf)
    hosts, hosts_capac, hosts_general, links, links_capac, links_general = Net(file=opts.network)

    ilp_file(streams=streams, vertices=vertices, hosts=hosts, hosts_capac=hosts_capac, hosts_general=hosts_general,
             links=links, links_capac=links_capac, links_general=links_general, dir=opts.directory, conf=conf,
             seed=opts.seed)

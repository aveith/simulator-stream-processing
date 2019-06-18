from argparse import ArgumentParser
from tarfile import ReadError

from util import Config
from lib.read_files import App
from lib.read_files import Net


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


def validate_conf_file(conf=Config(), parameters=''):
    if not conf.getint('reconfiguration', 'reconfig_strategy') in [-1, 1, 2, 3, 4]:
        print "xxxxxxxxx"
        return False

def save_ilp_file(app='', net='', dir=''):
    k = app

def main():
    opts = parse_options()
    conf = Config(opts.parameter)

    # if validate_conf_file(conf):
    app = App(file=opts.application, seed=opts.seed)
    net = Net(file=opts.network)
    save_ilp_file(app=app, net=net, dir=opts.directory)



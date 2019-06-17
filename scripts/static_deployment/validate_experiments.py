import argparse
import os

def validate_workdir(_dir=''):

    os.system("mkdir " + _dir + "/discarded")
    count_valids = 0
    count_discarded = 0
    for cur in os.listdir(_dir):
        number_of_dirs = len(os.listdir(_dir + '/' + cur))

        if os.path.exists(_dir + '/' + cur + '/discarded'):
            number_of_dirs = number_of_dirs - 1

        if number_of_dirs < 4:
            print 'Discarded directory: ' + _dir + '/' + cur + ' Folders: ' + str(number_of_dirs)
            os.system("mv " + _dir + '/' + cur + " " + _dir + "/discarded")
            count_discarded = count_discarded + 1
        else:
            print 'Valid directory: ' + _dir + '/' + cur + ' Folders: ' + str(number_of_dirs)
            count_valids = count_valids + 1

    print "Valids: " + str(count_valids) + " Discarded: " + str(count_discarded)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('-d', '--directory',
                        help='base directory')
    args = parser.parse_args()

    if args.directory:
        validate_workdir(args.directory)
    else:
        print 'Argument -d (directory of simulations) must be filled!'



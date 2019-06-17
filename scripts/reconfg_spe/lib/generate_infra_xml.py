import xml.etree.ElementTree as et
from argparse import ArgumentParser
import os
import random


def parse_options():
    """Parse the command line options for workload creation"""
    parser = ArgumentParser(description='Prepare the infrastruture XML using the parameters.')

    parser.add_argument('-e', '--number_of_edge_devices_per_cloudlet', type=int, required=True,
                        help='number of edge devices per cloudlet')

    parser.add_argument('-l', '--number_of_cloudlets', type=int, required=True,
                        help='number of cloudlets')

    parser.add_argument('-c', '--number_of_clouds', type=int, required=True,
                        help='number of clouds')

    parser.add_argument('-x', '--app_xml', type=str, required=True,
                        help='.xml of the application')

    parser.add_argument('-p', '--perc_of_pi_3', type=float, required=True,
                        help='percentage of Raspberry 3')

    parser.add_argument('-o', '--output_file_name', type=str, required=True,
                        help='output file name')

    args = parser.parse_args()
    return args


def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            indent(elem, level + 1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i


def buildTree(edge_devices_cloudlet=0, number_cloudlets=0, number_clouds=0, app_xml='', perc_rpi_3=0,
              output_file='sample.xml'):
    links = []
    switches = []
    clouds = []
    host_id = 0

    random.seed(a=854741)

    rpi3 = int(edge_devices_cloudlet) * float(perc_rpi_3)

    tree = et.ElementTree()
    root = et.Element('network')

    for cloudlet in range(number_cloudlets):
        switch = et.SubElement(root, 'host',
                               {'id': str(host_id),
                                'type': '1',
                                'cpu': '9488000',
                                'mem': '2000000000',
                                'cost': '0',
                                'slots': '-1',
                                })
        switch_id = host_id
        switches.append(switch_id)
        host_id += 1

        for device in range(edge_devices_cloudlet):
            if rpi3 > 0:
                device = et.SubElement(root, 'host',
                                       {'id': str(host_id),
                                        'type': '0',
                                        'cpu': '9488000',
                                        'mem': '2000000000',
                                        'cost': '0',
                                        'slots': '4',
                                        })
                rpi3 -= 1

            else:
                device = et.SubElement(root, 'host',
                                       {'id': str(host_id),
                                        'type': '0',
                                        'cpu': '4744000',
                                        'mem': '1000000000',
                                        'cost': '0',
                                        'slots': '4',
                                        })
            links.append([switch_id, host_id])
            host_id += 1

    for cloud in range(number_clouds):
        cloud = et.SubElement(root, 'host',
                              {'id': str(host_id),
                               'type': '2',
                               'cpu': '6090200000',
                               'mem': '2000000000000',
                               'cost': '0',
                               'slots': '-1',
                               })
        clouds.append(host_id)
        host_id += 1

    # Create the links
    link_id = 0

    # Link between switch and edge device
    for i in range(len(links)):
        link = et.SubElement(root, 'link',
                             {'id': str(link_id),
                              'bandwidth': '100000000',
                              'latency': str(random.uniform(.00011, .0010)),
                              'src_id': str(links[i][0]),
                              'dst_id': str(links[i][1]),
                              'cost': '0',
                              })
        link_id += 1

        link = et.SubElement(root, 'link',
                             {'id': str(link_id),
                              'bandwidth': '100000000',
                              'latency': str(random.uniform(.00011, .0010)),
                              'src_id': str(links[i][1]),
                              'dst_id': str(links[i][0]),
                              'cost': '0',
                              })
        link_id += 1

    # Link between clouds
    for i in range(len(switches)):
        for j in range(len(switches)):
            if switches[i] != switches[j]:
                link = et.SubElement(root, 'link',
                                     {'id': str(link_id),
                                      'bandwidth': '10000000000',
                                      'latency': str(random.uniform(.0605, .0805)),
                                      'src_id': str(switches[i]),
                                      'dst_id': str(switches[j]),
                                      'cost': '0',
                                      })
                link_id += 1

                link = et.SubElement(root, 'link',
                                     {'id': str(link_id),
                                      'bandwidth': '10000000000',
                                      'latency': str(random.uniform(.0605, .0805)),
                                      'src_id': str(switches[j]),
                                      'dst_id': str(switches[i]),
                                      'cost': '0',
                                      })
                link_id += 1

    # Link between switches
    for i in range(len(clouds)):
        for j in range(len(clouds)):
            if clouds[i] != clouds[j]:
                link = et.SubElement(root, 'link',
                                     {'id': str(link_id),
                                      'bandwidth': '10000000000',
                                      'latency': str(random.uniform(.0105, .0505)),
                                      'src_id': str(clouds[i]),
                                      'dst_id': str(clouds[j]),
                                      'cost': '0',
                                      })
                link_id += 1

                link = et.SubElement(root, 'link',
                                     {'id': str(link_id),
                                      'bandwidth': '10000000000',
                                      'latency': str(random.uniform(.0105, .0605)),
                                      'src_id': str(clouds[j]),
                                      'dst_id': str(clouds[i]),
                                      'cost': '0',
                                      })
                link_id += 1

    # Link between cloud and switches
    for i in range(len(clouds)):
        for j in range(len(switches)):
            if clouds[i] != switches[j]:
                link = et.SubElement(root, 'link',
                                     {'id': str(link_id),
                                      'bandwidth': '10000000000',
                                      'latency': str(random.uniform(.0605, .0805)),
                                      'src_id': str(clouds[i]),
                                      'dst_id': str(switches[j]),
                                      'cost': '0',
                                      })
                link_id += 1

                link = et.SubElement(root, 'link',
                                     {'id': str(link_id),
                                      'bandwidth': '10000000000',
                                      'latency': str(random.uniform(.0605, .0805)),
                                      'src_id': str(switches[j]),
                                      'dst_id': str(clouds[i]),
                                      'cost': '0',
                                      })
                link_id += 1

    indent(root)
    tree._setroot(root)
    tree.write(output_file, encoding='utf-8', xml_declaration=True, method="xml")


def validate_app_xml(app_xml=''):
    return True


def main(number_of_edge_devices_per_cloudlet, number_of_cloudlets, number_of_clouds, app, perc_of_pi_3,
         output_file_name):
    if validate_app_xml(app):
        buildTree(edge_devices_cloudlet=int(number_of_edge_devices_per_cloudlet),
                  number_cloudlets=int(number_of_cloudlets),
                  number_clouds=int(number_of_clouds), app_xml=app,
                  perc_rpi_3=perc_of_pi_3, output_file=output_file_name)
    else:
        print 'The xml of the application is not valid!'


if __name__ == '__main__':
    opts = parse_options()

    main(number_of_edge_devices_per_cloudlet=opts.number_of_edge_devices_per_cloudlet,
         number_of_cloudlets=opts.number_of_cloudlets, number_of_clouds=opts.number_of_clouds, app=opts.app_xml,
         perc_of_pi_3=opts.perc_of_pi_3,
         output_file_name=opts.output_file_name)

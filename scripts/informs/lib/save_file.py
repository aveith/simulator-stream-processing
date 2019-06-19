import os
import sys


def get_link(src=0, src_type=0, dst=0, dst_type=0, links_general=[]):
    link = []

    gtw1 = -1
    if src_type == 0:
        for lnk in range(len(links_general)):
            if links_general[lnk][1] == src:
                gtw1 = links_general[lnk][2]
                link.append(links_general[lnk])
                break
    else:
        gtw1 = src

    gtw2 = -1
    if dst_type == 0:
        for lnk in range(len(links_general)):
            if links_general[lnk][1] == dst:
                gtw2 = links_general[lnk][2]
                link.append(links_general[lnk])
                break
    else:
        gtw2 = dst

    if gtw1 != gtw2:
        for lnk in range(len(links_general)):
            if links_general[lnk][1] == gtw1 and links_general[lnk][2] == gtw2:
                link.append(links_general[lnk])
                break

    min = sys.float_info.max
    for c in range(len(link)):
        if link[c][4] < min:
            min = link[c][4]

    return [src, dst], [min]


def links_simplification(hosts_general=[], links_general=[]):
    links = []
    capac = []
    for src in range(len(hosts_general)):
        if hosts_general[src][3] != 1:
            for dst in range(len(hosts_general)):
                if hosts_general[dst][3] != 1:
                    l, c = get_link(src=hosts_general[src][0], src_type=hosts_general[src][3], dst=hosts_general[dst][0],
                                    dst_type=hosts_general[dst][3], links_general=links_general)
                    links.append(l)
                    capac.append(c)

    return links, capac


def vertice_ordering(streams=[], verices=[]):
    return []


def app_requirements():
    return [], []


def app_costs():
    return [], []


def ilp_file(streams=[], verices=[], hosts=[], hosts_capac=[], hosts_general=[],
             links=[], links_capac=[], links_general=[], dir=''):
    f = open(dir, "w")
    f.write("%s %s %s %s\n" % (hosts.__len__(), links.__len__(), 2, 1))

    links_simp, links_cap_simp = links_simplification(hosts_general=hosts_general, links_general=links_general)

    f.write("%s\n" % str(links_simp).replace('\'', ''))
    f.write("%s\n" % str(hosts_capac).replace('\'', ''))
    f.write("%s\n" % str(links_cap_simp).replace('\'', ''))

    f.write("%s %s\n" % (verices.__len__(), streams.__len__()))
    f.write("%s\n" % str(streams).replace('\'', ''))
    ordered_vertices = vertice_ordering(streams=streams, verices=verices)

    vertice_reqs, stream_reqs = app_requirements()
    vertice_costs, stream_costs = app_costs()
    f.close()

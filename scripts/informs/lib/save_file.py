import os
import sys
import numpy as np
from util import Config
import random


def get_link(src=0, src_type=0, dst=0, dst_type=0, links_general=[]):
    link = []

    gtw1 = -1
    if src_type == 0:
        for lnk in range(len(links_general)):
            if links_general[lnk][0] == src:
                gtw1 = links_general[lnk][1]
                link.append(links_general[lnk])
                break
    else:
        gtw1 = src

    gtw2 = -1
    if dst_type == 0:
        for lnk in range(len(links_general)):
            if links_general[lnk][0] == dst:
                gtw2 = links_general[lnk][1]
                link.append(links_general[lnk])
                break
    else:
        gtw2 = dst

    if gtw1 != gtw2:
        for lnk in range(len(links_general)):
            if links_general[lnk][0] == gtw1 and links_general[lnk][1] == gtw2:
                link.append(links_general[lnk])
                break

    min = sys.float_info.max
    for c in range(len(link)):
        if link[c][3] < min:
            min = link[c][3]

    return [src, dst], [min]


def links_simplification(hosts_general=[], links_general=[], hosts=[]):
    links = []
    capac = []
    for src in range(len(hosts_general)):
        if hosts_general[src][3] != 1:
            for dst in range(len(hosts_general)):
                if hosts_general[dst][3] != 1:
                    if hosts_general[src][0] != hosts_general[dst][0]:
                        l, c = get_link(src=hosts_general[src][0], src_type=hosts_general[src][3],
                                        dst=hosts_general[dst][0],
                                        dst_type=hosts_general[dst][3], links_general=links_general)
                        links.append(l)
                        capac.append(c)

    links_conv = []
    for i in range(len(links)):
        src = -1
        dst = -1
        for j in range(len(hosts)):
            if hosts[j][0] == links[i][0]:
                src = j

        for j in range(len(hosts)):
            if hosts[j][0] == links[i][1]:
                dst = j

        links_conv.append([src, dst])

    return links, capac, links_conv


def get_sources(streams=[]):
    sources = []
    for i in range(len(streams)):
        bsource = True
        for j in range(len(streams)):
            if streams[i][0] == streams[j][1]:
                bsource = False
                break
        if bsource:
            sources.append(streams[i][0])

    return np.unique(sources)


def get_sinks(streams=[]):
    sinks = []
    for i in range(len(streams)):
        bSink = True
        for j in range(len(streams)):
            if streams[i][1] == streams[j][0]:
                bSink = False
                break
        if bSink:
            sinks.append(streams[i][1])

    return np.unique(sinks)


def hasPendency(ope=0, list=[], streams=[]):
    for j in range(len(streams)):
        if streams[j][1] == ope:
            if list.count(streams[j][0]) == 0:
                return True

    return False


def vertice_ordering(streams=[]):
    sources = get_sources(streams=streams)
    sinks = get_sinks(streams=streams)

    queue = sources.tolist()
    ordered = []
    while len(queue) > 0:
        ope = queue.pop(0)
        if not hasPendency(ope=ope, list=ordered, streams=streams):
            if ordered.count(ope) == 0:
                ordered.append(ope)

            for j in range(len(streams)):
                if streams[j][0] == ope:
                    if queue.count(streams[j][1]) == 0:
                        queue.append(streams[j][1])

        else:
            queue.append(ope)

    return ordered, sources, sinks


def estimate_time_queues(service=0, arrival=0, overhead=0):
    service = float(service)
    arrival = float(arrival)
    overhead = float(overhead)
    rho = float(arrival / service)
    meanTimeinSystem = float((1 / (service - arrival)) + overhead)
    meanTimeinQueue = float(rho / (service - arrival))

    meanNumberofMsgsinSystem = float(rho / (1 - rho))
    meanNumberinQueue = float(rho * arrival / (service - arrival))

    return [rho, meanTimeinSystem, meanTimeinQueue, meanNumberofMsgsinSystem, meanNumberinQueue]


def get_arrival(ope=0, req=[], streams=[]):
    rate = 0
    size = 0
    count = 0
    stream_req = []
    for i in range(len(streams)):
        if streams[i][1] == ope:
            for j in range(len(req)):
                if req[j][0] == streams[i][0]:
                    rate += req[j][1]
                    size += req[j][2]
                    stream_req.append([streams[i][0], streams[i][1], req[j][1] * req[j][2]])
                    count += 1

    return rate, size / count if count > 0 else 0, stream_req


def app_requirements(ordered=[], sources=[], streams=[], conf=Config(), vertices=[]):
    ope_req = []
    stream_req = []
    for i in range(len(ordered)):
        rate, size, stream = get_arrival(ope=ordered[i], req=ope_req, streams=streams)
        if stream.__len__() > 0:
            for s in range(len(stream)):
                stream_req.append(stream[s])

        mem = 0

        if sources.count(ordered[i]) > 0:
            rate = int(random.uniform(conf.getint('general', 'input_rate_min'),
                                      conf.getint('general', 'input_rate_max')))
            size = int(random.uniform(conf.getint('general', 'input_size_min'),
                                      conf.getint('general', 'input_size_max')))

        arrival = rate
        arrival_size = size
        for j in range(len(vertices)):
            if vertices[j][0] == ordered[i]:
                total_cpu = rate * vertices[j][1]
                rate = rate * vertices[j][3]
                size = size * vertices[j][4]
                mem = vertices[j][2]
                break

        ope_req.append([ordered[i], rate, size, total_cpu, mem, vertices[j][1], arrival, arrival_size])

    return ope_req, stream_req


def get_operator_requirement(ope=0, requirement=[]):
    for i in range(len(requirement)):
        if requirement[i][0] == ope:
            return requirement[i]


def get_link_index(src=0, dst=0, links=[]):
    for i in range(len(links)):
        if links[i][0] == src and links[i][1] == dst:
            return i


def is_valid(vertices=[], host=0, vertex=0):
    for i in range(len(vertices)):
        if vertices[i][0] == vertex and vertices[i][1] == host:
            return False
    return True


def app_costs(vertice_reqs=[], stream_reqs=[], hosts_general=[], links_capac=[], sources=[], vertices=[], links=[],
              sinks=[], hosts=[]):
    hard_mapping = []
    for i in range(len(sources)):
        pos = int(random.uniform(0, hosts.__len__() - 1))
        for j in range(len(hosts)):
            if hosts[j][0] == hosts[pos][0]:
                hard_mapping.append([j, sources[i]])

    for i in range(len(sinks)):
        pos = int(random.uniform(0, hosts.__len__() - 1))
        for j in range(len(hosts)):
            if hosts[j][0] == hosts[pos][0]:
                hard_mapping.append([j, sinks[i]])

    vertice_costs = []
    stream_costs = []
    sources = sources.tolist()
    sinks = sinks.tolist()
    for i in range(len(stream_reqs)):
        src_req = get_operator_requirement(ope=stream_reqs[i][0], requirement=vertice_reqs)
        dst_req = get_operator_requirement(ope=stream_reqs[i][1], requirement=vertice_reqs)

        for j in range(len(hosts_general)):
            if hosts_general[j][3] != 1:
                for k in range(len(hosts_general)):
                    if hosts_general[k][3] != 1:

                        if is_valid(vertices=vertice_costs, host=hosts_general[j][0], vertex=src_req[0]):
                            if sources.count(stream_reqs[i][0]) > 0:
                                if hard_mapping.count([stream_reqs[i][0], hosts_general[j][0]]) > 0:
                                    src_queue_values = estimate_time_queues(service=hosts_general[j][1] / src_req[5],
                                                                            arrival=src_req[6], overhead=0)
                                    src_cpu_cost = src_queue_values[1]
                                    src_mem_cost = src_req[4] + src_queue_values[3] * src_req[7]
                                    vertice_costs.append([src_req[0], hosts_general[j][0], src_cpu_cost, src_mem_cost])
                            else:
                                src_queue_values = estimate_time_queues(service=hosts_general[j][1] / src_req[5],
                                                                        arrival=src_req[6], overhead=0)
                                src_cpu_cost = src_queue_values[1]
                                src_mem_cost = src_req[4] + src_queue_values[3] * src_req[7]
                                vertice_costs.append([src_req[0], hosts_general[j][0], src_cpu_cost, src_mem_cost])

                        if is_valid(vertices=vertice_costs, host=hosts_general[k][0], vertex=dst_req[0]):
                            if sinks.count(stream_reqs[i][1]) > 0:
                                if hard_mapping.count([stream_reqs[i][1], hosts_general[k][0]]) > 0:
                                    dst_queue_values = estimate_time_queues(service=hosts_general[k][1] / dst_req[5],
                                                                            arrival=dst_req[6], overhead=0)
                                    dst_cpu_cost = dst_queue_values[1]
                                    dst_mem_cost = dst_req[4] + dst_queue_values[3] * dst_req[7]
                                    vertice_costs.append([dst_req[0], hosts_general[k][0], dst_cpu_cost, dst_mem_cost])
                            else:
                                dst_queue_values = estimate_time_queues(service=hosts_general[k][1] / dst_req[5],
                                                                        arrival=dst_req[6], overhead=0)
                                dst_cpu_cost = dst_queue_values[1]
                                dst_mem_cost = dst_req[4] + dst_queue_values[3] * dst_req[7]
                                vertice_costs.append([dst_req[0], hosts_general[k][0], dst_cpu_cost, dst_mem_cost])

                        if hosts_general[j][0] != hosts_general[k][0]:

                            stream_queue_values = estimate_time_queues(service=links_capac[
                                                                                   get_link_index(
                                                                                       src=hosts_general[j][0],
                                                                                       dst=hosts_general[k][0],
                                                                                       links=links)][0] / (
                                                                                   src_req[2]),
                                                                       arrival=src_req[1] * src_req[2], overhead=0)
                            stream_costs.append(
                                [stream_reqs[i][0], stream_reqs[i][1], hosts_general[j][0], hosts_general[k][0],
                                 stream_queue_values[1]])
                        else:
                            stream_costs.append(
                                [stream_reqs[i][0], stream_reqs[i][1], hosts_general[j][0], hosts_general[k][0],
                                 0])

    return vertice_costs, stream_costs, hard_mapping


def get_app_requirements(vertice_reqs=[], stream_reqs=[], vertices=[], streams=[]):
    v = []
    s = []
    for i in range(len(vertices)):
        for j in range(len(vertice_reqs)):
            if vertices[i][0] == vertice_reqs[i][0]:
                v.append([vertice_reqs[i][3], vertice_reqs[i][4]])
                break

    for i in range(len(streams)):
        for j in range(len(stream_reqs)):
            if streams[i][0] == stream_reqs[j][0] and streams[i][1] == stream_reqs[j][1]:
                s.append([stream_reqs[j][2]])
                break
    return v, s


def get_final_costs(vertice_costs=[], stream_costs=[],
                    vertices=[], streams=[], links=[],
                    hosts=[]):
    v = []
    s = []
    for i in range(len(hosts)):
        lv = []
        for j in range(len(vertices)):
            find = False
            for k in range(len(vertice_costs)):
                if vertice_costs[k][0] == vertices[j][0] and vertice_costs[k][1] == hosts[i][0]:
                    lv.append(vertice_costs[k][2])
                    find = True
                    break
            if not find:
                lv.append(0)

        v.append(lv)

    for i in range(len(links)):
        ls = []
        for j in range(len(streams)):
            find = False
            for k in range(len(stream_costs)):
                if stream_costs[k][0] == streams[j][0] and stream_costs[k][1] == streams[j][1] and stream_costs[k][2] == \
                        links[i][0] and stream_costs[k][3] == links[i][1]:
                    find = True
                    ls.append(stream_costs[k][4])
                    break
            if not find:
                ls.append(0)
        s.append(ls)

    return v, s


def ilp_file(streams=[], vertices=[], hosts=[], hosts_capac=[], hosts_general=[],
             links=[], links_capac=[], links_general=[], dir='', conf=Config(), seed=854741):
    random.seed(a=seed)

    f = open(dir, "w")
    links_simp, links_cap_simp, links_simp_conv = links_simplification(hosts_general=hosts_general,
                                                                       links_general=links_general, hosts=hosts)

    f.write("%s %s %s %s\n" % (hosts_capac.__len__(), links_simp.__len__(), 2, 1))
    f.write("%s\n" % links_simp_conv)
    f.write("%s\n" % hosts_capac)
    f.write("%s\n" % links_cap_simp)

    f.write("%s %s\n" % (vertices.__len__(), streams.__len__()))
    f.write("%s\n" % streams)
    ordered_vertices, sources, sinks = vertice_ordering(streams=streams)

    vertice_reqs, stream_reqs = app_requirements(ordered=ordered_vertices, sources=sources.tolist(), streams=streams,
                                                 conf=conf, vertices=vertices)

    vertex_reqs, streams_reqs = get_app_requirements(vertice_reqs=vertice_reqs, stream_reqs=stream_reqs,
                                                     vertices=vertices, streams=streams)
    f.write("%s\n" % vertex_reqs)
    f.write("%s\n" % streams_reqs)

    vertice_costs, stream_costs, hard_mapping = app_costs(vertice_reqs=vertice_reqs, stream_reqs=stream_reqs,
                                                          hosts_general=hosts_general,
                                                          links=links_simp,
                                                          links_capac=links_cap_simp, sources=sources,
                                                          vertices=vertices, sinks=sinks, hosts=hosts)

    final_vertex_costs, final_stream_costs = get_final_costs(vertice_costs=vertice_costs, stream_costs=stream_costs,
                                                             vertices=vertices, streams=streams, links=links_simp,
                                                             hosts=hosts)

    f.write("%s\n" % final_vertex_costs)
    f.write("%s\n" % final_stream_costs)

    f.write("%s\n" % hard_mapping)
    f.close()

from argparse import ArgumentParser
import os
import random
import xml.etree.ElementTree as ET

def App(file='', seed=854741):
    random.seed(a=seed)
    with open(file, 'r') as file:
        data = file.read().replace('\n', '')

    data = data.replace('digraph graphname {', '')
    data = data.replace('}', '')
    data = data.split(";")
    app = []
    for r in range(len(data)):
        values = data[r].split(" -> ")
        src = int(values[0])
        dst = int(values[1])
        app.append([src,dst])
    return app

def Net(file=''):
    tree = ET.parse(file)
    root = tree.getroot()
    for child in root:
        print child.tag, child.attrib
        print child.get('id')

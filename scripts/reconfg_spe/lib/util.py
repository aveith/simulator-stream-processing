#!/usr/bin/env python

from __future__ import print_function

import os
import glob
import os.path
import subprocess

from Queue import Queue, Full
from threading import Thread
from multiprocessing import cpu_count

try:
    import configparser as ConfigParser
except ImportError:
    import ConfigParser


def split_list(alist, wanted_parts=1):
    length = len(alist)
    return [alist[i*length: (i+1)*length] for i in range(wanted_parts)]


def frange(start, end=None, inc=None):
    """A range function, that does accept float increments..."""

    if end is None:
        end = start + 0.0
        start = 0.0

    if inc is None:
        inc = 1.0

    l = []
    while 1:
        next = start + len(l) * inc
        if inc > 0 and next >= end:
            break
        elif inc < 0 and next <= end:
            break
        l.append(next)
    return l


def rm(*args):
    for arg in args:
        for path in glob.iglob(arg):
            if os.path.isdir(path):
                os.rmdir(path)
            else:
                os.remove(path)


def spawn(command, stdout=None):
    fp = None if stdout is None else open(stdout, 'w')
    subprocess.check_call(command.split(), stdout = None if fp is None else fp)
    if fp != None:
        fp.close()


class Worker(Thread):
    """Thread executing tasks from a given tasks queue"""
    def __init__(self, tasks):
        Thread.__init__(self)
        self.tasks = tasks
        self.daemon = True
        # self.start()

    def run(self):
        while True:
            func, args, kargs = self.tasks.get()
            try:
                func(*args, **kargs)
            except Exception as e:
                print('An exception occurred while executing a task.')
                print(e)
                raise
            self.tasks.task_done()


class ThreadPool(object):
    """Pool of threads consuming tasks from a queue"""
    def __init__(self, num_threads=None):
        if num_threads is None:
            num_threads = cpu_count()
        else:
            # If this came from a property, it might be a string. Let's convert
            # it before passing it to the queue
            num_threads = int(num_threads)
        self.tasks = Queue(num_threads)
        self.workers = [Worker(self.tasks) for _ in range(num_threads)]

    def add_task(self, func, *args, **kargs):
        """Add a task to the queue"""
        added = False
        while not added:
            try:
                self.tasks.put((func, args, kargs), timeout=1)
                added = True
            except Full:
                pass

    def start_workers(self):
        for worker in self.workers:
            worker.start()

    def wait_completion(self):
        """Wait for completion of all the tasks in the queue"""
        self.tasks.join()


class Config(ConfigParser.RawConfigParser):

    def __init__(self, files=None):
        """Initializes the object.

        This method has a "triple" behavior on top of initializing the class,
        it combines the readfp and read methods of the ConfigParser class. When
        files is a file-like object, it will invoke readfp. Otherwise, it will
        invoke read. Please refer to those methods' documentation for details.

        @param files: the file(s) to read from.
        """
        ConfigParser.RawConfigParser.__init__(self)
        if files is None:
            return
        if isinstance(files, file):
            self.read_file(files)
        else:
            self.read(files)

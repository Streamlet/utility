#!/bin/python
# -*- coding: utf-8 -*-

import os, subprocess


def cmd(cmd):
    print(cmd)
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               shell=True)
    (stdoutdata, stderrdata) = process.communicate()
    if stdoutdata is not None:
        print(stdoutdata)
    if stderrdata is not None:
        print(stderrdata)
    return process.wait() == 0


def main():
    if not os.path.exists('.srcdep/repo'):
        cmd('git clone https://github.com/Streamlet/srcdep.git .srcdep/repo')
    cmd('.srcdep/repo/srcdep update')


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main()

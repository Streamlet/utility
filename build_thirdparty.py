#!/bin/python
# -*- coding: utf-8 -*-

import os, sys, subprocess


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


def build_boost():
    os.chdir('boost')
    boost_libs = '--with-test'
    if sys.platform == 'win32':
        b2 = 'b2.exe'
        platform_args = 'cxxflags="/std:c++17" define=BOOST_AUTO_LINK_SYSTEM define=BOOST_TEST_NO_MAIN define=BOOST_TEST_ALTERNATIVE_INIT_API'
    else:
        b2 = './b2'
        platform_args = 'cxxflags="-std=c++17"'
    if not os.path.exists(b2):
        if sys.platform == 'win32':
            cmd('bootstrap.bat')
        else:
            cmd('./bootstrap.sh')
    cmd('%s install %s link=static runtime-link=static threading=multi address-model=64 variant=release --layout=system --prefix=build %s'
        % (b2, platform_args, boost_libs))
    # if sys.platform == 'win32':
    # os.chdir(os.path.join('build', 'lib'))
    # cmd('del /S /Q boost_*.lib')
    # cmd('rename "libboost_*.lib" "///boost_*.lib"')
    # os.chdir(os.path.join('..', '..'))
    os.chdir('..')


def main():
    os.chdir('thirdparty')
    build_boost()
    os.chdir('..')


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main()

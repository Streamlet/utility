#!/bin/python
# -*- coding: utf-8 -*-

import os, sys, subprocess


def cmd(cmd):
    print(cmd)
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               shell=True)
    ret_code = 0
    while True:
        nextline = process.stdout.readline()
        if nextline == '' and process.poll() is not None:
            break
        sys.stdout.write(nextline)
        sys.stdout.flush()

    ret_code = process.returncode
    return ret_code


def build_boost(debug):
    os.chdir('boost')
    config = 'debug' if debug else 'release'
    boost_libs = '--with-test'
    if sys.platform == 'win32':
        b2 = 'b2.exe'
        platform_args = 'cxxflags="/std:c++17" define=BOOST_AUTO_LINK_SYSTEM define=BOOST_TEST_NO_MAIN define=BOOST_TEST_ALTERNATIVE_INIT_API define=BOOST_BEAST_USE_STD_STRING_VIEW'
    else:
        b2 = './b2'
        platform_args = 'cxxflags="-std=c++17"'
    if not os.path.exists(b2):
        if sys.platform == 'win32':
            cmd('bootstrap.bat')
        else:
            cmd('./bootstrap.sh')
    cmd('%s install %s link=static runtime-link=static threading=multi address-model=64 variant=%s --layout=system --prefix=build/%s %s'
        % (b2, platform_args, config, config, boost_libs))
    os.chdir('..')

def build_openssl(debug):
    os.chdir('openssl')
    config = 'debug' if debug else 'release'
    prefix = os.path.abspath(os.path.join('build', config))
    cmd('perl Configure VC-WIN64A threads no-shared --%s --prefix=%s' % (config, prefix))
    cmd('nmake')
    cmd('nmake install')
    os.chdir('..')


def main(args):
    debug = 'debug' in args
    os.chdir('thirdparty')
    build_boost(debug)
    build_openssl(debug)
    os.chdir('..')


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main(sys.argv[1:])

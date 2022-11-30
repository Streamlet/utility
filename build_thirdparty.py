#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys, subprocess, locale


def cmd(cmd):
    print(cmd)
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               shell=True)
    encoding = locale.getpreferredencoding(False)
    while True:
        nextline = process.stdout.readline()
        if nextline is not None:
            sys.stdout.write(nextline.decode(encoding))
            sys.stdout.flush()
        if process.poll() is not None:
            break
    return process.returncode


def build_gtest(config):
    os.chdir('googletest')
    cmd('cmake -S . -B build')
    cmd('cmake --build build --config %s' % config)
    cmd('cmake --install build  --config %s --prefix build/%s' % (config, config))
    os.chdir('..')


def build_boost(config):
    os.chdir('boost')
    boost_libs = ''
    if len(boost_libs) > 0:
        if sys.platform == 'win32':
            b2 = 'b2.exe'
            platform_args = 'cxxflags="/std:c++17" runtime-link=static'
        else:
            b2 = './b2'
            platform_args = 'cxxflags="-std=c++17" runtime-link=shared'
        if not os.path.exists(b2):
            if sys.platform == 'win32':
                cmd('bootstrap.bat')
            else:
                cmd('./bootstrap.sh')
        cmd('%s install %s link=static threading=multi address-model=64 variant=%s  define=BOOST_AUTO_LINK_SYSTEM --layout=system --prefix=build/%s --build_dir=build/%s %s'
            % (b2, platform_args, config, config, config, boost_libs))
    os.chdir('..')


def build_curl(config):
    os.chdir('curl')
    prefix = os.path.abspath(os.path.join('build', config))
    if sys.platform == 'win32':
        os.chdir('winbuild')
        debug_yes_no = 'yes' if config == 'debug' else 'no'
        cmd('nmake /f Makefile.vc mode=static WITH_PREFIX=%s ENABLE_UNICODE=yes GEN_PDB=yes DEBUG=%s MACHINE=x64 RTLIBCFG=static'
            % (prefix, debug_yes_no))
        os.chdir('..')
    else:
        enable_debug = '--enable-debug' if config == 'debug' else ''
        # install openssl-devel, zlib-devel in your system
        cmd('./configure --prefix=%s --with-ssl %s' %
            (prefix, enable_debug))
        cmd('make')
        cmd('make install')
    os.chdir('..')


def build_openssl(config):
    os.chdir('openssl')
    prefix = os.path.abspath(os.path.join('build', config))
    if sys.platform == 'win32':
        cmd('perl Configure VC-WIN64A threads no-shared --%s --prefix=%s --openssldir=%s'
            % (config, prefix, prefix))
        cmd('nmake include/openssl/opensslconf.h')
    else:
        cmd('./config threads shared --%s --prefix=%s --openssldir=%s' %
            (config, prefix, prefix))
        cmd('make include/openssl/opensslconf.h')
    os.chdir('..')


def main():
    os.chdir('thirdparty')
    for config in ['debug', 'release']:
        build_boost(config)
        build_gtest(config)
        build_curl(config)
        build_openssl(config)
    os.chdir('..')


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    main()

#!/usr/bin/bash
QTDIR=/opt/csw/kde-gcc KDE_DIR=/opt/csw/kde-gcc AR="/opt/csw/bin/gar" CC="/opt/csw/gcc3/bin/gcc" CXX="/opt/csw/gcc3/bin/g++" CFLAGS="-I/usr/local/include -I/opt/csw/include -I/opt/csw/kde-gcc/include" CXXFLAGS="-I/usr/local/include -I/opt/csw/include -I/opt/csw/kde-gcc/include" LDFLAGS="-L/opt/csw/lib -L/opt/csw/kde-gcc/lib -L/usr/local/lib" ./configure --prefix=/opt/csw/kde-gcc

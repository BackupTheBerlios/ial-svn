#!/bin/bash

libtoolize --force --copy;
aclocal; autoconf; automake --add-missing --force-missing --gnu;
PKG_CONFIG_PATH=/opt/dbus/lib/pkgconfig:$PKG_CONFIG_PATH ./configure --prefix=/usr

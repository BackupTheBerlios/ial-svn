#!/bin/bash

libtoolize --force --copy;
aclocal; autoconf; automake --add-missing --force-missing --gnu; ./configure $@

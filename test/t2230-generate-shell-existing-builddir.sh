#! /bin/sh -e
# tup - A file-based build system
#
# Copyright (C) 2021  Mike Shal <marfey@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# Try 'tup generate --builddir build' when build already exists.

. ./tup.sh

mkdir build
cat > build/Tupfile << HERE
ifdef CONFIG_FOO
srcs += foo.c
endif
ifdef CONFIG_BAR
srcs += bar.c
endif
: foreach \$(srcs) |> gcc -c %f -o %o |> %B.o
HERE
touch build/foo.c build/bar.c

echo 'CONFIG_FOO=y' > foo.config

generate_fail_msg 'builddir directory must be a non-existent directory'  --config foo.config --builddir build $generate_script_name

eotup

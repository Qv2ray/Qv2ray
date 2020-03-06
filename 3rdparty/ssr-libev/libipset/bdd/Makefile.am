# This file is part of libasyncns.
#
# Copyright 2005-2008 Lennart Poettering
#
# libasyncns is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 2.1 of the
# License, or (at your option) any later version.
#
# libasyncns is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libasyncns. If not, see
# <http://www.gnu.org/licenses/>.

noinst_LTLIBRARIES = libipset.la

bdd_src = bdd/assignments.c bdd/basics.c bdd/bdd-iterator.c bdd/expanded.c \
		  bdd/reachable.c bdd/read.c bdd/write.c 
map_src = map/allocation.c map/inspection.c map/ipv4_map.c map/ipv6_map.c \
		  map/storage.c
set_src = set/allocation.c set/inspection.c set/ipv4_set.c set/ipv6_set.c \
		  set/iterator.c set/storage.c

libipset_la_SOURCES = ${bdd_src} ${map_src} ${set_src}

libipset_la_LDFLAGS = -static

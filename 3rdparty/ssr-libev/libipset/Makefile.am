noinst_LTLIBRARIES = libipset.la

bdd_src = bdd/assignments.c bdd/basics.c bdd/bdd-iterator.c bdd/expanded.c \
		  bdd/reachable.c bdd/read.c bdd/write.c 
map_src = map/allocation.c map/inspection.c map/ipv4_map.c map/ipv6_map.c \
		  map/storage.c
set_src = set/allocation.c set/inspection.c set/ipv4_set.c set/ipv6_set.c \
		  set/iterator.c set/storage.c

libipset_la_SOURCES = general.c ${bdd_src} ${map_src} ${set_src}
libipset_la_CFLAGS = -I$(top_srcdir)/libipset/include -I$(top_srcdir)/libcork/include

libipset_la_LDFLAGS = -static

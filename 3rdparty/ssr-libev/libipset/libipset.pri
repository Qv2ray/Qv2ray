INCLUDEPATH += $$PWD/include/
HEADERS += $$PWD/include/ipset/errors.h
$$PWD/include/ipset/logging.h
$$PWD/include/ipset/bits.h
$$PWD/include/ipset/bdd/nodes.h
$$PWD/include/ipset/ipset.h

SOURCES += $$PWD/general.c
SOURCES += $$PWD/bdd/assignments.c $$PWD/bdd/basics.c $$PWD/bdd/bdd-iterator.c $$PWD/bdd/expanded.c \
                  $$PWD/bdd/reachable.c $$PWD/bdd/read.c $$PWD/bdd/write.c
SOURCES += $$PWD/map/allocation.c $$PWD/map/inspection.c $$PWD/map/ipv4_map.c $$PWD/map/ipv6_map.c \
                  $$PWD/map/storage.c
SOURCES += $$PWD/set/allocation.c $$PWD/set/inspection.c $$PWD/set/ipv4_set.c $$PWD/set/ipv6_set.c \
                  $$PWD/set/iterator.c $$PWD/set/storage.c

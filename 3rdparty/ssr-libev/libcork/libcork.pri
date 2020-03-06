INCLUDEPATH += $$PWD/include/
HEADERS += $$PWD/include/libcork/threads/basics.h\
$$PWD/include/libcork/threads/atomics.h\
$$PWD/include/libcork/threads.h\
$$PWD/include/libcork/cli.h\
$$PWD/include/libcork/os.h\
$$PWD/include/libcork/os/process.h\
$$PWD/include/libcork/os/files.h\
$$PWD/include/libcork/os/subprocess.h\
$$PWD/include/libcork/config/solaris.h\
$$PWD/include/libcork/config/version.h\
$$PWD/include/libcork/config/mingw32.h\
$$PWD/include/libcork/config/config.h\
$$PWD/include/libcork/config/gcc.h\
$$PWD/include/libcork/config/arch.h\
$$PWD/include/libcork/config/macosx.h\
$$PWD/include/libcork/config/linux.h\
$$PWD/include/libcork/config/bsd.h\
$$PWD/include/libcork/core/u128.h\
$$PWD/include/libcork/core/byte-order.h\
$$PWD/include/libcork/core/timestamp.h\
$$PWD/include/libcork/core/api.h\
$$PWD/include/libcork/core/net-addresses.h\
$$PWD/include/libcork/core/gc.h\
$$PWD/include/libcork/core/allocator.h\
$$PWD/include/libcork/core/error.h\
$$PWD/include/libcork/core/attributes.h\
$$PWD/include/libcork/core/types.h\
$$PWD/include/libcork/core/id.h\
$$PWD/include/libcork/core/mempool.h\
$$PWD/include/libcork/core/callbacks.h\
$$PWD/include/libcork/core/hash.h\
$$PWD/include/libcork/config.h\
$$PWD/include/libcork/ds.h\
$$PWD/include/libcork/ds/buffer.h\
$$PWD/include/libcork/ds/hash-table.h\
$$PWD/include/libcork/ds/array.h\
$$PWD/include/libcork/ds/slice.h\
$$PWD/include/libcork/ds/ring-buffer.h\
$$PWD/include/libcork/ds/managed-buffer.h\
$$PWD/include/libcork/ds/bitset.h\
$$PWD/include/libcork/ds/dllist.h\
$$PWD/include/libcork/ds/stream.h\
$$PWD/include/libcork/core.h\
$$PWD/include/libcork/helpers/errors.h\
$$PWD/include/libcork/helpers/gc.h\
$$PWD/include/libcork/helpers/posix.h\
$$PWD/include/libcork/cli/commands.h

SOURCES += $$PWD/cli/commands.c
SOURCES += $$PWD/core/allocator.c $$PWD/core/error.c $$PWD/core/gc.c \
                   $$PWD/core/hash.c $$PWD/core/ip-address.c $$PWD/core/mempool.c \
                   $$PWD/core/timestamp.c $$PWD/core/u128.c
SOURCES += $$PWD/ds/array.c    $$PWD/ds/buffer.c $$PWD/ds/dllist.c \
                 $$PWD/ds/file-stream.c $$PWD/ds/hash-table.c $$PWD/ds/managed-buffer.c \
                 $$PWD/ds/ring-buffer.c $$PWD/ds/slice.c
SOURCES += $$PWD/ds/bitset.c
SOURCES += $$PWD/posix/directory-walker.c $$PWD/posix/env.c $$PWD/posix/exec.c \
                        $$PWD/posix/files.c $$PWD/posix/process.c $$PWD/posix/subprocess.c
SOURCES += $$PWD/pthreads/thread.c

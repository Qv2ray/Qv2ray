noinst_LTLIBRARIES = libcork.la

cli_src = cli/commands.c
core_src = core/allocator.c core/error.c core/gc.c \
		   core/hash.c core/ip-address.c core/mempool.c \
		   core/timestamp.c core/u128.c
ds_src = ds/array.c ds/bitset.c ds/buffer.c ds/dllist.c \
		 ds/file-stream.c ds/hash-table.c ds/managed-buffer.c \
		 ds/ring-buffer.c ds/slice.c
posix_src = posix/directory-walker.c posix/env.c posix/exec.c \
			posix/files.c posix/process.c posix/subprocess.c
pthreads_src = pthreads/thread.c

libcork_la_SOURCES = $(cli_src) $(core_src) $(ds_src) \
					 $(posix_src) $(pthreads_src)
libcork_la_CFLAGS = -I$(top_srcdir)/libcork/include -DCORK_API=CORK_LOCAL
					 
libcork_la_LDFLAGS = -static

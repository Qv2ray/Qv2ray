# Build args

if (${with_crypto_library} STREQUAL "openssl")
    find_package(ZLIB REQUIRED)
    find_package(OpenSSL REQUIRED)
    set(USE_CRYPTO_OPENSSL 1)
    set(LIBCRYPTO
            ${ZLIB_LIBRARIES}
            ${OPENSSL_CRYPTO_LIBRARY})

    include_directories(${ZLIB_INCLUDE_DIR})
    include_directories(${OPENSSL_INCLUDE_DIR})

    list ( APPEND CMAKE_REQUIRED_INCLUDES ${ZLIB_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

elseif(${with_crypto_library} STREQUAL "polarssl")
    find_package(polarssl REQUIRED)
    set(USE_CRYPTO_POLARSSL 1)
elseif(${with_crypto_library} STREQUAL "mbedtls")
    find_package(mbedtls REQUIRED)
    set(USE_CRYPTO_MBEDTLS 1)
endif()

find_package(PCRE REQUIRED)
include_directories(${PCRE_INCLUDE_DIR})
list ( APPEND CMAKE_REQUIRED_INCLUDES ${PCRE_INCLUDE_DIR})


# Platform checks
include ( CheckFunctionExists )
include ( CheckIncludeFiles )
include ( CheckSymbolExists )
include ( CheckCSourceCompiles )
include ( CheckTypeSize )
include ( CheckSTDC )

check_include_files ( "arpa/inet.h" HAVE_ARPA_INET_H )
check_include_files ( "CommonCrypto/CommonCrypto.h" HAVE_COMMONCRYPTO_COMMONCRYPTO_H )
check_include_files ( dlfcn.h HAVE_DLFCN_H )
check_include_files ( fcntl.h HAVE_FCNTL_H )
check_include_files ( inttypes.h HAVE_INTTYPES_H )
check_include_files ( langinfo.h HAVE_LANGINFO_H )
check_include_files ( limits.h HAVE_LIMITS_H )
check_include_files ( "linux/if.h" HAVE_LINUX_IF_H )
check_include_files ( "linux/netfilter_ipv4.h" HAVE_LINUX_NETFILTER_IPV4_H )
check_include_files ( "linux/netfilter_ipv6/ip6_tables.h" HAVE_LINUX_NETFILTER_IPV6_IP6_TABLES_H )
check_include_files ( locale.h HAVE_LOCALE_H )
check_include_files ( memory.h HAVE_MEMORY_H )
check_include_files ( netdb.h HAVE_NETDB_H )
check_include_files ( "net/if.h" HAVE_NET_IF_H )
check_include_files ( "openssl/engine.h" HAVE_OPENSSL_ENGINE_H )
check_include_files ( "openssl/err.h" HAVE_OPENSSL_ERR_H )
check_include_files ( "openssl/evp.h" HAVE_OPENSSL_EVP_H )
check_include_files ( "openssl/pem.h" HAVE_OPENSSL_PEM_H )
check_include_files ( "openssl/rand.h" HAVE_OPENSSL_RAND_H )
check_include_files ( "openssl/rsa.h" HAVE_OPENSSL_RSA_H )
check_include_files ( "openssl/sha.h" HAVE_OPENSSL_SHA_H )
check_include_files ( pcre.h HAVE_PCRE_H )
check_include_files ( "pcre/pcre.h" HAVE_PCRE_PCRE_H )
check_include_files ( poll.h HAVE_POLL_H )
check_include_files ( port.h HAVE_PORT_H )
check_include_files ( stdint.h HAVE_STDINT_H )
check_include_files ( stdlib.h HAVE_STDLIB_H )
check_include_files ( strings.h HAVE_STRINGS_H )
check_include_files ( string.h HAVE_STRING_H )
check_include_files ( "sys/epoll.h" HAVE_SYS_EPOLL_H )
check_include_files ( "sys/eventfd.h" HAVE_SYS_EVENTFD_H )
check_include_files ( "sys/event.h" HAVE_SYS_EVENT_H )
check_include_files ( "sys/inotify.h" HAVE_SYS_INOTIFY_H )
check_include_files ( "sys/ioctl.h" HAVE_SYS_IOCTL_H )
check_include_files ( "sys/select.h" HAVE_SYS_SELECT_H )
check_include_files ( "sys/signalfd.h" HAVE_SYS_SIGNALFD_H )
check_include_files ( "sys/socket.h" HAVE_SYS_SOCKET_H )
check_include_files ( "sys/stat.h" HAVE_SYS_STAT_H )
check_include_files ( "sys/types.h" HAVE_SYS_TYPES_H )
check_include_files ( "sys/wait.h" HAVE_SYS_WAIT_H )
check_include_files ( unistd.h HAVE_UNISTD_H )
check_include_files ( vfork.h HAVE_VFORK_H )
check_include_files ( windows.h HAVE_WINDOWS_H )
check_include_files ( winsock2.h HAVE_WINSOCK2_H )
check_include_files ( ws2tcpip.h HAVE_WS2TCPIP_H )
check_include_files ( zlib.h HAVE_ZLIB_H )
check_include_files ( "sys/syscall.h" HAVE_SYS_CALL_H )
check_include_files ( "minix/config.h" _MINIX)

check_function_exists ( CCCryptorCreateWithMode HAVE_CCCRYPTORCREATEWITHMODE )
check_function_exists ( clock_gettime HAVE_CLOCK_GETTIME )
check_function_exists ( epoll_ctl HAVE_EPOLL_CTL )
check_function_exists ( eventfd HAVE_EVENTFD )
check_function_exists ( EVP_EncryptInit_ex HAVE_EVP_ENCRYPTINIT_EX )
check_function_exists ( floor HAVE_FLOOR )
check_function_exists ( fork HAVE_FORK )
check_function_exists ( getpwnam_r HAVE_GETPWNAM_R )
check_function_exists ( inet_ntop HAVE_INET_NTOP )
check_function_exists ( inotify_init HAVE_INOTIFY_INIT )
check_function_exists ( kqueue HAVE_KQUEUE )
check_function_exists ( malloc HAVE_MALLOC )
check_function_exists ( memset HAVE_MEMSET )
check_function_exists ( nanosleep HAVE_NANOSLEEP )
check_function_exists ( poll HAVE_POLL )
check_function_exists ( port_create HAVE_PORT_CREATE )
check_function_exists ( RAND_pseudo_bytes HAVE_RAND_PSEUDO_BYTES )
check_function_exists ( select HAVE_SELECT )
check_function_exists ( setresuid HAVE_SETRESUID )
check_function_exists ( setreuid HAVE_SETREUID )
check_function_exists ( setrlimit HAVE_SETRLIMIT )
check_function_exists ( signalfd HAVE_SIGNALFD )
check_function_exists ( socket HAVE_SOCKET )
check_function_exists ( strerror HAVE_STRERROR )
check_function_exists ( vfork HAVE_VFORK )
check_function_exists ( inet_ntop HAVE_DECL_INET_NTOP )

check_symbol_exists ( PTHREAD_PRIO_INHERIT "pthread.h" HAVE_PTHREAD_PRIO_INHERIT )
check_symbol_exists ( PTHREAD_CREATE_JOINABLE "pthread.h" HAVE_PTHREAD_CREATE_JOINABLE )
check_symbol_exists ( EINPROGRESS "sys/errno.h" HAVE_EINPROGRESS )
check_symbol_exists ( WSAEWOULDBLOCK "winerror.h" HAVE_WSAEWOULDBLOCK )

# winsock2.h and ws2_32 should provide these
if(HAVE_WINSOCK2_H)
    set(HAVE_GETHOSTNAME ON)
    set(HAVE_SELECT ON)
    set(HAVE_SOCKET ON)
    set(HAVE_INET_NTOA ON)
    set(HAVE_RECV ON)
    set(HAVE_SEND ON)
    set(HAVE_RECVFROM ON)
    set(HAVE_SENDTO ON)
    set(HAVE_GETHOSTBYNAME ON)
    set(HAVE_GETSERVBYNAME ON)
else(HAVE_WINSOCK2_H)
    check_function_exists(gethostname HAVE_GETHOSTNAME)
    check_function_exists(select HAVE_SELECT)
    check_function_exists(socket HAVE_SOCKET)
    check_function_exists(inet_ntoa HAVE_INET_NTOA)
    check_function_exists(recv HAVE_RECV)
    check_function_exists(send HAVE_SEND)
    check_function_exists(recvfrom HAVE_RECVFROM)
    check_function_exists(sendto HAVE_SENDTO)
    check_function_exists(gethostbyname HAVE_GETHOSTBYNAME)
    check_function_exists(getservbyname HAVE_GETSERVBYNAME)
endif(HAVE_WINSOCK2_H)

find_library ( HAVE_LIBPCRE pcre )
find_library ( HAVE_LIBRT rt )
find_library ( HAVE_LIBSOCKET socket )

check_c_source_compiles(
        "
#include <sys/time.h>
#include <time.h>
int main(int argc, char** argv) {return 0;}
"
        TIME_WITH_SYS_TIME
)

check_c_source_compiles("
__thread int tls;

int main(void) {
    return 0;
}" TLS)

check_type_size(pid_t PID_T)

# Tweaks
if (${HAVE_SYS_CALL_H})
    set(HAVE_CLOCK_SYSCALL ${HAVE_CLOCK_GETTIME})
endif ()
if (ZLIB_FOUND)
    set (HAVE_ZLIB 1)
endif()

if (NOT HAVE_DECL_INET_NTOP)
    set(HAVE_DECL_INET_NTOP 0)
endif()
if (NOT HAVE_PTHREAD_CREATE_JOINABLE)
    set (PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_UNDETACHED)
endif()
if (${_MINIX})
    set (_POSIX_1_SOURCE 2)
    set (_POSIX_SOURCE 1)
endif()

if (${HAVE_EINPROGRESS})
    set (CONNECT_IN_PROGRESS EINPROGRESS)
elseif(${HAVE_WSAEWOULDBLOCK})
    set (CONNECT_IN_PROGRESS WSAEWOULDBLOCK)
endif()

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")



set (HAVE_IPv6 1)

ADD_DEFINITIONS(-DHAVE_CONFIG_H)
ADD_DEFINITIONS(-D__USE_MINGW_ANSI_STDIO=1)

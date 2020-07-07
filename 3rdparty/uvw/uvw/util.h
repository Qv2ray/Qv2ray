#ifndef UVW_UTIL_INCLUDE_H
#define UVW_UTIL_INCLUDE_H


#include <string_view>
#include <type_traits>
#include <cstddef>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <uv.h>


namespace uvw {


namespace details {


enum class UVHandleType: std::underlying_type_t<uv_handle_type> {
    UNKNOWN = UV_UNKNOWN_HANDLE,
    ASYNC = UV_ASYNC,
    CHECK = UV_CHECK,
    FS_EVENT = UV_FS_EVENT,
    FS_POLL = UV_FS_POLL,
    HANDLE = UV_HANDLE,
    IDLE = UV_IDLE,
    PIPE = UV_NAMED_PIPE,
    POLL = UV_POLL,
    PREPARE = UV_PREPARE,
    PROCESS = UV_PROCESS,
    STREAM = UV_STREAM,
    TCP = UV_TCP,
    TIMER = UV_TIMER,
    TTY = UV_TTY,
    UDP = UV_UDP,
    SIGNAL = UV_SIGNAL,
    FILE = UV_FILE
};


template<typename T>
struct UVTypeWrapper {
    using Type = T;

    constexpr UVTypeWrapper(): value{} {}
    constexpr UVTypeWrapper(Type val): value{val} {}

    constexpr operator Type() const noexcept { return value; }

    bool operator==(UVTypeWrapper other) const noexcept {
        return value == other.value;
    }

private:
    const Type value;
};


template<typename T>
bool operator==(UVTypeWrapper<T> lhs, UVTypeWrapper<T> rhs) {
    return !(lhs == rhs);
}


}


/**
 * @brief Utility class to handle flags.
 *
 * This class can be used to handle flags of a same enumeration type.<br/>
 * It is meant to be used as an argument for functions and member methods and
 * as part of events.<br/>
 * `Flags<E>` objects can be easily _or-ed_ and _and-ed_ with other instances of
 * the same type or with instances of the type `E` (that is, the actual flag
 * type), thus converted to the underlying type when needed.
 */
template<typename E>
class Flags final {
    using InnerType = std::underlying_type_t<E>;

    constexpr InnerType toInnerType(E flag) const noexcept { return static_cast<InnerType>(flag); }

public:
    using Type = InnerType;

    /**
     * @brief Utility factory method to pack a set of values all at once.
     * @return A valid instance of Flags instantiated from values `V`.
     */
    template<E... V>
    static constexpr Flags<E> from() {
        return (Flags<E>{} | ... | V);
    }

    /**
     * @brief Constructs a Flags object from a value of the enum `E`.
     * @param flag A value of the enum `E`.
     */
    constexpr Flags(E flag) noexcept: flags{toInnerType(flag)} {}

    /**
     * @brief Constructs a Flags object from an instance of the underlying type
     * of the enum `E`.
     * @param f An instance of the underlying type of the enum `E`.
     */
    constexpr Flags(Type f): flags{f} {}

    /**
     * @brief Constructs an uninitialized Flags object.
     */
    constexpr Flags(): flags{} {}

    constexpr Flags(const Flags &f) noexcept: flags{f.flags} {  }
    constexpr Flags(Flags &&f) noexcept: flags{std::move(f.flags)} {  }

    ~Flags() noexcept { static_assert(std::is_enum_v<E>); }

    constexpr Flags & operator=(const Flags &f) noexcept {
        flags = f.flags;
        return *this;
    }

    constexpr Flags & operator=(Flags &&f) noexcept {
        flags = std::move(f.flags);
        return *this;
    }

    /**
     * @brief Or operator.
     * @param f A valid instance of Flags.
     * @return This instance _or-ed_ with `f`.
     */
    constexpr Flags operator|(const Flags &f) const noexcept { return Flags{flags | f.flags}; }

    /**
     * @brief Or operator.
     * @param flag A value of the enum `E`.
     * @return This instance _or-ed_ with `flag`.
     */
    constexpr Flags operator|(E flag) const noexcept { return Flags{flags | toInnerType(flag)}; }

    /**
     * @brief And operator.
     * @param f A valid instance of Flags.
     * @return This instance _and-ed_ with `f`.
     */
    constexpr Flags operator&(const Flags &f) const noexcept { return Flags{flags & f.flags}; }

    /**
     * @brief And operator.
     * @param flag A value of the enum `E`.
     * @return This instance _and-ed_ with `flag`.
     */
    constexpr Flags operator&(E flag) const noexcept { return Flags{flags & toInnerType(flag)}; }

    /**
     * @brief Checks if this instance is initialized.
     * @return False if it's uninitialized, true otherwise.
     */
    explicit constexpr operator bool() const noexcept { return !(flags == InnerType{}); }

    /**
     * @brief Casts the instance to the underlying type of `E`.
     * @return An integral representation of the contained flags.
     */
    constexpr operator Type() const noexcept { return flags; }

private:
    InnerType flags;
};


/**
 * @brief Windows size representation.
 */
struct WinSize {
    int width; /*!< The _width_ of the given window. */
    int height; /*!< The _height_ of the given window. */
};


using HandleType = details::UVHandleType; /*!< The type of a handle. */

using HandleCategory = details::UVTypeWrapper<uv_handle_type>; /*!< Utility class that wraps an internal handle type. */
using FileHandle = details::UVTypeWrapper<uv_file>; /*!< Utility class that wraps an internal file handle. */
using OSSocketHandle = details::UVTypeWrapper<uv_os_sock_t>; /*!< Utility class that wraps an os socket handle. */
using OSFileDescriptor = details::UVTypeWrapper<uv_os_fd_t>; /*!< Utility class that wraps an os file descriptor. */
using PidType = details::UVTypeWrapper<uv_pid_t>; /*!< Utility class that wraps a cross platform representation of a pid. */

constexpr FileHandle StdIN{0}; /*!< Placeholder for stdin descriptor. */
constexpr FileHandle StdOUT{1}; /*!< Placeholder for stdout descriptor. */
constexpr FileHandle StdERR{2}; /*!< Placeholder for stderr descriptor. */

using TimeSpec = uv_timespec_t; /*!< Library equivalent for uv_timespec_t. */
using Stat = uv_stat_t; /*!< Library equivalent for uv_stat_t. */
using Statfs = uv_statfs_t; /*!< Library equivalent for uv_statfs_t. */
using Uid = uv_uid_t; /*!< Library equivalent for uv_uid_t. */
using Gid = uv_gid_t; /*!< Library equivalent for uv_gid_t. */

using TimeVal = uv_timeval_t; /*!< Library equivalent for uv_timeval_t. */
using TimeVal64 = uv_timeval64_t; /*!< Library equivalent for uv_timeval64_t. */
using RUsage = uv_rusage_t; /*!< Library equivalent for uv_rusage_t. */


/**
 * @brief Utility class.
 *
 * This class can be used to query the subset of the password file entry for the
 * current effective uid (not the real uid).
 *
 * \sa Utilities::passwd
 */
struct Passwd {
    Passwd(std::shared_ptr<uv_passwd_t> pwd);

    /**
     * @brief Gets the username.
     * @return The username of the current effective uid (not the real uid).
     */
    std::string username() const noexcept;

    /**
     * @brief Gets the uid.
     * @return The current effective uid (not the real uid).
     */
    decltype(uv_passwd_t::uid) uid() const noexcept;

    /**
     * @brief Gets the gid.
     * @return The gid of the current effective uid (not the real uid).
     */
    decltype(uv_passwd_t::gid) gid() const noexcept;

    /**
     * @brief Gets the shell.
     * @return The shell of the current effective uid (not the real uid).
     */
    std::string shell() const noexcept;

    /**
     * @brief Gets the homedir.
     * @return The homedir of the current effective uid (not the real uid).
     */
    std::string homedir() const noexcept;

    /**
     * @brief Checks if the instance contains valid data.
     * @return True if data are all valid, false otherwise.
     */
    operator bool() const noexcept;

private:
    std::shared_ptr<uv_passwd_t> passwd;
};


/**
 * @brief Utility class.
 *
 * This class can be used to get name and information about the current kernel.
 * The populated data includes the operating system name, release, version, and
 * machine.
 *
 * \sa Utilities::uname
 */
struct UtsName {
    UtsName(std::shared_ptr<uv_utsname_t> utsname);

    /**
     * @brief Gets the operating system name (like "Linux").
     * @return The operating system name.
     */
    std::string sysname() const noexcept;

    /**
     * @brief Gets the operating system release (like "2.6.28").
     * @return The operating system release.
     */
    std::string release() const noexcept;

    /**
     * @brief Gets the operating system version.
     * @return The operating system version
     */
    std::string version() const noexcept;

    /**
     * @brief Gets the hardware identifier.
     * @return The hardware identifier.
     */
    std::string machine() const noexcept;

private:
    std::shared_ptr<uv_utsname_t> utsname;
};


/**
 * @brief The IPv4 tag.
 *
 * To be used as template parameter to switch between IPv4 and IPv6.
 */
struct IPv4 {};


/**
 * @brief The IPv6 tag.
 *
 * To be used as template parameter to switch between IPv4 and IPv6.
 */
struct IPv6 {};


/**
 * @brief Address representation.
 */
struct Addr {
    std::string ip; /*!< Either an IPv4 or an IPv6. */
    unsigned int port; /*!< A valid service identifier. */
};


/**
 * \brief CPU information.
 */
struct CPUInfo {
    using CPUTime = decltype(uv_cpu_info_t::cpu_times);

    std::string model; /*!< The model of the CPU. */
    int speed; /*!< The frequency of the CPU. */

    /**
     * @brief CPU times.
     *
     * It is built up of the following data members: `user`, `nice`, `sys`,
     * `idle`, `irq`, all of them having type `uint64_t`.
     */
    CPUTime times;
};


/**
 * \brief Interface address.
 */
struct InterfaceAddress {
    std::string name; /*!< The name of the interface (as an example _eth0_). */
    char physical[6]; /*!< The physical address. */
    bool internal; /*!< True if it is an internal interface (as an example _loopback_), false otherwise. */
    Addr address; /*!< The address of the given interface. */
    Addr netmask; /*!< The netmask of the given interface. */
};


namespace details {


static constexpr std::size_t DEFAULT_SIZE = 128;


template<typename>
struct IpTraits;


template<>
struct IpTraits<IPv4> {
    using Type = sockaddr_in;
    using AddrFuncType = int(*)(const char *, int, Type *);
    using NameFuncType = int(*)(const Type *, char *, std::size_t);
    static constexpr AddrFuncType addrFunc = &uv_ip4_addr;
    static constexpr NameFuncType nameFunc = &uv_ip4_name;
    static constexpr auto sinPort(const Type *addr) { return addr->sin_port; }
};


template<>
struct IpTraits<IPv6> {
    using Type = sockaddr_in6;
    using AddrFuncType = int(*)(const char *, int, Type *);
    using NameFuncType = int(*)(const Type *, char *, std::size_t);
    static constexpr AddrFuncType addrFunc = &uv_ip6_addr;
    static constexpr NameFuncType nameFunc = &uv_ip6_name;
    static constexpr auto sinPort(const Type *addr) { return addr->sin6_port; }
};


template<typename I>
Addr address(const typename details::IpTraits<I>::Type *aptr) noexcept {
    Addr addr;
    char name[DEFAULT_SIZE];

    int err = details::IpTraits<I>::nameFunc(aptr, name, DEFAULT_SIZE);

    if(0 == err) {
        addr.port = ntohs(details::IpTraits<I>::sinPort(aptr));
        addr.ip = std::string{name};
    }

    return addr;
}


template<typename I, typename F, typename H>
Addr address(F &&f, const H *handle) noexcept {
    sockaddr_storage ssto;
    int len = sizeof(ssto);
    Addr addr{};

    int err = std::forward<F>(f)(handle, reinterpret_cast<sockaddr *>(&ssto), &len);

    if(0 == err) {
        typename IpTraits<I>::Type *aptr = reinterpret_cast<typename IpTraits<I>::Type *>(&ssto);
        addr = address<I>(aptr);
    }

    return addr;
}


template<typename F, typename... Args>
std::string tryRead(F &&f, Args&&... args) noexcept {
    std::size_t size = DEFAULT_SIZE;
    char buf[DEFAULT_SIZE];
    std::string str{};
    auto err = std::forward<F>(f)(args..., buf, &size);

    if(UV_ENOBUFS == err) {
        std::unique_ptr<char[]> data{new char[size]};
        err = std::forward<F>(f)(args..., data.get(), &size);

        if(0 == err) {
            str = data.get();
        }
    } else if(0 == err) {
        str.assign(buf, size);
    }

    return str;
}


}


/**
 * @brief Miscellaneous utilities.
 *
 * Miscellaneous functions that don’t really belong to any other class.
 */
struct Utilities {
    using MallocFuncType = void*(*)(size_t);
    using ReallocFuncType = void*(*)(void*, size_t);
    using CallocFuncType = void*(*)(size_t, size_t);
    using FreeFuncType = void(*)(void*);

    /**
     * @brief OS dedicated utilities.
     */
    struct OS {
        /**
         * @brief Returns the current process id.
         *
         * See the official
         * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_os_getpid)
         * for further details.
         *
         * @return The current process id.
         */
        static PidType pid() noexcept;

        /**
         * @brief Returns the parent process id.
         *
         * See the official
         * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_os_getppid)
         * for further details.
         *
         * @return The parent process id.
         */
        static PidType parent() noexcept;

        /**
         * @brief Gets the current user's home directory.
         *
         * See the official
         * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_os_homedir)
         * for further details.
         *
         * @return The current user's home directory, an empty string in case of
         * errors.
         */
        static std::string homedir() noexcept;

        /**
         * @brief Gets the temp directory.
         *
         * See the official
         * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_os_tmpdir)
         * for further details.
         *
         * @return The temp directory, an empty string in case of errors.
         */
        static std::string tmpdir() noexcept;

        /**
         * @brief Retrieves an environment variable.
         * @param name The name of the variable to be retrieved.
         * @return The value of the environment variable, an empty string in
         * case of errors.
         */
        static std::string env(const std::string &name) noexcept;

        /**
         * @brief Creates, updates or deletes an environment variable.
         * @param name The name of the variable to be updated.
         * @param value The value to be used for the variable (an empty string
         * to unset it).
         * @return True in case of success, false otherwise.
         */
        static bool env(const std::string &name, const std::string &value) noexcept;

        /**
         * @brief Retrieves all environment variables and iterates them.
         *
         * Environment variables are passed one at a time to the callback in the
         * form of `std::string_view`s.<br/>
         * The signature of the function call operator must be such that it
         * accepts two parameters, the name and the value of the i-th variable.
         *
         * @tparam Func Type of a function object to which to pass environment
         * variables.
         * @param func A function object to which to pass environment variables.
         * @return True in case of success, false otherwise.
         */
        template<typename Func>
        static std::enable_if_t<std::is_invocable_v<Func, std::string_view, std::string_view>, bool>
        env(Func func) noexcept {
            uv_env_item_t *items = nullptr;
            int count{};

            const bool ret = (uv_os_environ(&items, &count) == 0);

            if(ret) {
                for(int pos = 0; pos < count; ++pos) {
                    func(std::string_view{items[pos].name}, std::string_view{items[pos].value});
                }

                uv_os_free_environ(items, count);
            }

            return ret;
        }

        /**
         * @brief Returns the hostname.
         * @return The hostname, an empty string in case of errors.
         */
        static std::string hostname() noexcept;

        /**
         * @brief Gets name and information about the current kernel.
         *
         * This function can be used to get name and information about the
         * current kernel. The populated data includes the operating system
         * name, release, version, and machine.
         *
         * @return Name and information about the current kernel.
         */
        static UtsName uname() noexcept;

        /**
         * @brief Gets a subset of the password file entry.
         *
         * This function can be used to get the subset of the password file
         * entry for the current effective uid (not the real uid).
         *
         * See the official
         * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_os_get_passwd)
         * for further details.
         *
         * @return The accessible subset of the password file entry.
         */
        static Passwd passwd() noexcept;
    };

    /**
     * @brief Retrieves the scheduling priority of a process.
     *
     * The returned value is between -20 (high priority) and 19 (low priority).
     * A value that is out of range is returned in case of errors.
     *
     * @note
     * On Windows, the result won't equal necessarily the exact value of the
     * priority because of a mapping to a Windows priority class.
     *
     * @param pid A valid process id.
     * @return The scheduling priority of the process.
     */
    static int osPriority(PidType pid);

    /**
     * @brief Sets the scheduling priority of a process.
     *
     * The returned value range is between -20 (high priority) and 19 (low
     * priority).
     *
     * @note
     * On Windows, the priority is mapped to a Windows priority class. When
     * retrieving the process priority, the result won't equal necessarily the
     * exact value of the priority.
     *
     * @param pid A valid process id.
     * @param prio The scheduling priority to set to the process.
     * @return True in case of success, false otherwise.
     */
    static bool osPriority(PidType pid, int prio);

    /**
     * @brief Gets the type of the handle given a category.
     * @param category A properly initialized handle category.
     * @return The actual type of the handle as defined by HandleType
     */
    static HandleType guessHandle(HandleCategory category) noexcept;

    /**
     * @brief Gets the type of the stream to be used with the given descriptor.
     *
     * Returns the type of stream that should be used with a given file
     * descriptor.<br/>
     * Usually this will be used during initialization to guess the type of the
     * stdio streams.
     *
     * @param file A valid descriptor.
     * @return One of the following types:
     *
     * * `HandleType::UNKNOWN`
     * * `HandleType::PIPE`
     * * `HandleType::TCP`
     * * `HandleType::TTY`
     * * `HandleType::UDP`
     * * `HandleType::FILE`
     */
    static HandleType guessHandle(FileHandle file) noexcept;

    /** @brief Gets information about the CPUs on the system.
     *
     * This function can be used to query the underlying system and get a set of
     * descriptors of all the available CPUs.
     *
     * @return A set of descriptors of all the available CPUs.
     */
    static std::vector<CPUInfo> cpuInfo() noexcept;

    /**
     * @brief Gets a set of descriptors of all the available interfaces.
     *
     * This function can be used to query the underlying system and get a set of
     * descriptors of all the available interfaces, either internal or not.
     *
     * @return A set of descriptors of all the available interfaces.
     */
    static std::vector<InterfaceAddress> interfaceAddresses() noexcept;

    /**
     * @brief IPv6-capable implementation of
     * [if_indextoname](https://linux.die.net/man/3/if_indextoname).
     *
     * Mapping between network interface names and indexes.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_if_indextoname)
     * for further details.
     *
     * @param index Network interface index.
     * @return Network interface name.
     */
    static std::string indexToName(unsigned int index) noexcept;

    /**
     * @brief Retrieves a network interface identifier.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/misc.html#c.uv_if_indextoiid)
     * for further details.
     *
     * @param index Network interface index.
     * @return Network interface identifier.
     */
    static std::string indexToIid(unsigned int index) noexcept;

    /**
     * @brief Override the use of some standard library’s functions.
     *
     * Override the use of the standard library’s memory allocation
     * functions.<br/>
     * This method must be invoked before any other `uvw` function is called or
     * after all resources have been freed and thus the underlying library
     * doesn’t reference any allocated memory chunk.
     *
     * If any of the function pointers is _null_, the invokation will fail.
     *
     * @note
     * There is no protection against changing the allocator multiple times. If
     * the user changes it they are responsible for making sure the allocator is
     * changed while no memory was allocated with the previous allocator, or
     * that they are compatible.
     *
     * @param mallocFunc Replacement function for _malloc_.
     * @param reallocFunc Replacement function for _realloc_.
     * @param callocFunc Replacement function for _calloc_.
     * @param freeFunc Replacement function for _free_.
     * @return True in case of success, false otherwise.
     */
    static bool replaceAllocator(MallocFuncType mallocFunc, ReallocFuncType reallocFunc, CallocFuncType callocFunc, FreeFuncType freeFunc) noexcept;

    /**
     * @brief Gets the load average.
     * @return `[0,0,0]` on Windows (not available), the load average otherwise.
     */
    static std::array<double, 3> loadAverage() noexcept;

    /**
     * @brief Store the program arguments.
     *
     * Required for getting / setting the process title.
     *
     * @return Arguments that haven't been consumed internally.
     */
    static char ** setupArgs(int argc, char** argv);

    /**
     * @brief Gets the title of the current process.
     * @return The process title.
     */
    static std::string processTitle();

    /**
     * @brief Sets the current process title.
     * @param title The process title to be set.
     * @return True in case of success, false otherwise.
     */
    static bool processTitle(std::string title);

    /**
     * @brief Gets memory information (in bytes).
     * @return Memory information.
     */
    static uint64_t totalMemory() noexcept;

    /**
     * @brief Gets the amount of memory available to the process (in bytes).
     *
     * Gets the amount of memory available to the process based on limits
     * imposed by the OS. If there is no such constraint, or the constraint is
     * unknown, `0` is returned.<br/>
     * Note that it is not unusual for this value to be less than or greater
     * than `totalMemory`.
     *
     * @return Amount of memory available to the process.
     */
    static uint64_t constrainedMemory() noexcept;

    /**
     * @brief Gets the current system uptime.
     * @return The current system uptime or 0 in case of errors.
     */
    static double uptime() noexcept;

    /**
     * @brief Gets the resource usage measures for the current process.
     * @return Resource usage measures, zeroes-filled object in case of errors.
     */
    static RUsage rusage() noexcept;

    /**
     * @brief Gets the current high-resolution real time.
     *
     * The time is expressed in nanoseconds. It is relative to an arbitrary time
     * in the past. It is not related to the time of the day and therefore not
     * subject to clock drift. The primary use is for measuring performance
     * between interval.
     *
     * @return The current high-resolution real time.
     */
    static uint64_t hrtime() noexcept;

    /**
     * @brief Gets the executable path.
     * @return The executable path, an empty string in case of errors.
     */
    static std::string path() noexcept;

    /**
     * @brief Gets the current working directory.
     * @return The current working directory, an empty string in case of errors.
     */
    static std::string cwd() noexcept;

    /**
     * @brief Changes the current working directory.
     * @param dir The working directory to be set.
     * @return True in case of success, false otherwise.
     */
    static bool chdir(const std::string &dir) noexcept;

    /**
     * @brief Cross-platform implementation of
     * [`gettimeofday`](https://linux.die.net/man/2/gettimeofday)
     * @return The current time.
     */
    static TimeVal64 timeOfDay() noexcept;

    /**
     * @brief Causes the calling thread to sleep for a while.
     * @param msec Number of milliseconds to sleep.
     */
    static void sleep(unsigned int msec) noexcept;
};


}


#ifndef UVW_AS_LIB
#include "util.cpp"
#endif

#endif // UVW_UTIL_INCLUDE_H

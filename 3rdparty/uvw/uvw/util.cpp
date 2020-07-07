#ifdef UVW_AS_LIB
#include "util.h"
#endif

#include <algorithm>

#include "config.h"


namespace uvw {


UVW_INLINE Passwd::Passwd(std::shared_ptr<uv_passwd_t> pwd)
    : passwd{pwd}
{}


UVW_INLINE std::string Passwd::username() const noexcept {
    return ((passwd && passwd->username) ? passwd->username : "");
}


UVW_INLINE decltype(uv_passwd_t::uid) Passwd::uid() const noexcept {
    return (passwd ? passwd->uid : decltype(uv_passwd_t::uid){});
}


UVW_INLINE decltype(uv_passwd_t::gid) Passwd::gid() const noexcept {
    return (passwd ?  passwd->gid : decltype(uv_passwd_t::gid){});
}


UVW_INLINE std::string Passwd::shell() const noexcept {
    return ((passwd && passwd->shell) ? passwd->shell : "");
}


UVW_INLINE std::string Passwd::homedir() const noexcept {
    return ((passwd && passwd->homedir) ? passwd->homedir: "");
}


UVW_INLINE Passwd::operator bool() const noexcept {
    return static_cast<bool>(passwd);
}


UVW_INLINE UtsName::UtsName(std::shared_ptr<uv_utsname_t> utsname)
    : utsname{utsname}
{}


UVW_INLINE std::string UtsName::sysname() const noexcept {
    return utsname ? utsname->sysname : "";
}


UVW_INLINE std::string UtsName::release() const noexcept {
    return utsname ? utsname->release : "";
}


UVW_INLINE std::string UtsName::version() const noexcept {
    return utsname ? utsname->version : "";
}


UVW_INLINE std::string UtsName::machine() const noexcept {
    return utsname ? utsname->machine : "";
}


UVW_INLINE PidType Utilities::OS::pid() noexcept {
    return uv_os_getpid();
}


UVW_INLINE PidType Utilities::OS::parent() noexcept {
    return uv_os_getppid();
}


UVW_INLINE std::string Utilities::OS::homedir() noexcept {
    return details::tryRead(&uv_os_homedir);
}


UVW_INLINE std::string Utilities::OS::tmpdir() noexcept {
    return details::tryRead(&uv_os_tmpdir);
}


UVW_INLINE std::string Utilities::OS::env(const std::string &name) noexcept {
    return details::tryRead(&uv_os_getenv, name.c_str());
}


UVW_INLINE bool Utilities::OS::env(const std::string &name, const std::string &value) noexcept {
    return (0 == (value.empty() ? uv_os_unsetenv(name.c_str()) : uv_os_setenv(name.c_str(), value.c_str())));
}


UVW_INLINE std::string Utilities::OS::hostname() noexcept {
    return details::tryRead(&uv_os_gethostname);
}


UVW_INLINE UtsName Utilities::OS::uname() noexcept {
    auto ptr = std::make_shared<uv_utsname_t>();
    uv_os_uname(ptr.get());
    return ptr;
}


UVW_INLINE Passwd Utilities::OS::passwd() noexcept {
    auto deleter = [](uv_passwd_t *passwd){
        uv_os_free_passwd(passwd);
        delete passwd;
    };

    std::shared_ptr<uv_passwd_t> ptr{new uv_passwd_t, std::move(deleter)};
    uv_os_get_passwd(ptr.get());
    return ptr;
}


UVW_INLINE int Utilities::osPriority(PidType pid) {
    int prio = 0;

    if(uv_os_getpriority(pid, &prio)) {
        prio = UV_PRIORITY_LOW + 1;
    }

    return prio;
}


UVW_INLINE bool Utilities::osPriority(PidType pid, int prio) {
    return 0 == uv_os_setpriority(pid, prio);
}


UVW_INLINE HandleType Utilities::guessHandle(HandleCategory category) noexcept {
    switch(category) {
    case UV_ASYNC:
        return HandleType::ASYNC;
    case UV_CHECK:
        return HandleType::CHECK;
    case UV_FS_EVENT:
        return HandleType::FS_EVENT;
    case UV_FS_POLL:
        return HandleType::FS_POLL;
    case UV_HANDLE:
        return HandleType::HANDLE;
    case UV_IDLE:
        return HandleType::IDLE;
    case UV_NAMED_PIPE:
        return HandleType::PIPE;
    case UV_POLL:
        return HandleType::POLL;
    case UV_PREPARE:
        return HandleType::PREPARE;
    case UV_PROCESS:
        return HandleType::PROCESS;
    case UV_STREAM:
        return HandleType::STREAM;
    case UV_TCP:
        return HandleType::TCP;
    case UV_TIMER:
        return HandleType::TIMER;
    case UV_TTY:
        return HandleType::TTY;
    case UV_UDP:
        return HandleType::UDP;
    case UV_SIGNAL:
        return HandleType::SIGNAL;
    case UV_FILE:
        return HandleType::FILE;
    default:
        return HandleType::UNKNOWN;
    }
}


UVW_INLINE HandleType Utilities::guessHandle(FileHandle file) noexcept {
    HandleCategory category = uv_guess_handle(file);
    return guessHandle(category);
}


UVW_INLINE std::vector<CPUInfo> Utilities::cpuInfo() noexcept {
    std::vector<CPUInfo> cpuinfos;

    uv_cpu_info_t *infos;
    int count;

    if(0 == uv_cpu_info(&infos, &count)) {
        std::for_each(infos, infos+count, [&cpuinfos](const auto &info) {
            cpuinfos.push_back({ info.model, info.speed, info.cpu_times });
        });

        uv_free_cpu_info(infos, count);
    }

    return cpuinfos;
}


UVW_INLINE std::vector<InterfaceAddress> Utilities::interfaceAddresses() noexcept {
    std::vector<InterfaceAddress> interfaces;

    uv_interface_address_t *ifaces{nullptr};
    int count{0};

    if(0 == uv_interface_addresses(&ifaces, &count)) {
        std::for_each(ifaces, ifaces+count, [&interfaces](const auto &iface) {
            InterfaceAddress interfaceAddress;

            interfaceAddress.name = iface.name;
            std::copy(iface.phys_addr, (iface.phys_addr+6), interfaceAddress.physical);
            interfaceAddress.internal = iface.is_internal == 0 ? false : true;

            if(iface.address.address4.sin_family == AF_INET) {
                interfaceAddress.address = details::address<IPv4>(&iface.address.address4);
                interfaceAddress.netmask = details::address<IPv4>(&iface.netmask.netmask4);
            } else if(iface.address.address4.sin_family == AF_INET6) {
                interfaceAddress.address = details::address<IPv6>(&iface.address.address6);
                interfaceAddress.netmask = details::address<IPv6>(&iface.netmask.netmask6);
            }

            interfaces.push_back(std::move(interfaceAddress));
        });

        uv_free_interface_addresses(ifaces, count);
    }

    return interfaces;
}


UVW_INLINE std::string Utilities::indexToName(unsigned int index) noexcept {
    return details::tryRead(&uv_if_indextoname, index);
}


UVW_INLINE std::string Utilities::indexToIid(unsigned int index) noexcept {
    return details::tryRead(&uv_if_indextoiid, index);
}


UVW_INLINE bool Utilities::replaceAllocator(MallocFuncType mallocFunc, ReallocFuncType reallocFunc, CallocFuncType callocFunc, FreeFuncType freeFunc) noexcept {
    return (0 == uv_replace_allocator(mallocFunc, reallocFunc, callocFunc, freeFunc));
}


UVW_INLINE std::array<double, 3> Utilities::loadAverage() noexcept {
    std::array<double, 3> avg;
    uv_loadavg(avg.data());
    return avg;
}


UVW_INLINE char ** Utilities::setupArgs(int argc, char** argv) {
    return uv_setup_args(argc, argv);
}


UVW_INLINE std::string Utilities::processTitle() {
    std::size_t size = details::DEFAULT_SIZE;
    char buf[details::DEFAULT_SIZE];
    std::string str{};

    if(0 == uv_get_process_title(buf, size)) {
        str.assign(buf, size);
    }

    return str;
}


UVW_INLINE bool Utilities::processTitle(std::string title) {
    return (0 == uv_set_process_title(title.c_str()));
}


UVW_INLINE uint64_t Utilities::totalMemory() noexcept {
    return uv_get_total_memory();
}


UVW_INLINE uint64_t Utilities::constrainedMemory() noexcept {
    return uv_get_constrained_memory();
}


UVW_INLINE double Utilities::uptime() noexcept {
    double ret;

    if(0 != uv_uptime(&ret)) {
        ret = 0;
    }

    return ret;
}


UVW_INLINE RUsage Utilities::rusage() noexcept {
    RUsage ru;
    auto err = uv_getrusage(&ru);
    return err ? RUsage{} : ru;
}


UVW_INLINE uint64_t Utilities::hrtime() noexcept {
    return uv_hrtime();
}


UVW_INLINE std::string Utilities::path() noexcept {
    return details::tryRead(&uv_exepath);
}


UVW_INLINE std::string Utilities::cwd() noexcept {
    return details::tryRead(&uv_cwd);
}


UVW_INLINE bool Utilities::chdir(const std::string &dir) noexcept {
    return (0 == uv_chdir(dir.data()));
}


UVW_INLINE TimeVal64 Utilities::timeOfDay() noexcept {
    uv_timeval64_t ret;
    uv_gettimeofday(&ret);
    return ret;
}


UVW_INLINE void Utilities::sleep(unsigned int msec) noexcept {
    uv_sleep(msec);
}


}

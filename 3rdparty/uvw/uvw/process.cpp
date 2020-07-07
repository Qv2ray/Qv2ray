#ifdef UVW_AS_LIB
#include "process.h"
#endif
#include <algorithm>

#include "config.h"


namespace uvw {


UVW_INLINE ExitEvent::ExitEvent(int64_t code, int sig) noexcept
    : status{code}, signal{sig}
{}


UVW_INLINE void ProcessHandle::exitCallback(uv_process_t *handle, int64_t exitStatus, int termSignal) {
    ProcessHandle &process = *(static_cast<ProcessHandle *>(handle->data));
    process.publish(ExitEvent{exitStatus, termSignal});
}


UVW_INLINE ProcessHandle::ProcessHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref)
    : Handle{ca, std::move(ref)}
{}


UVW_INLINE void ProcessHandle::disableStdIOInheritance() noexcept {
    uv_disable_stdio_inheritance();
}


UVW_INLINE bool ProcessHandle::kill(int pid, int signum) noexcept {
    return (0 == uv_kill(pid, signum));
}


UVW_INLINE bool ProcessHandle::init() {
    // deferred initialization: libuv initializes process handles only when
    // uv_spawn is invoked and uvw stays true to the underlying library
    return true;
}


UVW_INLINE void ProcessHandle::spawn(const char *file, char **args, char **env) {
    uv_process_options_t po;

    po.exit_cb = &exitCallback;
    po.file = file;
    po.args = args;
    po.env = env;
    po.cwd = poCwd.empty() ? nullptr : poCwd.data();
    po.flags = poFlags;
    po.uid = poUid;
    po.gid = poGid;

    std::vector<uv_stdio_container_t> poStdio;
    poStdio.reserve(poFdStdio.size() + poStreamStdio.size());
    poStdio.insert(poStdio.begin(), poFdStdio.cbegin(), poFdStdio.cend());
    poStdio.insert(poStdio.end(), poStreamStdio.cbegin(), poStreamStdio.cend());

    po.stdio_count = static_cast<decltype(po.stdio_count)>(poStdio.size());
    po.stdio = poStdio.data();

    // fake initialization so as to have leak invoked
    // see init member function for more details
    initialize([](auto...) {
        return 0;
    });

    invoke(&uv_spawn, parent(), get(), &po);
}


UVW_INLINE void ProcessHandle::kill(int signum) {
    invoke(&uv_process_kill, get(), signum);
}


UVW_INLINE int ProcessHandle::pid() noexcept {
    return get()->pid;
}


UVW_INLINE ProcessHandle &ProcessHandle::cwd(std::string path) noexcept {
    poCwd = path;
    return *this;
}


UVW_INLINE ProcessHandle &ProcessHandle::flags(Flags<Process> flags) noexcept {
    poFlags = flags;
    return *this;
}


UVW_INLINE ProcessHandle &ProcessHandle::stdio(FileHandle fd, Flags<StdIO> flags) {
    auto fgs = static_cast<uv_stdio_flags>(Flags<StdIO>::Type{flags});

    auto actual = FileHandle::Type{fd};

    auto it = std::find_if(poFdStdio.begin(), poFdStdio.end(), [actual](auto &&container) {
        return container.data.fd == actual;
    });

    if(it == poFdStdio.cend()) {
        uv_stdio_container_t container;
        container.flags = fgs;
        container.data.fd = actual;
        poFdStdio.push_back(std::move(container));
    } else {
        it->flags = fgs;
        it->data.fd = actual;
    }

    return *this;
}


UVW_INLINE ProcessHandle &ProcessHandle::uid(Uid id) {
    poUid = id;
    return *this;
}


UVW_INLINE ProcessHandle &ProcessHandle::gid(Gid id) {
    poGid = id;
    return *this;
}


}

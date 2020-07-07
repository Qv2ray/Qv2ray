#ifndef UVW_PROCESS_INCLUDE_H
#define UVW_PROCESS_INCLUDE_H


#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <uv.h>
#include "handle.hpp"
#include "stream.h"
#include "util.h"
#include "loop.h"


namespace uvw {


namespace details {


enum class UVProcessFlags: std::underlying_type_t<uv_process_flags> {
    SETUID = UV_PROCESS_SETUID,
    SETGID = UV_PROCESS_SETGID,
    WINDOWS_VERBATIM_ARGUMENTS = UV_PROCESS_WINDOWS_VERBATIM_ARGUMENTS,
    DETACHED = UV_PROCESS_DETACHED,
    WINDOWS_HIDE = UV_PROCESS_WINDOWS_HIDE,
    WINDOWS_HIDE_CONSOLE = UV_PROCESS_WINDOWS_HIDE_CONSOLE,
    WINDOWS_HIDE_GUI = UV_PROCESS_WINDOWS_HIDE_GUI
};


enum class UVStdIOFlags: std::underlying_type_t<uv_stdio_flags> {
    IGNORE_STREAM = UV_IGNORE,
    CREATE_PIPE = UV_CREATE_PIPE,
    INHERIT_FD = UV_INHERIT_FD,
    INHERIT_STREAM = UV_INHERIT_STREAM,
    READABLE_PIPE = UV_READABLE_PIPE,
    WRITABLE_PIPE = UV_WRITABLE_PIPE,
    OVERLAPPED_PIPE = UV_OVERLAPPED_PIPE
};


}


/**
 * @brief ExitEvent event.
 *
 * It will be emitted by ProcessHandle according with its functionalities.
 */
struct ExitEvent {
    explicit ExitEvent(int64_t code, int sig) noexcept;

    int64_t status; /*!< The exit status. */
    int signal; /*!< The signal that caused the process to terminate, if any. */
};

/**
 * @brief The ProcessHandle handle.
 *
 * Process handles will spawn a new process and allow the user to control it and
 * establish communication channels with it using streams.
 */
class ProcessHandle final: public Handle<ProcessHandle, uv_process_t> {
    static void exitCallback(uv_process_t *handle, int64_t exitStatus, int termSignal);

public:
    using Process = details::UVProcessFlags;
    using StdIO = details::UVStdIOFlags;

    ProcessHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref);

    /**
     * @brief Disables inheritance for file descriptors/handles.
     *
     * Disables inheritance for file descriptors/handles that this process
     * inherited from its parent. The effect is that child processes spawned by
     * this process don’t accidentally inherit these handles.<br/>
     * It is recommended to call this function as early in your program as
     * possible, before the inherited file descriptors can be closed or
     * duplicated.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/process.html#c.uv_disable_stdio_inheritance)
     * for further details.
     */
    static void disableStdIOInheritance() noexcept;

    /**
     * @brief kill Sends the specified signal to the given PID.
     * @param pid A valid process id.
     * @param signum A valid signal identifier.
     * @return True in case of success, false otherwise.
     */
    static bool kill(int pid, int signum) noexcept;

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief spawn Starts the process.
     *
     * If the process isn't successfully spawned, an ErrorEvent event will be
     * emitted by the handle.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/process.html)
     * for further details.
     *
     * @param file Path pointing to the program to be executed.
     * @param args Command line arguments.
     * @param env Optional environment for the new process.
     */
    void spawn(const char *file, char **args, char **env = nullptr);

    /**
     * @brief Sends the specified signal to the internal process handle.
     * @param signum A valid signal identifier.
     */
    void kill(int signum);

    /**
     * @brief Gets the PID of the spawned process.
     *
     * It’s set after calling `spawn()`.
     *
     * @return The PID of the spawned process.
     */
    int pid() noexcept;

    /**
     * @brief Sets the current working directory for the subprocess.
     * @param path The working directory to be used when `spawn()` is invoked.
     * @return A reference to this process handle.
     */
    ProcessHandle & cwd(std::string path) noexcept;

    /**
     * @brief Sets flags that control how `spawn()` behaves.
     *
     * Available flags are:
     *
     * * `ProcessHandle::Process::SETUID`
     * * `ProcessHandle::Process::SETGID`
     * * `ProcessHandle::Process::WINDOWS_VERBATIM_ARGUMENTS`
     * * `ProcessHandle::Process::DETACHED`
     * * `ProcessHandle::Process::WINDOWS_HIDE`
     * * `ProcessHandle::Process::WINDOWS_HIDE_CONSOLE`
     * * `ProcessHandle::Process::WINDOWS_HIDE_GUI`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/process.html#c.uv_process_flags)
     * for further details.
     *
     * @param flags A valid set of flags.
     * @return A reference to this process handle.
     */
    ProcessHandle & flags(Flags<Process> flags) noexcept;

    /**
     * @brief Makes a `stdio` handle available to the child process.
     *
     * Available flags are:
     *
     * * `ProcessHandle::StdIO::IGNORE_STREAM`
     * * `ProcessHandle::StdIO::CREATE_PIPE`
     * * `ProcessHandle::StdIO::INHERIT_FD`
     * * `ProcessHandle::StdIO::INHERIT_STREAM`
     * * `ProcessHandle::StdIO::READABLE_PIPE`
     * * `ProcessHandle::StdIO::WRITABLE_PIPE`
     * * `ProcessHandle::StdIO::OVERLAPPED_PIPE`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/process.html#c.uv_stdio_flags)
     * for further details.
     *
     * @param stream A valid `stdio` handle.
     * @param flags A valid set of flags.
     * @return A reference to this process handle.
     */
    template<typename T, typename U>
    ProcessHandle & stdio(StreamHandle<T, U> &stream, Flags<StdIO> flags) {
        uv_stdio_container_t container;
        Flags<StdIO>::Type fgs = flags;
        container.flags = static_cast<uv_stdio_flags>(fgs);
        container.data.stream = get<uv_stream_t>(stream);
        poStreamStdio.push_back(std::move(container));
        return *this;
    }

    /**
     * @brief Makes a file descriptor available to the child process.
     *
     * Available flags are:
     *
     * * `ProcessHandle::StdIO::IGNORE_STREAM`
     * * `ProcessHandle::StdIO::CREATE_PIPE`
     * * `ProcessHandle::StdIO::INHERIT_FD`
     * * `ProcessHandle::StdIO::INHERIT_STREAM`
     * * `ProcessHandle::StdIO::READABLE_PIPE`
     * * `ProcessHandle::StdIO::WRITABLE_PIPE`
     * * `ProcessHandle::StdIO::OVERLAPPED_PIPE`
     *
     * Default file descriptors are:
     *     * `uvw::StdIN` for `stdin`
     *     * `uvw::StdOUT` for `stdout`
     *     * `uvw::StdERR` for `stderr`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/process.html#c.uv_stdio_flags)
     * for further details.
     *
     * @param fd A valid file descriptor.
     * @param flags A valid set of flags.
     * @return A reference to this process handle.
     */
    ProcessHandle & stdio(FileHandle fd, Flags<StdIO> flags);

    /**
     * @brief Sets the child process' user id.
     * @param id A valid user id to be used.
     * @return A reference to this process handle.
     */
    ProcessHandle & uid(Uid id);

    /**
     * @brief Sets the child process' group id.
     * @param id A valid group id to be used.
     * @return A reference to this process handle.
     */
    ProcessHandle & gid(Gid id);

private:
    std::string poCwd;
    Flags<Process> poFlags;
    std::vector<uv_stdio_container_t> poFdStdio;
    std::vector<uv_stdio_container_t> poStreamStdio;
    Uid poUid;
    Gid poGid;
};


}


#ifndef UVW_AS_LIB
#include "process.cpp"
#endif

#endif // UVW_PROCESS_INCLUDE_H

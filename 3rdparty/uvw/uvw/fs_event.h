#ifndef UVW_FS_EVENT_INCLUDE_H
#define UVW_FS_EVENT_INCLUDE_H


#include <type_traits>
#include <string>
#include <uv.h>
#include "handle.hpp"
#include "util.h"
#include "loop.h"


namespace uvw {


namespace details {


enum class UVFsEventFlags: std::underlying_type_t<uv_fs_event_flags> {
    WATCH_ENTRY = UV_FS_EVENT_WATCH_ENTRY,
    STAT = UV_FS_EVENT_STAT,
    RECURSIVE = UV_FS_EVENT_RECURSIVE
};


enum class UVFsEvent: std::underlying_type_t<uv_fs_event> {
    RENAME = UV_RENAME,
    CHANGE = UV_CHANGE
};


}


/**
 * @brief FsEventEvent event.
 *
 * It will be emitted by FsEventHandle according with its functionalities.
 */
struct FsEventEvent {
    FsEventEvent(const char * pathname, Flags<details::UVFsEvent> events);

    /**
     * @brief The path to the file being monitored.
     *
     * If the handle was started with a directory, the filename parameter will
     * be a relative path to a file contained in the directory.
     */
    const char * filename;

    /**
     * @brief Detected events all in one.
     *
     * Available flags are:
     *
     * * `FsEventHandle::Watch::RENAME`
     * * `FsEventHandle::Watch::CHANGE`
     */
    Flags<details::UVFsEvent> flags;
};


/**
 * @brief The FsEventHandle handle.
 *
 * These handles allow the user to monitor a given path for changes, for
 * example, if the file was renamed or there was a generic change in it. The
 * best backend for the job on each platform is chosen by the handle.
 *
 * To create a `FsEventHandle` through a `Loop`, no arguments are required.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/fs_event.html)
 * for further details.
 */
class FsEventHandle final: public Handle<FsEventHandle, uv_fs_event_t> {
    static void startCallback(uv_fs_event_t *handle, const char *filename, int events, int status);

public:
    using Watch = details::UVFsEvent;
    using Event = details::UVFsEventFlags;

    using Handle::Handle;

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Starts watching the specified path.
     *
     * It will watch the specified path for changes.<br/>
     * As soon as a change is observed, a FsEventEvent is emitted by the
     * handle.<br>
     * It could happen that ErrorEvent events are emitted while running.
     *
     * Available flags are:
     *
     * * `FsEventHandle::Event::WATCH_ENTRY`
     * * `FsEventHandle::Event::STAT`
     * * `FsEventHandle::Event::RECURSIVE`
     *
     * @param path The file or directory to be monitored.
     * @param flags Additional flags to control the behavior.
     */
    void start(std::string path, Flags<Event> flags = Flags<Event>{});

    /**
     * @brief Starts watching the specified path.
     *
     * It will watch the specified path for changes.<br/>
     * As soon as a change is observed, a FsEventEvent is emitted by the
     * handle.<br>
     * It could happen that ErrorEvent events are emitted while running.
     *
     * Available flags are:
     *
     * * `FsEventHandle::Event::WATCH_ENTRY`
     * * `FsEventHandle::Event::STAT`
     * * `FsEventHandle::Event::RECURSIVE`
     *
     * @param path The file or directory to be monitored.
     * @param flag Additional flag to control the behavior.
     */
    void start(std::string path, Event flag);

    /**
     * @brief Stops polling the file descriptor.
     */
    void stop();

    /**
     * @brief Gets the path being monitored.
     * @return The path being monitored, an empty string in case of errors.
     */
    std::string path() noexcept;
};


}


#ifndef UVW_AS_LIB
#include "fs_event.cpp"
#endif

#endif // UVW_FS_EVENT_INCLUDE_H

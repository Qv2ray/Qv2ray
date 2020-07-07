#ifndef UVW_FS_POLL_INCLUDE_H
#define UVW_FS_POLL_INCLUDE_H


#include <string>
#include <chrono>
#include <uv.h>
#include "handle.hpp"
#include "util.h"
#include "loop.h"


namespace uvw {


/**
 * @brief FsPollEvent event.
 *
 * It will be emitted by FsPollHandle according with its functionalities.
 */
struct FsPollEvent {
    explicit FsPollEvent(Stat previous, Stat current) noexcept;

    Stat prev; /*!< The old Stat struct. */
    Stat curr; /*!< The new Stat struct. */
};


/**
 * @brief The FsPollHandle handle.
 *
 * It allows the user to monitor a given path for changes. Unlike FsEventHandle
 * handles, FsPollHandle handles use stat to detect when a file has changed so
 * they can work on file systems where FsEventHandle handles can’t.
 *
 * To create a `FsPollHandle` through a `Loop`, no arguments are required.
 */
class FsPollHandle final: public Handle<FsPollHandle, uv_fs_poll_t> {
    static void startCallback(uv_fs_poll_t *handle, int status, const uv_stat_t *prev, const uv_stat_t *curr);

public:
    using Time = std::chrono::duration<unsigned int, std::milli>;

    using Handle::Handle;

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Starts the handle.
     *
     * The handle will start emitting FsPollEvent when needed.
     *
     * @param file The path to the file to be checked.
     * @param interval Milliseconds between successive checks.
     */
    void start(std::string file, Time interval);

    /**
     * @brief Stops the handle.
     */
    void stop();

    /**
     * @brief Gets the path being monitored by the handle.
     * @return The path being monitored by the handle, an empty string in case
     * of errors.
     */
    std::string path() noexcept;
};


}


#ifndef UVW_AS_LIB
#include "fs_poll.cpp"
#endif

#endif // UVW_FS_POLL_INCLUDE_H

#ifndef UVW_TTY_INCLUDE_H
#define UVW_TTY_INCLUDE_H


#include <type_traits>
#include <memory>
#include <uv.h>
#include "stream.h"
#include "util.h"


namespace uvw {


namespace details {


struct ResetModeMemo {
    ~ResetModeMemo();
};


enum class UVTTYModeT: std::underlying_type_t<uv_tty_mode_t> {
    NORMAL = UV_TTY_MODE_NORMAL,
    RAW = UV_TTY_MODE_RAW,
    IO = UV_TTY_MODE_IO
};


enum class UVTTYVTermStateT: std::underlying_type_t<uv_tty_vtermstate_t> {
    SUPPORTED = UV_TTY_SUPPORTED,
    UNSUPPORTED = UV_TTY_UNSUPPORTED
};


}


/**
 * @brief The TTYHandle handle.
 *
 * TTY handles represent a stream for the console.
 *
 * To create a `TTYHandle` through a `Loop`, arguments follow:
 *
 * * A valid FileHandle. Usually the file descriptor will be:
 *     * `uvw::StdIN` or `0` for `stdin`
 *     * `uvw::StdOUT` or `1` for `stdout`
 *     * `uvw::StdERR` or `2` for `stderr`
 * * A boolean value that specifies the plan on calling `read()` with this
 * stream. Remember that `stdin` is readable, `stdout` is not.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/tty.html#c.uv_tty_init)
 * for further details.
 */
class TTYHandle final: public StreamHandle<TTYHandle, uv_tty_t> {
    static std::shared_ptr<details::ResetModeMemo> resetModeMemo();

public:
    using Mode = details::UVTTYModeT;
    using VTermState = details::UVTTYVTermStateT;

    explicit TTYHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, FileHandle desc, bool readable);

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Sets the TTY using the specified terminal mode.
     *
     * Available modes are:
     *
     * * `TTY::Mode::NORMAL`
     * * `TTY::Mode::RAW`
     * * `TTY::Mode::IO`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/tty.html#c.uv_tty_mode_t)
     * for further details.
     *
     * @param m The mode to be set.
     * @return True in case of success, false otherwise.
     */
    bool mode(Mode m);

    /**
     * @brief Resets TTY settings to default values.
     * @return True in case of success, false otherwise.
     */
    bool reset() noexcept;

    /**
     * @brief Gets the current Window size.
     * @return The current Window size or `{-1, -1}` in case of errors.
     */
    WinSize getWinSize();

    /**
     * @brief Controls whether console virtual terminal sequences are processed
     * by the library or console.
     *
     * This function is only meaningful on Windows systems. On Unix it is
     * silently ignored.
     *
     * Available states are:
     *
     * * `TTY::VTermState::SUPPORTED`
     * * `TTY::VTermState::UNSUPPORTED`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/tty.html#c.uv_tty_vtermstate_t)
     * for further details.
     *
     * @param s The state to be set.
     */
    void vtermState(VTermState s) const noexcept;

    /**
     * @brief Gets the current state of whether console virtual terminal
     * sequences are handled by the library or the console.
     *
     * This function is not implemented on Unix.
     *
     * Available states are:
     *
     * * `TTY::VTermState::SUPPORTED`
     * * `TTY::VTermState::UNSUPPORTED`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/tty.html#c.uv_tty_vtermstate_t)
     * for further details.
     *
     * @return The current state.
     */
    VTermState vtermState() const noexcept;

private:
    std::shared_ptr<details::ResetModeMemo> memo;
    FileHandle::Type fd;
    int rw;
};


}


#ifndef UVW_AS_LIB
#include "tty.cpp"
#endif

#endif // UVW_TTY_INCLUDE_H

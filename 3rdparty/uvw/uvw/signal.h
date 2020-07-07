#ifndef UVW_SIGNAL_INCLUDE_H
#define UVW_SIGNAL_INCLUDE_H


#include <uv.h>
#include "handle.hpp"
#include "loop.h"


namespace uvw {


/**
 * @brief SignalEvent event.
 *
 * It will be emitted by SignalHandle according with its functionalities.
 */
struct SignalEvent {
    explicit SignalEvent(int sig) noexcept;

    int signum; /*!< The signal being monitored by this handle. */
};


/**
 * @brief The SignalHandle handle.
 *
 * Signal handles implement Unix style signal handling on a per-event loop
 * bases.<br/>
 * Reception of some signals is emulated on Windows.
 *
 * To create a `SignalHandle` through a `Loop`, no arguments are required.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/signal.html)
 * for further details.
 */
class SignalHandle final: public Handle<SignalHandle, uv_signal_t> {
    static void startCallback(uv_signal_t *handle, int signum);

public:
    using Handle::Handle;

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Starts the handle.
     *
     * The handle will start emitting SignalEvent when needed.
     *
     * @param signum The signal to be monitored.
     */
    void start(int signum);

    /**
     * @brief Starts the handle.
     *
     * Same functionality as SignalHandle::start but the signal handler is reset
     * the moment the signal is received.
     *
     * @param signum
     */
    void oneShot(int signum);

    /**
     * @brief Stops the handle.
     */
    void stop();

    /**
     * @brief Gets the signal being monitored.
     * @return The signal being monitored.
     */
    int signal() const noexcept;
};


}


#ifndef UVW_AS_LIB
#include "signal.cpp"
#endif

#endif // UVW_SIGNAL_INCLUDE_H

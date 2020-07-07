#ifndef UVW_POLL_INCLUDE_H
#define UVW_POLL_INCLUDE_H


#include <type_traits>
#include <memory>
#include <uv.h>
#include "handle.hpp"
#include "util.h"


namespace uvw {


namespace details {


enum class UVPollEvent: std::underlying_type_t<uv_poll_event> {
    READABLE = UV_READABLE,
    WRITABLE = UV_WRITABLE,
    DISCONNECT = UV_DISCONNECT,
    PRIORITIZED = UV_PRIORITIZED
};


}


/**
 * @brief PollEvent event.
 *
 * It will be emitted by PollHandle according with its functionalities.
 */
struct PollEvent {
    explicit PollEvent(Flags<details::UVPollEvent> events) noexcept;

    /**
     * @brief Detected events all in one.
     *
     * Available flags are:
     *
     * * `PollHandle::Event::READABLE`
     * * `PollHandle::Event::WRITABLE`
     * * `PollHandle::Event::DISCONNECT`
     * * `PollHandle::Event::PRIORITIZED`
     */
    Flags<details::UVPollEvent> flags;
};


/**
 * @brief The PollHandle handle.
 *
 * Poll handles are used to watch file descriptors for readability, writability
 * and disconnection.
 *
 * To create a `PollHandle` through a `Loop`, arguments follow:
 *
 * * A descriptor that can be:
 *     * either an `int` file descriptor
 *     * or a `OSSocketHandle` socket descriptor
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/poll.html)
 * for further details.
 */
class PollHandle final: public Handle<PollHandle, uv_poll_t> {
    static void startCallback(uv_poll_t *handle, int status, int events);

public:
    using Event = details::UVPollEvent;

    explicit PollHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, int desc);
    explicit PollHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, OSSocketHandle sock);

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Starts polling the file descriptor.
     *
     * Available flags are:
     *
     * * `PollHandle::Event::READABLE`
     * * `PollHandle::Event::WRITABLE`
     * * `PollHandle::Event::DISCONNECT`
     * * `PollHandle::Event::PRIORITIZED`
     *
     * As soon as an event is detected, a PollEvent is emitted by the
     * handle.<br>
     * It could happen that ErrorEvent events are emitted while running.
     *
     * Calling more than once this method will update the flags to which the
     * caller is interested.
     *
     * @param flags The events to which the caller is interested.
     */
    void start(Flags<Event> flags);

    /**
     * @brief Starts polling the file descriptor.
     *
     * Available flags are:
     *
     * * `PollHandle::Event::READABLE`
     * * `PollHandle::Event::WRITABLE`
     * * `PollHandle::Event::DISCONNECT`
     * * `PollHandle::Event::PRIORITIZED`
     *
     * As soon as an event is detected, a PollEvent is emitted by the
     * handle.<br>
     * It could happen that ErrorEvent events are emitted while running.
     *
     * Calling more than once this method will update the flags to which the
     * caller is interested.
     *
     * @param event The event to which the caller is interested.
     */
    void start(Event event);

    /**
     * @brief Stops polling the file descriptor.
     */
    void stop();

private:
    enum { FD, SOCKET } tag;
    union {
        int fd;
        OSSocketHandle::Type socket;
    };
};


}


#ifndef UVW_AS_LIB
#include "poll.cpp"
#endif

#endif // UVW_POLL_INCLUDE_H

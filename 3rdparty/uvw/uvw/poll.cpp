#ifdef UVW_AS_LIB
#include "poll.h"
#endif

#include <utility>

#include "config.h"


namespace uvw {


UVW_INLINE PollEvent::PollEvent(Flags<details::UVPollEvent> events) noexcept
    : flags{std::move(events)}
{}


UVW_INLINE PollHandle::PollHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, int desc)
    : Handle{ca, std::move(ref)}, tag{FD}, fd{desc}
{}


UVW_INLINE PollHandle::PollHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, OSSocketHandle sock)
    : Handle{ca, std::move(ref)}, tag{SOCKET}, socket{sock}
{}


UVW_INLINE void PollHandle::startCallback(uv_poll_t *handle, int status, int events) {
    PollHandle &poll = *(static_cast<PollHandle *>(handle->data));

    if(status) {
        poll.publish(ErrorEvent{status});
    } else {
        poll.publish(PollEvent{static_cast<std::underlying_type_t<Event>>(events)});
    }
}


UVW_INLINE bool PollHandle::init() {
    return (tag == SOCKET) ? initialize(&uv_poll_init_socket, socket) : initialize(&uv_poll_init, fd);
}


UVW_INLINE void PollHandle::start(Flags<PollHandle::Event> flags) {
    invoke(&uv_poll_start, get(), flags, &startCallback);
}


UVW_INLINE void PollHandle::start(PollHandle::Event event) {
    start(Flags<Event>{event});
}


UVW_INLINE void PollHandle::stop() {
    invoke(&uv_poll_stop, get());
}


}

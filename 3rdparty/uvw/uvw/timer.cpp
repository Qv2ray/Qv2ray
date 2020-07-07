#ifdef UVW_AS_LIB
#include "timer.h"
#endif

#include "config.h"


namespace uvw {


UVW_INLINE void TimerHandle::startCallback(uv_timer_t *handle) {
    TimerHandle &timer = *(static_cast<TimerHandle *>(handle->data));
    timer.publish(TimerEvent{});
}


UVW_INLINE bool TimerHandle::init() {
    return initialize(&uv_timer_init);
}


UVW_INLINE void TimerHandle::start(TimerHandle::Time timeout, TimerHandle::Time repeat) {
    invoke(&uv_timer_start, get(), &startCallback, timeout.count(), repeat.count());
}


UVW_INLINE void TimerHandle::stop() {
    invoke(&uv_timer_stop, get());
}


UVW_INLINE void TimerHandle::again() {
    invoke(&uv_timer_again, get());
}


UVW_INLINE void TimerHandle::repeat(TimerHandle::Time repeat) {
    uv_timer_set_repeat(get(), repeat.count());
}


UVW_INLINE TimerHandle::Time TimerHandle::repeat() {
    return Time{uv_timer_get_repeat(get())};
}


}

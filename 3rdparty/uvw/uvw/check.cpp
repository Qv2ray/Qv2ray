#ifdef UVW_AS_LIB
#include "check.h"
#endif

#include "config.h"


namespace uvw {


UVW_INLINE void CheckHandle::startCallback(uv_check_t *handle) {
    CheckHandle &check = *(static_cast<CheckHandle *>(handle->data));
    check.publish(CheckEvent{});
}


UVW_INLINE bool CheckHandle::init() {
    return initialize(&uv_check_init);
}


UVW_INLINE void CheckHandle::start() {
    invoke(&uv_check_start, get(), &startCallback);
}


UVW_INLINE void CheckHandle::stop() {
    invoke(&uv_check_stop, get());
}


}

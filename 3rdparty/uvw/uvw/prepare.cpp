#ifdef UVW_AS_LIB
#include "prepare.h"
#endif

#include "config.h"


namespace uvw {


UVW_INLINE void PrepareHandle::startCallback(uv_prepare_t *handle) {
    PrepareHandle &prepare = *(static_cast<PrepareHandle *>(handle->data));
    prepare.publish(PrepareEvent{});
}


UVW_INLINE bool PrepareHandle::init() {
    return initialize(&uv_prepare_init);
}


UVW_INLINE void PrepareHandle::start() {
    invoke(&uv_prepare_start, get(), &startCallback);
}


UVW_INLINE void PrepareHandle::stop() {
    invoke(&uv_prepare_stop, get());
}


}

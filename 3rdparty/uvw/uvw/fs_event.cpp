#ifdef UVW_AS_LIB
#include "fs_event.h"
#endif

#include <utility>

#include "config.h"


namespace uvw {


UVW_INLINE FsEventEvent::FsEventEvent(const char * pathname, Flags<details::UVFsEvent> events)
    : filename{pathname}, flags{std::move(events)}
{}


UVW_INLINE void FsEventHandle::startCallback(uv_fs_event_t *handle, const char *filename, int events, int status) {
    FsEventHandle &fsEvent = *(static_cast<FsEventHandle *>(handle->data));

    if(status) {
        fsEvent.publish(ErrorEvent{status});
    } else {
        fsEvent.publish(FsEventEvent{filename, static_cast<std::underlying_type_t<details::UVFsEvent>>(events)});
    }
}


UVW_INLINE bool FsEventHandle::init() {
    return initialize(&uv_fs_event_init);
}


UVW_INLINE void FsEventHandle::start(std::string path, Flags<Event> flags) {
    invoke(&uv_fs_event_start, get(), &startCallback, path.data(), flags);
}


UVW_INLINE void FsEventHandle::start(std::string path, FsEventHandle::Event flag) {
    start(std::move(path), Flags<Event>{flag});
}


UVW_INLINE void FsEventHandle::stop() {
    invoke(&uv_fs_event_stop, get());
}


UVW_INLINE std::string FsEventHandle::path() noexcept {
    return details::tryRead(&uv_fs_event_getpath, get());
}


}

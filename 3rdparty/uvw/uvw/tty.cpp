#ifdef UVW_AS_LIB
#include "tty.h"
#endif

#include <utility>

#include "config.h"


namespace uvw {


UVW_INLINE details::ResetModeMemo::~ResetModeMemo() {
    uv_tty_reset_mode();
}


UVW_INLINE TTYHandle::TTYHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, FileHandle desc, bool readable)
    : StreamHandle{ca, std::move(ref)},
      memo{resetModeMemo()},
      fd{desc},
      rw{readable}
{}


UVW_INLINE std::shared_ptr<details::ResetModeMemo> TTYHandle::resetModeMemo() {
    static std::weak_ptr<details::ResetModeMemo> weak;
    auto shared = weak.lock();
    if(!shared) { weak = shared = std::make_shared<details::ResetModeMemo>(); }
    return shared;
};


UVW_INLINE bool TTYHandle::init() {
    return initialize(&uv_tty_init, fd, rw);
}


UVW_INLINE bool TTYHandle::mode(TTYHandle::Mode m) {
    return (0 == uv_tty_set_mode(get(), static_cast<std::underlying_type_t<Mode>>(m)));
}


UVW_INLINE bool TTYHandle::reset() noexcept {
    return (0 == uv_tty_reset_mode());
}


UVW_INLINE WinSize TTYHandle::getWinSize() {
    WinSize size;

    if(0 != uv_tty_get_winsize(get(), &size.width, &size.height)) {
        size.width = -1;
        size.height = -1;
    }

    return size;
}


UVW_INLINE void TTYHandle::vtermState(TTYHandle::VTermState s) const noexcept {
    switch(s) {
        case VTermState::SUPPORTED:
            uv_tty_set_vterm_state(uv_tty_vtermstate_t::UV_TTY_SUPPORTED);
            break;
        case VTermState::UNSUPPORTED:
            uv_tty_set_vterm_state(uv_tty_vtermstate_t::UV_TTY_UNSUPPORTED);
            break;
    }
}


UVW_INLINE TTYHandle::VTermState TTYHandle::vtermState() const noexcept {
    uv_tty_vtermstate_t state;
    uv_tty_get_vterm_state(&state);
    return VTermState{state};
}


}

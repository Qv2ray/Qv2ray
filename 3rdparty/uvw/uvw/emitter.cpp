#ifdef UVW_AS_LIB
#include "emitter.h"
#endif

#include "config.h"


namespace uvw {


UVW_INLINE int ErrorEvent::translate(int sys) noexcept {
    return uv_translate_sys_error(sys);
}



UVW_INLINE const char * ErrorEvent::what() const noexcept {
    return uv_strerror(ec);
}


UVW_INLINE const char * ErrorEvent::name() const noexcept {
    return uv_err_name(ec);
}


UVW_INLINE int ErrorEvent::code() const noexcept {
    return ec;
}


UVW_INLINE ErrorEvent::operator bool() const noexcept {
    return ec < 0;
}


}

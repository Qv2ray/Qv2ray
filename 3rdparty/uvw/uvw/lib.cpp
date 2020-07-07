#ifdef UVW_AS_LIB
#include "lib.h"
#endif

#include <utility>

#include "config.h"


namespace uvw {


UVW_INLINE SharedLib::SharedLib(UnderlyingType<SharedLib, uv_lib_t>::ConstructorAccess ca, std::shared_ptr<Loop> ref, std::string filename) noexcept
    : UnderlyingType{ca, std::move(ref)}
{
    opened = (0 == uv_dlopen(filename.data(), get()));
}


UVW_INLINE SharedLib::~SharedLib() noexcept {
    uv_dlclose(get());
}


UVW_INLINE SharedLib::operator bool() const noexcept {
    return opened;
}


UVW_INLINE const char *SharedLib::error() const noexcept {
    return uv_dlerror(get());
}


}

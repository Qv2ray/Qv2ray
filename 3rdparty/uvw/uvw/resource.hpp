#ifndef UVW_RESOURCE_INCLUDE_H
#define UVW_RESOURCE_INCLUDE_H


#include <memory>
#include <utility>
#include "emitter.h"
#include "underlying_type.hpp"


namespace uvw {


/**
 * @brief Common class for almost all the resources available in `uvw`.
 *
 * This is the base class for handles and requests.
 */
template<typename T, typename U>
class Resource: public UnderlyingType<T, U>, public Emitter<T>, public std::enable_shared_from_this<T> {
protected:
    using ConstructorAccess = typename UnderlyingType<T, U>::ConstructorAccess;

    auto parent() const noexcept {
        return this->loop().loop.get();
    }

    void leak() noexcept {
        sPtr = this->shared_from_this();
    }

    void reset() noexcept {
        sPtr.reset();
    }

    bool self() const noexcept {
        return static_cast<bool>(sPtr);
    }

public:
    explicit Resource(ConstructorAccess ca, std::shared_ptr<Loop> ref)
        : UnderlyingType<T, U>{ca, std::move(ref)}
    {
        this->get()->data = this;
    }

    /**
     * @brief Gets user-defined data. `uvw` won't use this field in any case.
     * @return User-defined data if any, an invalid pointer otherwise.
     */
    template<typename R = void>
    std::shared_ptr<R> data() const {
        return std::static_pointer_cast<R>(userData);
    }

    /**
     * @brief Sets arbitrary data. `uvw` won't use this field in any case.
     * @param uData User-defined arbitrary data.
     */
    void data(std::shared_ptr<void> uData) {
        userData = std::move(uData);
    }

private:
    std::shared_ptr<void> userData{nullptr};
    std::shared_ptr<void> sPtr{nullptr};
};

}

#endif // UVW_RESOURCE_INCLUDE_H

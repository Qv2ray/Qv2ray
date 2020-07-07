#ifndef UVW_PREPARE_INCLUDE_H
#define UVW_PREPARE_INCLUDE_H


#include <uv.h>
#include "handle.hpp"
#include "loop.h"


namespace uvw {


/**
 * @brief PrepareEvent event.
 *
 * It will be emitted by PrepareHandle according with its functionalities.
 */
struct PrepareEvent {};


/**
 * @brief The PrepareHandle handle.
 *
 * Prepare handles will emit a PrepareEvent event once per loop iteration, right
 * before polling for I/O.
 *
 * To create a `PrepareHandle` through a `Loop`, no arguments are required.
 */
class PrepareHandle final: public Handle<PrepareHandle, uv_prepare_t> {
    static void startCallback(uv_prepare_t *handle);

public:
    using Handle::Handle;

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Starts the handle.
     *
     * A PrepareEvent event will be emitted once per loop iteration, right
     * before polling for I/O.
     *
     * The handle will start emitting PrepareEvent when needed.
     */
    void start();

    /**
     * @brief Stops the handle.
     */
    void stop();
};


}


#ifndef UVW_AS_LIB
#include "prepare.cpp"
#endif

#endif // UVW_PREPARE_INCLUDE_H

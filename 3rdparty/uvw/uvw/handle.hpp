#ifndef UVW_HANDLE_INCLUDE_H
#define UVW_HANDLE_INCLUDE_H


#include <cstddef>
#include <utility>
#include <memory>
#include <uv.h>
#include "resource.hpp"
#include "util.h"


namespace uvw {


/**
 * @brief CloseEvent event.
 *
 * It will be emitted by the handles according with their functionalities.
 */
struct CloseEvent {};


/**
 * @brief Handle base class.
 *
 * Base type for all `uvw` handle types.
 */
template<typename T, typename U>
class Handle: public Resource<T, U>, public BaseHandle {
protected:
    static void closeCallback(uv_handle_t *handle) {
        Handle<T, U> &ref = *(static_cast<T*>(handle->data));
        auto ptr = ref.shared_from_this();
        (void)ptr;
        ref.reset();
        ref.publish(CloseEvent{});
    }

    static void allocCallback(uv_handle_t *, std::size_t suggested, uv_buf_t *buf) {
        auto size = static_cast<unsigned int>(suggested);
        *buf = uv_buf_init(new char[size], size);
    }

    template<typename F, typename... Args>
    bool initialize(F &&f, Args&&... args) {
        if(!this->self()) {
            auto err = std::forward<F>(f)(this->parent(), this->get(), std::forward<Args>(args)...);

            if(err) {
                this->publish(ErrorEvent{err});
            } else {
                this->leak();
            }
        }

        return this->self();
    }

    template<typename F, typename... Args>
    void invoke(F &&f, Args&&... args) {
        auto err = std::forward<F>(f)(std::forward<Args>(args)...);
        if(err) { Emitter<T>::publish(ErrorEvent{err}); }
    }

public:
    using Resource<T, U>::Resource;

    /**
     * @brief Gets the category of the handle.
     *
     * A base handle offers no functionality to promote it to the actual handle
     * type. By means of this function, an opaque value that identifies the
     * category of the handle is made available to the users.
     *
     * @return The actual category of the handle.
     */
    HandleCategory category() const noexcept override {
        return HandleCategory{this->template get<uv_handle_t>()->type};
    }

    /**
     * @brief Gets the type of the handle.
     *
     * A base handle offers no functionality to promote it to the actual handle
     * type. By means of this function, the type of the underlying handle as
     * specified by HandleType is made available to the users.
     *
     * @return The actual type of the handle.
     */
    HandleType type() const noexcept override {
        return Utilities::guessHandle(category());
    }

    /**
     * @brief Checks if the handle is active.
     *
     * What _active_ means depends on the type of handle:
     *
     * * An AsyncHandle handle is always active and cannot be deactivated,
     * except by closing it with uv_close().
     * * A PipeHandle, TCPHandle, UDPHandle, etc. handle - basically any handle
     * that deals with I/O - is active when it is doing something that involves
     * I/O, like reading, writing, connecting, accepting new connections, etc.
     * * A CheckHandle, IdleHandle, TimerHandle, etc. handle is active when it
     * has been started with a call to `start()`.
     *
     * Rule of thumb: if a handle of type `FooHandle` has a `start()` member
     * method, then it’s active from the moment that method is called. Likewise,
     * `stop()` deactivates the handle again.
     *
     * @return True if the handle is active, false otherwise.
     */
    bool active() const noexcept override {
        return !(uv_is_active(this->template get<uv_handle_t>()) == 0);
    }

    /**
     * @brief Checks if a handle is closing or closed.
     *
     * This function should only be used between the initialization of the
     * handle and the arrival of the close callback.
     *
     * @return True if the handle is closing or closed, false otherwise.
     */
    bool closing() const noexcept override {
        return !(uv_is_closing(this->template get<uv_handle_t>()) == 0);
    }

    /**
     * @brief Request handle to be closed.
     *
     * This **must** be called on each handle before memory is released.<br/>
     * In-progress requests are cancelled and this can result in an ErrorEvent
     * emitted.
     *
     * The handle will emit a CloseEvent when finished.
     */
    void close() noexcept override {
        if(!closing()) {
            uv_close(this->template get<uv_handle_t>(), &Handle<T, U>::closeCallback);
        }
    }

    /**
     * @brief Reference the given handle.
     *
     * References are idempotent, that is, if a handle is already referenced
     * calling this function again will have no effect.
     */
    void reference() noexcept override {
        uv_ref(this->template get<uv_handle_t>());
    }

    /**
     * @brief Unreference the given handle.
     *
     * References are idempotent, that is, if a handle is not referenced calling
     * this function again will have no effect.
     */
    void unreference() noexcept override {
        uv_unref(this->template get<uv_handle_t>());
    }

    /**
     * @brief Checks if the given handle referenced.
     * @return True if the handle referenced, false otherwise.
     */
    bool referenced() const noexcept override {
        return !(uv_has_ref(this->template get<uv_handle_t>()) == 0);
    }

    /**
     * @brief Returns the size of the underlying handle type.
     * @return The size of the underlying handle type.
     */
    std::size_t size() const noexcept {
        return uv_handle_size(this->template get<uv_handle_t>()->type);
    }

    /**
     * @brief Gets the size of the send buffer used for the socket.
     *
     * Gets the size of the send buffer that the operating system uses for the
     * socket.<br/>
     * This function works for TCPHandle, PipeHandle and UDPHandle handles on
     * Unix and for TCPHandle and UDPHandle handles on Windows.<br/>
     * Note that Linux will return double the size of the original set value.
     *
     * @return The size of the send buffer, 0 in case of errors.
     */
    int sendBufferSize() {
        int value = 0;
        auto err = uv_send_buffer_size(this->template get<uv_handle_t>(), &value);
        return err ? 0 : value;
    }

    /**
     * @brief Sets the size of the send buffer used for the socket.
     *
     * Sets the size of the send buffer that the operating system uses for the
     * socket.<br/>
     * This function works for TCPHandle, PipeHandle and UDPHandle handles on
     * Unix and for TCPHandle and UDPHandle handles on Windows.<br/>
     * Note that Linux will set double the size.
     *
     * @return True in case of success, false otherwise.
     */
    bool sendBufferSize(int value) {
        return (0 == uv_send_buffer_size(this->template get<uv_handle_t>(), &value));
    }

    /**
     * @brief Gets the size of the receive buffer used for the socket.
     *
     * Gets the size of the receive buffer that the operating system uses for
     * the socket.<br/>
     * This function works for TCPHandle, PipeHandle and UDPHandle handles on
     * Unix and for TCPHandle and UDPHandle handles on Windows.<br/>
     * Note that Linux will return double the size of the original set value.
     *
     * @return The size of the receive buffer, 0 in case of errors.
     */
    int recvBufferSize() {
        int value = 0;
        auto err = uv_recv_buffer_size(this->template get<uv_handle_t>(), &value);
        return err ? 0 : value;
    }

    /**
     * @brief Sets the size of the receive buffer used for the socket.
     *
     * Sets the size of the receive buffer that the operating system uses for
     * the socket.<br/>
     * This function works for TCPHandle, PipeHandle and UDPHandle handles on
     * Unix and for TCPHandle and UDPHandle handles on Windows.<br/>
     * Note that Linux will set double the size.
     *
     * @return True in case of success, false otherwise.
     */
    bool recvBufferSize(int value) {
        return (0 == uv_recv_buffer_size(this->template get<uv_handle_t>(), &value));
    }

    /**
     * @brief Gets the platform dependent file descriptor equivalent.
     *
     * Supported handles:
     *
     * * TCPHandle
     * * PipeHandle
     * * TTYHandle
     * * UDPHandle
     * * PollHandle
     *
     * It will emit an ErrorEvent event if invoked on any other handle.<br/>
     * If a handle doesn’t have an attached file descriptor yet or the handle
     * itself has been closed, an ErrorEvent event will be emitted.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/handle.html#c.uv_fileno)
     * for further details.
     *
     * @return The file descriptor attached to the hande or a negative value in
     * case of errors.
     */
    OSFileDescriptor fd() const {
        uv_os_fd_t fd;
        uv_fileno(this->template get<uv_handle_t>(), &fd);
        return fd;
    }
};


}

#endif // UVW_HANDLE_INCLUDE_H

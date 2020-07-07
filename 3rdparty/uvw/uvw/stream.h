#ifndef UVW_STREAM_INCLUDE_H
#define UVW_STREAM_INCLUDE_H


#include <algorithm>
#include <iterator>
#include <cstddef>
#include <utility>
#include <memory>
#include <uv.h>
#include "request.hpp"
#include "handle.hpp"
#include "loop.h"


namespace uvw {


/**
 * @brief ConnectEvent event.
 *
 * It will be emitted by StreamHandle according with its functionalities.
 */
struct ConnectEvent {};


/**
 * @brief EndEvent event.
 *
 * It will be emitted by StreamHandle according with its functionalities.
 */
struct EndEvent {};


/**
 * @brief ListenEvent event.
 *
 * It will be emitted by StreamHandle according with its functionalities.
 */
struct ListenEvent {};


/**
 * @brief ShutdownEvent event.
 *
 * It will be emitted by StreamHandle according with its functionalities.
 */
struct ShutdownEvent {};


/**
 * @brief WriteEvent event.
 *
 * It will be emitted by StreamHandle according with its functionalities.
 */
struct WriteEvent {};


/**
 * @brief DataEvent event.
 *
 * It will be emitted by StreamHandle according with its functionalities.
 */
struct DataEvent {
    explicit DataEvent(std::unique_ptr<char[]> buf, std::size_t len) noexcept;

    std::unique_ptr<char[]> data; /*!< A bunch of data read on the stream. */
    std::size_t length; /*!< The amount of data read on the stream. */
};


namespace details {


struct ConnectReq final: public Request<ConnectReq, uv_connect_t> {
    using Request::Request;

    template<typename F, typename... Args>
    void connect(F &&f, Args&&... args) {
        invoke(std::forward<F>(f), get(), std::forward<Args>(args)..., &defaultCallback<ConnectEvent>);
    }
};


struct ShutdownReq final: public Request<ShutdownReq, uv_shutdown_t> {
    using Request::Request;

    void shutdown(uv_stream_t *handle);
};


template<typename Deleter>
class WriteReq final: public Request<WriteReq<Deleter>, uv_write_t> {
    using ConstructorAccess = typename Request<WriteReq<Deleter>, uv_write_t>::ConstructorAccess;

public:
    WriteReq(ConstructorAccess ca, std::shared_ptr<Loop> loop, std::unique_ptr<char[], Deleter> dt, unsigned int len)
        : Request<WriteReq<Deleter>, uv_write_t>{ca, std::move(loop)},
          data{std::move(dt)},
          buf{uv_buf_init(data.get(), len)}
    {}

    void write(uv_stream_t *handle) {
        this->invoke(&uv_write, this->get(), handle, &buf, 1, &this->template defaultCallback<WriteEvent>);
    }

    void write(uv_stream_t *handle, uv_stream_t *send) {
        this->invoke(&uv_write2, this->get(), handle, &buf, 1, send, &this->template defaultCallback<WriteEvent>);
    }

private:
    std::unique_ptr<char[], Deleter> data;
    uv_buf_t buf;
};


}


/**
 * @brief The StreamHandle handle.
 *
 * Stream handles provide an abstraction of a duplex communication channel.
 * StreamHandle is an intermediate type, `uvw` provides three stream
 * implementations: TCPHandle, PipeHandle and TTYHandle.
 */
template<typename T, typename U>
class StreamHandle: public Handle<T, U> {
    static constexpr unsigned int DEFAULT_BACKLOG = 128;

    static void readCallback(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
        T &ref = *(static_cast<T*>(handle->data));
        // data will be destroyed no matter of what the value of nread is
        std::unique_ptr<char[]> data{buf->base};

        // nread == 0 is ignored (see http://docs.libuv.org/en/v1.x/stream.html)
        // equivalent to EAGAIN/EWOULDBLOCK, it shouldn't be treated as an error
        // for we don't have data to emit though, it's fine to suppress it

        if(nread == UV_EOF) {
            // end of stream
            ref.publish(EndEvent{});
        } else if(nread > 0) {
            // data available
            ref.publish(DataEvent{std::move(data), static_cast<std::size_t>(nread)});
        } else if(nread < 0) {
            // transmission error
            ref.publish(ErrorEvent(nread));
        }
    }

    static void listenCallback(uv_stream_t *handle, int status) {
        T &ref = *(static_cast<T*>(handle->data));
        if(status) { ref.publish(ErrorEvent{status}); }
        else { ref.publish(ListenEvent{}); }
    }

public:
#ifdef _MSC_VER
    StreamHandle(typename Handle<T, U>::ConstructorAccess ca, std::shared_ptr<Loop> ref)
        : Handle<T, U>{ca, std::move(ref)}
    {}
#else
    using Handle<T, U>::Handle;
#endif

    /**
     * @brief Shutdowns the outgoing (write) side of a duplex stream.
     *
     * It waits for pending write requests to complete. The handle should refer
     * to a initialized stream.<br/>
     * A ShutdownEvent event will be emitted after shutdown is complete.
     */
    void shutdown() {
        auto listener = [ptr = this->shared_from_this()](const auto &event, const auto &) {
            ptr->publish(event);
        };

        auto shutdown = this->loop().template resource<details::ShutdownReq>();
        shutdown->template once<ErrorEvent>(listener);
        shutdown->template once<ShutdownEvent>(listener);
        shutdown->shutdown(this->template get<uv_stream_t>());
    }

    /**
     * @brief Starts listening for incoming connections.
     *
     * When a new incoming connection is received, a ListenEvent event is
     * emitted.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param backlog Indicates the number of connections the kernel might
     * queue, same as listen(2).
     */
    void listen(int backlog = DEFAULT_BACKLOG) {
        this->invoke(&uv_listen, this->template get<uv_stream_t>(), backlog, &listenCallback);
    }

    /**
     * @brief Accepts incoming connections.
     *
     * This call is used in conjunction with `listen()` to accept incoming
     * connections. Call this function after receiving a ListenEvent event to
     * accept the connection. Before calling this function, the submitted handle
     * must be initialized.<br>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * When the ListenEvent event is emitted it is guaranteed that this
     * function will complete successfully the first time. If you attempt to use
     * it more than once, it may fail.<br/>
     * It is suggested to only call this function once per ListenEvent event.
     *
     * @note
     * Both the handles must be running on the same loop.
     *
     * @param ref An initialized handle to be used to accept the connection.
     */
    template<typename S>
    void accept(S &ref) {
        this->invoke(&uv_accept, this->template get<uv_stream_t>(), this->template get<uv_stream_t>(ref));
    }

    /**
     * @brief Starts reading data from an incoming stream.
     *
     * A DataEvent event will be emitted several times until there is no more
     * data to read or `stop()` is called.<br/>
     * An EndEvent event will be emitted when there is no more data to read.
     */
    void read() {
        this->invoke(&uv_read_start, this->template get<uv_stream_t>(), &this->allocCallback, &readCallback);
    }

    /**
     * @brief Stops reading data from the stream.
     *
     * This function is idempotent and may be safely called on a stopped stream.
     */
    void stop() {
        this->invoke(&uv_read_stop, this->template get<uv_stream_t>());
    }

    /**
     * @brief Writes data to the stream.
     *
     * Data are written in order. The handle takes the ownership of the data and
     * it is in charge of delete them.
     *
     * A WriteEvent event will be emitted when the data have been written.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param data The data to be written to the stream.
     * @param len The lenght of the submitted data.
     */
    template<typename Deleter>
    void write(std::unique_ptr<char[], Deleter> data, unsigned int len) {
        auto req = this->loop().template resource<details::WriteReq<Deleter>>(std::move(data), len);
        auto listener = [ptr = this->shared_from_this()](const auto &event, const auto &) {
            ptr->publish(event);
        };

        req->template once<ErrorEvent>(listener);
        req->template once<WriteEvent>(listener);
        req->write(this->template get<uv_stream_t>());
    }

    /**
     * @brief Writes data to the stream.
     *
     * Data are written in order. The handle doesn't take the ownership of the
     * data. Be sure that their lifetime overcome the one of the request.
     *
     * A WriteEvent event will be emitted when the data have been written.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param data The data to be written to the stream.
     * @param len The lenght of the submitted data.
     */
    void write(char *data, unsigned int len) {
        auto req = this->loop().template resource<details::WriteReq<void(*)(char *)>>(std::unique_ptr<char[], void(*)(char *)>{data, [](char *) {}}, len);
        auto listener = [ptr = this->shared_from_this()](const auto &event, const auto &) {
            ptr->publish(event);
        };

        req->template once<ErrorEvent>(listener);
        req->template once<WriteEvent>(listener);
        req->write(this->template get<uv_stream_t>());
    }

    /**
     * @brief Extended write function for sending handles over a pipe handle.
     *
     * The pipe must be initialized with `ipc == true`.
     *
     * `send` must be a TCPHandle or PipeHandle handle, which is a server or a
     * connection (listening or connected state). Bound sockets or pipes will be
     * assumed to be servers.
     *
     * The handle takes the ownership of the data and it is in charge of delete
     * them.
     *
     * A WriteEvent event will be emitted when the data have been written.<br/>
     * An ErrorEvent wvent will be emitted in case of errors.
     *
     * @param send The handle over which to write data.
     * @param data The data to be written to the stream.
     * @param len The lenght of the submitted data.
     */
    template<typename S, typename Deleter>
    void write(S &send, std::unique_ptr<char[], Deleter> data, unsigned int len) {
        auto req = this->loop().template resource<details::WriteReq<Deleter>>(std::move(data), len);
        auto listener = [ptr = this->shared_from_this()](const auto &event, const auto &) {
            ptr->publish(event);
        };

        req->template once<ErrorEvent>(listener);
        req->template once<WriteEvent>(listener);
        req->write(this->template get<uv_stream_t>(), this->template get<uv_stream_t>(send));
    }

    /**
     * @brief Extended write function for sending handles over a pipe handle.
     *
     * The pipe must be initialized with `ipc == true`.
     *
     * `send` must be a TCPHandle or PipeHandle handle, which is a server or a
     * connection (listening or connected state). Bound sockets or pipes will be
     * assumed to be servers.
     *
     * The handle doesn't take the ownership of the data. Be sure that their
     * lifetime overcome the one of the request.
     *
     * A WriteEvent event will be emitted when the data have been written.<br/>
     * An ErrorEvent wvent will be emitted in case of errors.
     *
     * @param send The handle over which to write data.
     * @param data The data to be written to the stream.
     * @param len The lenght of the submitted data.
     */
    template<typename S>
    void write(S &send, char *data, unsigned int len) {
        auto req = this->loop().template resource<details::WriteReq<void(*)(char *)>>(std::unique_ptr<char[], void(*)(char *)>{data, [](char *) {}}, len);
        auto listener = [ptr = this->shared_from_this()](const auto &event, const auto &) {
            ptr->publish(event);
        };

        req->template once<ErrorEvent>(listener);
        req->template once<WriteEvent>(listener);
        req->write(this->template get<uv_stream_t>(), this->template get<uv_stream_t>(send));
    }

    /**
     * @brief Queues a write request if it can be completed immediately.
     *
     * Same as `write()`, but won’t queue a write request if it can’t be
     * completed immediately.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param data The data to be written to the stream.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    int tryWrite(std::unique_ptr<char[]> data, unsigned int len) {
        uv_buf_t bufs[] = { uv_buf_init(data.get(), len) };
        auto bw = uv_try_write(this->template get<uv_stream_t>(), bufs, 1);

        if(bw < 0) {
            this->publish(ErrorEvent{bw});
            bw = 0;
        }

        return bw;
    }

    /**
     * @brief Queues a write request if it can be completed immediately.
     *
     * Same as `write()`, but won’t queue a write request if it can’t be
     * completed immediately.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param data The data to be written to the stream.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    int tryWrite(char *data, unsigned int len) {
        uv_buf_t bufs[] = { uv_buf_init(data, len) };
        auto bw = uv_try_write(this->template get<uv_stream_t>(), bufs, 1);

        if(bw < 0) {
            this->publish(ErrorEvent{bw});
            bw = 0;
        }

        return bw;
    }

    /**
     * @brief Checks if the stream is readable.
     * @return True if the stream is readable, false otherwise.
     */
    bool readable() const noexcept {
        return (uv_is_readable(this->template get<uv_stream_t>()) == 1);
    }

    /**
     * @brief Checks if the stream is writable.
     * @return True if the stream is writable, false otherwise.
     */
    bool writable() const noexcept {
        return (uv_is_writable(this->template get<uv_stream_t>()) == 1);
    }

    /**
     * @brief Enables or disables blocking mode for a stream.
     *
     * When blocking mode is enabled all writes complete synchronously. The
     * interface remains unchanged otherwise, e.g. completion or failure of the
     * operation will still be reported through events which are emitted
     * asynchronously.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/stream.html#c.uv_stream_set_blocking)
     * for further details.
     *
     * @param enable True to enable blocking mode, false otherwise.
     * @return True in case of success, false otherwise.
     */
    bool blocking(bool enable = false) {
        return (0 == uv_stream_set_blocking(this->template get<uv_stream_t>(), enable));
    }

    /**
     * @brief Gets the amount of queued bytes waiting to be sent.
     * @return Amount of queued bytes waiting to be sent.
     */
    size_t writeQueueSize() const noexcept {
        return uv_stream_get_write_queue_size(this->template get<uv_stream_t>());
    }
};


}


#ifndef UVW_AS_LIB
#include "stream.cpp"
#endif

#endif // UVW_STREAM_INCLUDE_H

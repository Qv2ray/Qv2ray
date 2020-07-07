#ifndef UVW_UDP_INCLUDE_H
#define UVW_UDP_INCLUDE_H


#include <type_traits>
#include <utility>
#include <cstddef>
#include <memory>
#include <string>
#include <uv.h>
#include "request.hpp"
#include "handle.hpp"
#include "util.h"


namespace uvw {


/**
 * @brief SendEvent event.
 *
 * It will be emitted by UDPHandle according with its functionalities.
 */
struct SendEvent {};


/**
 * @brief UDPDataEvent event.
 *
 * It will be emitted by UDPHandle according with its functionalities.
 */
struct UDPDataEvent {
    explicit UDPDataEvent(Addr sndr, std::unique_ptr<char[]> buf, std::size_t len, bool part) noexcept;

    std::unique_ptr<char[]> data; /*!< A bunch of data read on the stream. */
    std::size_t length;  /*!< The amount of data read on the stream. */
    Addr sender; /*!< A valid instance of Addr. */
    bool partial; /*!< True if the message was truncated, false otherwise. */
};


namespace details {


enum class UVUDPFlags: std::underlying_type_t<uv_udp_flags> {
    IPV6ONLY = UV_UDP_IPV6ONLY,
    UDP_PARTIAL = UV_UDP_PARTIAL,
    REUSEADDR = UV_UDP_REUSEADDR,
    UDP_MMSG_CHUNK = UV_UDP_MMSG_CHUNK,
    UDP_RECVMMSG = UV_UDP_RECVMMSG
};


enum class UVMembership: std::underlying_type_t<uv_membership> {
    LEAVE_GROUP = UV_LEAVE_GROUP,
    JOIN_GROUP = UV_JOIN_GROUP
};


class SendReq final: public Request<SendReq, uv_udp_send_t> {
public:
    using Deleter = void(*)(char *);

    SendReq(ConstructorAccess ca, std::shared_ptr<Loop> loop, std::unique_ptr<char[], Deleter> dt, unsigned int len);

    void send(uv_udp_t *handle, const struct sockaddr* addr);

private:
    std::unique_ptr<char[], Deleter> data;
    uv_buf_t buf;
};


}


/**
 * @brief The UDPHandle handle.
 *
 * UDP handles encapsulate UDP communication for both clients and servers.<br/>
 * By default, _IPv4_ is used as a template parameter. The handle already
 * supports _IPv6_ out-of-the-box by using `uvw::IPv6`.
 *
 * To create an `UDPHandle` through a `Loop`, arguments follow:
 *
 * * An optional integer value that indicates optional flags used to initialize
 * the socket.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/udp.html#c.uv_udp_init_ex)
 * for further details.
 */
class UDPHandle final: public Handle<UDPHandle, uv_udp_t> {
    template<typename I>
    static void recvCallback(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const sockaddr *addr, unsigned flags) {
        const typename details::IpTraits<I>::Type *aptr = reinterpret_cast<const typename details::IpTraits<I>::Type *>(addr);

        UDPHandle &udp = *(static_cast<UDPHandle*>(handle->data));
        // data will be destroyed no matter of what the value of nread is
        std::unique_ptr<char[]> data{buf->base};

        if(nread > 0) {
            // data available (can be truncated)
            udp.publish(UDPDataEvent{details::address<I>(aptr), std::move(data), static_cast<std::size_t>(nread), !(0 == (flags & UV_UDP_PARTIAL))});
        } else if(nread == 0 && addr == nullptr) {
            // no more data to be read, doing nothing is fine
        } else if(nread == 0 && addr != nullptr) {
            // empty udp packet
            udp.publish(UDPDataEvent{details::address<I>(aptr), std::move(data), static_cast<std::size_t>(nread), false});
        } else {
            // transmission error
            udp.publish(ErrorEvent(nread));
        }
    }

public:
    using Membership = details::UVMembership;
    using Bind = details::UVUDPFlags;
    using IPv4 = uvw::IPv4;
    using IPv6 = uvw::IPv6;

    explicit UDPHandle(ConstructorAccess ca, std::shared_ptr<Loop> ref, unsigned int f = {});

    /**
     * @brief Initializes the handle. The actual socket is created lazily.
     * @return True in case of success, false otherwise.
     */
    bool init();

    /**
     * @brief Opens an existing file descriptor or SOCKET as a UDP handle.
     *
     * The passed file descriptor or SOCKET is not checked for its type, but
     * it’s required that it represents a valid datagram socket.
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/udp.html#c.uv_udp_open)
     * for further details.
     *
     * @param socket A valid socket handle (either a file descriptor or a SOCKET).
     */
    void open(OSSocketHandle socket);

    /**
     * @brief Binds the UDP handle to an IP address and port.
     *
     * Available flags are:
     *
     * * `UDPHandle::Bind::IPV6ONLY`
     * * `UDPHandle::Bind::UDP_PARTIAL`
     * * `UDPHandle::Bind::REUSEADDR`
     * * `UDPHandle::Bind::UDP_RECVMMSG`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/udp.html#c.uv_udp_flags)
     * for further details.
     *
     * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
     * @param opts Optional additional flags.
     */
    void bind(const sockaddr &addr, Flags<Bind> opts = Flags<Bind>{});

    /**
     * @brief Associates the handle to a remote address and port (either IPv4 or
     * IPv6).
     *
     * Every message sent by this handle is automatically sent to the given
     * destination.<br/>
     * Trying to call this function on an already connected handle isn't
     * allowed.
     *
     * An ErrorEvent event is emitted in case of errors during the connection.
     *
     * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
     */
    void connect(const sockaddr &addr);

    /**
     * @brief Associates the handle to a remote address and port (either IPv4 or
     * IPv6).
     *
     * Every message sent by this handle is automatically sent to the given
     * destination.<br/>
     * Trying to call this function on an already connected handle isn't
     * allowed.
     *
     * An ErrorEvent event is emitted in case of errors during the connection.
     *
     * @param ip The address to which to bind.
     * @param port The port to which to bind.
     */
    template<typename I = IPv4>
    void connect(std::string ip, unsigned int port);

    /**
     * @brief Associates the handle to a remote address and port (either IPv4 or
     * IPv6).
     *
     * Every message sent by this handle is automatically sent to the given
     * destination.<br/>
     * Trying to call this function on an already connected handle isn't
     * allowed.
     *
     * An ErrorEvent event is emitted in case of errors during the connection.
     *
     * @param addr A valid instance of Addr.
     */
    template<typename I = IPv4>
    void connect(Addr addr);

    /**
     * @brief Disconnects the handle.
     *
     * Trying to disconnect a handle that is not connected isn't allowed.
     *
     * An ErrorEvent event is emitted in case of errors.
     */
    void disconnect();

    /**
     * @brief Gets the remote address to which the handle is connected, if any.
     * @return A valid instance of Addr, an empty one in case of errors.
     */
    template<typename I = IPv4>
    Addr peer() const noexcept;

    /**
     * @brief Binds the UDP handle to an IP address and port.
     *
     * Available flags are:
     *
     * * `UDPHandle::Bind::IPV6ONLY`
     * * `UDPHandle::Bind::UDP_PARTIAL`
     * * `UDPHandle::Bind::REUSEADDR`
     * * `UDPHandle::Bind::UDP_MMSG_CHUNK`
     * * `UDPHandle::Bind::UDP_RECVMMSG`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/udp.html#c.uv_udp_flags)
     * for further details.
     *
     * @param ip The IP address to which to bind.
     * @param port The port to which to bind.
     * @param opts Optional additional flags.
     */
    template<typename I = IPv4>
    void bind(std::string ip, unsigned int port, Flags<Bind> opts = Flags<Bind>{});

    /**
     * @brief Binds the UDP handle to an IP address and port.
     *
     * Available flags are:
     *
     * * `UDPHandle::Bind::IPV6ONLY`
     * * `UDPHandle::Bind::UDP_PARTIAL`
     * * `UDPHandle::Bind::REUSEADDR`
     * * `UDPHandle::Bind::UDP_MMSG_CHUNK`
     * * `UDPHandle::Bind::UDP_RECVMMSG`
     *
     * See the official
     * [documentation](http://docs.libuv.org/en/v1.x/udp.html#c.uv_udp_flags)
     * for further details.
     *
     * @param addr A valid instance of Addr.
     * @param opts Optional additional flags.
     */
    template<typename I = IPv4>
    void bind(Addr addr, Flags<Bind> opts = Flags<Bind>{});

    /**
     * @brief Get the local IP and port of the UDP handle.
     * @return A valid instance of Addr, an empty one in case of errors.
     */
    template<typename I = IPv4>
    Addr sock() const noexcept;

    /**
     * @brief Sets membership for a multicast address.
     *
     * Available values for `membership` are:
     *
     * * `UDPHandle::Membership::LEAVE_GROUP`
     * * `UDPHandle::Membership::JOIN_GROUP`
     *
     * @param multicast Multicast address to set membership for.
     * @param iface Interface address.
     * @param membership Action to be performed.
     * @return True in case of success, false otherwise.
     */
    template<typename I = IPv4>
    bool multicastMembership(std::string multicast, std::string iface, Membership membership);

    /**
     * @brief Sets IP multicast loop flag.
     *
     * This makes multicast packets loop back to local sockets.
     *
     * @param enable True to enable multicast loop, false otherwise.
     * @return True in case of success, false otherwise.
     */
    bool multicastLoop(bool enable = true);

    /**
     * @brief Sets the multicast ttl.
     * @param val A value in the range `[1, 255]`.
     * @return True in case of success, false otherwise.
     */
    bool multicastTtl(int val);

    /**
     * @brief Sets the multicast interface to send or receive data on.
     * @param iface Interface address.
     * @return True in case of success, false otherwise.
     */
    template<typename I = IPv4>
    bool multicastInterface(std::string iface);

    /**
     * @brief Sets broadcast on or off.
     * @param enable True to set broadcast on, false otherwise.
     * @return True in case of success, false otherwise.
     */
    bool broadcast(bool enable = false);

    /**
     * @brief Sets the time to live.
     * @param val A value in the range `[1, 255]`.
     * @return True in case of success, false otherwise.
     */
    bool ttl(int val);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * will be bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a
     * random port number.
     *
     * The handle takes the ownership of the data and it is in charge of delete
     * them.
     *
     * A SendEvent event will be emitted when the data have been sent.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     */
    void send(const sockaddr &addr, std::unique_ptr<char[]> data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * will be bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a
     * random port number.
     *
     * The handle takes the ownership of the data and it is in charge of delete
     * them.
     *
     * A SendEvent event will be emitted when the data have been sent.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param ip The address to which to send data.
     * @param port The port to which to send data.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     */
    template<typename I = IPv4>
    void send(std::string ip, unsigned int port, std::unique_ptr<char[]> data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * will be bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a
     * random port number.
     *
     * The handle takes the ownership of the data and it is in charge of delete
     * them.
     *
     * A SendEvent event will be emitted when the data have been sent.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param addr A valid instance of Addr.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     */
    template<typename I = IPv4>
    void send(Addr addr, std::unique_ptr<char[]> data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * will be bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a
     * random port number.
     *
     * The handle doesn't take the ownership of the data. Be sure that their
     * lifetime overcome the one of the request.
     *
     * A SendEvent event will be emitted when the data have been sent.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     */
    void send(const sockaddr &addr, char *data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * will be bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a
     * random port number.
     *
     * The handle doesn't take the ownership of the data. Be sure that their
     * lifetime overcome the one of the request.
     *
     * A SendEvent event will be emitted when the data have been sent.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param ip The address to which to send data.
     * @param port The port to which to send data.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     */
    template<typename I = IPv4>
    void send(std::string ip, unsigned int port, char *data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * will be bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a
     * random port number.
     *
     * The handle doesn't take the ownership of the data. Be sure that their
     * lifetime overcome the one of the request.
     *
     * A SendEvent event will be emitted when the data have been sent.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     *
     * @param addr A valid instance of Addr.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     */
    template<typename I = IPv4>
    void send(Addr addr, char *data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Same as `send()`, but it won’t queue a send request if it can’t be
     * completed immediately.
     *
     * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    template<typename I = IPv4>
    int trySend(const sockaddr &addr, std::unique_ptr<char[]> data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Same as `send()`, but it won’t queue a send request if it can’t be
     * completed immediately.
     *
     * @param ip The address to which to send data.
     * @param port The port to which to send data.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    template<typename I = IPv4>
    int trySend(std::string ip, unsigned int port, std::unique_ptr<char[]> data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Same as `send()`, but it won’t queue a send request if it can’t be
     * completed immediately.
     *
     * @param addr A valid instance of Addr.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    template<typename I = IPv4>
    int trySend(Addr addr, std::unique_ptr<char[]> data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Same as `send()`, but it won’t queue a send request if it can’t be
     * completed immediately.
     *
     * @param addr Initialized `sockaddr_in` or `sockaddr_in6` data structure.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    template<typename I = IPv4>
    int trySend(const sockaddr &addr, char *data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Same as `send()`, but it won’t queue a send request if it can’t be
     * completed immediately.
     *
     * @param ip The address to which to send data.
     * @param port The port to which to send data.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    template<typename I = IPv4>
    int trySend(std::string ip, unsigned int port, char *data, unsigned int len);

    /**
     * @brief Sends data over the UDP socket.
     *
     * Same as `send()`, but it won’t queue a send request if it can’t be
     * completed immediately.
     *
     * @param addr A valid instance of Addr.
     * @param data The data to be sent.
     * @param len The lenght of the submitted data.
     * @return Number of bytes written.
     */
    template<typename I = IPv4>
    int trySend(Addr addr, char *data, unsigned int len);

    /**
     * @brief Prepares for receiving data.
     *
     * Note that if the socket has not previously been bound with `bind()`, it
     * is bound to `0.0.0.0` (the _all interfaces_ IPv4 address) and a random
     * port number.
     *
     * An UDPDataEvent event will be emitted when the handle receives data.<br/>
     * An ErrorEvent event will be emitted in case of errors.
     */
    template<typename I = IPv4>
    void recv();

    /**
     * @brief Stops listening for incoming datagrams.
     */
    void stop();

    /**
     * @brief Gets the number of bytes queued for sending.
     *
     * It strictly shows how much information is currently queued.
     *
     * @return Number of bytes queued for sending.
     */
    size_t sendQueueSize() const noexcept;

    /**
     * @brief Number of send requests currently in the queue awaiting to be processed.
     * @return Number of send requests currently in the queue.
     */
    size_t sendQueueCount() const noexcept;

private:
    enum { DEFAULT, FLAGS } tag{DEFAULT};
    unsigned int flags{};
};


/**
 * @cond TURN_OFF_DOXYGEN
 * Internal details not to be documented.
 */


// (extern) explicit instantiations

extern template void UDPHandle::connect<IPv4>(std::string, unsigned int);
extern template void UDPHandle::connect<IPv6>(std::string, unsigned int);

extern template void UDPHandle::connect<IPv4>(Addr);
extern template void UDPHandle::connect<IPv6>(Addr);

extern template Addr UDPHandle::peer<IPv4>() const noexcept;
extern template Addr UDPHandle::peer<IPv6>() const noexcept;

extern template void UDPHandle::bind<IPv4>(std::string, unsigned int, Flags<Bind>);
extern template void UDPHandle::bind<IPv6>(std::string, unsigned int, Flags<Bind>);

extern template void UDPHandle::bind<IPv4>(Addr, Flags<Bind>);
extern template void UDPHandle::bind<IPv6>(Addr, Flags<Bind>);

extern template Addr UDPHandle::sock<IPv4>() const noexcept;
extern template Addr UDPHandle::sock<IPv6>() const noexcept;

extern template bool UDPHandle::multicastMembership<IPv4>(std::string, std::string, Membership);
extern template bool UDPHandle::multicastMembership<IPv6>(std::string, std::string, Membership);

extern template bool UDPHandle::multicastInterface<IPv4>(std::string);
extern template bool UDPHandle::multicastInterface<IPv6>(std::string);

extern template void UDPHandle::send<IPv4>(std::string, unsigned int, std::unique_ptr<char[]>, unsigned int);
extern template void UDPHandle::send<IPv6>(std::string, unsigned int, std::unique_ptr<char[]>, unsigned int);

extern template void UDPHandle::send<IPv4>(Addr, std::unique_ptr<char[]>, unsigned int);
extern template void UDPHandle::send<IPv6>(Addr, std::unique_ptr<char[]>, unsigned int);

extern template void UDPHandle::send<IPv4>(std::string, unsigned int, char *, unsigned int);
extern template void UDPHandle::send<IPv6>(std::string, unsigned int, char *, unsigned int);

extern template void UDPHandle::send<IPv4>(Addr, char *, unsigned int);
extern template void UDPHandle::send<IPv6>(Addr, char *, unsigned int);

extern template int UDPHandle::trySend<IPv4>(const sockaddr &, std::unique_ptr<char[]>, unsigned int);
extern template int UDPHandle::trySend<IPv6>(const sockaddr &, std::unique_ptr<char[]>, unsigned int);

extern template int UDPHandle::trySend<IPv4>(std::string, unsigned int, std::unique_ptr<char[]>, unsigned int);
extern template int UDPHandle::trySend<IPv6>(std::string, unsigned int, std::unique_ptr<char[]>, unsigned int);

extern template int UDPHandle::trySend<IPv4>(Addr, std::unique_ptr<char[]>, unsigned int);
extern template int UDPHandle::trySend<IPv6>(Addr, std::unique_ptr<char[]>, unsigned int);

extern template int UDPHandle::trySend<IPv4>(const sockaddr &, char *, unsigned int);
extern template int UDPHandle::trySend<IPv6>(const sockaddr &, char *, unsigned int);

extern template int UDPHandle::trySend<IPv4>(std::string, unsigned int, char *, unsigned int);
extern template int UDPHandle::trySend<IPv6>(std::string, unsigned int, char *, unsigned int);

extern template int UDPHandle::trySend<IPv4>(Addr, char *, unsigned int);
extern template int UDPHandle::trySend<IPv6>(Addr, char *, unsigned int);

extern template void UDPHandle::recv<IPv4>();
extern template void UDPHandle::recv<IPv6>();


/**
 * Internal details not to be documented.
 * @endcond
 */


}


#ifndef UVW_AS_LIB
#include "udp.cpp"
#endif

#endif // UVW_UDP_INCLUDE_H

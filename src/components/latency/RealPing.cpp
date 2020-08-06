#include "RealPing.hpp"

#include <chrono>
#include <memory>
#include <uvw.hpp>

struct RealPingContext
{
    std::shared_ptr<uvw::PollHandle> handle;
    curl_socket_t sockfd;
};
struct RealPingGlobalInfo
{
    std::shared_ptr<Qv2ray::components::latency::realping::RealPing> _preserve_life_time;
    CURLM *multiHandle;
    std::shared_ptr<uvw::TimerHandle> timer;
    int *successCountPtr;
    LatencyTestResult *latencyResultPtr;
};
static RealPingContext *create_curl_context(curl_socket_t sockfd, uvw::Loop &loop)
{
    auto *context = new RealPingContext;
    context->sockfd = sockfd;
    uvw::OSSocketHandle osSocketHandle{ sockfd };
    context->handle = loop.resource<uvw::PollHandle>(osSocketHandle);
    return context;
}

static void check_multi_info(CURLM *curl_handle, int *success_num, LatencyTestResult *latencyTestResultPtr, RealPingGlobalInfo *info)
{
    CURLMsg *message{ nullptr };
    int pending;
    CURL *easy_handle;

    while ((message = curl_multi_info_read(curl_handle, &pending)))
    {
        switch (message->msg)
        {
            case CURLMSG_DONE:
                /* Do not use message data after calling curl_multi_remove_handle() and
                   curl_easy_cleanup(). As per curl_multi_info_read() docs:
                   "WARNING: The data the returned pointer points to will not survive
                   calling curl_multi_cleanup, curl_multi_remove_handle or
                   curl_easy_cleanup." */
                easy_handle = message->easy_handle;
                if (message->data.result == CURLE_OK)
                {
                    success_num[0] += 1;
                    auto ms = info->_preserve_life_time->getHandleTime(easy_handle);
                    latencyTestResultPtr->avg += ms;
                    latencyTestResultPtr->best = std::min(latencyTestResultPtr->best, ms);
                    latencyTestResultPtr->worst = std::max(latencyTestResultPtr->worst, ms);
                }
                else
                    latencyTestResultPtr->failedCount += 1;
                curl_multi_remove_handle(curl_handle, easy_handle);
                curl_easy_cleanup(easy_handle);
                break;
            default: break;
        }
    }
}

static int start_timeout(CURLM *, long timeout_ms, void *userp)
{
    auto globalInfo = static_cast<RealPingGlobalInfo *>(userp);
    if (timeout_ms < 0)
    {
        globalInfo->timer->stop();
    }
    else
    {
        if (timeout_ms == 0)
            timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it
                         in a bit */
        globalInfo->timer->start(uvw::TimerHandle::Time{ timeout_ms }, uvw::TimerHandle::Time{ 0 });
    }
    return 0;
}
static int handle_socket(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp)
{
    auto pRealPingGlobalInfo = static_cast<RealPingGlobalInfo *>(userp);
    RealPingContext *curl_context;
    uvw::Flags<uvw::PollHandle::Event> events{};

    switch (action)
    {
        case CURL_POLL_IN:
        case CURL_POLL_OUT:
            curl_context = socketp ? (RealPingContext *) socketp : create_curl_context(s, pRealPingGlobalInfo->timer->loop());

            if (!socketp)
                curl_multi_assign(pRealPingGlobalInfo->multiHandle, s, (void *) curl_context);

            if (action == CURL_POLL_IN)
                events = events | uvw::Flags<uvw::PollHandle::Event>::from<uvw::PollHandle::Event::READABLE>();
            if (action == CURL_POLL_OUT)
                events = events | uvw::Flags<uvw::PollHandle::Event>::from<uvw::PollHandle::Event::WRITABLE>();
            curl_context->handle->on<uvw::ErrorEvent>(
                [sockfd = curl_context->sockfd, pRealPingGlobalInfo](uvw::ErrorEvent &e, uvw::PollHandle &h) {
                    int running_handles;
                    auto flags = CURL_CSELECT_ERR;
                    curl_multi_socket_action(pRealPingGlobalInfo->multiHandle, sockfd, flags, &running_handles);
                    check_multi_info(pRealPingGlobalInfo->multiHandle, pRealPingGlobalInfo->successCountPtr,
                                     pRealPingGlobalInfo->latencyResultPtr, pRealPingGlobalInfo);
                    pRealPingGlobalInfo->_preserve_life_time->notifyTestHost();
                });
            curl_context->handle->on<uvw::PollEvent>(
                [sockfd = curl_context->sockfd, pRealPingGlobalInfo](uvw::PollEvent &e, uvw::PollHandle &h) {
                    int running_handles;
                    int flags = 0;

                    if (e.flags & uvw::Flags<uvw::PollHandle::Event>{ uvw::PollHandle::Event::READABLE })
                        flags |= CURL_CSELECT_IN;
                    if (e.flags & uvw::Flags<uvw::PollHandle::Event>{ uvw::PollHandle::Event::WRITABLE })
                        flags |= CURL_CSELECT_OUT;

                    curl_multi_socket_action(pRealPingGlobalInfo->multiHandle, sockfd, flags, &running_handles);
                    check_multi_info(pRealPingGlobalInfo->multiHandle, pRealPingGlobalInfo->successCountPtr,
                                     pRealPingGlobalInfo->latencyResultPtr, pRealPingGlobalInfo);
                    pRealPingGlobalInfo->_preserve_life_time->notifyTestHost();
                });
            pRealPingGlobalInfo->_preserve_life_time->recordHanleTime(easy);
            curl_context->handle->start(events);
            break;
        case CURL_POLL_REMOVE:
            if (socketp)
            {
                curl_context = static_cast<RealPingContext *>(socketp);
                curl_context->handle->stop();
                curl_context->handle->close();
                delete curl_context;
                curl_multi_assign(pRealPingGlobalInfo->multiHandle, s, nullptr);
            }
            break;
        default: break;
    }
    return 0;
}
static size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
    return size * nmemb;
}
namespace Qv2ray::components::latency::realping
{
    RealPing::RealPing(std::shared_ptr<uvw::Loop> loopin, LatencyTestRequest &req, LatencyTestHost *testHost)
        : loop(std::move(loopin)), req(std::move(req)), testHost(testHost), timeout(loop->resource<uvw::TimerHandle>())
    {
    }
    void RealPing::start(const std::string &request_name)
    {
        data.totalCount = 0;
        data.failedCount = 0;
        data.worst = 0;
        data.avg = 0;
        if (!GlobalConfig.inboundConfig.useSocks && !GlobalConfig.inboundConfig.useHTTP)
        {
            data.avg = LATENCY_TEST_VALUE_ERROR;
            testHost->OnLatencyTestCompleted(req.id, data);
            return;
        }
        auto local_proxy_address =
            (!GlobalConfig.inboundConfig.useHTTP ?
                 "socks5://" + GlobalConfig.inboundConfig.listenip + ":" + QSTRN(GlobalConfig.inboundConfig.socksSettings.port) :
                 "http://" + GlobalConfig.inboundConfig.listenip + ":" + QSTRN(GlobalConfig.inboundConfig.httpSettings.port))
                .toStdString();
        auto curlMultiHandle = curl_multi_init();
        auto globalInfo = std::make_shared<RealPingGlobalInfo>(
            RealPingGlobalInfo{ shared_from_this(), curlMultiHandle, timeout->shared_from_this(), &successCount, &data });
        timeout->once<uvw::CloseEvent>([globalInfo, curlMultiHandle](auto &, auto &h) {
            curl_multi_cleanup(curlMultiHandle);
            h.clear();
        });
        timeout->on<uvw::TimerEvent>([globalInfo, curlMultiHandle, suc_cnt_ptr = &successCount, fail_cnt_ptr = &data, this](auto &, auto &h) {
            int running_handles;
            curl_multi_socket_action(curlMultiHandle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
            check_multi_info(curlMultiHandle, suc_cnt_ptr, fail_cnt_ptr, globalInfo.get());
            notifyTestHost();
        });
        curl_multi_setopt(curlMultiHandle, CURLMOPT_SOCKETFUNCTION, handle_socket);
        curl_multi_setopt(curlMultiHandle, CURLMOPT_SOCKETDATA, globalInfo.get());
        curl_multi_setopt(curlMultiHandle, CURLMOPT_TIMERDATA, globalInfo.get());
        curl_multi_setopt(curlMultiHandle, CURLMOPT_TIMERFUNCTION, start_timeout);
        for (int i = 0; i < req.totalCount; ++i)
        {
            auto handle = curl_easy_init();
            curl_easy_setopt(handle, CURLOPT_URL, request_name.c_str());
            curl_easy_setopt(handle, CURLOPT_PROXY, local_proxy_address.c_str());
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, noop_cb);
            curl_multi_add_handle(curlMultiHandle, handle);
        }
    }
    void RealPing::notifyTestHost()
    {
        if (data.failedCount + successCount == req.totalCount)
        {
            if (data.failedCount == req.totalCount)
                data.avg = LATENCY_TEST_VALUE_ERROR;
            else
                data.errorMessage.clear(), data.avg = data.avg / successCount;
            testHost->OnLatencyTestCompleted(req.id, data);
            timeout->close();
        }
    }
    RealPing::~RealPing()
    {
        LOG(MODULE_NETWORK, "Realping done!")
    }
    long RealPing::getHandleTime(CURL *h)
    {
        if (reqStartTime.find(h) == reqStartTime.end())
            return 0;
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - reqStartTime[h]).count();
    }
    void RealPing::recordHanleTime(CURL *h)
    {
        reqStartTime.emplace(h, std::chrono::system_clock::now());
    }
} // namespace Qv2ray::components::latency::realping

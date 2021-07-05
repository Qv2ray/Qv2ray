#include "APIBackend.hpp"

#include "uvw.hpp"

#define QV_MODULE_NAME "gRPCBackend"

namespace Qv2ray::core::kernel{
    unsigned char settings_ack[9] = {0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00};

    unsigned char http2_headers[83] = {
        0x00, 0x00, 0x4a, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x83, 0x86, 0x04, 0xA4, 0x63, 0xB8, 0xAC,
        0x1F, 0xA5, 0xC8,
        0x7B, 0x0A, 0xB8, 0xEB, 0xAD, 0x74, 0x24, 0x69, 0x42, 0xE4, 0x3D, 0x34, 0x8E, 0xA9, 0x17, 0xDC,
        0x91, 0xA5, 0x1B, 0x8B, 0x67, 0x73, 0x10, 0xAC, 0x62, 0x2A, 0x77, 0x24, 0x69, 0x47, 0x40, 0x82,
        0x49, 0x7F, 0x86, 0x4D, 0x83, 0x35, 0x05, 0xB1, 0x1F, 0x5F, 0x8B, 0x1D, 0x75, 0xD0, 0x62, 0x0D,
        0x26, 0x3D, 0x4C, 0x4D, 0x65, 0x64, 0x7A, 0x88, 0x49, 0xEA, 0x31, 0x18, 0x02, 0xED, 0x2E, 0xCF,
    };

    unsigned char end_stream[9] = {
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01
    };

    char HTTP2_MAGIC[25] = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
    using uchar = unsigned char;

    /// Utility function to decode a SLEB128 value.
    inline int64_t decodeSLEB128(const uint8_t *p, unsigned *n = nullptr,
                                 const uint8_t *end = nullptr,
                                 const char **error = nullptr) {
        const uint8_t *orig_p = p;
        int64_t Value = 0;
        unsigned Shift = 0;
        uint8_t Byte;
        if (error)
            *error = nullptr;
        do {
            if (p == end) {
                if (error)
                    *error = "malformed sleb128, extends past end";
                if (n)
                    *n = (unsigned)(p - orig_p);
                return 0;
            }
            Byte = *p;
            uint64_t Slice = Byte & 0x7f;
            if ((Shift >= 64 && Slice != (Value < 0 ? 0x7f : 0x00)) ||
                (Shift == 63 && Slice != 0 && Slice != 0x7f)) {
                if (error)
                    *error = "sleb128 too big for int64";
                if (n)
                    *n = (unsigned)(p - orig_p);
                return 0;
            }
            Value |= Slice << Shift;
            Shift += 7;
            ++p;
        } while (Byte >= 128);
        // Sign extend negative numbers if needed.
        if (Shift < 64 && (Byte & 0x40))
            Value |= (-1ULL) << Shift;
        if (n)
            *n = (unsigned)(p - orig_p);
        return Value;
    }

    std::unique_ptr<char[]>
    construct_protobuf_request(const std::string &name, bool reset, int stream_identifier, size_t &total_len) {
        auto stream_id = htonl(stream_identifier);
        auto stream_id_ptr = reinterpret_cast<char *>(&stream_id);
        auto protobuf_len = name.size() + 2;
        if (reset)
            protobuf_len += 2;
        auto data_len = protobuf_len + 5;
        auto protobuf_len_be = htonl(protobuf_len);
        auto protobuf_len_be_ptr = reinterpret_cast<char *>(&protobuf_len_be);
        auto data_frame_len = data_len + 9;
        char user_data_frame[16] = {0x00, 0x00, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x0a, (char) (name.size())};
        auto data_len_be = htonl(data_len);
        auto data_len_be_ptr = reinterpret_cast<char *>(&data_len_be);
        memcpy(user_data_frame, data_len_be_ptr + 1, 3);
        memcpy(user_data_frame + 5, stream_id_ptr, sizeof(uint32_t));
        memcpy(user_data_frame + 10, protobuf_len_be_ptr, sizeof(uint32_t));
        total_len = sizeof(http2_headers) + data_frame_len + sizeof(end_stream);
        std::unique_ptr<char[]> ptr{new char[total_len]};
        memcpy(ptr.get(), http2_headers, sizeof(http2_headers));
        memcpy(ptr.get() + 5, stream_id_ptr, sizeof(uint32_t)); // set stream id
        memcpy(ptr.get() + sizeof(http2_headers), user_data_frame, sizeof(user_data_frame));
        memcpy(ptr.get() + sizeof(http2_headers) + sizeof(user_data_frame), name.data(), name.size());
        if (reset) {
            char reset_data[2] = {0x10, 0x01};
            memcpy(ptr.get() + sizeof(http2_headers) + sizeof(user_data_frame) + name.size(), reset_data, 2);
        }
        memcpy(ptr.get() + sizeof(http2_headers) + data_frame_len, end_stream, sizeof(end_stream));
        memcpy(ptr.get() + sizeof(http2_headers) + data_frame_len + 5, stream_id_ptr, sizeof(uint32_t)); // set stream id
        return ptr;
    }

    struct ParseV2rayAPI{
        void parse_v2ray_api_response(std::unique_ptr<char[]> &ptr, size_t st) {
            // ptr[st+9] => is compressed
            // ptr[st+10,st+13] => four bytes protobuf len
            // ptr[st+14] => field number and wired type => stats
            // ptr[st+15] => stats len
            // ptr[st+16] => field number and wired type => string
            // ptr[st+17] => string len
            using namespace std;
            int stream_id=
                ((uchar) (ptr[st + 5]) << 24) + ((uchar) (ptr[st + 6]) << 16) + ((uchar) (ptr[st + 7]) << 8) +
                ((uchar) ptr[st + 8]);
            size_t protobuf_len =
                ((uchar) (ptr[st + 10]) << 24) + ((uchar) (ptr[st + 11]) << 16) + ((uchar) (ptr[st + 12]) << 8) +
                ((uchar) ptr[st + 13]);
            std::size_t name_len = (uchar) ptr[st + 17];
            //printf("proto buf len:%ld, name len:%ld\n", protobuf_len, name_len);
            auto str = std::string{&ptr[st + 18], name_len};
            qint64 value = 0;
            if (protobuf_len - name_len - 4 != 0){
                unsigned value_len = protobuf_len - name_len - 5;
                value = decodeSLEB128(reinterpret_cast<const uint8_t *>(&ptr[st + 18 + name_len+1]), &value_len,
                                      reinterpret_cast<const uint8_t *>(&ptr[st + 18 + name_len +1+ value_len]));
            }
            if(str.find("uplink")){
                if(streamIDType.count(stream_id)){
                    statsResult[streamIDType[stream_id]].first+=std::max(value,0ll);
                }
            }else if(str.find("downlink")){
                if(streamIDType.count(stream_id)){
                    statsResult[streamIDType[stream_id]].second+=std::max(value,0ll);
                }
            }
            resCount+=1;
            //cout<<"res cnt:"<<resCount<<" req cnt:"<<reqCount<<endl;
        }
        void addRequest(std::shared_ptr<uvw::TCPHandle>& tcp,QvAPITagProtocolConfig& config){
            streamIDType.clear();
            statsResult.clear();
            reqCount=resCount=0;
            for (const auto &[tag, config] : config)
            {
                size_t data_len;
                const QString prefix = config.type == API_INBOUND ? "inbound" : "outbound";
                QString name = (prefix % ">>>" % tag % ">>>traffic>>>uplink");
                streamState += 2;
                auto protobufRequest = construct_protobuf_request(name.toStdString(), true, streamState, data_len);
                tcp->write(std::move(protobufRequest), data_len);
                streamIDType[streamState]=config.type;
                name = (prefix % ">>>" % tag % ">>>traffic>>>downlink");
                streamState += 2;
                protobufRequest = construct_protobuf_request(name.toStdString(), true, streamState, data_len);
                tcp->write(std::move(protobufRequest), data_len);
                streamIDType[streamState]=config.type;
                reqCount+=2;
            }
        }
        enum class ParserState{
            READY,
            NO_REQ,
            NOT_READY
        };
        ParserState poll() const{
            if(reqCount==0){
                return ParserState::NO_REQ;
            }else if(reqCount==resCount){
                return ParserState::READY;
            }else{
                return ParserState::NOT_READY;
            }
        }

        const QMap<StatisticsType,QvStatsSpeed>& emitData(){
            reqCount=resCount=0;
            return statsResult;
        }

      private:
        QMap<StatisticsType, QvStatsSpeed> statsResult;
        QMap<int, StatisticsType> streamIDType;
        int streamState = -1;
        int reqCount = 0;
        int resCount = 0;
    };

    void conn(const std::shared_ptr<uvw::TCPHandle> &tcp,int& apiFailCounter,bool& dialed,
              bool& readyReq,
              ParseV2rayAPI& apiParser
    ) {

        tcp->on<uvw::ErrorEvent>([&apiFailCounter,&dialed,&readyReq](const uvw::ErrorEvent & e, uvw::TCPHandle &tcp) { /* handle errors */
          LOG("API call failure with error:"+ QString(e.what()));
          if(dialed){
               apiFailCounter++;
               tcp.close();
               dialed= false;
               readyReq = false;
           }
        });

        tcp->once<uvw::ConnectEvent>([&dialed,&readyReq](const uvw::ConnectEvent &, uvw::TCPHandle &tcp) {
          tcp.read();
          tcp.write(HTTP2_MAGIC, 24);
          dialed= true;
          readyReq=true;
        });
        tcp->on<uvw::DataEvent>([&apiParser](uvw::DataEvent &data, uvw::TCPHandle &tcp) {
                                  using namespace std;
                                  auto st = size_t(0);
                                  while (st < data.length) {
                                      size_t frame_len =
                                          ((uchar) (data.data[st]) << 16) + (((uchar) data.data[st + 1]) << 8) + ((uchar) data.data[st + 2]) +
                                          9;
                                      unsigned frame_type = (unsigned char) (data.data[st + 3]);
                                      //cout << "frame len:" << frame_len << " frame type:" << frame_type << endl;
                                      std::unique_ptr<char[]> buf;
                                      switch (frame_type) {
                                          case 0x06:
                                              //ping, need response
                                              if (data.data[4] == 0x01) break;
                                              buf = make_unique<char[]>(frame_len);
                                              memcpy(buf.get(), data.data.get() + st, frame_len);
                                              buf[4] = 0x01;
                                              tcp.write(std::move(buf), frame_len);
                                              break;
                                          case 0x04:
                                              //settings, need response
                                              tcp.write(reinterpret_cast<char *>(settings_ack), 9);
                                              break;
                                          case 0x00:
                                              //data
                                              apiParser.parse_v2ray_api_response(data.data, st);
                                              break;
                                          default:
                                              // ignore it
                                              break;
                                      }
                                      st += frame_len;
                                  }
                                }
        );
        tcp->connect(std::string{"127.0.0.1"}, GlobalConfig.kernelConfig.statsPort);
    }
}

namespace Qv2ray::core::kernel
{
    constexpr auto Qv2ray_GRPC_ERROR_RETCODE = -1;
    static QvAPIDataTypeConfig DefaultInboundAPIConfig{ { API_INBOUND, { "dokodemo-door", "http", "socks" } } };
    static QvAPIDataTypeConfig DefaultOutboundAPIConfig{ { API_OUTBOUND_PROXY,
                                                           { "dns", "http", "mtproto", "shadowsocks", "socks", "vmess", "vless" } },
                                                         { API_OUTBOUND_DIRECT, { "freedom" } },
                                                         { API_OUTBOUND_BLACKHOLE, { "blackhole" } } };

    APIWorker::APIWorker()
    {
        workThread = new QThread();
        this->moveToThread(workThread);
        DEBUG("API Worker initialised.");
        connect(workThread, &QThread::started, this, &APIWorker::process);
        connect(workThread, &QThread::finished, [] { LOG("API thread stopped"); });
        started = true;
        workThread->start();
    }

    void APIWorker::StartAPI(const QMap<bool, QMap<QString, QString>> &tagProtocolPair)
    {
        // Config API
        tagProtocolConfig.clear();
        for (const auto &key : tagProtocolPair.keys())
        {
            const auto config = key ? DefaultOutboundAPIConfig : DefaultInboundAPIConfig;
            for (const auto &[tag, protocol] : tagProtocolPair[key].toStdMap())
            {
                for (const auto &[type, protocols] : config)
                {
                    if (protocols.contains(protocol))
                        tagProtocolConfig[tag] = { protocol, type };
                }
            }
        }

        running = true;
    }

    void APIWorker::StopAPI()
    {
        running = false;
    }

    // --- DESTRUCTOR ---
    APIWorker::~APIWorker()
    {
        StopAPI();
        // Set started signal to false and wait for API thread to stop.
        started = false;
        workThread->wait();
        delete workThread;
    }

    // API Core Operations
    // Start processing data.
    void APIWorker::process()
    {
        DEBUG("API Worker started.");
#ifndef _WIN32
        signal(SIGPIPE, SIG_IGN);
#endif
        bool dialed = false;
        bool readyReq = false;
        int apiFailCounter = 0;
        auto loop = uvw::Loop::getDefault();
        auto tcp = loop->resource<uvw::TCPHandle>();
        auto stopTimer = loop->resource<uvw::TimerHandle>();
        auto apiParser = ParseV2rayAPI{};
        stopTimer->on<uvw::TimerEvent>([this,&loop,&tcp,&apiFailCounter,&dialed,&apiParser,&readyReq](auto &, auto &handle) {
            if (!running)
            {
                int timer_count = 0;
                uv_walk(
                    loop->raw(),
                    [](uv_handle_t *handle, void *arg)
                    {
                        int &counter = *static_cast<int *>(arg);
                        if (uv_is_closing(handle) == 0)
                            counter++;
                    },
                    &timer_count);
                if (timer_count == 1) // only current timer
                {
                    handle.stop();
                    handle.close();
                    loop->clear();
                    loop->close();
                    loop->stop();
                }
            }else{
                if (!dialed){
                    tcp = loop->resource<uvw::TCPHandle>();
                    conn(tcp,apiFailCounter,dialed,readyReq,apiParser);
                    dialed=true;
                }
                if(!readyReq){
                    return;
                }
                if (apiFailCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
                {
                    LOG("API call failure threshold reached, cancelling further API aclls.");
                    emit OnAPIErrored(tr("Failed to get statistics data, please check if V2Ray is running properly"));
                    apiFailCounter++;
                    return;
                }
                else if (apiFailCounter > QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
                {
                    return;
                }
                switch (apiParser.poll())
                {
                    case ParseV2rayAPI::ParserState::READY:
                        emit onAPIDataReady(apiParser.emitData());
                        break;
                    case ParseV2rayAPI::ParserState::NO_REQ:
                        apiParser.addRequest(tcp,tagProtocolConfig);
                        break;
                    case ParseV2rayAPI::ParserState::NOT_READY:
                        break;
                }
            }
          });
        stopTimer->start(uvw::TimerHandle::Time{ 1000 }, uvw::TimerHandle::Time{ 1000 });
        loop->run();
    }

} // namespace Qv2ray::core::kernel

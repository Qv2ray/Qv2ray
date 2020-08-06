#pragma once
#include "DNSBase.hpp"
#include "LatencyTest.hpp"
#include "base/Qv2rayBase.hpp"

#include <type_traits>
namespace Qv2ray::components::latency::tcping
{
    class TCPing : public DNSBase<TCPing>
    {
      public:
        using DNSBase<TCPing>::DNSBase;
        void start();
        ~TCPing();

      protected:
      private:
        void ping() override;
        void notifyTestHost();
    };
} // namespace Qv2ray::components::latency::tcping

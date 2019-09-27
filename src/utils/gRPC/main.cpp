#include <iostream>
#include <grpcpp/grpcpp.h>
#include "command.pb.h"
#include "command.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace std;
using namespace v2ray::core::app::stats::command;


int main() {
    auto channel = grpc::CreateChannel("127.0.0.1:59722", grpc::InsecureChannelCredentials());
    StatsService service;
    auto stub = service.NewStub(channel);
    ClientContext context;
    GetStatsRequest request;
    GetStatsResponse response;
    request.set_name("inbound>>>socks-in>>>traffic>>>downlink");
    request.set_reset(false);
    Status status = stub->GetStats(&context, request, &response);
    cout << response.stat().value() <<endl;
    return 0;
}
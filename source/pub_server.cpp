#include "../external/spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>
#include "../framework/helpers.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
  auto ctx = zmq::context_t(1);
  auto skt = zmq::socket_t(ctx, ZMQ_PUB);
  skt.bind("tcp://127.0.0.1:9000");

  while (true) {
    std::string data = "Hello World";
    zmq::message_t reply(data.size());
    memcpy((unsigned char*) reply.data(), (const unsigned char*) data.c_str(), data.size());
    skt.send(reply);
  }
  return 0;
}

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
  auto skt = zmq::socket_t(ctx, ZMQ_SUB);
  skt.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  skt.connect("tcp://127.0.0.1:2100");

  std::cout << "Start receiving data" << std::endl;
  while (true) {
    zmq::message_t msg;
    skt.recv(&msg);
    auto i = std::string(static_cast<char*>(msg.data()),msg.size());
    std::cout << "New Data" << std::endl;
    std::cout << i << std::endl;
  }
  return 0;
}

#include <iostream>
#include "../external/spdlog/include/spdlog/spdlog.h"
#include "../framework/RGBDRIServer.hpp"

/*
Logger setup
*/
void logger_init(){
  auto sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
  auto logger = std::make_shared<spdlog::logger>("console",sink);
  spdlog::register_logger(logger);
  logger->set_level(spdlog::level::debug);
  spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
}


void server_init(){
  auto context = std::make_shared<zmq::context_t>(4);
  auto server = RGBDRIServer(context);
}

int main(int argc, char const *argv[]) {
  logger_init();
  spdlog::get("console")->info("Start Application");

  server_init();



  return 0;
}

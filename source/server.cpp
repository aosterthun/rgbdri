#include <iostream>
#include "../framework/RGBDRI.hpp"




int main(int argc, char const *argv[]) {

  auto sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
  auto logger = std::make_shared<spdlog::logger>("console",sink);
  spdlog::register_logger(logger);
  logger->set_level(spdlog::level::debug);
  spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");

  auto server = RGBDRI();

  return 0;
}

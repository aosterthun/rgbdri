#include <stdexcept>
#include <iostream>
#include <thread>
#include "../spdlog/include/spdlog/spdlog.h"
#include "../framework/zhelpers.hpp"
#include "../framework/helpers.hpp"
#include "../framework/play.hpp"
#include "../framework/record.hpp"

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

int main(int argc, char const *argv[]) {
  int rc = 1;


  logger_init();
  auto m_logger = spdlog::get("console");

  /* Single client test */
  auto rgbdri = RGBDRI("127.0.0.1","127.0.0.1");

  auto play = std::make_shared<Play>(rgbdri,"/opt/kinect-resources/calib_3dvc/latest_kinect_studio.stream","127.0.0.1:7000");

  while (true) {
    std::string cmd_choice;
    std::cin >> cmd_choice;
    if(cmd_choice == "START"){
      m_logger->debug("START");
      play->start();
    }
    if(cmd_choice == "PAUSE"){
      m_logger->debug("PAUSE");
      play->pause();
    }
    if(cmd_choice == "RESUME"){
      m_logger->debug("RESUME");
      play->resume();
    }
    if(cmd_choice == "LOOP"){
      m_logger->debug("LOOP");
      play->loop();
    }
    if(cmd_choice == "STOP"){
      m_logger->debug("STOP");
      play->stop();
    }
    if(cmd_choice == "RESET"){
      m_logger->debug("RESET");
      play->reset();
    }
    if(cmd_choice == "STATUS"){
      m_logger->debug("STATUS: {0:s}",play->status());
    }else{
      m_logger->debug("STATUS: {0:s}",play->status());
    }
  }



  return rc;
}

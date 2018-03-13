#ifndef record_hpp
#define record_hpp

#include "../external/spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>
#include "helpers.hpp"
#include <iostream>

class Record{
public:
  std::string filename;
  int number_rgbd_sensors;
  int duration;
  bool compressed;
  std::string stream_endpoint;
  std::string backchannel_endpoint;

  Record(std::string const& filename,std::string const& stream_endpoint, int number_rgbd_sensors,int duration,bool compressed, std::string const& backchannel_endpoint = "default");
  void start();
  std::string to_string();
  static Record from_string(std::string const& record_string);

private:
  std::shared_ptr<zmq::context_t> ctx;
  std::shared_ptr<zmq::socket_t> skt;
  bool req_inited;
  bool is_running;
  bool is_paused;
  std::shared_ptr<spdlog::logger> m_logger;

  void init_req();
  void init_rep();

};

#endif

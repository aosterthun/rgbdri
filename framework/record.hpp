#ifndef record_hpp
#define record_hpp

#include "../spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <FileBuffer.hpp>
#include <ChronoMeter.hpp>
#include <string>
#include <vector>
#include "helpers.hpp"
#include <iostream>
#include "RGBDRIClient.hpp"

#define MAX_FRAMES_TO_RECORD 1800

class Record{
public:
  std::string filename;
  int number_rgbd_sensors;
  int duration;
  bool compressed;
  std::string stream_endpoint;
  std::string backchannel_endpoint;

  Record(RGBDRIClient &client, std::string const& filename,std::string const& stream_endpoint, std::string const& backchannel_endpoint = "default");
  Record(std::string const& filename,std::string const& stream_endpoint, std::string const& backchannel_endpoint = "default");
  Record(std::string const& filename,std::string const& stream_endpoint, int number_rgbd_sensors,int duration,bool compressed, std::string const& backchannel_endpoint = "default");
  void start();
  void execute();
  void stop();
  std::string to_string();
  static Record from_string(std::string const& record_string);
  static void from_string(Record &record,std::string const& record_string);

  bool is_running;
  bool is_paused;
private:
  std::shared_ptr<zmq::context_t> ctx;
  std::shared_ptr<zmq::socket_t> skt;
  bool req_inited;
  RGBDRIClient client;

  std::shared_ptr<spdlog::logger> m_logger;
  // RGBDRIClient client;

  void init_req();
  void init_rep();


};

#endif

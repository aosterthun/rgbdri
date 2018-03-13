#ifndef play_hpp
#define play_hpp

#include "../external/spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>
#include "helpers.hpp"
#include <iostream>

class Play{
public:
  std::string filename;
  bool loop;
  int number_rgbd_sensors;
  int max_fps;
  bool compressed;
  int start_frame;
  int end_frame;
  std::string stream_endpoint;
  std::string backchannel_endpoint;

  Play(std::string const& filename,bool loop,int number_rgbd_sensors,int max_fps,bool compressed,int start_frame,int end_frame,std::string const& stream_endpoint = "self");
  Play(std::string const& filename,bool loop,int number_rgbd_sensors,int max_fps,bool compressed,int start_frame,int end_frame,std::string const& backchannel_endpoint,std::string const& stream_endpoint);
  void execute(); // TODO: Rename to start()
  void stop();
  void pause();
  void resume();
  std::string to_string();
  static Play from_string(std::string const& play_string);

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

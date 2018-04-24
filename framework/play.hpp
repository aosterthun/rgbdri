#ifndef play_hpp
#define play_hpp

#include "../spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <FileBuffer.hpp>
#include <ChronoMeter.hpp>
#include <stdio.h>
#include <string>
#include <vector>
#include "helpers.hpp"
#include <iostream>
#include "RGBDRIClient.hpp"

/*
  TODO:
    - Play holds an instance of RGBDRIClient to execute itself
      - Play.start(){
          rgbdri_clinet.execute(this);
        }
    - Rework constructor (Record)
    - is_prepared flag to check on play
*/

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

  Play(RGBDRIClient &client, std::string const& filename, std::string const& stream_endpoint);
  Play(std::string const& filename, std::string const& stream_endpoint);
  Play(std::string const& filename,bool loop,int number_rgbd_sensors,int max_fps,bool compressed,int start_frame,int end_frame,std::string const& stream_endpoint = "self");
  Play(std::string const& filename,bool loop,int number_rgbd_sensors,int max_fps,bool compressed,int start_frame,int end_frame,std::string const& backchannel_endpoint,std::string const& stream_endpoint);
  ~Play();
  bool is_playing();
  void execute(); //server side execute
  void stop();
  void pause();
  void play_as_loop();
  void resume();
  void start(); //client side request server.execute
  std::string to_string();
  static Play from_string(std::string const& play_string);
  static void from_string(Play &play,std::string const& play_string);

  bool is_running;
  bool is_paused;

private:
  std::shared_ptr<zmq::context_t> ctx;
  std::shared_ptr<zmq::socket_t> skt;
  bool req_inited;

  std::shared_ptr<spdlog::logger> m_logger;
  RGBDRIClient client;
  bool rep_thread_running;

  void init_req();
  void init_rep();

};

#endif

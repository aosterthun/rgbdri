#ifndef __PLAY_HPP_INCLUDED__
#define __PLAY_HPP_INCLUDED__

#include <thread>
#include "zmq.hpp"
#include "helpers.hpp"
#include "RGBDRI.hpp"
#include "ChronoMeter.hpp"
#include "FileBuffer.hpp"

class RGBDRI;

class Play: public std::enable_shared_from_this<Play>{
private:
  //debugging
  std::shared_ptr<spdlog::logger> m_console_logger;

  //data
  std::string m_filename;
  std::string m_stream_endpoint;
  std::string m_status;
  std::string m_last_status;

  //networking
  std::shared_ptr<zmq::context_t> m_context;
  std::shared_ptr<zmq::socket_t> m_req_socket;
  std::string m_backchannel_endpoint;

  //threads
  std::shared_ptr<std::thread> m_remote_control_thread;
  std::shared_ptr<std::thread> m_stream_thread;

public:
  Play(RGBDRI& rgbdri, std::string const& filename, std::string const& stream_endpoint);
  Play(std::string const& filename, std::string const& stream_endpoint, std::string const& backchannel_endpoint);
  std::string status();
  void start();
  void pause();
  void resume();
  void loop();
  void stop();
  void reset();
  void terminate();
  void stream_thread();
  void enable_remote_control();
  void remote_control_thread();
  std::string to_string();
  std::string& backchannel_endpoint();
  void backchannel_endpoint(std::string const& backchannel_endpoint);
  static void from_string(Play& play, std::string const& command_string);
  static std::shared_ptr<Play> from_string(std::string const& command_string);
  ~Play();
};

#endif

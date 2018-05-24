#ifndef __RECORD_HPP_INCLUDED__
#define __RECORD_HPP_INCLUDED__


#include <thread>
#include "../spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include "helpers.hpp"
#include "RGBDRI.hpp"
#include "ChronoMeter.hpp"
#include "FileBuffer.hpp"

#define MAX_FRAMES_TO_RECORD 1800

class RGBDRI;

class Record{
private:
  //debugging
  std::shared_ptr<spdlog::logger> m_console_logger;

  //data
  std::string m_filename;
  std::string m_stream_endpoint;
  std::string m_status;

  //networking
  std::shared_ptr<zmq::context_t> m_context;
  std::shared_ptr<zmq::socket_t> m_req_socket;
  std::string m_backchannel_endpoint;

  //threads
  std::shared_ptr<std::thread> m_remote_control_thread;
  std::shared_ptr<std::thread> m_record_thread;

public:
  Record(RGBDRI& rgbdri, std::string const& filename, std::string const& stream_endpoint);
  Record(std::string const& filename, std::string const& stream_endpoint, std::string const& backchannel_endpoint);
  std::string status();
  void start();
  void stop();
  void terminate();
  void record_thread();
  void enable_remote_control();
  void remote_control_thread();
  std::string to_string();
  std::string& backchannel_endpoint();
  void backchannel_endpoint(std::string const& backchannel_endpoint);
  static void from_string(Record& record, std::string const& command_string);
  static std::shared_ptr<Record> from_string(std::string const& command_string);
  ~Record();
};

#endif

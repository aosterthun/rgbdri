#ifndef RGBDRIClient_hpp
#define RGBDRIClient_hpp

#include "../spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include "helpers.hpp"
//#include "play.hpp"
//#include "record.hpp"
#include "GenericMessage.hpp"
#include <memory>
#include <iostream>

class RGBDRIClient{
private:
  std::shared_ptr<zmq::context_t> m_ctx;
  std::shared_ptr<zmq::socket_t> m_skt;
  std::shared_ptr<spdlog::logger> m_logger;
public:
  RGBDRIClient();
  RGBDRIClient(std::string const& stream_endpoint, std::string const& client_endpoint);
  //void execute(Play &play);
  //void execute(Record &record);
  void send(std::string const& data);
  std::vector<std::string> recv();
};

#endif

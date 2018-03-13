#ifndef RGBDRIClient_hpp
#define RGBDRIClient_hpp

#include "../external/spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include "helpers.hpp"
#include "play.hpp"
#include "record.hpp"
#include "GenericMessage.hpp"
#include <memory>
#include <iostream>

class RGBDRIClient{
private:
  std::shared_ptr<zmq::context_t> m_ctx;
  std::shared_ptr<zmq::socket_t> m_skt;
  std::shared_ptr<spdlog::logger> m_logger;
public:
  RGBDRIClient(std::shared_ptr<zmq::context_t> _ctx, char* id);
  void execute(Play &play);
  void execute(Record &record);
};

#endif

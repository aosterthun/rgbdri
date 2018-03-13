#ifndef RGBDRIServer_hpp
#define RGBDRIServer_hpp

#include "../external/spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <vector>
#include "play.hpp"
#include "record.hpp"
#include "GenericMessage.hpp"
#include "helpers.hpp"
#include "RGBDRIPortManager.hpp"
#include <iostream>

class RGBDRIServer{
private:
  std::shared_ptr<zmq::context_t> m_ctx;
  std::shared_ptr<zmq::socket_t> m_skt;
  std::shared_ptr<spdlog::logger> m_logger;
public:
  RGBDRIServer(std::shared_ptr<zmq::context_t> _ctx);
  void worker();

};


#endif

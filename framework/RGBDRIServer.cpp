#include "RGBDRIServer.hpp"

RGBDRIServer::RGBDRIServer(std::shared_ptr<zmq::context_t> _ctx)
:m_ctx{_ctx},
m_logger{spdlog::get("console")}{
  m_logger->info("Init RGBDRIServer");

  try
  {
    zmq::socket_t router(*m_ctx.get(), ZMQ_ROUTER);
    router.setsockopt(ZMQ_IDENTITY, "ROUTER", 7);
    router.bind("tcp://*:8000");

    while (true) {
      auto request = srecv(router);
      m_logger->debug(request.front());
      auto client_ip = split(request.front(),'#')[1];

      //Do some work (The server is not parallel so the work should be fast to compute)
      auto msg = GenericMessage::from_string(request.back());
      auto reply_msg = GenericMessage();
      reply_msg.type = 15;

      switch (msg.type) {
        case 0:{
          auto cmd = Play::from_string(msg.payload);
          if(cmd.stream_endpoint == "self"){
              auto next_port = ZMQPortManager::get_instance().get_next_free_port();
              cmd.stream_endpoint = client_ip+":"+std::to_string(next_port);
              cmd.backchannel_endpoint = client_ip+":"+std::to_string(next_port+1);
          }

          reply_msg.payload = cmd.to_string();
          auto cmd_thread = std::thread(&Play::execute, cmd);
          cmd_thread.detach();
          break;
        }
        case 1:{
          auto cmd = Record::from_string(msg.payload);
          auto next_port = ZMQPortManager::get_instance().get_next_free_port();
          cmd.backchannel_endpoint = client_ip+":"+std::to_string(next_port+1);
          m_logger->debug(cmd.backchannel_endpoint);
          reply_msg.payload = cmd.to_string();
          auto cmd_thread = std::thread(&Record::start, cmd);
          cmd_thread.detach();
          break;
        }
      }
      rt_send(router, request.front().c_str(), reply_msg.to_string().c_str());
    }
  }
  catch (zmq::error_t error)
  {
  std::string errorStr = error.what();
  }

}

void RGBDRIServer::worker(){

}

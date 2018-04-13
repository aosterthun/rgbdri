#include "RGBDRIClient.hpp"

RGBDRIClient::RGBDRIClient()
:m_ctx{},
m_skt{},
m_logger{}{


  m_logger = spdlog::get("console");

  m_logger->info("Init RGBDRIClient without data");
};

/*
  TODO:
    - Check if id is nessecary anymore. If so, make it static in client

*/
RGBDRIClient::RGBDRIClient(std::string const& server_endpoint, std::string const& client_endpoint)
:m_ctx{},
m_skt{},
m_logger{}{
  auto sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
  auto logger = std::make_shared<spdlog::logger>("console",sink);
  spdlog::register_logger(logger);
  logger->set_level(spdlog::level::debug);
  spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");

  m_logger = spdlog::get("console");

  m_logger->info("Init RGBDRIClient");
  m_ctx = std::make_shared<zmq::context_t>(4);

  std::string cid = "[1]#"+client_endpoint;
  try
  {
    m_skt = std::make_shared<zmq::socket_t>(*m_ctx.get(), ZMQ_DEALER);
    m_skt->setsockopt(ZMQ_IDENTITY, cid.c_str(), strlen(cid.c_str()));
    m_skt->connect("tcp://"+server_endpoint+":8000");
  }catch (zmq::error_t error)
  {
     //TODO: handle this error
  }
};

void RGBDRIClient::send(std::string const& data){
  dl_send(*m_skt.get(), data);
};

std::vector<std::string> RGBDRIClient::recv(){
  return srecv(*m_skt.get());
};

//
// void RGBDRIClient::execute(Play& play){
//   //Wrap command as GenericMessage
//   auto msg = GenericMessage();
//   msg.type = 0; //TODO: think about type handling
//   msg.payload = play.to_string();
//   m_logger->debug(msg.payload);
//   //Send command to server
//   dl_send(*m_skt.get(), msg.to_string());
//   m_logger->debug("Send request");
//
//   auto reply = srecv(*m_skt.get());
//   m_logger->debug("Received reply");
//   auto reply_msg = GenericMessage::from_string(reply.back());
//   switch (reply_msg.type) {
//     case 15:{
//       m_logger->debug(reply_msg.payload);
//       auto cmd = Play::from_string(reply_msg.payload);
//       play.stream_endpoint = cmd.stream_endpoint;
//       play.backchannel_endpoint = cmd.backchannel_endpoint;
//       m_logger->debug("Play on endpoint: {0:s}", cmd.stream_endpoint);
//       break;
//     }
//     case 1:{
//       m_logger->warn("Received non valid command");
//       break;
//     }
//   }
// };
//
// void RGBDRIClient::execute(Record& record){
//   //Wrap command as GenericMessage
//   auto msg = GenericMessage();
//   msg.type = 1; //TODO: think about type handling
//   msg.payload = record.to_string();
//   m_logger->debug(msg.payload);
//   //Send command to server
//   dl_send(*m_skt.get(), msg.to_string());
//   m_logger->debug("Send request");
//
//   auto reply = srecv(*m_skt.get());
//   m_logger->debug("Received reply");
//   auto reply_msg = GenericMessage::from_string(reply.back());
//   switch (reply_msg.type) {
//     case 15:{
//       m_logger->debug(reply_msg.payload);
//       auto cmd = Record::from_string(reply_msg.payload);
//       record.stream_endpoint = cmd.stream_endpoint;
//       record.backchannel_endpoint = cmd.backchannel_endpoint;
//       m_logger->debug("Record endpoint: {0:s}", cmd.stream_endpoint);
//       m_logger->debug("Backchannel endpoint: {0:s}", cmd.backchannel_endpoint);
//       break;
//     }
//     case 1:{
//       m_logger->warn("Received non valid command");
//       break;
//     }
//   }
// };

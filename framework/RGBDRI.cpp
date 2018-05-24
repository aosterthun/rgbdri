#include "RGBDRI.hpp"

RGBDRI::RGBDRI():
m_context{std::make_shared<zmq::context_t>(4)},
m_socket{std::make_shared<zmq::socket_t>(*m_context.get(),ZMQ_ROUTER)}
{
  m_socket->setsockopt(ZMQ_IDENTITY, "ROUTER", 7);
  m_socket->bind("tcp://*:8000");
  auto commands = std::vector<std::shared_ptr<Play>>();
  auto rec_commands = std::vector<std::shared_ptr<Record>>();
  while (true) {
    auto request = srecv(*m_socket.get());
    std::cout << "New Request" << std::endl;
    auto client_ip = split(request.front(),'#')[1];

    //Do some work (The server is not parallel so the work should be fast to compute)
    auto msg = GenericMessage::from_string(request.back());
    auto reply_msg = GenericMessage();
    reply_msg.type = 15;
    std::cout << std::to_string(msg.type) << std::endl;
    std::cout << msg.meta_data << std::endl;
    std::cout << msg.payload << std::endl;
    switch (msg.type) {
      case 0:{
        auto cmd = Play::from_string(msg.payload);
        commands.push_back(cmd);
        auto next_port = ZMQPortManager::get_instance().get_next_free_port();
        cmd->backchannel_endpoint(msg.meta_data+":"+std::to_string(next_port+1));
        cmd->enable_remote_control();
        reply_msg.payload = cmd->to_string();
        break;
      }
      case 1:{
        auto cmd = Record::from_string(msg.payload);
        rec_commands.push_back(cmd);
        auto next_port = ZMQPortManager::get_instance().get_next_free_port();
        cmd->backchannel_endpoint(msg.meta_data+":"+std::to_string(next_port+1));
        cmd->enable_remote_control();
        reply_msg.payload = cmd->to_string();
        break;
      }
    }
    rt_send(*m_socket.get(), request.front().c_str(), reply_msg.to_string().c_str());
    std::cout << request.front() << std::endl;
  }
};
RGBDRI::RGBDRI(std::string const& server_endpoint, std::string const& client_endpoint):
m_context{std::make_shared<zmq::context_t>(4)}, m_socket{std::make_shared<zmq::socket_t>(*m_context.get(),ZMQ_DEALER)},
m_server_endpoint{server_endpoint}
{
  std::string cid = "[1]#"+client_endpoint;

  m_socket->setsockopt(ZMQ_IDENTITY, cid.c_str(), strlen(cid.c_str()));
  m_socket->connect("tcp://"+server_endpoint+":8000");
  std::cout << "connected to rgbdri" << std::endl;
};

const std::string& RGBDRI::server_endpoint(){
  return m_server_endpoint;
};

void RGBDRI::init_play(std::shared_ptr<Play> play){
  auto msg = GenericMessage();
  msg.type = 0;
  msg.payload = play->to_string();

  send(msg.to_string());

  auto reply = recv();

  auto reply_msg = GenericMessage::from_string(reply.back());
  switch (reply_msg.type) {
    case 15:{
      Play::from_string(*play.get(), reply_msg.payload);
      break;
    }
    case 1:{

      break;
    }
  }
}
void RGBDRI::send(std::string const& data){
  dl_send(*m_socket.get(), data);
};

std::vector<std::string> RGBDRI::recv(){
  return srecv(*m_socket.get());
};

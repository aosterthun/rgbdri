#include "record.hpp"

Record::Record(std::string const& filename,std::string const& stream_endpoint, int number_rgbd_sensors,int duration,bool compressed,std::string const& backchannel_endpoint):
            filename{filename},
            stream_endpoint{stream_endpoint},
            number_rgbd_sensors{number_rgbd_sensors},
            duration{duration},
            compressed{compressed},
            ctx{},
            skt{},
            req_inited{false},
            m_logger{spdlog::get("console")},
            is_running{false},
            backchannel_endpoint{backchannel_endpoint}{
              ctx = std::make_shared<zmq::context_t>(4);
};

void Record::init_req(){
  if(!req_inited){
    skt = std::make_shared<zmq::socket_t>(*ctx.get(), ZMQ_REQ);
    req_inited = true;
  }
}

void Record::init_rep(){
  m_logger->debug("[START] void Record::init_rep()");
  skt = std::make_shared<zmq::socket_t>(*ctx.get(), ZMQ_REP);

  skt->bind("tcp://" + backchannel_endpoint);

  while(is_running){
    zmq::message_t message;
    auto request = srecv(*skt.get());

    if(request[0] == "STOP"){
      m_logger->debug("STOP");
      ssend(*skt.get(), "STOPED");
      is_running = false;
    }
  }

  skt->unbind("tcp://" + backchannel_endpoint);
  m_logger->debug("[END] void Record::init_rep()");
}

void Record::start(){
  m_logger->debug("[START] void Record::execute()");
  is_running = true;
  auto rep_thread = std::thread(&Record::init_rep,this);
  rep_thread.detach();

  auto _ctx = zmq::context_t(1);
  auto _skt = zmq::socket_t(_ctx, ZMQ_SUB);
  _skt.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  m_logger->debug(stream_endpoint);
  _skt.connect("tcp://"+stream_endpoint);
  m_logger->debug(stream_endpoint);

  while(is_running) {
    auto data = srecv(_skt);
    for (auto i : data)
      m_logger->info(i);
  }
  m_logger->debug("[END] void Record::execute()");
};

std::string Record::to_string(){
  std::string output;

  output += filename + "_";
  output += stream_endpoint + "_";
  output += std::to_string(number_rgbd_sensors) + "_";
  output += std::to_string(duration) + "_";
  output += std::to_string(compressed) + "_";
  output += backchannel_endpoint + "_";

  return output;
}

Record Record::from_string(std::string const& play_string){
  auto parts = split(play_string, '_');
  auto cmd = Record(
    parts[0],
    parts[1],
    std::stoi(parts[2]),
    std::stoi(parts[3]),
    to_bool(parts[4]),
    parts[5]
  );
  return cmd;
}

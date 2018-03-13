#include "play.hpp"

Play::Play(std::string const& filename,
            bool loop,
            int number_rgbd_sensors,
            int max_fps,
            bool compressed,
            int start_frame,
            int end_frame,
            std::string const& stream_endpoint):
            filename{filename},
            loop{loop},
            number_rgbd_sensors{number_rgbd_sensors},
            max_fps{max_fps},
            compressed{compressed},
            start_frame{start_frame},
            end_frame{end_frame},
            stream_endpoint{stream_endpoint},
            ctx{},
            skt{},
            req_inited{false},
            m_logger{spdlog::get("console")},
            is_running{false},
            backchannel_endpoint{}{
              ctx = std::make_shared<zmq::context_t>(4);
              m_logger->debug(stream_endpoint);
              if(stream_endpoint != "self"){
                auto endpoint = split(stream_endpoint,':');
                m_logger->debug(endpoint[1]);
                auto port = std::stoi(endpoint[1]);
                auto new_port = port + 1;
                backchannel_endpoint = endpoint[0] + ":" + std::to_string(new_port);
                m_logger->debug(backchannel_endpoint);
              }

};

Play::Play(std::string const& filename,
            bool loop,
            int number_rgbd_sensors,
            int max_fps,
            bool compressed,
            int start_frame,
            int end_frame,
            std::string const& backchannel_endpoint,
            std::string const& stream_endpoint):
            filename{filename},
            loop{loop},
            number_rgbd_sensors{number_rgbd_sensors},
            max_fps{max_fps},
            compressed{compressed},
            start_frame{start_frame},
            end_frame{end_frame},
            stream_endpoint{stream_endpoint},
            ctx{},
            skt{},
            req_inited{false},
            m_logger{spdlog::get("console")},
            is_running{false},
            backchannel_endpoint{backchannel_endpoint}{
              ctx = std::make_shared<zmq::context_t>(4);
};

void Play::init_req(){
  if(!req_inited){
    skt = std::make_shared<zmq::socket_t>(*ctx.get(), ZMQ_REQ);
    req_inited = true;
  }
}

void Play::init_rep(){
  m_logger->debug("[START] void Play::init_rep()");
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
    if(request[0] == "PAUSE"){
      m_logger->debug("PAUSE");
      ssend(*skt.get(), "PAUSED");
      is_paused = true;
    }
    if(request[0] == "RESUME"){
      m_logger->debug("RESUME");
      ssend(*skt.get(), "RESUMED");
      is_paused = false;
    }


  }

  skt->unbind("tcp://" + backchannel_endpoint);
  m_logger->debug("[END] void Play::init_rep()");
}

void Play::execute(){
  m_logger->debug("[START] void Play::execute()");
  is_running = true;
  auto rep_thread = std::thread(&Play::init_rep,this);
  rep_thread.detach();

  auto _ctx = zmq::context_t(1);
  auto _skt = zmq::socket_t(_ctx, ZMQ_PUB);
  _skt.bind("tcp://"+stream_endpoint);

  int i = 0;
  while(is_running) {
    if(!is_paused) {
      while(i < 30){
        if(is_paused or !is_running){
          break;
        }
        ssend(_skt,"working " + std::to_string(i));
        m_logger->info("working {0:d}",i);
        sleep(1);

        i++;
      }
    }
  }

  // for (size_t i = 0; i < 1000; i++) {
  //   if(!is_running){
  //     break;
  //   }
  //   m_logger->info("working {0:d}",i);
  //   sleep(1);
  // }
  m_logger->debug("[END] void Play::execute()");
};


void Play::stop(){
  init_req();
  m_logger->debug("[START] void Play::stop()");
  skt->connect("tcp://" + backchannel_endpoint);

  ssend(*skt.get(),"STOP");
  srecv(*skt.get());

  skt->disconnect("tcp://" + backchannel_endpoint);
  m_logger->debug("[END] void Play::stop()");
};

void Play::pause(){
  init_req();
  m_logger->debug("[START] void Play::pause()");
  skt->connect("tcp://" + backchannel_endpoint);


  ssend(*skt.get(),"PAUSE");
  srecv(*skt.get());

  skt->disconnect("tcp://" + backchannel_endpoint);
  m_logger->debug("[END] void Play::pause()");
};

void Play::resume(){
  init_req();
  m_logger->debug("[START] void Play::resume()");
  skt->connect("tcp://" + backchannel_endpoint);

  ssend(*skt.get(),"RESUME");
  srecv(*skt.get());

  skt->disconnect("tcp://" + backchannel_endpoint);
  m_logger->debug("[END] void Play::resume()");
};

std::string Play::to_string(){
  std::string output;

  output += filename + "_";
  output += std::to_string(loop) + "_";
  output += std::to_string(number_rgbd_sensors) + "_";
  output += std::to_string(max_fps) + "_";
  output += std::to_string(compressed) + "_";
  output += std::to_string(start_frame) + "_";
  output += std::to_string(end_frame) + "_";
  output += backchannel_endpoint + "_";
  output += stream_endpoint + "_";

  return output;
}

Play Play::from_string(std::string const& play_string){
  auto parts = split(play_string, '_');
  auto cmd = Play(
    parts[0],
    to_bool(parts[1]),
    std::stoi(parts[2]),
    std::stoi(parts[3]),
    to_bool(parts[4]),
    std::stoi(parts[5]),
    std::stoi(parts[6]),
    parts[7],
    parts[8]
  );
  return cmd;
}

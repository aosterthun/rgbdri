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

  FileBuffer* fb = nullptr;
  ChronoMeter cm;
  size_t framecounter = 0;
  const unsigned colorsize = compressed ? 691200 : 1280 * 1080 * 3;
  const unsigned depthsize = 512 * 424 * sizeof(float);
  const unsigned framesize = (colorsize + depthsize) * number_rgbd_sensors;

  if(fb != nullptr){
    fb->close();
    delete fb;
    fb = nullptr;
    std::cout << "closed" << std::endl;
  }

  fb = new FileBuffer(filename.c_str());
  if(!fb->open("w", 0)){
    std::cerr << "error opening " << filename << " exiting..." << std::endl;
    exit(0);
  }
  std::cout << "opened" << std::endl;
  framecounter = 0;

  zmq::socket_t  socket(*ctx.get(), ZMQ_SUB); // means a subscriber
  socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  uint32_t hwm = 1;
  socket.setsockopt(ZMQ_RCVHWM,&hwm, sizeof(hwm));
  std::string endpoint("tcp://" + stream_endpoint);
  socket.connect(endpoint.c_str());

  while(is_running) {
    zmq::message_t zmqm(framesize);
    socket.recv(&zmqm); // blocking
    const double currtime = cm.getTick();
    memcpy((char*) zmqm.data(), (const char*) &currtime, sizeof(double));
    if(fb == nullptr){
      m_logger->critical("ERROR: not recording, file not open!");
    }
    else{
      //std::cout << "recording frame " << ++framecounter << " at time " << currtime << std::endl;
      fb->write((unsigned char*) zmqm.data(), framesize);
      if(framecounter > MAX_FRAMES_TO_RECORD){
        if(fb != nullptr){
          fb->close();
          delete fb;
          fb = nullptr;
          std::cout << "closed" << std::endl;
        }
        m_logger->critical("ALTERT: closed recording since too much frames were already recorded!: {0:d} ",framecounter);
      }
    }
  }
  m_logger->debug("[END] void Record::execute()");
};

void Record::stop(){
  init_req();
  m_logger->debug("[START] void Record::stop()");
  skt->connect("tcp://" + backchannel_endpoint);

  ssend(*skt.get(),"STOP");
  srecv(*skt.get());

  skt->disconnect("tcp://" + backchannel_endpoint);
  m_logger->debug("[END] void Record::stop()");
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

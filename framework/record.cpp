#include "record.hpp"

// Record::Record(RGBDRIClient client):
// client{client}{
//
//
// };


Record::Record(RGBDRIClient &client, std::string const& filename, std::string const& stream_endpoint, std::string const& backchannel_endpoint):
            client{client},
            filename{filename},
            stream_endpoint{stream_endpoint},
            number_rgbd_sensors{4},
            compressed{true},
            ctx{},
            skt{},
            req_inited{false},
            m_logger{spdlog::get("console")},
            is_running{false},
            backchannel_endpoint{backchannel_endpoint}{
              ctx = std::make_shared<zmq::context_t>(4);
};

Record::Record(std::string const& filename, std::string const& stream_endpoint, std::string const& backchannel_endpoint):
            client{},
            filename{filename},
            stream_endpoint{stream_endpoint},
            number_rgbd_sensors{4},
            compressed{true},
            ctx{},
            skt{},
            req_inited{false},
            m_logger{spdlog::get("console")},
            is_running{false},
            backchannel_endpoint{backchannel_endpoint}{
              ctx = std::make_shared<zmq::context_t>(4);
};

Record::Record(std::string const& filename,std::string const& stream_endpoint, int number_rgbd_sensors,int duration,bool compressed,std::string const& backchannel_endpoint):
            client{},
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
  skt = std::make_shared<zmq::socket_t>(*ctx.get(), ZMQ_REP);

  skt->bind("tcp://" + backchannel_endpoint);

  while(is_running){
    zmq::message_t message;
    auto request = srecv(*skt.get());

    if(request[0] == "STOP"){
      ssend(*skt.get(), "STOPED");
      is_running = false;
    }
  }

  skt->unbind("tcp://" + backchannel_endpoint);
}

void Record::start(){
  //Wrap command as GenericMessage
  auto msg = GenericMessage();
  msg.type = 1; //TODO: think about type handling
  msg.payload = to_string();
  //Send command to server
  client.send(msg.to_string());

  auto reply = client.recv();
  auto reply_msg = GenericMessage::from_string(reply.back());
  switch (reply_msg.type) {
    case 15:{
      Record::from_string(*this,reply_msg.payload);
      break;
    }
    case 1:{
      m_logger->warn("Received non valid command");
      break;
    }
  }
}

void Record::execute(){
  m_logger->debug("==============[START]==============");
  m_logger->debug("void Record::execute()");
  m_logger->debug("Stream endpoint: {0:s}", stream_endpoint);
  m_logger->debug("Backchannel endpoint: {0:s}", backchannel_endpoint);
  m_logger->debug("===================================");
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
  }

  fb = new FileBuffer(filename.c_str());
  if(!fb->open("w", 0)){
    std::cerr << "error opening " << filename << " exiting..." << std::endl;
    exit(0);
  }
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
  m_logger->debug("===============[END]===============");
  m_logger->debug("void Record::execute()");
  m_logger->debug("Stream endpoint: {0:s}", stream_endpoint);
  m_logger->debug("Backchannel endpoint: {0:s}", backchannel_endpoint);
  m_logger->debug("===================================");
};

void Record::stop(){
  init_req();
  skt->connect("tcp://" + backchannel_endpoint);

  ssend(*skt.get(),"STOP");
  srecv(*skt.get());

  skt->disconnect("tcp://" + backchannel_endpoint);
};

std::string Record::to_string(){
  std::string output;

  output += filename + "$";
  output += stream_endpoint + "$";
  output += std::to_string(number_rgbd_sensors) + "$";
  output += std::to_string(duration) + "$";
  output += std::to_string(compressed) + "$";
  output += backchannel_endpoint + "$";

  return output;
};

Record Record::from_string(std::string const& play_string){
  auto parts = split(play_string, '$');
  auto cmd = Record(
    parts[0],
    parts[1],
    std::stoi(parts[2]),
    std::stoi(parts[3]),
    to_bool(parts[4]),
    parts[5]
  );
  return cmd;
};

void Record::from_string(Record &record, std::string const& play_string){
  auto parts = split(play_string, '$');
  record.filename = parts[0];
  record.stream_endpoint = parts[1];
  record.number_rgbd_sensors = std::stoi(parts[2]);
  record.duration = std::stoi(parts[3]);
  record.compressed = to_bool(parts[4]);
  record.backchannel_endpoint = parts[5];
};

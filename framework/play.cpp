#include "play.hpp"

Play::Play(std::string const& filename, std::string const& stream_endpoint, std::string const& backchannel_endpoint):
m_console_logger{spdlog::get("console")},
m_filename{filename},m_stream_endpoint{stream_endpoint},m_status{"UNDEFINED"},m_last_status{"UNDEFINED"},
m_context{std::make_shared<zmq::context_t>(4)},m_req_socket{std::make_shared<zmq::socket_t>(*m_context.get(),ZMQ_REQ)},m_backchannel_endpoint{backchannel_endpoint}
{
  m_stream_thread = std::make_shared<std::thread>(&Play::stream_thread, this);
  m_stream_thread->detach();
};

Play::Play(RGBDRI& rgbdri, std::string const& filename, std::string const& stream_endpoint):
m_console_logger{spdlog::get("console")},
m_filename{filename},m_stream_endpoint{stream_endpoint},m_status{"UNDEFINED"},m_last_status{"UNDEFINED"},
m_context{std::make_shared<zmq::context_t>(4)},m_req_socket{std::make_shared<zmq::socket_t>(*m_context.get(),ZMQ_REQ)},m_backchannel_endpoint{"UNDEFINED"}
{
  auto msg = GenericMessage();
  msg.type = 0;
  msg.meta_data = rgbdri.server_endpoint();
  msg.payload = to_string();

  rgbdri.send(msg.to_string());

  auto reply = rgbdri.recv();

  auto reply_msg = GenericMessage::from_string(reply.back());
  switch (reply_msg.type) {
    case 15:{
      Play::from_string(*this, reply_msg.payload);
      break;
    }
    case 1:{

      break;
    }
  }
  m_req_socket->connect("tcp://" + m_backchannel_endpoint);
};

std::string Play::status(){
  ssend(*m_req_socket.get(), "STATUS");
  m_status = srecv(*m_req_socket.get()).front();
  return m_status;
};
void Play::start(){
  ssend(*m_req_socket.get(), "START");
  m_status = srecv(*m_req_socket.get()).front();
};
void Play::pause(){
  ssend(*m_req_socket.get(), "PAUSE");
  m_status = srecv(*m_req_socket.get()).front();
};
void Play::resume(){
  ssend(*m_req_socket.get(), "RESUME");
  m_status = srecv(*m_req_socket.get()).front();
};
void Play::loop(){
  ssend(*m_req_socket.get(), "LOOP");
  m_status = srecv(*m_req_socket.get()).front();
};
void Play::stop(){
  ssend(*m_req_socket.get(), "STOP");
  m_status = srecv(*m_req_socket.get()).front();
};
void Play::terminate(){
  ssend(*m_req_socket.get(), "TERMINATE");
  m_status = srecv(*m_req_socket.get()).front();
};
void Play::reset(){
  ssend(*m_req_socket.get(), "RESET");
  m_status = srecv(*m_req_socket.get()).front();
}
void Play::enable_remote_control(){
  m_remote_control_thread = std::make_shared<std::thread>(&Play::remote_control_thread, this);
  m_remote_control_thread->detach();
};
void Play::remote_control_thread(){
  auto context = zmq::context_t(4);
  auto rep_socket = zmq::socket_t(context,ZMQ_REP);
  rep_socket.bind("tcp://" + m_backchannel_endpoint);

  while (m_status != "TERMINATE") {
    std::string request = srecv(rep_socket).front();
    if(request == "RESET"){
      m_status = "UNDEFINED";
    }
    if(request == "START"){
      if(m_status == "STOPED"){
        m_status = "PLAYING";
        m_last_status = m_status;
      }
      if(m_status == "PAUSED"){
        m_status = m_last_status;
      }
      if(m_status == "UNDEFINED"){
        m_status = "INIT_STREAM";
        m_last_status = "PLAYING";
      }
    }
    if(request == "PAUSE"){
      if(m_status == "PLAYING" || m_status == "LOOPING"){
        m_status = "PAUSED";
      }
    }
    if(request == "RESUME"){
      if(m_status == "PAUSED"){
        m_status = m_last_status;
      }
    }
    if(request == "LOOP"){
      if(m_status == "UNDEFINED"){
        m_status = "INIT_STREAM";
        m_last_status = "LOOPING";
      }
      if(m_status == "PAUSED"){
        m_status = m_last_status;
      }
      if(m_status == "STOPED"){
        m_status = "LOOPING";
        m_last_status = m_status;
      }
    }
    if(request == "STOP"){
      if(m_status != "STOPED"){
        m_status = "STOPED";
      }
    }
    if(request == "TERMINATE"){
      m_status = "TERMINATE";
    }
    if(request == "STATUS"){
      ssend(rep_socket, m_status);
    }else{
      ssend(rep_socket, m_status);
    }
  }
};
void Play::stream_thread(){
  //networking
  auto context = zmq::context_t(4);
  auto socket = zmq::socket_t(context, ZMQ_PUB);
  uint32_t hwm = 1;
  socket.setsockopt(ZMQ_SNDHWM,&hwm, sizeof(hwm));
  socket.bind("tcp://" + m_stream_endpoint);

  const unsigned colorsize = true ? 691200 : 1280 * 1080 * 3;
  const unsigned depthsize = 512 * 424 * sizeof(float);
  const unsigned framesize = (colorsize + depthsize) * 4;
  double frametime = 0;
  double last_frame_time = 0;
  FileBuffer* fb = nullptr;
  size_t framecounter = 0;
  size_t num_frames = 0;
  //stream loop
  while(m_status != "TERMINATE"){
    if(m_status == "INIT_STREAM"){
      fb = new FileBuffer(m_filename.c_str());
      if(!fb->open("r", 0)){
        std::cout << "ERROR" << '\n';
        exit(0);
      }
      fb->setLooping(true);

      num_frames = fb->getFileSizeBytes()/framesize;

      m_status = m_last_status;
    }else if(m_status == "PLAYING" || m_status == "LOOPING" || m_status == "PAUSED"){
      if(m_status != "PAUSED"){
        zmq::message_t zmqm(framesize);
        fb->read((unsigned char*) zmqm.data(), framesize);
        memcpy((char*) &frametime, (const char*) zmqm.data(), sizeof(double));
        const double elapsed_frame_time = frametime - last_frame_time;
        last_frame_time = frametime;
        const unsigned sleep_time = std::min(100u, std::max(0u, (unsigned)((elapsed_frame_time) * 1000u)));
        if(framecounter > 1){
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
        socket.send(zmqm);
        ++framecounter;

        if(framecounter >= num_frames){
          framecounter = 0;
          if(m_status != "LOOPING"){
            m_status = "STOPED";
          }
        }
      }
    }else if(m_status == "STOPED"){
      if(framecounter != 0){
        fb->rewindFile();
        framecounter = 0;
      }
    }
  }
};
std::string Play::to_string(){
  std::string output;

  output += m_filename + "$";
  output += m_stream_endpoint + "$";
  output += m_backchannel_endpoint + "$";

  return output;
};
std::string& Play::backchannel_endpoint(){
  return m_backchannel_endpoint;
};
void Play::backchannel_endpoint(std::string const& backchannel_endpoint){
  m_backchannel_endpoint = backchannel_endpoint;
};
void Play::from_string(Play& play, std::string const& command_string){
  auto parts = split(command_string, '$');
  play.backchannel_endpoint(parts[2]);
};
std::shared_ptr<Play> Play::from_string(std::string const& command_string){
  auto parts = split(command_string, '$');

  auto play = std::make_shared<Play>(parts[0], parts[1], parts[2]);

  return play;
};
Play::~Play(){
  std::cout << "DESTRUCTOR" << std::endl;
};

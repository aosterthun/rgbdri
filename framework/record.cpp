#include "record.hpp"

Record::Record(std::string const& filename, std::string const& stream_endpoint, std::string const& backchannel_endpoint):
m_console_logger{spdlog::get("console")},
m_filename{filename},m_stream_endpoint{stream_endpoint},m_status{"UNDEFINED"},
m_context{std::make_shared<zmq::context_t>(4)},m_req_socket{std::make_shared<zmq::socket_t>(*m_context.get(),ZMQ_REQ)},m_backchannel_endpoint{backchannel_endpoint}
{
  m_record_thread = std::make_shared<std::thread>(&Record::record_thread, this);
  m_record_thread->detach();
};

Record::Record(RGBDRI& rgbdri, std::string const& filename, std::string const& stream_endpoint):
m_console_logger{spdlog::get("console")},
m_filename{filename},m_stream_endpoint{stream_endpoint},m_status{"UNDEFINED"},
m_context{std::make_shared<zmq::context_t>(4)},m_req_socket{std::make_shared<zmq::socket_t>(*m_context.get(),ZMQ_REQ)},m_backchannel_endpoint{"UNDEFINED"}
{
  auto msg = GenericMessage();
  msg.type = 1;
  msg.meta_data = rgbdri.server_endpoint();
  msg.payload = to_string();

  rgbdri.send(msg.to_string());

  auto reply = rgbdri.recv();

  auto reply_msg = GenericMessage::from_string(reply.back());
  switch (reply_msg.type) {
    case 15:{
      Record::from_string(*this, reply_msg.payload);
      break;
    }
    case 1:{

      break;
    }
  }
  m_req_socket->connect("tcp://" + m_backchannel_endpoint);
};

std::string Record::status(){
  ssend(*m_req_socket.get(), "STATUS");
  m_status = srecv(*m_req_socket.get()).front();
  return m_status;
};
void Record::start(){
  ssend(*m_req_socket.get(), "START");
  m_status = srecv(*m_req_socket.get()).front();
};
void Record::stop(){
  ssend(*m_req_socket.get(), "STOP");
  m_status = srecv(*m_req_socket.get()).front();
};
void Record::terminate(){
  ssend(*m_req_socket.get(), "TERMINATE");
  m_status = srecv(*m_req_socket.get()).front();
};
void Record::enable_remote_control(){
  m_remote_control_thread = std::make_shared<std::thread>(&Record::remote_control_thread, this);
  m_remote_control_thread->detach();
};
void Record::remote_control_thread(){
  auto context = zmq::context_t(4);
  auto rep_socket = zmq::socket_t(context,ZMQ_REP);
  auto parts = split(m_backchannel_endpoint, ':');
  rep_socket.bind("tcp://*:" + parts[1]);
  std::string last_status;
  while (m_status != "TERMINATE") {
    std::string request = srecv(rep_socket).front();
    if(request == "START"){
      if(m_status == "STOPED" || m_status == "UNDEFINED"){
        m_status = "RECORDING";
        last_status = m_status;
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
void Record::record_thread(){
  FileBuffer* fb = nullptr;
  ChronoMeter cm;
  size_t framecounter = 0;
  const unsigned colorsize = true ? 691200 : 1280 * 1080 * 3;
  const unsigned depthsize = 512 * 424 * sizeof(float);
  const unsigned framesize = (colorsize + depthsize) * 4;
  std::cout << "record setup done / networking / filebuffer" << std::endl;
  fb = new FileBuffer(m_filename.c_str());
  if(!fb->open("w", 0)){
    std::cout << "Couldn't open file!" << std::endl;
    exit(0);
  }
  framecounter = 0;
  auto context = zmq::context_t(4);
  auto socket = zmq::socket_t(context, ZMQ_SUB); // means a subscriber
  socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
  uint32_t hwm = 1;
  socket.setsockopt(ZMQ_RCVHWM,&hwm, sizeof(hwm));
  std::string endpoint("tcp://" + m_stream_endpoint);
  socket.connect(endpoint.c_str());

  while(m_status != "TERMINATE"){
    if(m_status == "RECORDING"){
      zmq::message_t zmqm(framesize);
      socket.recv(&zmqm); // blocking
      const double currtime = cm.getTick();
      memcpy((char*) zmqm.data(), (const char*) &currtime, sizeof(double));
      if(fb != nullptr){
        fb->write((unsigned char*) zmqm.data(), framesize);
        if(framecounter > MAX_FRAMES_TO_RECORD){
          fb->close();
          delete fb;
          fb = nullptr;
          std::cout << "closed" << std::endl;
        }
      }
    }
  }
};
std::string Record::to_string(){
  std::string output;

  output += m_filename + "$";
  output += m_stream_endpoint + "$";
  output += m_backchannel_endpoint + "$";

  return output;
};
std::string& Record::backchannel_endpoint(){
  return m_backchannel_endpoint;
};
void Record::backchannel_endpoint(std::string const& backchannel_endpoint){
  m_backchannel_endpoint = backchannel_endpoint;
};
void Record::from_string(Record& play, std::string const& command_string){
  auto parts = split(command_string, '$');
  play.backchannel_endpoint(parts[2]);
};
std::shared_ptr<Record> Record::from_string(std::string const& command_string){
  auto parts = split(command_string, '$');

  auto play = std::make_shared<Record>(parts[0], parts[1], parts[2]);

  return play;
};
Record::~Record(){
  std::cout << "DESTRUCTOR" << std::endl;
};

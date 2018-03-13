#ifndef helpers_hpp
#define helpers_hpp
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

template<typename Out>
inline void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

static char* int_to_char(int i){
  static char j[10];
  sprintf(j, "%d", i);
  return j;
};

inline bool to_bool(std::string const& s) {
     return s != "0";
}

inline std::vector<zmq::message_t> rt_message(std::string const& id, std::string const& data){
  std::vector<zmq::message_t> messages;
  zmq::message_t ident_msg(id.size());
  zmq::message_t del_msg(0);
  zmq::message_t data_msg(data.size());

  memcpy((unsigned char*) ident_msg.data(), (const unsigned char*) id.c_str(), id.size());
  memcpy((unsigned char*) data_msg.data(), (const unsigned char*) data.c_str(), data.size());

  messages.push_back(std::move(ident_msg));
  messages.push_back(std::move(del_msg));
  messages.push_back(std::move(data_msg));

  return messages;
};

inline std::vector<zmq::message_t> dl_message(std::string const& data){
  std::vector<zmq::message_t> messages;

  zmq::message_t del_msg(0);
  zmq::message_t data_msg(data.size());

  memcpy((unsigned char*) data_msg.data(), (const unsigned char*) data.c_str(), data.size());

  messages.push_back(std::move(del_msg));
  messages.push_back(std::move(data_msg));

  return messages;
};

inline void rt_send(zmq::socket_t &skt, std::string const& id, std::string const& data){
  std::vector<zmq::message_t> reply = rt_message(id, data);

  skt.send(reply[0], ZMQ_SNDMORE);
  skt.send(reply[1], ZMQ_SNDMORE);
  skt.send(reply[2]);
};

inline void dl_send(zmq::socket_t &skt, std::string const& data){
  std::vector<zmq::message_t> reply = dl_message(data);

  skt.send(reply[0], ZMQ_SNDMORE);
  skt.send(reply[1]);
};

inline void ssend(zmq::socket_t &skt, std::string const& data){
  zmq::message_t reply(data.size());
  memcpy((unsigned char*) reply.data(), (const unsigned char*) data.c_str(), data.size());
  skt.send(reply);
}

inline std::vector<std::string> srecv(zmq::socket_t &skt){
  int more = 1;
  size_t more_size = sizeof(more);
  std::vector<std::string> shist;

  while(more != 0){
    zmq::message_t msg;
    skt.recv(&msg);
    skt.getsockopt(ZMQ_RCVMORE, &more, &more_size);
    auto i = std::string(static_cast<char*>(msg.data()),msg.size());
    shist.push_back(i);
  }

  return shist;
};

inline std::vector<zmq::message_t> recv(zmq::socket_t &skt){
  int more = 1;
  size_t more_size = sizeof(more);
  std::vector<zmq::message_t> shist;

  while(more != 0){
    zmq::message_t msg;
    skt.recv(&msg);
    skt.getsockopt(ZMQ_RCVMORE, &more, &more_size);
    shist.push_back(std::move(msg));
  }

  return shist;
};


#endif

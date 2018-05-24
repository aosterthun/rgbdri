#ifndef __RGBDRI_HPP_INCLUDED__
#define __RGBDRI_HPP_INCLUDED__

#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <vector>
#include <GenericMessage.hpp>
#include <play.hpp>
#include <record.hpp>
#include <helpers.hpp>
#include <RGBDRIPortManager.hpp>
#include <iostream>

class Play;
class Record;

class RGBDRI{
private:
  std::shared_ptr<zmq::context_t> m_context;
  std::shared_ptr<zmq::socket_t> m_socket;
  std::string m_server_endpoint;
public:
  RGBDRI();
  RGBDRI(std::string const& server_endpoint, std::string const& client_endpoint);
  void init_play(std::shared_ptr<Play> play);
  const std::string& server_endpoint();
  void send(std::string const& data);
  std::vector<std::string> recv();
};
#endif

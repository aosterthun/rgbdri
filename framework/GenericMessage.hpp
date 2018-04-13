#ifndef GenericMessage_hpp
#define GenericMessage_hpp

#include "../spdlog/include/spdlog/spdlog.h"
#include "zmq.hpp"
#include <memory>
#include <stdio.h>
#include <vector>
#include "helpers.hpp"
#include <iostream>

struct GenericMessage{
  int type;
  std::string payload;
  std::string to_string();
  static GenericMessage from_string(std::string const& generic_message_string);
};

#endif

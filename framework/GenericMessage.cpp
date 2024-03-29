#include "GenericMessage.hpp"

std::string GenericMessage::to_string(){
  std::string output;

  output += std::to_string(type) + "+";
  output += meta_data + "+";
  output += payload + "+";

  return output;
}

GenericMessage GenericMessage::from_string(std::string const& generic_message_string){
  auto parts = split(generic_message_string, '+');
  auto msg = GenericMessage();
  msg.type = stoi(parts[0]);
  msg.meta_data = parts[1];
  msg.payload = parts[2];

  return msg;
}

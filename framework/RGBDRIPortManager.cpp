#include "RGBDRIPortManager.hpp"

ZMQPortManager& ZMQPortManager::get_instance(){
  static ZMQPortManager instance{8001,2};
	return instance;
}

ZMQPortManager::ZMQPortManager(unsigned _start_port, unsigned _port_offset){
	this->next_free_port = _start_port;
  this->port_offset = _port_offset;
}

unsigned ZMQPortManager::get_next_free_port(){
	std::lock_guard<std::mutex> _lock{this->mutex};
	unsigned _cur_free_port = this->next_free_port;
	this->next_free_port += this->port_offset;
	return _cur_free_port;
}

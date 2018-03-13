#ifndef ZMQPortManager_hpp
#define ZMQPortManager_hpp

#include <mutex>
#include <memory>
class ZMQPortManager
{
public:
	static ZMQPortManager& get_instance();
	//
  	unsigned get_next_free_port();
private:
  	unsigned next_free_port;
  	unsigned port_offset;
  	std::mutex mutex;
    static ZMQPortManager instance;
	ZMQPortManager()= default;
	ZMQPortManager(unsigned _start_port);
  ZMQPortManager(unsigned _start_port, unsigned _port_offset);
	~ZMQPortManager()= default;
	ZMQPortManager(const ZMQPortManager&)= delete;
	ZMQPortManager& operator=(const ZMQPortManager&)= delete;

};



#endif

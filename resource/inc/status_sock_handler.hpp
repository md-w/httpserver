//
// status_sock_handler.hpp
// Singleton class that handles the status send portion,
// reconnects to socket if connection closes.
//

#pragma once
#include <Poco/Net/SocketStream.h>

#include <iostream>
class StatusSockHandler {
 private:
  std::string _host_address;
  Poco::UInt16 _port_number;

  Poco::Net::SocketAddress* _p_sock_addr;
  Poco::Net::StreamSocket* _p_stream_sock;
  Poco::Net::SocketStream* _p_sock_stream;
  bool connect();
  bool disconnect();
  bool connected();  
 public:
  StatusSockHandler(const std::string& host_address, Poco::UInt16 port_number);
  ~StatusSockHandler();
  Poco::Net::SocketStream* get_sock();
};
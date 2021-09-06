#include "status_sock_handler.hpp"

StatusSockHandler::StatusSockHandler(const std::string& host_address, Poco::UInt16 port_number)
    : _host_address(host_address), _port_number(port_number), _p_sock_addr(NULL), _p_stream_sock(NULL), _p_sock_stream(NULL) {}

StatusSockHandler::~StatusSockHandler() { disconnect(); }

bool StatusSockHandler::connect() {
  disconnect();
  _p_sock_addr = new Poco::Net::SocketAddress(_host_address, _port_number);
  try {
    _p_stream_sock = new Poco::Net::StreamSocket(*_p_sock_addr);
  } catch (const std::exception& e) {
  }
  try {
    if (_p_stream_sock) _p_sock_stream = new Poco::Net::SocketStream(*_p_stream_sock);
    return true;
  } catch (const std::exception& e) {
  }
  return false;
}

bool StatusSockHandler::disconnect() {
  if (_p_sock_addr) {
    delete _p_sock_addr;
    _p_sock_addr = NULL;
  }
  if (_p_stream_sock) {
    _p_stream_sock->close();
    delete _p_stream_sock;
    _p_stream_sock = NULL;
  }
  if (_p_sock_stream) {
    _p_sock_stream->flush();
    _p_sock_stream->close();
    delete _p_sock_stream;
    _p_sock_stream = NULL;
  }
  return true;
}

bool StatusSockHandler::connected() { return (_p_sock_stream && _p_stream_sock && _p_sock_stream->good() && _p_stream_sock->impl()->initialized()); }

Poco::Net::SocketStream* StatusSockHandler::get_sock() {
  if (!connected()) {
    connect();
  }
  if (connected())
    return _p_sock_stream;
  else
    return NULL;
}
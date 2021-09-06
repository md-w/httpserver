#pragma once
#ifndef LogReceiverTCPServerConnectionFactory_h
#define LogReceiverTCPServerConnectionFactory_h

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>

#include "LogReceiverServer.h"
#include <logging.h>

class LogReceiverTCPServerConnectionFactory : public Poco::Net::TCPServerConnectionFactory {
 private:
  std::vector<LogReceiverServer*> list_of_sockets;

 public:
  LogReceiverTCPServerConnectionFactory() {}

  ~LogReceiverTCPServerConnectionFactory() {
    RAY_LOG(INFO) << "LogReceiverTCPServerConnectionFactory destructor start" ;
    for (auto&& it : list_of_sockets) {
      delete it;
    }
    RAY_LOG(INFO) << "LogReceiverTCPServerConnectionFactory destructor end" ;
  }

  Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket) {
    LogReceiverServer* s = new LogReceiverServer(socket);
    list_of_sockets.push_back(s);
    return s;
  }
};
#endif
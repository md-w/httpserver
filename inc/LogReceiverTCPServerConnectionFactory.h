#pragma once
#ifndef LogReceiverTCPServerConnectionFactory_h
#define LogReceiverTCPServerConnectionFactory_h

#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/TCPServerConnection.h>
#include <Poco/Net/TCPServerConnectionFactory.h>

#include "LogReceiverServer.h"

class LogReceiverTCPServerConnectionFactory : public Poco::Net::TCPServerConnectionFactory {
 private:
  std::vector<LogReceiverServer*> list_of_sockets;

 public:
  LogReceiverTCPServerConnectionFactory();

  ~LogReceiverTCPServerConnectionFactory();

  Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket);

  void shutDown();
};
#endif
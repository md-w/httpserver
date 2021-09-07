#include "LogReceiverTCPServerConnectionFactory.h"

#include <logging.h>

LogReceiverTCPServerConnectionFactory::LogReceiverTCPServerConnectionFactory() {}

LogReceiverTCPServerConnectionFactory::~LogReceiverTCPServerConnectionFactory() {}

Poco::Net::TCPServerConnection* LogReceiverTCPServerConnectionFactory::createConnection(
    const Poco::Net::StreamSocket& socket) {
  LogReceiverServer* s = new LogReceiverServer(socket);
  list_of_sockets.push_back(s);
  return s;
}

void LogReceiverTCPServerConnectionFactory::shutDown() {
  RAY_LOG(INFO) << "LogReceiverTCPServerConnectionFactory destructor start";
  for (auto&& it : list_of_sockets) {
    it->shutDown();
    delete it;
  }
  RAY_LOG(INFO) << "LogReceiverTCPServerConnectionFactory destructor end";
}
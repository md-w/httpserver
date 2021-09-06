//
// LogReceiverServer.h
// A TCP based server, responsible for receiving logs from other components
//
// Videonetics
//
#pragma once

#ifndef LogReceiverServer_HH
#define LogReceiverServer_HH

#include <atomic>
#include <iostream>
#include <thread>

#include "Poco/Exception.h"
#include "Poco/Logger.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Subsystem.h"

class LogReceiverServer : public Poco::Net::TCPServerConnection {
 private:
  // stream socket
  std::atomic<bool> _is_shutdown_command_received;

 public:
  void run();

 private:
  int getData(Poco::Net::StreamSocket& ss, uint8_t* buff, int bytes_to_read);

 public:
  LogReceiverServer(const Poco::Net::StreamSocket& s);
  ~LogReceiverServer();
  void shutDown();
};

// typedef Poco::Net::TCPServerConnectionFactoryImpl<LogReceiverServer> TCPFactory;

#endif
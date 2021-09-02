//
// LogReceiverServer.h
// A TCP based server, responsible for receiving logs from other components
//
// Videonetics
//
#pragma once

#ifndef LogReceiverServer_HH
#define LogReceiverServer_HH

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

class LogReceiverServer : public Poco::Util::Subsystem, public Poco::Net::TCPServerConnection {
 private:
  // stream socket
  bool _is_shutdown_command_received;

 public:
  /// Returns the name of the subsystem.
  /// Must be implemented by subclasses.
  const char* name() const { return "LogReceiverServer"; }
  /// Initializes the subsystem.
  void initialize(Poco::Util::Application& app) {}

  /// Uninitializes the subsystem.
  void uninitialize() {}

 private:
  void run();

 public:
  LogReceiverServer(const Poco::Net::StreamSocket& s);
  ~LogReceiverServer();
  void shutDown();
};

typedef Poco::Net::TCPServerConnectionFactoryImpl<LogReceiverServer> TCPFactory;

#endif
#include "LogReceiverServer.h"

#include <logging.h>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"

#define MODULE_NAME "LogReceiverServer"
static std::string get_session_folder() { return "./session/"; }
std::string get_name_of_app() { return MODULE_NAME; }

LogReceiverServer::LogReceiverServer(const Poco::Net::StreamSocket& s)
    : TCPServerConnection(s), _is_shutdown_command_received(false) {
  ::ray::RayLog::StartRayLog(get_name_of_app(), ::ray::RayLogLevel::DEBUG, get_session_folder());
  RAY_LOG(INFO) << "Started: Inside constructor." << get_name_of_app();
}
LogReceiverServer::~LogReceiverServer() { RAY_LOG(INFO) << "Finished: Inside constructor." << get_name_of_app(); }
void LogReceiverServer::shutDown() { _is_shutdown_command_received = true; }
void LogReceiverServer::run() {
  Poco::Net::StreamSocket& ss = socket();
  try {
    char buffer[256];
    int n = ss.receiveBytes(buffer, sizeof(buffer));
    while (n > 0) {
      std::string msg;
      Poco::Logger::formatDump(msg, buffer, n);
      RAY_LOG(INFO) << "Received " << n << " bytes:" << msg;
      n = ss.receiveBytes(buffer, sizeof(buffer));
    }
  } catch (Poco::Exception& exc) {
    std::cerr << "ClientConnection: " << exc.displayText() << std::endl;
  }
}
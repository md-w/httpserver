#include "LogReceiverServer.h"

#include <logging.h>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "status.pb.h"

#define FUNC_TYPE_DATA 1
#define FUNC_TYPE_EOF 2

enum class ReceiverState { None = 0, FuncType = 1, Timestamp = 2, Len = 3, Data = 4, End = 100 };

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
bool LogReceiverServer::getData(Poco::Net::StreamSocket& ss, void* buff, int bytes_to_read) {
  return ss.receiveBytes(buff, buff_len);
  // int length = 0;
  // while (true)
  // {
  //   int x = bytes_to_read - length;
  //   if 
  // }
  // return length == bytes_to_read;
  

// def read_data_n(s: socket.socket, data_array: bytearray, bytes_to_read: int) -> bool:
//     length = 0
//     if not s:
//         return False
//     while True:
//         x = bytes_to_read - length
//         if x > 0:
//             try:
//                 length_received = s.recv_into(memoryview(data_array)[length:], x)
//                 if length_received <= 0:
//                     break
//                 length += length_received
//             except (socket.timeout, OSError) as e:
//                 # LOGGER.exception(e)
//                 break
//         else:
//             break
//     return length == bytes_to_read

//   return ss.receiveBytes(buff, buff_len);
// }
}
void LogReceiverServer::run() {
  Poco::Net::StreamSocket& ss = socket();
  ReceiverState state = ReceiverState::None;
  do {
    try {
      unsigned long to_len = 0;
      
        while (state != ReceiverState::End) {
          switch (state) {
          case ReceiverState::None:
          case ReceiverState::FuncType: {
            int to_func_type = 0;
            int nb = getData(ss, &to_func_type, 4);
            if (to_func_type == FUNC_TYPE_DATA) {
              // OK
              state = ReceiverState::Timestamp;
            } else if (to_func_type == FUNC_TYPE_EOF) {
              // OK, exit
              state = ReceiverState::End;
            }
          } break;
          case ReceiverState::Timestamp: {
            long to_ts = std::stoll(buffer);
            state = ReceiverState::Len;
          } break;
          case ReceiverState::Len: {
            to_len = 0;
            to_len = (unsigned long)std::stol(buffer);
            state = ReceiverState::Data;
          } break;     
          case ReceiverState::Data: {

          } break;  
          case ReceiverState::End: {} break;          
          }
        }
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
    }

  } while (!_is_shutdown_command_received);
  try {
    char buffer[10];
    int n = ss.receiveBytes(buffer, sizeof(buffer));
    while (n > 0) {
      std::string msg;
      Poco::Logger::formatDump(msg, buffer, n);
      // RAY_LOG(INFO) << "Received " << n << " bytes:" << msg;
      RAY_LOG(INFO) << "Received " << n << " bytes:";

      // n = ss.receiveBytes(buffer, sizeof(buffer));
    }
  } catch (Poco::Exception& exc) {
    std::cerr << "ClientConnection: " << exc.displayText() << std::endl;
  }
}
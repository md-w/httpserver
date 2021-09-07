#include "LogReceiverServer.h"

#include <Poco/Net/NetException.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>
#include <logging.h>

#include "CentralDataRepo.h"
#include "interfaces/status.pb.h"

#define FUNC_TYPE_DATA 1
#define FUNC_TYPE_EOF 2

enum class ReceiverState { Init = 0, Timestamp = 1, Length = 2, Data = 3, End = 100 };
// ReceiverState operator++(ReceiverState& state) {
//   switch (state) {
//     case ReceiverState::Init:
//       state = ReceiverState::Timestamp;
//       break;
//     case ReceiverState::Timestamp:
//       state = ReceiverState::Length;
//       break;
//     case ReceiverState::Length:
//       state = ReceiverState::Data;
//       break;
//     case ReceiverState::Data:
//       state = ReceiverState::Init;
//       break;
//     default:
//       state = ReceiverState::Init;
//       break;
//   }
//   return state;
// }

LogReceiverServer::LogReceiverServer(const Poco::Net::StreamSocket& s)
    : TCPServerConnection(s), _is_shutdown_command(false), _is_already_shutting_down(false) {}
LogReceiverServer::~LogReceiverServer() { shutDown(); }
void LogReceiverServer::shutDown() {
  if (_is_already_shutting_down) return;
  _is_already_shutting_down = true;
  _is_shutdown_command = true;
}

// returns -1 for fatal error
// returns 0 for success
// returns 1 for not fatal errors
int LogReceiverServer::getData(Poco::Net::StreamSocket& ss, uint8_t* buff, int bytes_to_read) {
  int ret = 1;
  int length = 0;
  while (!_is_shutdown_command) {
    int x = bytes_to_read - length;
    if (x > 0) {
      try {
        int length_received = ss.receiveBytes((buff + length), x);
        if (length_received <= 0) {
          ret = -1;
          break;
        }
        length += length_received;
        ret = 0;
      } catch (const Poco::TimeoutException& e) {
        ret = 1;
        break;
      } catch (const Poco::Net::NetException& e) {
        RAY_LOG(ERROR) << "Net exception";
        ret = -1;
        break;
      }
    } else {
      break;
    }
  }
  return ret;
}

void LogReceiverServer::run() {
  Poco::Net::StreamSocket& ss = socket();
  ss.setReceiveTimeout(Poco::Timespan(5, 0));
  std::string& strSocketAddress = ss.peerAddress().toString();
  ReceiverState state = ReceiverState::Init;
  std::vector<char> data_buff;
  int data_buff_size = 100;
  data_buff.resize(data_buff_size);
  int function_type = 0;
  int64_t data_timestamp = 0;
  int64_t data_length = 0;
  int r = 0;
  RAY_LOG(INFO) << "Started";
  while (!_is_shutdown_command) {
    if (state == ReceiverState::End) break;
    // RAY_LOG(INFO) << "In State: " << (int)state;
    switch (state) {
      case ReceiverState::Init:
        state = ReceiverState::End;
        r = getData(ss, (uint8_t*)&function_type, 4);
        if (r == 0) {
          if (function_type == FUNC_TYPE_DATA) {
            state = ReceiverState::Timestamp;
          }
        } else if (r > 0) {
          state = ReceiverState::Init;
        }
        break;
      case ReceiverState::Timestamp:
        state = ReceiverState::End;
        r = getData(ss, (uint8_t*)&data_timestamp, 8);
        if (r == 0) {
          state = ReceiverState::Length;
        }
        break;
      case ReceiverState::Length:
        state = ReceiverState::End;
        r = getData(ss, (uint8_t*)&data_length, 8);
        if (r == 0) {
          if (data_buff_size < data_length) {
            data_buff.resize(data_length);
            data_buff_size = data_length;
          }
          state = ReceiverState::Data;
        }
        break;
      case ReceiverState::Data:
        state = ReceiverState::End;
        r = getData(ss, (uint8_t*)data_buff.data(), data_length);
        if (r == 0) {
          state = ReceiverState::Init;

          ::resource::MachineStatus m;
          m.ParseFromString(std::string(data_buff.data(), data_length));

          (*CentralDataRepo::getInstance()) << m;

          // print
          RAY_LOG(INFO) << "\nConnection from: " << strSocketAddress << " Time: " << data_timestamp
                        << " data_length: " << data_length << " Received Data: " << m.DebugString();
        }
        break;
      case ReceiverState::End:
        break;
      default:
        break;
    }
  }
  try {
    ss.shutdown();
  } catch (const Poco::Net::NetException& e) {
  }
  try {
    ss.close();
  } catch (const Poco::Net::NetException& e) {
  }
  RAY_LOG(INFO) << "End";
}

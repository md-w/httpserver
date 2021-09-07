#include "monitoring_thread.hpp"

#include <Poco/Net/NetException.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "interfaces/status.pb.h"

#define FUNC_TYPE_DATA 1
#define FUNC_TYPE_EOF 2

MonitoringThread* MonitoringThread::instance_ = nullptr;

MonitoringThread::MonitoringThread() : _is_shutdown_command(false), _is_already_shutting_down(false) {
  _this_thread = std::thread(&MonitoringThread::run, this);
}

MonitoringThread::~MonitoringThread() {
  shutDown();

  // TODO: delete _p_machine_status etc.
}

MonitoringThread* MonitoringThread::createInstance() {
  if (!instance_) {
    instance_ = new MonitoringThread();
  }
  return instance_;
}

MonitoringThread* MonitoringThread::getInstance() { return createInstance(); }

void MonitoringThread::deleteInstance() {
  if (instance_) {
    delete instance_;
  }
  instance_ = NULL;
}

void MonitoringThread::shutDown() {
  if (_is_already_shutting_down) return;
  _is_already_shutting_down = true;
  _is_shutdown_command = true;
  _this_thread.join();
  for (auto&& it : _resource_map) {
    it.second.reset();
    it.second = nullptr;
  }
}

uint64_t getCurrentTimeInMs() {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  return millis;
}
void MonitoringThread::setStatus(uint64_t id) {
  if (_is_already_shutting_down) return;
  std::map<uint64_t, std::unique_ptr<Status>>::iterator it = _resource_map.find(id);
  if (it != _resource_map.end()) {
    it->second->value++;
    it->second->last_update_time_in_ms.store(getCurrentTimeInMs());
  } else {
    _resource_map.insert(std::make_pair(id, std::move(std::make_unique<Status>())));
  }
}

void MonitoringThread::run() {
  std::cout << "Started monitoring thread" << std::endl;
  int sleep_upto_sec = 10;
  int iteration_counter = 0;
  while (!_is_shutdown_command) {
    if (sleep_upto_sec > 0) {
      sleep_upto_sec--;
    } else {
      sleep_upto_sec = 10;
      try {
        std::cout << "Trying to connect" << std::endl;
        std::unique_ptr<Poco::Net::StreamSocket> s = std::make_unique<Poco::Net::StreamSocket>();
        s->connect(Poco::Net::SocketAddress("127.0.0.1", 23000), Poco::Timespan(2, 0));
        s->setSendTimeout(Poco::Timespan(2, 0));
        Poco::Net::SocketStream ss(*s);
        sleep_upto_sec = 10;
        while (!_is_shutdown_command) {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          if (sleep_upto_sec > 0) {
            sleep_upto_sec--;
          } else {
            sleep_upto_sec = 10;
            ::resource::MachineStatus machine_status;
            machine_status.set_id(1);
            machine_status.set_channel_id(iteration_counter++);
            ::resource::ProcessStatus* p_process_status = machine_status.add_process_status();
            p_process_status->set_id(1);
            p_process_status->set_channel_id(1);

            for (auto&& it : _resource_map) {
              float diff = (float)(it.second->value - it.second->last_value) / 10;
              std::stringstream ss1;
              ss1 << "FPS : " << std::setfill('0') << std::setw(5) << it.first << " : " << std::setfill('0')
                  << std::setw(5) << diff;
              std::cout << ss1.str() << std::endl;
              ::resource::ThreadStatus* l_p_th = p_process_status->add_thread_status();
              l_p_th->set_id(it.first);
              l_p_th->set_channel_id(it.first);
              l_p_th->set_value(it.second->value);
              l_p_th->set_last_value(it.second->last_value);
              l_p_th->set_last_updated_in_ms(it.second->last_update_time_in_ms);

              it.second->last_value.store(it.second->value);
            }
            int func_type = (int)FUNC_TYPE_DATA;
            int64_t data_timestamp = getCurrentTimeInMs();
            int64_t data_len = machine_status.ByteSizeLong();
            try {
              ss.write((char*)&func_type, 4);
              ss.write((char*)&data_timestamp, 8);
              ss.write((char*)&data_len, 8);
              if (machine_status.SerializeToOstream(&ss) == false) {
                break;
              }
              std::cout << machine_status.DebugString() << std::endl;
              ss.flush();
            } catch (const Poco::Net::NetException& e) {
              break;
            }
          }
        }
        sleep_upto_sec = 10;
        try {
          s->shutdown();
        } catch (const Poco::Net::NetException& e) {
        }
        try {
          s->close();
        } catch (const Poco::Net::NetException& e) {
        }
      } catch (const Poco::Net::NetException& e) {
        sleep_upto_sec = 10;
        std::cout << "Unable to connect retrying in " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      } catch (const Poco::TimeoutException& e) {
        sleep_upto_sec = 10;
        std::cout << "Unable to connect retrying in " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }
  std::cout << "Ended monitoring thread" << std::endl;
}
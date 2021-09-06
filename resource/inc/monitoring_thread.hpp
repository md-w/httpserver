#pragma once

#ifndef _monitoring_thread_h_
#define _monitoring_thread_h_

#include <atomic>
#include <map>
#include <thread>

class Status {
 public:
  std::atomic_uint_fast64_t last_update_time_in_ms;
  // std::atomic_uint_fast64_t last_update_time_in_ms;
  std::atomic_uint_fast64_t last_value;
  std::atomic_uint_fast64_t value;
  Status() : last_value(0), value(0) {}
  ~Status() {}
};

class MonitoringThread {
 private:
  static MonitoringThread *createInstance();
  MonitoringThread();
  ~MonitoringThread();
  void run();
  std::map<uint64_t, std::unique_ptr<Status>> _resource_map;
  std::atomic_bool _is_shutdown_command;
  std::atomic_bool _is_already_shutting_down;
  std::thread _this_thread;
  static MonitoringThread *instance_;
  void shutDown();

 public:
  static MonitoringThread *getInstance();
  static void deleteInstance();
  void setStatus(uint64_t id);
};

#endif
//
// CentralDataRepo.h
// Central status data repository
//
//

#pragma once
#ifndef CentralDataRepo_HH
#define CentralDataRepo_HH

#include <atomic>
#include <iostream>
#include <map>
#include <memory>

#include "interfaces/status.pb.h"

class ThreadStatus {
 public:
  int64_t id;
  int64_t channel_id;
  std::atomic<int64_t> value;
  std::atomic<int64_t> last_value;
  std::atomic<int64_t> last_updated_in_ms;
};

class ProcessStatus {
 public:
  ProcessStatus(int64_t _id, int64_t _channel_id) : id(_id), channel_id(_channel_id) {}
  int64_t id;
  int64_t channel_id;
  std::map<int64_t, std::unique_ptr<ThreadStatus>> thread_status;
  friend void operator<<(ProcessStatus& p, const ::resource::ThreadStatus& t);
};

class MachineStatus {
 public:
  MachineStatus(int64_t _id, int64_t _channel_id) : id(_id), channel_id(_channel_id) {}
  int64_t id;
  int64_t channel_id;
  std::map<int64_t, std::unique_ptr<ProcessStatus>> process_status;
  // friend void operator<<(MachineStatus& m, const ::resource::ProcessStatus& p);
  friend void operator<<(MachineStatus& m, const ::resource::MachineStatus& p);
};

class ClusterStatus {
 public:
  ClusterStatus(int64_t _id, int64_t _channel_id) : id(_id), channel_id(_channel_id) {}
  int64_t id;
  int64_t channel_id;
  std::map<int64_t, std::unique_ptr<MachineStatus>> machine_status;
  // friend void operator<<(ClusterStatus& c, ::resource::MachineStatus& m);
  friend void operator<<(ClusterStatus& c, ::resource::ClusterStatus& m);
};

class CentralDataRepo {
 private:
  static CentralDataRepo* createInstance();
  static CentralDataRepo* instance_;
  CentralDataRepo() : cluster_status(0, 0) {}
  ~CentralDataRepo() {}

 public:
  ClusterStatus cluster_status;
  static CentralDataRepo* getInstance();
  static void deleteInstance();
  // friend void operator<<(CentralDataRepo& repo, ::resource::MachineStatus& m);
  friend void operator<<(CentralDataRepo& repo, ::resource::ClusterStatus& c_in);
};

#endif
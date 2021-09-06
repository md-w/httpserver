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
#include "Poco/JSON/Object.h"

class ThreadStatus {
 public:
  int64_t id;
  int64_t channel_id;
  std::atomic<int64_t> value;
  std::atomic<int64_t> last_value;
  std::atomic<int64_t> last_updated_in_ms;
  ThreadStatus();
  ThreadStatus(int64_t _id, int64_t _channel_id, int64_t _value, int64_t _last_value, int64_t _last_updated_in_ms);
  ThreadStatus(const ::resource::ThreadStatus& in);
  friend ThreadStatus& operator<<(ThreadStatus& out, const ::resource::ThreadStatus& in);
  friend void operator>>(ThreadStatus& in, Poco::JSON::Object::Ptr out);
};

class ProcessStatus {
 public:
  int64_t id;
  int64_t channel_id;
  std::map<int64_t, std::unique_ptr<ThreadStatus>> thread_status;
  ProcessStatus();
  ProcessStatus(int64_t _id);
  ProcessStatus(const ::resource::ProcessStatus in);
  friend ProcessStatus& operator<<(ProcessStatus& out, const ::resource::ThreadStatus& in);
  friend ProcessStatus& operator<<(ProcessStatus& out, const ::resource::ProcessStatus& in);
  friend void operator>>(ProcessStatus& in, Poco::JSON::Object::Ptr out);
};

class MachineStatus {
 public:
  int64_t id;
  int64_t channel_id;
  std::map<int64_t, std::unique_ptr<ProcessStatus>> process_status;
  MachineStatus();
  MachineStatus(int64_t _id);
  MachineStatus(const ::resource::MachineStatus in);
  friend MachineStatus& operator<<(MachineStatus& out, const ::resource::ProcessStatus& in);
  friend MachineStatus& operator<<(MachineStatus& out, const ::resource::MachineStatus& in);
  friend void operator>>(MachineStatus& in, Poco::JSON::Object::Ptr out);
};

class ClusterStatus {
 public:
  int64_t id;
  int64_t channel_id;
  std::map<int64_t, std::unique_ptr<MachineStatus>> machine_status;
  ClusterStatus();
  ClusterStatus(int64_t _id);
  ClusterStatus(const ::resource::ClusterStatus in);
  friend ClusterStatus& operator<<(ClusterStatus& out, const ::resource::MachineStatus& in);
  friend ClusterStatus& operator<<(ClusterStatus& out, const ::resource::ClusterStatus& in);
  friend void operator>>(ClusterStatus& in, Poco::JSON::Object::Ptr out);
};

class CentralDataRepo {
 private:
  static CentralDataRepo* createInstance();
  static CentralDataRepo* instance_;
  CentralDataRepo();

 public:
  ClusterStatus cluster_status;
  static CentralDataRepo* getInstance();
  static void deleteInstance();
  friend CentralDataRepo& operator<<(CentralDataRepo& out, const ::resource::MachineStatus& in);
  friend CentralDataRepo& operator<<(CentralDataRepo& out, const ::resource::ClusterStatus& in);
  friend std::ostream& operator>>(CentralDataRepo& in, std::ostream& ostr);

  // std::string getJson();
};

#endif
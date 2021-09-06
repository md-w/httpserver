#include "CentralDataRepo.h"

#include "interfaces/status.pb.h"
#include "Poco/JSON/Object.h"

CentralDataRepo* CentralDataRepo::instance_ = nullptr;

CentralDataRepo* CentralDataRepo::createInstance() {
  if (!instance_) {
    instance_ = new CentralDataRepo();
  }
  return instance_;
}

CentralDataRepo* CentralDataRepo::getInstance() { return createInstance(); }

void CentralDataRepo::deleteInstance() {
  if (instance_) {
    delete instance_;
  }
  instance_ = NULL;
}

ThreadStatus::ThreadStatus() : id(0), channel_id(0), value(0), last_value(0), last_updated_in_ms(0) {}

ThreadStatus::ThreadStatus(int64_t _id, int64_t _channel_id, int64_t _value, int64_t _last_value,
                           int64_t _last_updated_in_ms)
    : id(_id),
      channel_id(_channel_id),
      value(_value),
      last_value(_last_value),
      last_updated_in_ms(_last_updated_in_ms) {}

ThreadStatus::ThreadStatus(const ::resource::ThreadStatus& in)
    : ThreadStatus(in.id(), in.channel_id(), in.value(), in.last_value(), in.last_updated_in_ms()) {}

ThreadStatus& operator<<(ThreadStatus& out, const ::resource::ThreadStatus& in) {
  assert(out.id == in.id());  // "Same thread id is not getting updated"
  out.channel_id = in.channel_id();
  out.value = in.value();
  out.last_value = in.last_value();
  out.last_updated_in_ms = in.last_updated_in_ms();
  return out;
}

ProcessStatus::ProcessStatus() : id(0), channel_id(0) {}

ProcessStatus::ProcessStatus(int64_t _id) : id(_id) {}

ProcessStatus::ProcessStatus(const ::resource::ProcessStatus in) : id(in.id()) { *this << in; }

ProcessStatus& operator<<(ProcessStatus& out, const ::resource::ThreadStatus& in) {
  auto it = out.thread_status.find(in.id());
  if (it == out.thread_status.end()) {
    out.thread_status.insert(std::make_pair(in.id(), std::move(std::make_unique<ThreadStatus>(in))));
  } else {
    *(it->second) << in;
  }
  return out;
}

ProcessStatus& operator<<(ProcessStatus& out, const ::resource::ProcessStatus& in) {
  assert(out.id == in.id());  // "Same thread id is not getting updated"
  out.channel_id = in.channel_id();
  for (auto&& it : in.thread_status()) {
    out << it;
  }
  return out;
}

MachineStatus::MachineStatus() : id(0), channel_id(0) {}

MachineStatus::MachineStatus(int64_t _id) : id(_id) {}

MachineStatus::MachineStatus(const ::resource::MachineStatus in) : id(in.id()) { *this << in; }

MachineStatus& operator<<(MachineStatus& out, const ::resource::ProcessStatus& in) {
  auto it = out.process_status.find(in.id());
  if (it == out.process_status.end()) {
    out.process_status.insert(std::make_pair(in.id(), std::move(std::make_unique<ProcessStatus>(in))));
  } else {
    *(it->second) << in;
  }
  return out;
}

MachineStatus& operator<<(MachineStatus& out, const ::resource::MachineStatus& in) {
  assert(out.id == in.id());  // "Same thread id is not getting updated"
  out.channel_id = in.channel_id();
  for (auto&& it : in.process_status()) {
    out << it;
  }
  return out;
}

ClusterStatus::ClusterStatus() : id(0), channel_id(0) {}

ClusterStatus::ClusterStatus(int64_t _id) : id(_id) {}

ClusterStatus::ClusterStatus(const ::resource::ClusterStatus in) : id(in.id()) { *this << in; }

ClusterStatus& operator<<(ClusterStatus& out, const ::resource::MachineStatus& in) {
  auto it = out.machine_status.find(in.id());
  if (it == out.machine_status.end()) {
    out.machine_status.insert(std::make_pair(in.id(), std::move(std::make_unique<MachineStatus>(in))));
  } else {
    *(it->second) << in;
  }
  return out;
}

ClusterStatus& operator<<(ClusterStatus& out, const ::resource::ClusterStatus& in) {
  assert(out.id == in.id());  // "Same thread id is not getting updated"
  out.channel_id = in.channel_id();
  for (auto&& it : in.machine_status()) {
    out << it;
  }
  return out;
}

CentralDataRepo::CentralDataRepo() : cluster_status(0) {}

CentralDataRepo& operator<<(CentralDataRepo& out, const ::resource::MachineStatus& in) {
  out.cluster_status << in;
  return out;
}

CentralDataRepo& operator<<(CentralDataRepo& out, const ::resource::ClusterStatus& in) {
  out.cluster_status << in;
  return out;
}

std::ostream& operator>>(CentralDataRepo& out, std::ostream& ostr) {
  Poco::JSON::Object obj_cluster_status;
  obj_cluster_status.set("channel_id", 1);
  obj_cluster_status.set("id", 1);
  obj_cluster_status.stringify(ostr);
  return ostr;
}
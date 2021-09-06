#include "CentralDataRepo.h"

#include "interfaces/status.pb.h"

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

void operator>>(ThreadStatus& in, Poco::JSON::Object::Ptr out) {
  out->set("id", in.id);
  out->set("channel_id", in.channel_id);
  out->set("value", in.value.load());
  out->set("last_value", in.last_value.load());
  out->set("last_updated_in_ms", in.last_updated_in_ms.load());
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

void operator>>(ProcessStatus& in, Poco::JSON::Object::Ptr out) {
  out->set("id", in.id);
  out->set("channel_id", in.channel_id);
  Poco::JSON::Array::Ptr p_thread_status_arr = new Poco::JSON::Array();
  for (auto&& it : in.thread_status) {
    Poco::JSON::Object::Ptr p_thread_status = new Poco::JSON::Object();
    *(it.second.get()) >> p_thread_status;
    p_thread_status_arr->add(p_thread_status);
  }
  out->set("thread_status", p_thread_status_arr);
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

void operator>>(MachineStatus& in, Poco::JSON::Object::Ptr out) {
  out->set("id", in.id);
  out->set("channel_id", in.channel_id);
  Poco::JSON::Array::Ptr p_process_status_arr = new Poco::JSON::Array();
  for (auto&& it : in.process_status) {
    Poco::JSON::Object::Ptr p_process_status = new Poco::JSON::Object();
    *(it.second.get()) >> p_process_status;
    p_process_status_arr->add(p_process_status);
  }
  out->set("process_status", p_process_status_arr);
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

void operator>>(ClusterStatus& in, Poco::JSON::Object::Ptr out) {
  // TODO
  // out.assign(new Poco::JSON::Object());
  out->set("id", in.id);
  out->set("channel_id", in.channel_id);
  Poco::JSON::Array::Ptr p_machine_status_arr = new Poco::JSON::Array();
  for (auto&& it : in.machine_status) {
    Poco::JSON::Object::Ptr p_machine_status = new Poco::JSON::Object();
    *(it.second.get()) >> p_machine_status;
    p_machine_status_arr->add(p_machine_status);
  }
  out->set("machine_status", p_machine_status_arr);
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

std::ostream& operator>>(CentralDataRepo& in, std::ostream& ostr) {
  Poco::JSON::Object obj;
  Poco::JSON::Object::Ptr p = new Poco::JSON::Object();
  in.cluster_status >> p;
  obj.set("cluster", p);
  obj.stringify(ostr);
  return ostr;
}
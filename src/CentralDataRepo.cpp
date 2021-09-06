#include "CentralDataRepo.h"

#include "interfaces/status.pb.h"

CentralDataRepo *CentralDataRepo::instance_ = nullptr;

CentralDataRepo *CentralDataRepo::createInstance() {
  if (!instance_) {
    instance_ = new CentralDataRepo();
  }
  return instance_;
}

CentralDataRepo *CentralDataRepo::getInstance() { return createInstance(); }

void CentralDataRepo::deleteInstance() {
  if (instance_) {
    delete instance_;
  }
  instance_ = NULL;
}
// void operator<<(CentralDataRepo &repo, ::resource::MachineStatus &m) {
//   auto &cluster_status = repo.cluster_status;
//   cluster_status << m;
// }
void operator<<(CentralDataRepo &repo, ::resource::ClusterStatus &c_in) { repo.cluster_status << c_in; }
void operator<<(ClusterStatus &c_db, ::resource::ClusterStatus &c_in) {}
// void operator<<(ClusterStatus &c, ::resource::MachineStatus &m) {
//   auto it = c.machine_status.find(m.id());
//   if (it != c.machine_status.end()) {
//     std::unique_ptr<MachineStatus> p_machine_status = std::make_unique<MachineStatus>(m.id(), m.channel_id());
//     c.machine_status.insert(
//         std::make_pair<uint64_t, std::unique_ptr<MachineStatus>>(m.id(), std::move(p_machine_status)));
//   }
//   // Machine Status is in Sync.
//   MachineStatus *db_m = c.machine_status.find(m.id())->second.get();
//   for (auto &in_proc_itr : m.process_status()) {
//     (*db_m) << in_proc_itr;
//   }
// }
void operator<<(MachineStatus &m, const ::resource::ProcessStatus &p) {
  auto it = m.process_status.find(p.id());
  if (it != m.process_status.end()) {
    std::unique_ptr<ProcessStatus> p_process_status = std::make_unique<ProcessStatus>(p.id(), p.channel_id());
    m.process_status.insert(
        std::make_pair<uint64_t, std::unique_ptr<ProcessStatus>>(p.id(), std::move(p_process_status)));
  }
  // Process Status is in Sync.
  ProcessStatus *db_p = m.process_status.find(p.id())->second.get();
  for (auto &in_thread_itr : p.thread_status()) {
    (*db_p) << in_thread_itr;
  }
}
void operator<<(ProcessStatus &p, const ::resource::ThreadStatus &t) {
  auto it = p.thread_status.find(t.id());
  if (it != p.thread_status.end()) {
    std::unique_ptr<ThreadStatus> p_thread_status = std::make_unique<ThreadStatus>();
    p_thread_status->channel_id = t.channel_id();
    p_thread_status->id = t.id();
    p_thread_status->last_updated_in_ms = t.last_updated_in_ms();
    p_thread_status->last_value = t.last_value();
    p_thread_status->value = t.value();
    p.thread_status.insert(std::make_pair<uint64_t, std::unique_ptr<ThreadStatus>>(t.id(), std::move(p_thread_status)));
  } else {
    it->second->channel_id = t.channel_id();
    it->second->id = t.id();
    it->second->last_updated_in_ms = t.last_updated_in_ms();
    it->second->last_value = t.last_value();
    it->second->value = t.value();
  }
  // Thread Status is in Sync.
}
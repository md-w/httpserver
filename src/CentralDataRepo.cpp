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

void operator<<(CentralDataRepo* repo, ::resource::MachineStatus &m) {
  // Machine Status
  auto& cluster_status = repo->cluster_status;
  auto it = cluster_status.machine_status.find(m.id());
  if (it != cluster_status.machine_status.end()) {
    std::unique_ptr<MachineStatus> p_machine_status = std::make_unique<MachineStatus>(m.id(), m.channel_id());
    cluster_status.machine_status.insert(
        std::make_pair<uint64_t, std::unique_ptr<MachineStatus>>(m.id(), std::move(p_machine_status)));
  }
  // Machine Status is in Sync.

  // Check Process Status.
  auto &db_machine_map = cluster_status.machine_status;
  auto &db_target_machine = cluster_status.machine_status.find(m.id())->second;
  auto &db_proc_map = db_target_machine->process_status;
  for (auto &in_proc_itr : m.process_status()) {
    if (db_proc_map.find(in_proc_itr.id()) == db_proc_map.end()) {
      // Incoming Process Status is NOT in DB; create new
      std::unique_ptr<ProcessStatus> p = std::make_unique<ProcessStatus>(in_proc_itr.id(), in_proc_itr.channel_id());
      db_proc_map.insert(std::make_pair<int64_t, std::unique_ptr<ProcessStatus>>(in_proc_itr.id(), std::move(p)));
    }
  }
  // Process Status is in sync;

  // Check Thread Status, by iterating the process status data
  for (auto &in_proc_itr : m.process_status()) {
    auto &db_process = db_proc_map.find(in_proc_itr.id())->second;
    auto &db_thread_map = db_process->thread_status;
    for (auto& in_thread_itr : in_proc_itr.thread_status()) {
      auto db_thread_status = db_thread_map.find(in_thread_itr.id());
      if (db_thread_status == db_thread_map.end()) {
        // Incoming Thread Status is NOT in DB; create new
        std::unique_ptr<ThreadStatus> p = std::make_unique<ThreadStatus>();
        p->id = in_thread_itr.id();
        p->channel_id = in_thread_itr.channel_id();
        p->value = in_thread_itr.value();
        p->last_value = in_thread_itr.last_value();
        p->last_updated_in_ms = in_thread_itr.last_updated_in_ms();
        db_thread_map.insert(std::make_pair<int64_t, std::unique_ptr<ThreadStatus>>(in_proc_itr.id(), std::move(p)));
      } else {
        db_thread_status->second->id = in_thread_itr.id();
        db_thread_status->second->channel_id = in_thread_itr.channel_id();
        db_thread_status->second->value = in_thread_itr.value();
        db_thread_status->second->last_value = in_thread_itr.last_value();
        db_thread_status->second->last_updated_in_ms = in_thread_itr.last_updated_in_ms();
      }
    }
  }
}
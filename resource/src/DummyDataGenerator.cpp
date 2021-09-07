#include "DummyDataGenerator.hpp"

#include <iostream>
#include <thread>

#include "monitoring_thread.hpp"

DummyDataGenerator::DummyDataGenerator(int id)
    : _is_shutdown_command(false), _is_already_shutting_down(false), _id(id) {}
DummyDataGenerator::~DummyDataGenerator() {}
void DummyDataGenerator::run() {
  std::cout << "Start " << _id << std::endl;
  while (!_is_shutdown_command) {
    MonitoringThread::getInstance()->setStatus(_id);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << "end " << _id << std::endl;
}
void DummyDataGenerator::shutDown() {
  if (_is_already_shutting_down) return;
  _is_already_shutting_down = true;
  _is_shutdown_command = true;
}
#pragma once
#ifndef DummyDataGenerator_H
#define DummyDataGenerator_H
#include <Poco/Runnable.h>

#include <atomic>
class DummyDataGenerator : public Poco::Runnable {
 private:
  std::atomic_bool _is_shutdown_command;
  std::atomic_bool _is_already_shutting_down;
  int _id;

 protected:
  void run();

 public:
  DummyDataGenerator(int id);
  ~DummyDataGenerator();
  void shutDown();
};
#endif
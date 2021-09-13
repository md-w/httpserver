#ifndef MultiPartHandler_H
#define MultiPartHandler_H
#include <Poco/Net/MessageHeader.h>
#include <Poco/Net/PartHandler.h>

#include <iostream>
#include <string>

class MultiPartHandler : public Poco::Net::PartHandler {
 private:
  std::string _basePath;

 public:
  MultiPartHandler(const std::string& basePath);
  void handlePart(const Poco::Net::MessageHeader& header, std::istream& stream);
  ~MultiPartHandler();
};

#endif
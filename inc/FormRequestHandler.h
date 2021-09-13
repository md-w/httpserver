#ifndef FormRequestHandler_H
#define FormRequestHandler_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/SharedPtr.h>

class FormRequestHandler : public Poco::Net::HTTPRequestHandler {
 private:
  std::string getStaticResult();

 public:
  FormRequestHandler();
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
  ~FormRequestHandler();
};

#endif

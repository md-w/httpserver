#ifndef GenericHTTPRequestHandlerFactory_H
#define GenericHTTPRequestHandlerFactory_H

#include <Poco/Event.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/SharedPtr.h>
#include <Poco/Util/JSONConfiguration.h>

class GenericHTTPRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
 public:
  Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request);

  GenericHTTPRequestHandlerFactory();
  ~GenericHTTPRequestHandlerFactory();
};

#endif

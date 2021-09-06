#include "GenericHTTPRequestHandlerFactory.h"

#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>

#include "StatusRequestHandler.h"
#include "MethodNotSupportedRequestHandler.h"
#include "NotFoundRequestHandler.h"
#include "OptionsRequestHandler.h"

GenericHTTPRequestHandlerFactory::GenericHTTPRequestHandlerFactory() {}

GenericHTTPRequestHandlerFactory::~GenericHTTPRequestHandlerFactory() {}

Poco::Net::HTTPRequestHandler *GenericHTTPRequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest &request) {
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
    Poco::URI u(request.getURI());
		std::string pathAndQuery = u.getPathAndQuery();
		std::string statusStr("/api/status");
		if (pathAndQuery.find(statusStr) != std::string::npos) {
      return new StatusRequestHandler();
    } else {
      return new NotFoundRequestHandler();
    }
  } else {
    return new MethodNotSupportedRequestHandler();
  }
}
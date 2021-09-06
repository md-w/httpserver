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
  // double totalElapsedTime = 0;
  // ElapsedTime e(totalElapsedTime, typeid(*this).name());
  // std::stringstream ss_req_cookies;
  // Poco::Net::NameValueCollection cookies;
  // request.getCookies(cookies);
  // ss_req_cookies << "No. of Cookies[" << cookies.size() << "]\n";
  // ss_req_cookies << "Cookies: {\n";
  // int count = 0;
  // for (Poco::Net::NameValueCollection::ConstIterator it = cookies.begin(); it != cookies.end(); ++it, ++count) {
  //   ss_req_cookies << "Idx[" << count << "] : Key[" << it->first << "], Value[" << it->second << "]\n";
  // }
  // ss_req_cookies << "\n}";
  // VTPL_TRACE(typeid(*this).name(),
  //            "Request. Host[{}] Client[{}] Method[{}] ContentType[{}] ContentLength[{}] URI[{}] Cookies[{}]",
  //            request.getHost(), request.clientAddress().toString(), request.getMethod(), request.getContentType(),
  //            request.getContentLength(), request.getURI(), ss_req_cookies.str());
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
    Poco::URI u(request.getURI());
		std::string pathAndQuery = u.getPathAndQuery();
		std::string statusStr("/api/status/");
		if (pathAndQuery.find(statusStr) != std::string::npos) {
      return new StatusRequestHandler();
    } else {
      return new NotFoundRequestHandler();
    }
  } else {
    return new MethodNotSupportedRequestHandler();
  }
}
#include "GenericHTTPRequestHandlerFactory.h"

#include <Poco/StringTokenizer.h>
#include <Poco/URI.h>

#include "FormRequestHandler.h"
#include "MethodNotSupportedRequestHandler.h"
#include "NotFoundRequestHandler.h"
#include "OptionsRequestHandler.h"
#include "StatusRequestHandler.h"

GenericHTTPRequestHandlerFactory::GenericHTTPRequestHandlerFactory() {}

GenericHTTPRequestHandlerFactory::~GenericHTTPRequestHandlerFactory() {}

Poco::Net::HTTPRequestHandler* GenericHTTPRequestHandlerFactory::createRequestHandler(
    const Poco::Net::HTTPServerRequest& request) {
  const std::string& l_in_request_method(request.getMethod());
  const std::string& l_in_content_type(request.getContentType());
  const std::string& l_in_request_uri_str(request.getURI());
  const Poco::URI l_request_url(l_in_request_uri_str);
  const std::string l_request_url_path_query = l_request_url.getPathAndQuery();

  const std::string l_const_status_str("/api/status");
  const std::string l_const_multipart_str("multipart/form-data");
  const std::string l_const_seperator_str("/");

  if (l_in_request_method == Poco::Net::HTTPRequest::HTTP_GET) {
    if (l_request_url_path_query.find(l_const_status_str) != std::string::npos) {
      return new StatusRequestHandler();
    } else {
      return new NotFoundRequestHandler();
    }
  } else if (l_in_request_method == Poco::Net::HTTPRequest::HTTP_POST) {
    return new FormRequestHandler();
    // if (l_in_content_type.find(l_const_multipart_str) != std::string::npos) {
    //   size_t l_path_0_pos = l_request_url_path_query.find(l_const_seperator_str);
    //   if (l_path_0_pos != std::string::npos) {
    //     return new FormRequestHandler();
    //   }
    // } else {
    //   return new MethodNotSupportedRequestHandler();
    // }
  } else {
    return new MethodNotSupportedRequestHandler();
  }
}
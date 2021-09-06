#include "MethodNotSupportedRequestHandler.h"

#include <sstream>
MethodNotSupportedRequestHandler::MethodNotSupportedRequestHandler() {}

MethodNotSupportedRequestHandler::~MethodNotSupportedRequestHandler() {}

void MethodNotSupportedRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                                     Poco::Net::HTTPServerResponse& response) {
  response.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
  response.setReason("HTTP method not allowed");
  // response.setChunkedTransferEncoding(true);
  response.setContentType("text/html");
  // response.setKeepAlive(false);
  std::stringstream ss;
  ss << "HTTP method not allowed";
  std::string sSend = ss.str();
  response.setContentLength(sSend.length());

  response.send() << sSend;
}
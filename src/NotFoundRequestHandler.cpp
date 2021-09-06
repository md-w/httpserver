#include "NotFoundRequestHandler.h"
NotFoundRequestHandler::NotFoundRequestHandler() {}

NotFoundRequestHandler::~NotFoundRequestHandler() {}

void NotFoundRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                           Poco::Net::HTTPServerResponse& response) {
  response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
  response.setReason("Not Found");
  response.setChunkedTransferEncoding(true);
  response.setContentType("text/html");
  std::ostream& ostr = response.send();
  ostr << "Not Found\n";
}
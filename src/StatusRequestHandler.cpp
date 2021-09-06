#include "StatusRequestHandler.h"

#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Path.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>

#include <iostream>

#include "CentralDataRepo.h"

StatusRequestHandler::StatusRequestHandler() {}
void StatusRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
                                         Poco::Net::HTTPServerResponse &response) {
  std::streamsize bytesWritten = 0;
  // response.setChunkedTransferEncoding(true);
  response.setContentType("application/json");

  std::string origin = "*";
  try {
    origin = request.get("Origin");
  } catch (const std::exception &e) {}

  response.add("Access-Control-Allow-Origin", origin);
  response.add("Access-Control-Allow-Credentials", "true");
  response.add("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  response.add("Access-Control-Allow-Headers",
               "Accept,Authorization, "
               "Keep-Alive,X-CustomHeader,Origin,DNT,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,"
               "Content-Type,Range");
  response.add("Access-Control-Expose-Headers", "Content-Length,Content-Range");

  // response.setContentLength(fileSize);
  std::ostream &ostr = response.send();
  *(CentralDataRepo::getInstance()) >> ostr;
  if (ostr.good()) {
    ostr.flush();
  }
}

StatusRequestHandler::~StatusRequestHandler() {}

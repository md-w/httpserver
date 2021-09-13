#include "FormRequestHandler.h"

#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/InflatingStream.h>
#include <Poco/JSON/Object.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Path.h>
#include <Poco/StreamCopier.h>
#include <Poco/Thread.h>
#include <Poco/URI.h>
#include <Poco/UUIDGenerator.h>
#include <logging.h>

#include <fstream>
#include <iostream>

#include "MultiPartHandler.h"

FormRequestHandler::FormRequestHandler() { RAY_LOG(INFO) << "Constructor "; }

FormRequestHandler::~FormRequestHandler() { RAY_LOG(INFO) << "Destructor "; }

void FormRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) {
  // RAY_LOG(INFO) << fmt::format("{} {} {}", request.getURI(), request.getContentType(),
  //                              request.getChunkedTransferEncoding());
  // for (Poco::Net::NameValueCollection::ConstIterator it = request.begin(); it != request.end(); ++it) {
  //   RAY_LOG(INFO) << fmt::format("Named iterator {} {}", it->first, it->second);
  // }
  Poco::URI uri(request.getURI());
  std::vector<std::string> segments;
  uri.getPathSegments(segments);

  std::stringstream sb;

  sb << "Data";

  Poco::File f(sb.str());
  if (!f.exists()) f.createDirectories();

  sb << "/";
  sb << segments[1];
  sb << "_";
  sb << segments[2];
  sb << ".png";

  Poco::FileStream ostr(sb.str());
  Poco::StreamCopier::copyStreamUnbuffered(request.stream(), ostr);

  std::string origin = "*";
  if (request.has("Origin")) {
    try {
      origin = request.get("Origin");
    } catch (const std::exception &ex) {
      RAY_LOG(ERROR) << fmt::format("OptionsRequestHandler Origin error [{}]", ex.what());
    }
  }
  response.add("Access-Control-Allow-Origin", origin);
  response.add("Access-Control-Allow-Credentials", "true");
  response.add("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  response.add("Access-Control-Allow-Headers",
               "Accept,Authorization, "
               "Keep-Alive,X-CustomHeader,Origin,DNT,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,"
               "Content-Type,Range");
  response.add("Access-Control-Expose-Headers", "Content-Length,Content-Range");

  // response.setChunkedTransferEncoding(true);
  response.setContentType("application/json");
  response.setKeepAlive(request.getKeepAlive());
  std::stringstream ss;
  ss << "{\"items\":[{\"Location\":\"";
  // ss << partHandler.relativePathWithFileName(); // TODO
  ss << "\"}]}";
  std::string s = ss.str();

  RAY_LOG(INFO) << fmt::format("File Location [{}]", s);
  response.setContentLength(s.size());
  response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
  response.send() << s;
}
/*{
  std::string fileUploadPathStr = "";
  Poco::Int64 fileSizeKB = 0;

  int tid = Poco::Thread::currentTid();
  std::string host = request.clientAddress().host().toString();
  std::string path = request.getURI();
  // if (path.at(0) == '/' || path.at(0) == '\\') {
  //   path = path.substr(1, path.length());
  // }
  RAY_LOG(INFO) << fmt::format("Request from [{}], url [{}], served by thread [{}]", host, path, tid);
  Poco::Net::HTMLForm form;
  std::vector<std::string> mutipartContent;
  Poco::Net::HTTPResponse::HTTPStatus returnStatus = Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR;
  MultiPartHandler partHandler(path);
  if (Poco::toLower(request.getMethod()) == "post") {
    try {
      if (request.get("Content-Encoding", "") == "gzip") {
        Poco::InflatingInputStream inflater(request.stream(), Poco::InflatingStreamBuf::STREAM_GZIP);
        form.load(request, inflater, partHandler);
        returnStatus = Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED;
      } else {
        form.load(request, request.stream(), partHandler);
        returnStatus = Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED;
      }
      // if (partHandler.isFile()) {
      //   Poco::DateTime dt;
      //   dt.makeLocal(Poco::LocalDateTime().tzd());
      //   mutipartContent.push_back(Poco::format("<h2>Upload</h2><p>%s", partHandler.name()));
      //   mutipartContent.push_back(Poco::format("<br>File Name: %s", partHandler.fileName()));
      //   mutipartContent.push_back(Poco::format("<br>Type: %s", partHandler.contentType()));
      //   mutipartContent.push_back(Poco::format("<br>Size:%d<br></p>", partHandler.length()));
      // }
    } catch (const std::exception &ex) {
      RAY_LOG(ERROR) << fmt::format("Exception [{}]", ex.what());
    }
  } else {
    form.load(request);
  }
  if (returnStatus == Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED) {
    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

    std::string origin = "*";
    if (request.has("Origin")) {
      try {
        origin = request.get("Origin");
      } catch (const std::exception &ex) {
        RAY_LOG(ERROR) << fmt::format("OptionsRequestHandler Origin error [{}]", ex.what());
      }
    }

    response.add("Access-Control-Allow-Origin", origin);
    response.add("Access-Control-Allow-Credentials", "true");
    response.add("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    response.add("Access-Control-Allow-Headers",
                 "Accept,Authorization, "
                 "Keep-Alive,X-CustomHeader,Origin,DNT,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,"
                 "Content-Type,Range");
    response.add("Access-Control-Expose-Headers", "Content-Length,Content-Range");

    // response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");

    RAY_LOG(INFO) << "request.getKeepAlive()::: " << request.getKeepAlive();

    response.setKeepAlive(request.getKeepAlive());
    std::stringstream ss;
    ss << "{\"items\":[{\"Location\":\"";
    // ss << partHandler.relativePathWithFileName(); // TODO
    ss << "\"}]}";
    std::string s = ss.str();

    RAY_LOG(INFO) << fmt::format("File Location [{}]", s);
    RAY_LOG(INFO) << fmt::format("Response Status [{}]", returnStatus);

    response.setContentLength(s.size());
    response.setStatus(returnStatus);
    response.send() << s;
  } else {
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");
    // response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED);
    std::ostream &ostr = response.send();

    ostr << this->getStaticResult();

    ostr << "<h2>Request</h2><p>\n";
    ostr << Poco::format(std::string("Method: %s <br>\n"), request.getMethod());
    ostr << Poco::format(std::string("URI: %s <br>\n"), request.getURI());
    auto it = request.begin();
    for (; it != request.end(); ++it) {
      ostr << it->first << ": " << it->second << "<br>\n";
    }
    ostr << "</p>";

    if (!form.empty()) {
      ostr << "<h2>Form</h2><p>\n";
      it = form.begin();
      for (; it != form.end(); ++it) {
        ostr << it->first << ": " << it->second << "<br>\n";
      }
      ostr << "</p>";
    }

    if (!mutipartContent.empty()) {
      ostr << Poco::cat(std::string(""), mutipartContent.begin(), mutipartContent.end());
    }
    ostr << "</body>\n";
  }

  // VTPL_FWORK_TRACE("TotalDelay[{}]", nowE.epochMicroseconds() / 1000 - startTime);
}
*/
inline std::string FormRequestHandler::getStaticResult() {
  return "<html>\n"
         "<head>\n"
         "<title>POCO Form Server Sample</title>\n"
         "</head>\n"
         "<body>\n"
         "<h1>POCO Form Server Sample</h1>\n"
         "<h2>GET Form</h2>\n"
         "<form method=\"GET\" action=\"/form\">\n"
         "<input type=\"text\" name=\"text\" size=\"31\">\n"
         "<input type=\"submit\" value=\"GET\">\n"
         "</form>\n"
         "<h2>POST Form</h2>\n"
         "<form method=\"POST\" action=\"/form\">\n"
         "<input type=\"text\" name=\"text\" size=\"31\">\n"
         "<input type=\"submit\" value=\"POST\">\n"
         "</form>\n"
         "<h2>File Upload</h2>\n"
         "<form method=\"POST\" action=\"/form\" enctype=\"multipart/form-data\">\n"
         "<input type=\"file\" name=\"file\" size=\"31\"> \n"
         "<input type=\"submit\" value=\"Upload\">\n"
         "</form>\n";
}
#include "MultiPartHandler.h"

#include <Poco/CountingStream.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/NullStream.h>
#include <Poco/Path.h>
#include <Poco/StreamCopier.h>
#include <logging.h>

#include <sstream>

MultiPartHandler::MultiPartHandler(const std::string &basePath) : _basePath(basePath) {}
MultiPartHandler::~MultiPartHandler() {}
void MultiPartHandler::handlePart(const Poco::Net::MessageHeader &header, std::istream &stream) {
  std::string l_name("");
  std::string l_file_name("");
  if (header.has("Content-Disposition")) {
    std::string disp;
    Poco::Net::NameValueCollection params;
    Poco::Net::MessageHeader::splitParameters(header["Content-Disposition"], disp, params);
    l_name = params.get("name", "(unnamed)");
    l_file_name = params.get("filename", "(unnamed)");
  }
  std::size_t bufferSize = 10000000;  // 10 MB
  Poco::CountingInputStream istr(stream);
  if (l_file_name.length() > 0) {
    std::stringstream sb;

    sb << "Data";
    sb << _basePath;

    Poco::File f(sb.str());
    if (!f.exists()) f.createDirectories();

    sb << "/";
    sb << l_file_name;

    Poco::FileStream ostr(sb.str());
    Poco::StreamCopier::copyStream(istr, ostr, bufferSize);
  } else {
    Poco::NullOutputStream ostr;
    Poco::StreamCopier::copyStream(istr, ostr, bufferSize);
  }
}
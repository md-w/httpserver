#include <Poco/ErrorHandler.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <logging.h>

#include "CentralDataRepo.h"
#include "GenericHTTPRequestHandlerFactory.h"
#include "LogReceiverServer.h"
#include "LogReceiverTCPServerConnectionFactory.h"

class ServerErrorHandler : public Poco::ErrorHandler {
 public:
  void exception(const Poco::Exception &exc) { RAY_LOG(FATAL) << "Poco::Exception " << exc.what(); }
  void exception(const std::exception &exc) { RAY_LOG(FATAL) << "std::exception " << exc.what(); }
  void exception() { RAY_LOG(FATAL) << "unknown exception "; }
};

class EntryPoint : public Poco::Util::ServerApplication {
 public:
  EntryPoint() {}
  ~EntryPoint() {}
  void initialize(Application &self) {
    loadConfiguration();  // load default configuration files, if present
    Poco::ErrorHandler::set(&_serverErrorHandler);
    ServerApplication::initialize(self);
  }

  void uninitialize() { ServerApplication::uninitialize(); }

  void defineOptions(Poco::Util::OptionSet &options) {
    ServerApplication::defineOptions(options);
    options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments")
                          .required(false)
                          .repeatable(false));
  }

  void handleOption(const std::string &name, const std::string &value) { ServerApplication::handleOption(name, value); }

  void displayHelp() {
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A web server that shows how to work with HTML forms.");
    helpFormatter.format(std::cout);
  }
  std::string get_session_folder() {
    Poco::Path base_path;
    if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
      base_path.assign(config().getString("application.dataDir", config().getString("application.dir", "./")));
      base_path.pushDirectory(config().getString("application.baseName", "ojana"));
    } else {
      base_path.assign(config().getString("application.dir", "./"));
    }
    base_path.pushDirectory("session");
    std::cout << "Session folder is at: " << base_path.toString() << std::endl;
    return base_path.toString();
  }

  int main(const ArgVec &args) {
    std::string name_of_app = config().getString("application.baseName");
    ::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, get_session_folder());
    RAY_LOG(INFO) << "Started: " << name_of_app;
    CentralDataRepo::getInstance();
    {
      Poco::UInt16 tcpServerPort = 23000;
      Poco::UInt16 httpServerPort = 9983;
      Poco::Net::ServerSocket svsTcpServer(tcpServerPort);
      svsTcpServer.setReuseAddress(true);
      svsTcpServer.setReusePort(false);
      Poco::Net::ServerSocket svsHttpServer(httpServerPort);
      svsHttpServer.setReuseAddress(true);
      svsHttpServer.setReusePort(false);

      Poco::Net::TCPServerParams::Ptr pTcpServerParams = new Poco::Net::TCPServerParams();
      pTcpServerParams->setMaxQueued(100);
      pTcpServerParams->setMaxThreads(100);
      LogReceiverTCPServerConnectionFactory *pLogReceiverTcpServerConnectionFactory =
          new LogReceiverTCPServerConnectionFactory();
      std::unique_ptr<Poco::Net::TCPServer> pTcpServer(
          new Poco::Net::TCPServer(pLogReceiverTcpServerConnectionFactory, svsTcpServer, pTcpServerParams));

      Poco::Net::HTTPServerParams::Ptr pHttpServerParams = new Poco::Net::HTTPServerParams;
      pHttpServerParams->setMaxQueued(100);
      pHttpServerParams->setMaxThreads(100);
      std::unique_ptr<Poco::Net::HTTPServer> pHttpServer(
          new Poco::Net::HTTPServer(new GenericHTTPRequestHandlerFactory(), svsHttpServer, pHttpServerParams));

      pTcpServer->start();
      pHttpServer->start();

      RAY_LOG(INFO) << "TCP server listening on port " << tcpServerPort << " HTTP server listening on port "
                    << httpServerPort;

      waitForTerminationRequest();
      RAY_LOG(INFO) << "Shutdown request received.";
      pLogReceiverTcpServerConnectionFactory->shutDown();
      pTcpServer->stop();
      pHttpServer->stopAll();
      // Poco::ThreadPool::defaultPool().stopAll();
      Poco::ThreadPool::defaultPool().joinAll();
    }
    CentralDataRepo::deleteInstance();
    RAY_LOG(INFO) << "Stopped: " << name_of_app;
    return Application::EXIT_OK;
  }

 private:
  ServerErrorHandler _serverErrorHandler;
};

POCO_SERVER_MAIN(EntryPoint);
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <logging.h>

#include "monitoring_thread.hpp"

static std::string get_session_folder() { return "./session/"; }
class EntryPoint : public Poco::Util::ServerApplication {
 public:
  EntryPoint() : _shutDown(false) {
    ::ray::RayLog::StartRayLog(_name_of_app, ::ray::RayLogLevel::DEBUG, get_session_folder());
    RAY_LOG(INFO) << "Started: " << _name_of_app;
  }
  ~EntryPoint() {
    RAY_LOG(INFO) << "Stopped: " << _name_of_app;
    ::ray::RayLog::ShutDownRayLog();
  }
  void initialize(Application &self) {
    loadConfiguration();  // load default configuration files, if present
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

  int main(const ArgVec &args) {
    Poco::UInt16 port = 23000;
    MonitoringThread::getInstance();
    waitForTerminationRequest();
    RAY_LOG(INFO) << "Shutdown request received.";
    MonitoringThread::deleteInstance();
    Poco::ThreadPool::defaultPool().joinAll();
    return Application::EXIT_OK;
  }

 private:
  bool _shutDown;
  const std::string _name_of_app = "HttpServer";
};

POCO_SERVER_MAIN(EntryPoint);
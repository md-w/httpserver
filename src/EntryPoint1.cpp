#include <iostream>
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include "HTTPSFileServer.h"
#include "HTTPFileServer.h"
#include "Logger.h"

class EntryPoint : public Poco::Util::ServerApplication
{
public:
    EntryPoint()
		: _shutDown(false)
    {
		vtpl::Logger::Init(typeid(*this).name());
        ServerApplication::addSubsystem(new HTTPFileServer(100, 16));
        // ServerApplication::addSubsystem(new HTTPSFileServer(100, 16));
    }
    ~EntryPoint()
    {
        VTPL_TRACE(typeid(*this).name(), "Dtor called");
    }

private:
    bool _shutDown;

public:
    void initialize(Application &self)
    {
        loadConfiguration(); // load default configuration files, if present
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(Poco::Util::OptionSet &options)
    {
        ServerApplication::defineOptions(options);
        options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments").required(false).repeatable(false));
    }

    void handleOption(const std::string &name, const std::string &value)
    {
        ServerApplication::handleOption(name, value);
    }

    void displayHelp()
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A web server that shows how to work with HTML forms.");
        helpFormatter.format(std::cout);
    }

    int main(const ArgVec &args)
    {
        waitForTerminationRequest();
        VTPL_WARN(typeid(*this).name(), "Shutdown request received.");
        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(EntryPoint);
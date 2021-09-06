#ifndef HTTPServer_H
#define HTTPServer_H

#include <Poco/Runnable.h>
#include <Poco/Util/Subsystem.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/AutoPtr.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/ErrorHandler.h>

class HTTPServerErrorHandler : public Poco::ErrorHandler
{
public:
	HTTPServerErrorHandler();
	void exception(const Poco::Exception &exc);
	void exception(const std::exception &exc);
	void exception();
	void log(const std::string &message);

private:
};

class HTTPServer : public Poco::Util::Subsystem
{
protected:
	inline const char *name() const
	{
		return "HTTPServer";
	}
	void initialize(Poco::Util::Application &app);
	void reinitialize(Poco::Util::Application &app);
	void uninitialize();

public:
	void cancelInit();
	HTTPServer(int maxRequestQueueSize, int maxThreads);
	~HTTPServer();

private:
	Poco::Net::HTTPServer *pSrv;
	bool _cancelInit;
	int _portNumber;
	int _maxRequestQueueSize;
	int _maxThreads;
	HTTPServerErrorHandler _errorHandler;
};
#endif
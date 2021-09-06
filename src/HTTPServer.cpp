#include "HTTPServer.h"
#include "GenericHTTPRequestHandlerFactory.h"

HTTPServerErrorHandler::HTTPServerErrorHandler() {
	
}

void HTTPServerErrorHandler::exception(const Poco::Exception &exc)
{
	log(exc.displayText());
}

void HTTPServerErrorHandler::exception(const std::exception &exc)
{
	log(exc.what());
}

void HTTPServerErrorHandler::exception()
{
	log("unknown exception");
}

void HTTPServerErrorHandler::log(const std::string &message)
{
}

HTTPServer::HTTPServer(int maxRequestQueueSize, int maxThreads)
	: _cancelInit(false),
	  _portNumber(9983),
	  _maxRequestQueueSize(maxRequestQueueSize),
	  _maxThreads(maxThreads)
{
	if (maxRequestQueueSize < 100)
		_maxRequestQueueSize = 100;
	if (maxThreads < 16)
		_maxThreads = 16;
}

HTTPServer::~HTTPServer()
{
}

void HTTPServer::reinitialize(Poco::Util::Application &app)
{
}

void HTTPServer::cancelInit()
{
	_cancelInit = true;
}
void HTTPServer::initialize(Poco::Util::Application &app)
{
	//https://github.com/aurusov/poco-http-server.git
	Poco::ErrorHandler::set(&_errorHandler);
	if (!app.config().has("HTTPServer.port"))
		app.config().setInt("HTTPServer.port", _portNumber);
	_portNumber = (unsigned short)app.config().getInt("HTTPServer.port", _portNumber);
	Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams;
	pParams->setMaxQueued(_maxRequestQueueSize);
	pParams->setMaxThreads(_maxThreads);
	Poco::Net::ServerSocket svs(_portNumber);
	svs.setReuseAddress(true);
	svs.setReusePort(false);
	pSrv = new Poco::Net::HTTPServer(new GenericHTTPRequestHandlerFactory(), svs, pParams);
	pSrv->start();
}
void HTTPServer::uninitialize()
{
	pSrv->stopAll(true);
	// Wait for threads to complete
	Poco::ThreadPool::defaultPool().joinAll();
	delete pSrv;
}
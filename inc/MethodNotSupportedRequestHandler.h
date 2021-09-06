#ifndef MethodNotSupportedRequestHandler_H
#define MethodNotSupportedRequestHandler_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/SharedPtr.h>

class MethodNotSupportedRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
public:
	MethodNotSupportedRequestHandler();
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	~MethodNotSupportedRequestHandler();
};

#endif // !MethodNotSupportedRequestHandler_H

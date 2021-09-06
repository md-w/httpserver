#ifndef OptionsRequestHandler_H
#define OptionsRequestHandler_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/SharedPtr.h>

class OptionsRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
public:
	OptionsRequestHandler();
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	~OptionsRequestHandler();
};

#endif // !OptionsRequestHandler_H

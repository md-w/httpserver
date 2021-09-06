#ifndef StatusRequestHandler_H
#define StatusRequestHandler_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/SharedPtr.h>
#include <Poco/Event.h>

class StatusRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	StatusRequestHandler();
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	~StatusRequestHandler();
private:
};

#endif // !StatusRequestHandler_H

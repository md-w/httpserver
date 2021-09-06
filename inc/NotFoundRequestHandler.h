#ifndef NotFoundRequestHandler_H
#define NotFoundRequestHandler_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/SharedPtr.h>

class NotFoundRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
public:
	NotFoundRequestHandler();
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
	~NotFoundRequestHandler();
};

#endif // !NotFoundRequestHandler_H

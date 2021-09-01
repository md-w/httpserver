// #include "logging.h"
#include <Poco/Foundation.h>
#include "logging.h"

int main(int argc, char const *argv[])
{
    ::ray::RayLog::StartRayLog("http", ::ray::RayLogLevel::DEBUG, "./log/");
    RAY_CHECK(1) << "Hello world";
    RAY_LOG(INFO) << "monotosh";
    /* code */
    ::ray::RayLog::ShutDownRayLog();
    return 0;
}

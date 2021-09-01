#include "logging.h"

int main(int argc, char const *argv[])
{
    ::ray::RayLog::StartRayLog("aa", ::ray::RayLogLevel::DEBUG, "");
    RAY_CHECK(1) << "Hello world";
    /* code */
    ::ray::RayLog::ShutDownRayLog();
    return 0;
}

#include "Utils.h"
#include "DeviceUtils.h"
#include <ctime>
#include <windows.h>

std::string getDeviceGUID() {
    return GetDeviceMachineGUID();
}

std::string getCurrentDate() {
    std::time_t t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

std::time_t getCurrentTimestamp() {
    return std::time(nullptr);
}

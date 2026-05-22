#pragma once
#include <string>
#include <ctime>

std::string getDeviceGUID();  // Renamed from getComputerName()
std::string getCurrentDate();
std::time_t getCurrentTimestamp();

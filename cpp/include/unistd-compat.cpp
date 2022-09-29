#include "unistd-compat.h"

#ifdef _WIN32
#include <windows.h>

unsigned long long getCurrentTimeMs() {
	__int64 wintime;
	GetSystemTimeAsFileTime((_FILETIME *)&wintime);
	wintime -= 116444736000000000i64; // 1jan1601 to 1jan1970
	return wintime / 10000i64;
}

#else // not _WIN32

#include <time.h>
#include <chrono>

unsigned long long getCurrentTimeMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

#endif // _WIN32

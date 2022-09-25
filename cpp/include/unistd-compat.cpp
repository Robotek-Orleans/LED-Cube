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

unsigned long long getCurrentTimeMs() {
	struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	return start.tv_sec * 1000 + start.tv_nsec / 1000000;
}

#endif // _WIN32

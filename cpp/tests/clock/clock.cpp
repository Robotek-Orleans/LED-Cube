#include <chrono>
#include <time.h>
#include <iostream>
#include <thread>
#include <stdio.h>

// Output just a table or everything
#define OUTPUT_TABLE 1

#if defined(__i386__) || defined(__x86_64__) || defined(__powerpc__)
#define USE__builtin_ia32_rdtsc true
#else
#define USE__builtin_ia32_rdtsc false
#endif

/**
 * Which method is the fastest on:
 * - Windows
 * - Linux WSL
 * - Linux VM
 * - Raspberry Pi
 */

void usleep(unsigned int useconds) {
	std::this_thread::sleep_for(std::chrono::microseconds(useconds));
}

// Clock definition
#define MS_CLOCK (CLOCKS_PER_SEC / 1000)

long long getChronoSteadyMS() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

long long getChronoSystemMS() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long long getChronoSteadyNoCastMS() {
	return std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
}

long long getChronoSystemNoCastMS() {
#if _WIN32
	return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
#else
	return std::chrono::system_clock::now().time_since_epoch().count() / 1000000;
#endif
}

int getCpuFrequencykHz() {
#if USE__builtin_ia32_rdtsc
	long long rdtsc_ticks = __builtin_ia32_rdtsc();
	long long local_time = getChronoSteadyMS();
	int freq = rdtsc_ticks / local_time;
#else
	int freq = 1;
#endif
	printf("CPU Frequency: %d kHz\r\n", freq);
	return freq;
}
int cpuFrequencykHz = getCpuFrequencykHz(); // At 0.1 %

#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void) {
	unsigned long long int x;
	__asm__ volatile(".byte 0x0f, 0x31"
					 : "=A"(x));
	return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void) {
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc"
						 : "=a"(lo), "=d"(hi));
	return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}
#elif defined(__powerpc__)
static __inline__ unsigned long long rdtsc(void) {
	unsigned long long int result = 0;
	unsigned long int upper, lower, tmp;
	__asm__ volatile(
		"0:                  \n"
		"\tmftbu   %0           \n"
		"\tmftb    %1           \n"
		"\tmftbu   %2           \n"
		"\tcmpw    %2,%0        \n"
		"\tbne     0b         \n"
		: "=r"(upper), "=r"(lower), "=r"(tmp));
	result = upper;
	result = result << 32;
	result = result | lower;

	return (result);
}
#else
#warning "No rdtsc() implementation for this architecture (or already defined)"
#endif

long long getRdtscMS() {
	return rdtsc() / cpuFrequencykHz;
}

long long get__rdtscMS() {
#if _WIN32 // __rdtsc
	return __rdtsc() / cpuFrequencykHz;
#else
	return 0;
#endif
}

long long get__builtin_rdtscMS() {
#if USE__builtin_ia32_rdtsc
	return __builtin_ia32_rdtsc() / cpuFrequencykHz;
#else
	return 0;
#endif
}

#if defined(_WIN32)
#include <windows.h>
int clock_gettime(int, struct timespec *spec) // C-file part
{
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME *)&wintime);
	wintime -= 116444736000000000i64;			 // 1jan1601 to 1jan1970
	spec->tv_sec = wintime / 10000000i64;		 // seconds
	spec->tv_nsec = wintime % 10000000i64 * 100; // nano-seconds
	return 0;
}
#endif

long long getclock_gettimeMS() {
#ifdef _WIN32
#define CLOCK_REALTIME 0
	struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	return start.tv_sec * 1000 + start.tv_nsec / 1000000;
#else
	struct timespec start;
	clock_gettime(CLOCK_REALTIME, &start);
	return start.tv_sec * 1000 + start.tv_nsec / 1000000;
#endif
}

long long getSystemTimeMS() {
#ifdef _WIN32
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME *)&wintime);
	wintime -= 116444736000000000i64; // 1jan1601 to 1jan1970
	return wintime / 10000i64;
#else
	return 0;
#endif
}

// Check if the clock can keep track of the time
long long runDifferenceTest(long long (*getTime)()) {
	long long start = getTime();
	usleep(100 * 1000);
	long long end = getTime();
	return end - start;
}
// Check the time it takes to get the time with the function
long long runSpeedTest(long long (*getTime)()) {
	long long start = getChronoSteadyMS();
	long long t = 0;
	long long t2 = 0;
	for (int i = 0; i < 10000000; i++) {
		// Run the function
		t = getTime();
		t2 += t;
	}
	long long end = getChronoSteadyMS();
#if !OUTPUT_TABLE
	std::cout << "        runSpeedTest: " << t << " (" << t2 << "), duration:" << (end - start) << " ms" << std::endl;
#endif
	return end - start;
}

void runTests(std::string functionName, long long (*getTime)()) {
#if !OUTPUT_TABLE
	std::cout << std::endl;
	std::cout << "Testing " << functionName << std::endl;
#endif
	long long duration = runDifferenceTest(getTime);
	long long speedTest = runSpeedTest(getTime);
#if !OUTPUT_TABLE
	std::cout << " - Difference of 100 ms: " << duration << " ticks" << std::endl;
	std::cout << " - Speed: " << speedTest << " ms" << std::endl;
	std::cout << std::endl;
#else
	std::cout << "| " << functionName << " | " << duration << " | " << speedTest << " | " << getTime() << " |" << std::endl;
#endif
}

void testClock() {
// clock() is special because it returns a clock_t
#if !OUTPUT_TABLE
	std::cout << std::endl;
	std::cout << "Testing clock()" << std::endl;
#endif
	long long difference;

	{
		clock_t start = clock();
		usleep(100 * 1000);
		clock_t end = clock();
		difference = (end - start);
	}

	long long speedTest;
	{
		long long start = getChronoSteadyMS();
		clock_t t = 0;
		clock_t t2 = 0;
		for (int i = 0; i < 10000000; i++) {
			// Run the function
			t = clock();
			t2 += t;
		}
		long long end = getChronoSteadyMS();
#if !OUTPUT_TABLE
		std::cout << "        runSpeedTest: " << t << " (" << t2 << "), duration: " << (end - start) << " ms" << std::endl;
#endif
		speedTest = end - start;
	}

#if !OUTPUT_TABLE
	std::cout << " - Difference of 100 ms: " << difference << " ticks" << std::endl;
	std::cout << " - Speed: " << speedTest << " ms" << std::endl;
	std::cout << std::endl;
#else
	std::cout << "| clock() | " << difference << " | " << speedTest << " | " << clock() << " |" << std::endl;
#endif
}

int main(int argc, char **argv) {
#if OUTPUT_TABLE
	std::cout << "| Function | Difference (100 ms) | Time (ms) | Example |" << std::endl;
	std::cout << "|----------|---------------------|-----------|---------|" << std::endl;
#endif
	testClock();
	runTests("std::chrono::steady_clock", getChronoSteadyMS);
	runTests("std::chrono::system_clock", getChronoSystemMS);
	runTests("std::chrono::steady_clock no cast", getChronoSteadyNoCastMS);
	runTests("std::chrono::system_clock no cast", getChronoSystemNoCastMS);
	runTests("rdtsc()", getRdtscMS);
	runTests("__rdtsc()", get__rdtscMS);
	runTests("__builtin_ia32_rdtsc()", get__builtin_rdtscMS);
	runTests("clock_gettime()", getclock_gettimeMS);
	runTests("GetSystemTimeAsFileTime()", getSystemTimeMS);
}

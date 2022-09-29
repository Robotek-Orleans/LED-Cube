#include "HardwareMonitor.h"
#include <time.h>
#include <chrono>
#include <string>
#include <stdexcept>
#include <stdio.h>

#if defined(__linux__) || defined(UNIX) // Linux
#include <fstream>

// Mem Usage
double mem_usage() {
	unsigned int memTotal, memFree, buffers, cached;
	char id[18], kb[2];

	std::ifstream file("/proc/meminfo");

	file >> id >> memTotal >> kb >> id >> memFree >> kb >> id >> buffers >> kb >> id >> buffers >> kb >> id >> cached >> kb;

	file.close();

	return (int)((memTotal - memFree) * 100.0 / memTotal * 10) / 10.0;
}

// CPU Usage
double cpu_usage() {
	try {
		std::ifstream file("/proc/loadavg");

		char line[5];
		file.getline(line, 5);

		float prct = std::stof(line) * 100 / 4;
		return prct;
	} catch (...) {
		return -1;
	}
}

// Temperature
double temperature() {
	try {
		std::string temp_str;
		std::ifstream ifile("/sys/class/thermal/thermal_zone0/temp");
		ifile >> temp_str;
		ifile.close();
		int temp_milli = std::stoi(temp_str);
		return temp_milli / 1000.0 + 273.15;
	} catch (...) {
		return -1;
	}
}

// Network Usage => RX per ct
int net_usage() {
	std::string packet_rx = "0", packet_tx = "0";

	std::ifstream ifiler("/sys/class/net/laser0/statistics/rx_packets");
	ifiler >> packet_rx;
	ifiler.close();
	std::ifstream ifilet("/sys/class/net/laser0/statistics/tx_packets");
	ifilet >> packet_tx;
	ifilet.close();

	if ((std::stoi(packet_rx) + std::stoi(packet_tx)) == 0) return 0;
	return std::stoi(packet_rx) * 100 / (std::stoi(packet_rx) + std::stoi(packet_tx));
}

#elif _WIN32 // Windows

#include <windows.h>

// Mem Usage
double mem_usage() {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return statex.dwMemoryLoad;
}

std::string exec(std::string command) {
	char buffer[128];
	std::string result = "";

	// Open pipe to file
	FILE *pipe = _popen(command.c_str(), "r");
	if (!pipe) {
		return ""; // popen failed!
	}

	// read till end of process:
	while (!feof(pipe)) {

		// use buffer to read and add to result
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}

	_pclose(pipe);
	return result;
}

double cpu_usage() {
	std::string result = exec("wmic cpu get loadpercentage");
	result.erase(0, 18);
	char *end = &result[result.size() - 1];
	return std::strtod(result.c_str(), &end);
}

double temperature() {
	std::string result = exec("powershell.exe -command \"$data = Get-WMIObject -Query \\\"SELECT Temperature FROM Win32_PerfFormattedData_Counters_ThermalZoneInformation\\\" -Namespace \\\"root/CIMV2\\\"; @($data)[0].Temperature\"");
	char *end = &result[result.size() - 1];
	return std::strtod(result.c_str(), &end);
}

// Network Usage => RX per ct
int net_usage() {
	return -1;
}

#else // Not Linux nor Windows
#error "Unsupported platform"

// Mem Usage
double mem_usage() {
	return -1;
}

// CPU Usage
double cpu_usage() {
	return -1;
}

// Temperature
double temperature() {
	return -1;
}

// Network Usage => RX per ct
int net_usage() {
	return -1;
}

#endif

long long boot_time_result = 0;
long long boot_time() {
	if (boot_time_result == 0) {
		long long uptime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		boot_time_result = now - uptime;
	}
	return boot_time_result;
}
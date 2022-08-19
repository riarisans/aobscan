#ifndef GUARD
#define GUARD

#include <vector>
#include <string>
#include <Windows.h>

class Scanner {
public:
	static std::vector<DWORD> scan(const HANDLE& handle, const std::string& pattern);
private:
	static std::vector<DWORD> scan(const HANDLE& handle, const char* pattern, const char* mask, int length);
	static std::vector<DWORD> find_pattern(const char* buffer, int buffer_length, const char* pattern, int pattern_length, const char* mask);
};

#endif
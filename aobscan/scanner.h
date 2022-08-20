#ifndef GUARD
#define GUARD

#include <vector>
#include <string>
#include <Windows.h>

class Scanner {
public:
	static std::vector<DWORD> scan(const HANDLE& handle, const std::string& pattern);
private:
	static std::vector<DWORD> old_scan(const HANDLE& handle, const char* pattern, const char* mask, int length);
	static std::vector<DWORD> new_scan(const HANDLE& handle, const char* pattern, const char* mask, int length);
};

#endif
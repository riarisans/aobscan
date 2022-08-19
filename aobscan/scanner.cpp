#include "scanner.h"

std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> result;
	std::string temp;
	for (int i = 0; !(str[i] == 13 || str[i] == '\n' || str[i] == 0); i++)
	{
		if (str[i] == delimiter)
		{
			result.push_back(temp);
			temp.clear();

			continue;
		}
		temp.push_back(str[i]);
	}
	result.push_back(temp);
	return result;
}

std::vector<DWORD> Scanner::scan(const HANDLE& handle, const std::string& pattern) {
	std::vector<char> mask;
	std::vector<char> pattern_char;
	std::vector<std::string> parsed_pattern = split(pattern, ' ');
	for (int i = 0; i < parsed_pattern.size(); ++i) {
		if (parsed_pattern[i].find('?') != std::string::npos) {
			mask.push_back('x');
			pattern_char.push_back('x');
		}
		else { 
			mask.push_back('.');
			pattern_char.push_back((char)strtol(parsed_pattern[i].c_str(), NULL, 16));
		}
	}
	return kmp_scan(handle, pattern_char.data(), mask.data(), mask.size());
}

std::vector<DWORD> Scanner::scan(const HANDLE& handle, const char* pattern, const char* mask, int length) {
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T lpNumberOfBytesRead;
	char* buffer;
	std::vector<DWORD> result;

	for (SIZE_T base_addr = 0x0; VirtualQueryEx(handle, (LPCVOID)base_addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); base_addr += mbi.RegionSize) {
		if (mbi.Protect != PAGE_READWRITE || mbi.Type != MEM_PRIVATE) continue;
		buffer = new char[mbi.RegionSize];
		ReadProcessMemory(handle, mbi.BaseAddress, buffer, mbi.RegionSize, &lpNumberOfBytesRead);
		for (unsigned long i = 0; i < lpNumberOfBytesRead - length; ++i) {
			bool found = true;
			for (int j = 0; j < length; ++j) {
				if (mask[j] != 'x' && pattern[j] != *(char*)((DWORD)buffer + i + j)) {
					found = false;
					break;
				}
			}
			if (found) {
				result.push_back(i);
			}
		}
		delete[] buffer;
	}
	return result;
}

std::vector<DWORD> Scanner::kmp_scan(const HANDLE& handle, const char* pattern, const char* mask, int pattern_length) {
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T lpNumberOfBytesRead;
	char* buffer;
	std::vector<DWORD> result;

	for (SIZE_T base_addr = 0x1000000; VirtualQueryEx(handle, (LPCVOID)base_addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); base_addr += mbi.RegionSize) {
		if (mbi.Protect != PAGE_READWRITE || mbi.Type != MEM_PRIVATE) continue;
		buffer = new char[mbi.RegionSize];
		ReadProcessMemory(handle, mbi.BaseAddress, buffer, mbi.RegionSize, &lpNumberOfBytesRead);

		int j = 0;
		std::vector<int> table(pattern_length, 0);
		for (int i = 1; i < pattern_length; ++i) {
			while (j > 0 && pattern[i] != pattern[j])
				j = table[j - 1];
			if (pattern[i] == pattern[j])
				table[i] = ++j;
		}

		j = 0;
		for (int i = 0; i < lpNumberOfBytesRead; ++i) {
			while (j > 0 && buffer[i] != pattern[j] && mask[j] != 'x')
				j = table[j - 1];
			if ((buffer[i] == pattern[j]) || (mask[j] == 'x')) {
				if (j == pattern_length - 1) {
					result.push_back(i - pattern_length + 1);
					j = table[j];
				}
				else j++;
			}
		}
		delete[] buffer;
	}
	return result;
}

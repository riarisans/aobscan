#include "scanner.h"
#include <future>

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

inline std::vector<DWORD> find_pattern(const char* buffer, int buffer_length, const char* pattern, int pattern_length, const char* mask) {
	std::vector<DWORD> result;
	for (unsigned long i = 0; i < buffer_length - pattern_length; ++i) {
		bool found = true;
		for (int j = 0; j < pattern_length; ++j) {
			if (mask[j] != 'x' && pattern[j] != *(char*)((DWORD)buffer + i + j)) {
				found = false;
				break;
			}
		}
		if (found) result.push_back(i);
	}
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
	return new_scan(handle, pattern_char.data(), mask.data(), mask.size());
}

std::vector<DWORD> Scanner::old_scan(const HANDLE& handle, const char* pattern, const char* mask, int length) {
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T lpNumberOfBytesRead;
	char* buffer;
	std::vector<DWORD> result;

	for (SIZE_T base_addr = 0x0; VirtualQueryEx(handle, (LPCVOID)base_addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); base_addr += mbi.RegionSize) {
		bool valid = mbi.State == MEM_COMMIT;
		valid &= ((mbi.Protect & PAGE_GUARD) == 0);
		valid &= ((mbi.Protect & PAGE_NOACCESS) == 0);
		valid &= (mbi.Type == MEM_PRIVATE) || (mbi.Type == MEM_IMAGE);

		if (!valid) continue;

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
				result.push_back(i + base_addr);
			}
		}
		delete[] buffer;
	}
	return result;
}

std::vector<DWORD> Scanner::new_scan(const HANDLE& handle, const char* pattern, const char* mask, int pattern_length) {
	std::vector<std::future<std::vector<DWORD>>> scan_vector;
	MEMORY_BASIC_INFORMATION mbi;
	std::vector<DWORD> result;

	for (SIZE_T base_addr = 0x0; VirtualQueryEx(handle, (LPCVOID)base_addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)); base_addr += mbi.RegionSize) {
		scan_vector.push_back(std::async(std::launch::async, [mbi, handle, pattern_length, mask, pattern, base_addr]() -> std::vector<DWORD>
			{
				std::vector<DWORD> scan_result;
				SIZE_T lpNumberOfBytesRead;
				char* buffer;
				bool valid = mbi.State == MEM_COMMIT;
				valid &= ((mbi.Protect & PAGE_GUARD) == 0);
				valid &= ((mbi.Protect & PAGE_NOACCESS) == 0);
				valid &= (mbi.Type == MEM_PRIVATE) || (mbi.Type == MEM_IMAGE);

				if (!valid) return scan_result;

				buffer = new char[mbi.RegionSize];
				ReadProcessMemory(handle, mbi.BaseAddress, buffer, mbi.RegionSize, &lpNumberOfBytesRead);
				auto found = find_pattern(buffer, lpNumberOfBytesRead, pattern, pattern_length, mask);
				for (const auto& e : found) scan_result.push_back(e + base_addr);
				delete[] buffer;
				return scan_result;
			}));
	}
	for (int i = 0; i < scan_vector.size(); ++i) {
		auto async_result = scan_vector[i].get();
		for (const auto& e : async_result) {
			result.push_back(e);
		}
	}
	return result;
}
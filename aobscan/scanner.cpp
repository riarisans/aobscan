#include "scanner.h"

std::vector<DWORD> Scanner::find_pattern(const char* buffer, int buffer_length, const char* pattern, int pattern_length, const char* mask) {
	std::vector<DWORD> result;
	for (int i = 0; i < buffer_length - pattern_length; ++i) {
		bool found = true;
		for(int j = 0; j < pattern_length; ++j) {
			if (mask[j] != '?' && buffer[i + j] != pattern[j]) {
				found = false;
				break;
			}
		}
		if (found) {
			result.push_back(i);
		}
	}
	return result;
}
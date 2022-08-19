#include "scanner.h"
#include <iostream>

int main() {
	std::vector<DWORD> result;
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, 0x8574);
	clock_t start = clock();
	result = Scanner::scan(handle, "30 ?? 4C ?? ?? ?? 4C 73");
	clock_t end = clock();
	std::cout << std::endl << (end - start) << std::endl << result.size();
}
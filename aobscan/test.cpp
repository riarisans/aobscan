#include "scanner.h"
#include <iostream>

int main() {
	std::vector<DWORD> result;
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, 0xC9E8);

	clock_t start = clock();
	result = Scanner::scan(handle, "10 10 ?? ?? ?? 20 20");
	clock_t end = clock();

	std::cout
		<< std::endl << std::endl
		<< "나온 값 갯수: " << result.size() << std::endl
		<< "걸린 시간: " << end << std::endl
		<< "-------------------" << std::endl;
}
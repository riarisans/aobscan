#include "scanner.h"
#include <iostream>

int main(int argc, char **argv) {
	char* arr = nullptr;
	if (argc > 2) {
		int length1 = strlen(argv[1]);
		for (int i = 0; i < length1; ++i) {
			if (!std::isdigit(argv[1][i])) {
				puts("PID decimal�� �ȹٷγ�����");
				return 0;
			}
		}
		arr = argv[2];
	}
	else {
		puts("aobscan.exe [pid] \"[aobCode: 12 34 ?? 78]\"");
		return 0;
	}
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, std::stoi(argv[1]));
	std::vector<DWORD> result = Scanner::scan(handle, arr);
	for (const auto& e : result) {
		printf("%x\n", e);
	}
	//clock_t start = clock();
	//clock_t end = clock();

	//std::cout
	//	<< std::endl << std::endl
	//	<< "���� �� ����: " << result.size() << std::endl
	//	<< "�ɸ� �ð�: " << (end - start) << "ms" << std::endl
	//	<< "-------------------" << std::endl;
}
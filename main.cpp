#pragma warning(disable : 4996)
#include "CPUclass.h"

int main(int argc, char* argv[]) {

	CPU cd;

	cd.preparer(argv[1]);

	cd.executor();

	system("pause");

	return 0;
	
}

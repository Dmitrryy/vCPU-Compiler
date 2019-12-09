#pragma warning(disable : 4996)
#include "CPUclass.h"

int main() {

	CPU cd;

	cd.preparer("sours_bin.bin");

	/*if (cd.m_time_change > cd.m_time_compl) {

		Compl("sourse.txt", "sours_bin.bin");
		cd.preparer("sours_bin.bin");
	}*/

	cd.executor();

	return 0;
	
}

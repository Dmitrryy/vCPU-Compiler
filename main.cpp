#pragma warning(disable : 4996)
#include "CPUclass.h"

int main(int argc, char* argv[]) {

	CPU cd;

	if (argc < 2) {
	    cd.OK(FILE_NAME_NOT_PASSED);

	    return FILE_NAME_NOT_PASSED;
	}
    if (argc > 2) {
        //cd.OK(MORE_ARG_COMMAND_LINE);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
        cout << endl << "expected 1 argument on the command line, but the resulting = " << argc - 1 << endl;
        cout << "They are:" << endl;

        for (int i = 1; i < argc; i++)
            cout << argv[i] << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        return MORE_ARG_COMMAND_LINE;
    }

	cd.preparer(argv[1]);

	cd.executor();

	//system("pause");

	return 0;
	
}

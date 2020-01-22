#pragma warning(disable : 4996)

#include "Compiler_class.h"


int main(int argc, char* argv[]) {
    Compiler sours;

    sours.read(argv[1]);

    sours.processing();

    sours.parse();

    sours.semantic_analysis();

    sours.output(argv[2]);

    //system("pause");

    return 0;
}
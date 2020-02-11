#pragma warning(disable : 4996)

#include "Compiler.h"


int main(int argc, char* argv[]) {
    Compiler sours;

    char* text = sours.read(argv[1]);

    ptl* point_to_lex = sours.preprocessor(text);

    Lexem* lex = sours.parse(point_to_lex);

    Instruction* inst = sours.semantic_analysis(lex, point_to_lex);

    sours.output(inst, argv[2]);

    cout << endl << "time = " << clock() / 1000.0 << endl;
    system("pause");

    return 0;
}
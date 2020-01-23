#pragma once

#include <cstddef>
#include <iostream>
#include <cctype>
#include <functional>
#include <ctime>
#include <windows.h>
#include <fstream>
#include <cmath>
#include <csignal>
#include <iomanip>

using namespace std;

//#include "Compiler_class.h"
#include "m_strtok.cpp"
//#include "Compiler_class.h"

class Compiler;

class CmpMistake {


private: char* m_str_orig;
private: void get_orig_str(char* str, uint64_t len);

private: void comment_mistake (uint64_t begin_char, uint64_t max_len);

private: uint64_t nom_line (uint64_t nom_elem);

    friend Compiler;
};

void CmpMistake::get_orig_str(char* str, uint64_t len) {

    m_str_orig = new char [(len + 1) * sizeof(char)]();
    assert(m_str_orig != NULL);
    memmove(m_str_orig, str, (len + 1) * sizeof(char));

    return;
}

void CmpMistake::comment_mistake(uint64_t begin_char, uint64_t max_len) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(begin_char); //т.к. функция считает первую строчку нулевой

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

    cout << setw(4) << left << line << " : The comment is not closed." << endl;
    cout << ". . ." << endl;

    char* tmp = NULL;
    cout << setw(4) << left << line << " : " << (tmp = m_strtok(m_str_orig, "\n", line)) << endl;

    for (uint64_t i = 0; i < begin_char - (uint64_t)(tmp - m_str_orig); i++)
        cout << " ";
    cout << "       ^~~~~" << endl;

    cout << ". . ." << endl;

    system("pause");
    delete m_str_orig;
    exit(0);
}

uint64_t CmpMistake::nom_line(uint64_t nom_elem) {

    uint64_t line = 0;

    for(uint64_t i = 0; i < nom_elem; i++)
        if (m_str_orig[i] == '\n')
            line++;

    return line;
}
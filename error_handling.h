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

#include "m_strtok.cpp"


#define MIS_IN_COMMENTS00 401
#define MIS_IN_COMMENTS01 402
#define WAR_IN_COMMENTS00 501


class Compiler;

class CmpMistake {

private: void mistake (uint64_t begin_char, int code);

private: int m_nom_war;
private: int m_nom_err;

private: char* m_str_orig;
private: uint64_t max_len;

private: void get_orig_str(char* str, uint64_t len);

private: void comment_mistake (uint64_t begin_char, int code);

private: uint64_t nom_line (uint64_t nom_elem);
private: void writer_position (char* str, uint64_t line, uint64_t nom_char_mis);

    friend Compiler;

    CmpMistake():
    m_nom_war(0),
    m_nom_err(0),
    m_str_orig(NULL),
    max_len(0)
    {};
};

void CmpMistake::get_orig_str(char* str, uint64_t len) {

    m_str_orig = new char [(len + 1) * sizeof(char)]();
    assert(m_str_orig != NULL);
    memmove(m_str_orig, str, (len + 1) * sizeof(char));
    max_len = len;
}

void CmpMistake::mistake(uint64_t begin_char, int code) {

    if (code == MIS_IN_COMMENTS00 || code == MIS_IN_COMMENTS01 || code == WAR_IN_COMMENTS00)
        comment_mistake(begin_char, code);

    //system("pause");
}

void CmpMistake::comment_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(begin_char); //т.к. функция считает первую строчку нулевой

    if (code == MIS_IN_COMMENTS00) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : The comment is not closed." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MIS_IN_COMMENTS01) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : The comment is not open." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == WAR_IN_COMMENTS00) {
        m_nom_war++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

        cout << "warning" << " : '/*' within block comment" << endl;
        cout << ". . ." << endl;

        char* tmp = new char [max_len + 1]();
        assert(tmp != NULL);
        memmove(tmp, m_str_orig, (max_len + 1) * sizeof(char));

        writer_position(tmp, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        delete [] tmp;
    }
}

uint64_t CmpMistake::nom_line(uint64_t nom_elem) {

    uint64_t line = 0;

    for(uint64_t i = 0; i < nom_elem; i++)
        if (m_str_orig[i] == '\n')
            line++;

    return line;
}

void CmpMistake::writer_position(char *str, uint64_t line, uint64_t nom_char_mis) {

    char * ptr_now = NULL;
    cout << setw(4) << left << line << " : " << (ptr_now = m_strtok(str, "\n", line)) << endl;

    for (uint64_t i = 0; i < nom_char_mis - (uint64_t) (ptr_now - str); i++)
        cout << " ";
    cout << "       ^~~~~" << endl;
}
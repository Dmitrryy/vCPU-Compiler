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

#define UNDECLARED_IDENTIFIER 601
#define WRONG_CREATED_LABEL00 701
#define WRONG_CREATED_LABEL01 702
#define MULTIPLE_IDENTICAL_LABELS_cr 801
#define MULTIPLE_IDENTICAL_FUNC 901
#define MULTIPLE_IDENTICAL_BEGIN 902
#define MULTIPLE_IDENTICAL_END 903
#define BODY_OF_FUNC_NOTFOUND 1001

#define BEGIN_NOTFOUND 1002
#define END_NOTFOUND 1003
#define INVALID_LOCATION_BEGIN_END00 1004
#define INVALID_LOCATION_BEGIN_END01 1005

#define INCORRECT_ARGUMENTS_REG_or_NUM 1100
#define INCORRECT_ARGUMENTS_REG 1101
#define INCORRECT_ARGUMENTS_LABEL 1102
#define INCORRECT_ARGUMENTS_NULL 1103

#define UNkNOWN_TOKEN_FLAG 1200


class Compiler;

class CmpMistake {

private: void mistake (uint64_t begin_char, int code);

private: int m_nom_war;
private: int m_nom_err;

private: char* m_str_orig;
private: uint64_t max_len;

private: void get_orig_str(char* str, uint64_t len);

private: void comment_mistake (uint64_t begin_char, int code);
private: void parse_mistake (uint64_t begin_char, int code);
private: void semantic_mistake (uint64_t begin_char, int code);

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

    if (code == UNDECLARED_IDENTIFIER || code == WRONG_CREATED_LABEL00 || code == WRONG_CREATED_LABEL01 || code == MULTIPLE_IDENTICAL_LABELS_cr || code == MULTIPLE_IDENTICAL_FUNC|| code == BODY_OF_FUNC_NOTFOUND || code == MULTIPLE_IDENTICAL_BEGIN || code == MULTIPLE_IDENTICAL_END || code == BEGIN_NOTFOUND || code == END_NOTFOUND || code == INVALID_LOCATION_BEGIN_END00 || code == INVALID_LOCATION_BEGIN_END01)
        parse_mistake(begin_char, code);

    if (code == INCORRECT_ARGUMENTS_REG_or_NUM || code == INCORRECT_ARGUMENTS_REG || code == INCORRECT_ARGUMENTS_LABEL || code == INCORRECT_ARGUMENTS_NULL)
        semantic_mistake(begin_char, code);

    //system("pause");
}

void CmpMistake::comment_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(begin_char);

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

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
    }
}

void CmpMistake::parse_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(begin_char);

    if (code == UNDECLARED_IDENTIFIER) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : use of undeclared identifier." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == WRONG_CREATED_LABEL00) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : incorrect label creation." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects:" << endl;
        cout << "jump point: 'jmp :labels_name'" << endl;
        cout << "destination of the jump: 'labels_name:'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == WRONG_CREATED_LABEL01) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : not discovered the destination of the jump." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_LABELS_cr) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : multiple identical labels. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_FUNC) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : multiple identical function declarations. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == BODY_OF_FUNC_NOTFOUND) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : not found the body of the function" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_BEGIN) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : multiple identical 'begin'. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_END) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : multiple identical 'end'. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == BEGIN_NOTFOUND) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : not found the 'begin'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == END_NOTFOUND) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : not found the 'end'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INVALID_LOCATION_BEGIN_END00) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : invalid location 'begin' and 'end':" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
    }

    if (code == INVALID_LOCATION_BEGIN_END01) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << ". . ." << endl;
        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }
}

void CmpMistake::semantic_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(begin_char);

    if (code == INCORRECT_ARGUMENTS_REG) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: REG" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INCORRECT_ARGUMENTS_REG_or_NUM) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: REG or NUM" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INCORRECT_ARGUMENTS_LABEL) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: ':labels_name'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INCORRECT_ARGUMENTS_NULL) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << "error" << " : incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: nothing" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
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

    char* tmp = new char [max_len + 1]();
    assert(tmp != NULL);
    memmove(tmp, str, (max_len + 1) * sizeof(char));

    char * ptr_now = NULL;
    cout << setw(4) << left << line << " : " << (ptr_now = m_strtok(tmp, "\n", line)) << endl;

    for (uint64_t i = 0; i < nom_char_mis - (uint64_t)(ptr_now - tmp); i++)
        cout << " ";
    cout << "       ^~~~~" << endl;

    delete [] tmp;
}
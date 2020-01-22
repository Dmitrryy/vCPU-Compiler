#pragma once

#include "Compiler_class.h"

class Compiler;

class CmpMistake {


private: char* m_str_orig;
private: void get_orig_str(char* str, uint64_t len);

private: void comment_mistake (uint64_t begin_char, Compiler* cmp);

private: uint64_t nom_line (uint64_t nom_elem);
private: char* get_line_ptr(uint64_t nom_line);


    friend Compiler;
};

void CmpMistake::get_orig_str(char* str, uint64_t len) {

    m_str_orig = new char [(len + 1) * sizeof(char)]();
    assert(m_str_orig != NULL);
    memmove(m_str_orig, m_str_orig, (len + 1) * sizeof(char));

    return;
}

void CmpMistake::comment_mistake(uint64_t begin_char, Compiler *cmp) {

    assert(begin_char < cmp.m_len);

    uint64_t line = nom_line(begin_char);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

    cout << line << " : The comment is not closed." << endl;
    cout << ". . ." << endl;
    cout << line - 1 << " : " << get_line_ptr(line - 1) << endl;

    cout << line << " : " << get_line_ptr(line) << endl;
    cout << "^~~~~" << endl;

    cout << line + 1 << " : " << get_line_ptr(line + 1) << endl;
    cout << ". . ." << endl;

    delete m_str_orig[];
    delete cmp->m_str[];

}

char* CmpMistake::get_line_ptr(uint64_t nom_line) {  //пофиксить свой стрток


}

uint64_t CmpMistake::nom_line(uint64_t nom_elem) {

    uint64_t line = 0;

    for(uint64_t i = 0; i < nom_elem; i++)
        if (m_str_orig[i] == '\n')
            line++;

    return line;
}
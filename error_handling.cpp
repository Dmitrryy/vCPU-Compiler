#include "Compiler.h"

char* m_strtok (char* str, const char* skp, unsigned int nom_lex_skip = 0);



void CmpMistake::get_orig_str(char* str, uint64_t len) {

    m_str_orig = new char [(len + 1) * sizeof(char)]();
    assert(m_str_orig != NULL);
    memmove(m_str_orig, str, (len + 1) * sizeof(char));
    max_len = len;
}

void CmpMistake::mistake(uint64_t begin_char, int code) {

    if (code == COMMENT_ISNT_CLOSED || code == COMMENT_ISNT_OPEN || code == WAR_IN_COMMENTS00)
        comment_mistake(begin_char, code);

    if (code == UNDECLARED_IDENTIFIER || code == WRONG_CREATED_LABEL00 || code == WRONG_CREATED_LABEL01 || code == MULTIPLE_IDENTICAL_LABELS_cr || code == MULTIPLE_IDENTICAL_FUNC|| code == BODY_OF_FUNC_NOTFOUND || code == MULTIPLE_IDENTICAL_BEGIN || code == MULTIPLE_IDENTICAL_END || code == BEGIN_NOTFOUND || code == END_NOTFOUND || code == INVALID_LOCATION_BEGIN_END00 || code == INVALID_LOCATION_BEGIN_END01)
        parse_mistake(begin_char, code);

    if (code == INCORRECT_ARGUMENTS_REG_or_NUM || code == INCORRECT_ARGUMENTS_REG || code == INCORRECT_ARGUMENTS_LABEL || code == INCORRECT_ARGUMENTS_NULL || code == UNKNOWN_ARGUMENT)
        semantic_mistake(begin_char, code);

    //system("pause");
}

void CmpMistake::comment_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(m_str_orig, begin_char);

    if (code == COMMENT_ISNT_CLOSED) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " The comment is not closed." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == COMMENT_ISNT_OPEN) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " The comment is not open." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == WAR_IN_COMMENTS00) {
        m_nom_war++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        cout << "warning:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

        cout << " '/*' within block comment" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
    }
}

void CmpMistake::parse_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(m_str_orig, begin_char);

    if (code == UNDECLARED_IDENTIFIER) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " use of undeclared identifier." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == WRONG_CREATED_LABEL00) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " incorrect label creation." << endl;
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

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " not discovered the destination of the jump." << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_LABELS_cr) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " multiple identical labels. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_FUNC) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " multiple identical function declarations. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == BODY_OF_FUNC_NOTFOUND) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " not found the body of the function" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_BEGIN) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " multiple identical 'begin'. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == MULTIPLE_IDENTICAL_END) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " multiple identical 'end'. second is:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == BEGIN_NOTFOUND) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " not found the 'begin'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == END_NOTFOUND) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " not found the 'end'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INVALID_LOCATION_BEGIN_END00) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " invalid location 'begin' and 'end':" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
    }

    if (code == INVALID_LOCATION_BEGIN_END01) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << ". . ." << endl;
        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }
}

void CmpMistake::semantic_mistake(uint64_t begin_char, int code) {

    assert(begin_char < max_len);

    uint64_t line = nom_line(m_str_orig, begin_char);

    if (code == INCORRECT_ARGUMENTS_REG) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: REG" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INCORRECT_ARGUMENTS_REG_or_NUM) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: REG or NUM" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INCORRECT_ARGUMENTS_LABEL) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: ':labels_name'" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == INCORRECT_ARGUMENTS_NULL) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << "incorrect argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        cout << "expects: nothing" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }

    if (code == UNKNOWN_ARGUMENT) {
        m_nom_err++;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << "error:";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

        cout << " unknown argument:" << endl;
        cout << ". . ." << endl;

        writer_position(m_str_orig, line, begin_char);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

        system("pause");
        delete [] m_str_orig;
    }
}

int CmpMistake::check_line_style(uint64_t begin_char_1, uint64_t begin_char_2, int code_place) {

    uint64_t line1 = nom_line(m_str_orig, begin_char_1);
    uint64_t line2 = nom_line(m_str_orig, begin_char_2);
    int stat = 0;

    if (code_place == LINE_NULL) {

        if (line1 >= line_now) {
            line_now = line1 + 1;
            return 0;
        }
        else {
            m_nom_err++;
            line_now = line1 + 1;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
            cout << "error:";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

            cout << " CODEE STYLEEE!!!!!:" << endl;
            cout << ". . ." << endl;

            writer_position(m_str_orig, line1, begin_char_1);

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

            return 1;
        }
    }

    if (code_place == LINE_ONE) {
        if (line1 == line2 && line1 >= line_now) {
            line_now = line1 + 1;
            return 0;
        }
        else {
            m_nom_err++;
            line_now = line2 + 1;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
            cout << "error:";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);

            cout << " CODEE STYLEEE!!!!!:" << endl;
            cout << ". . ." << endl;

            writer_position(m_str_orig, line1, begin_char_1);
            writer_position(m_str_orig, line2, begin_char_2);

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

            return 1;
        }
    }

    return 0;
}

uint64_t CmpMistake::nom_line(const char* str, uint64_t nom_elem) {

    uint64_t line = 0;

    for(uint64_t i = 0; i < nom_elem; i++)
        if (str[i] == '\n')
            line++;

    return line + 1;
}

void CmpMistake::writer_position(char *str, uint64_t line, uint64_t nom_char_mis) {

    char* tmp = new char [max_len + 1]();
    assert(tmp != NULL);
    memmove(tmp, str, (max_len + 1) * sizeof(char));

    char * ptr_now = NULL;
    cout << setw(4) << left << line << " : " << (ptr_now = m_strtok(tmp, "\n", line - 1)) << endl;


    for (uint64_t i = 0; i < nom_char_mis - (uint64_t) (ptr_now - tmp); i++)
        cout << " ";
    cout << "       ^~~~~" << endl;

    delete [] tmp;
}
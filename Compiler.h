
#include "prototypes.h"

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

private: int check_line_style (uint64_t begin_char_1, uint64_t begin_char_2, int code_place);
private: uint64_t line_now;

private: uint64_t nom_line (const char* str, uint64_t nom_elem);
private: void writer_position (char* str, uint64_t line, uint64_t nom_char_mis);

    friend Compiler;

    CmpMistake():
            m_nom_war(0),
            m_nom_err(0),
            m_str_orig(NULL),
            max_len(0),
            line_now(1)
    {};
};

struct ptl {
    char* point;

    uint64_t first_char;
};

struct Lexem {
    int flag;
    int64_t integer;

    uint64_t first_char;
};

class Compiler {

    char*        m_str_pr;
    uint64_t     m_ptl_len;

    uint64_t m_nom_cmd;
    uint64_t m_len;
    uint64_t m_len_pr;
    uint64_t m_progma_begin;

private: map <string, int>* Label_cr;
private: map <string, int>* func;

public: char* read(const char* fname_in);
public: ptl* preprocessor(char* m_str);
public: Lexem* parse(ptl* m_ptl);
public: Instruction* semantic_analysis(Lexem* m_lex, ptl* m_ptl);
public: int output(Instruction* m_inst, const char* fname_out);

private: int search_code(int code, const int* data, uint64_t nelem);
private: int search_comments(char* str, uint64_t len);

private: CmpMistake* mis;
private: void terminate();

public: Compiler() :
            m_str_pr(NULL),
            m_ptl_len(0),
            m_len(0),
            m_len_pr(0),
            m_progma_begin(0),
            m_nom_cmd(0)
    {
        mis = new CmpMistake();

        Label_cr = new map<string, int> ();
        func = new map<string, int> ();
    };
};


#define COMMENT_ISNT_CLOSED 401
#define COMMENT_ISNT_OPEN 402

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

#define UNKNOWN_ARGUMENT 1201

#define UNKNOWN_TOKEN_FLAG 1200


#define LINE_NULL 111
#define LINE_ONE 112
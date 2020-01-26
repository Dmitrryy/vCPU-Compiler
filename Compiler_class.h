#pragma once

#include <cassert>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>

#include "CmdFlagReg.h"
#include "error_handling.h"




struct Lexem {
	int flag;

	int64_t integer;
};

class Compiler {
	char*        m_str;
	char**       m_point_str;
	Lexem*       m_lex;
	Instruction* m_instr;

	uint64_t m_nom_cmd;
	uint64_t m_len;
	uint64_t m_progma_begin;

	uint16_t m_nom_label;

private: map <string, int>* Label_cr;
private: map <string, int>* func;

public: int read(const char* fname_in);
public: int processing();
public: int parse();
public: int semantic_analysis();
public: int output(const char* fname_out);

private: int search_code(int code, const int* data, uint64_t nelem);
private: int search_comments();

friend CmpMistake;

private: CmpMistake* mis;
private: void terminate();

public: Compiler() :
		   m_str(NULL),
		   m_point_str(NULL),
		   m_lex(NULL),
		   m_instr(NULL),
		   m_len(0),
	       m_progma_begin(0),
		   m_nom_label(0),
	       m_nom_cmd(0)
	   {
    mis = new CmpMistake();

    Label_cr = new map<string, int> ();
    func = new map<string, int> ();
	   };
};

int Compiler::read(const char* fname_in) {

	ifstream file(fname_in);

    if (!file.is_open())
        assert(0);
	
	file.seekg(0, ios::end);

	m_len = file.tellg();
	assert(m_len != 0);
	
	file.seekg(0, ios::beg);

	m_str = new char [(m_len + 1) * sizeof(char)]();
	assert(m_str != NULL);

	file.getline(m_str, m_len, '\0');

	file.close();

	mis->get_orig_str(m_str, m_len);

	m_str[m_len] = '\0';

	return 0;
}

int Compiler::output(const char* fname_out) {

	std::ofstream out(fname_out);
	
	//out << "DMRY";
	time_t sec = time(NULL);

	m_progma_begin++;

	out.write((char*)&sec, sizeof(time_t));                //время компиляции
	out.write((char*)&m_len, sizeof(uint64_t));		   //кол-во команд
	out.write((char*)&m_progma_begin, sizeof(uint64_t));//номер команды начала

	for (int i = 0; i < m_len; i++)
		out.write((char*)(m_instr + i), sizeof(Instruction));

	out.close();

	terminate();

	return 0;
}

int Compiler::search_comments() {

	int stat = 0;
	char* beg_comm = NULL;

	for (uint64_t i = 0; i < m_len; i++) {

        if (m_str[i] == '/' && i + 1 < m_len) // /*. . .
            if (m_str[i + 1] == '*') {
                if (stat == 0)
                    beg_comm = &(m_str[i]);

                if (stat == 1)
                    mis->mistake(i, WAR_IN_COMMENTS00); //встречен блок комментарий внутри уже открытого блока

                stat = 1;
            }

        if (m_str[i] == '*' && i + 1 < m_len) // . . .*/
            if (m_str[i + 1] == '/') {
                if (stat == 1) {
                    m_str[i] = m_str[i + 1] = ' ';
                    stat = 0;
                }
                else {
                    mis->mistake(i, MIS_IN_COMMENTS01); //символ не закрывает ни одного комментария
                    terminate();
                    exit(MIS_IN_COMMENTS01);
                }
            }

		if (stat == 1)
			m_str[i] = ' ';

		if (stat == 0 && i + 1 < m_len)                     //комментарии вида: // . . . \n
		    if (m_str[i] == '/' && m_str[i + 1] == '/') {

		        for ( ; i < m_len && m_str[i] != '\n' && m_str[i] != '\0'; i++)
		            m_str[i] = ' ';
		    }
	}


	if (stat == 1) {
        mis->mistake((uint64_t) (beg_comm - m_str), MIS_IN_COMMENTS00); // блок комментарий не закрыт
        terminate();

        exit(MIS_IN_COMMENTS00);
    }

	return 0;
}

int Compiler::processing() {

	search_comments();

	m_point_str = (char**)calloc(10, sizeof(char*));
	
	char* tmp = strtok(m_str, " \n\t\r");

	if (tmp != NULL)
		*m_point_str = tmp;

	int i = 1;
	for ( ; (tmp = strtok(NULL, " \n\t\r")) != NULL; i++) {

		if (i % 10 == 0 && i != 0) {
			char** tmp_str = (char**)realloc(m_point_str, (i + 10) * sizeof(char*));
			if (tmp_str == NULL) {
				terminate();
				assert(0); //ошибка realloc
			}
			m_point_str = tmp_str;
			}

		*(m_point_str + i) = tmp;
	}
	m_len = i;

	return 0;
}

int Compiler::search_code(int code, const int* data, uint64_t nelem) {

	for (uint64_t i = 0; i < nelem; i++) {
		if (code == data[i])
			return 1;
	}

	return 0;
}

int Compiler::parse() {

	m_lex = new Lexem[m_len + 1]();

	for (int i = 0; i < m_len; i++) {

		auto result = CMD.find(m_point_str[i]);

		if (result != CMD.end()) {
			//фиксация обьявления функции
			if (result->second == CMD_func) {

			    if (func->find(m_point_str[i + 1]) == func->end()) {
                    func->emplace(m_point_str[i + 1], m_nom_cmd);
                    m_lex[i].flag = _SKIP_;
                    m_lex[++i].flag = _SKIP_;
                }
			    else {
                    mis->mistake((uint64_t)(m_point_str[i + 1] - m_str), MULTIPLE_IDENTICAL_FUNC);
                    terminate();
                    exit(MULTIPLE_IDENTICAL_FUNC);
			    }
			}
			//фиксация вызова функции
			else if (result->second == CMD_call) {

				m_lex[i] = { _Func_, (int64_t)m_nom_cmd };
				m_lex[++i] = { _SKIP_, 0 };

				m_nom_cmd++;
			}
			else {
				m_lex[i] = { _CMD_, result->second };

				if (result->second == CMD_begin)
					m_progma_begin = m_nom_cmd;

				m_nom_cmd++;
			}
		}

		else if ((result = REG.find(m_point_str[i])) != REG.end())
			m_lex[i] = { _REG_, result->second };

		else {
			char* mb_label = strstr(m_point_str[i], ":"); 
			if (mb_label == NULL) {

			    int sum = 0;
			    for (int k = 0; m_point_str[i][k] != '\0'; k++) {

			        if (isdigit(m_point_str[i][k]))
			            sum = sum * 10 + m_point_str[i][k] - '0';

			        else {
                        mis->mistake((uint64_t) (m_point_str[i] - m_str), UNDECLARED_IDENTIFIER);
                        terminate();
                        exit(UNDECLARED_IDENTIFIER);
                    }

			    }

					m_lex[i] = { _NUM_, sum };
					sum = 0;
			}

			else if (*(mb_label - 1) == '\0') {

				m_lex[i] = { _Label_arg_, i };
			}
			else if (*(mb_label + 1) == '\0') {
				*(mb_label) = '\0';

				if (Label_cr->find(m_point_str[i]) == Label_cr->end()) {
                    Label_cr->emplace(m_point_str[i], m_nom_cmd);

                    m_lex[i] = {_Label_create_, (int64_t) (m_nom_cmd)};
                    m_nom_label++;
                }
				else {
				    mis->mistake((uint64_t)(m_point_str[i] - m_str), MULTIPLE_IDENTICAL_LABELS_cr);
				    terminate();
				    exit(MULTIPLE_IDENTICAL_LABELS_cr);
				}
			}
			else {
			    mis->mistake((uint64_t) (m_point_str[i] - m_str), WRONG_CREATED_LABEL00); //встретилось обозначение метки (":")
			    terminate();                                                                        //но в неправильном написании
			    exit(WRONG_CREATED_LABEL00);                                                        //до и после метки должен быть разделительный знак
			}
		}
	}

	uint64_t sit_begin = 0, sit_end = 0;

    //второй обход
	for (uint64_t i = 0; i < m_len; i++) {

	    //разрешение меток
		if (m_lex[i].flag == _Label_arg_) {

			auto result = Label_cr->find(m_point_str[i] + 1); //индекс i у лексемы и у указателя на на лексему (m_pointer_str) одинаков

			if (result != Label_cr->end())
				m_lex[i] = { _Label_resolv_, result->second };

			else {
			    mis->mistake((uint64_t) (m_point_str[i] - m_str), WRONG_CREATED_LABEL01);
			    terminate();
			    exit(WRONG_CREATED_LABEL01);                     //проверить на ошибку в написании и существует ли вообще
			}                                                    //метка под таким именем
		}
		//разрешение вызова функции
		else if (m_lex[i].flag == _Func_) {

			auto result = func->find(m_point_str[i + 1]);

			if (result != func->end())
				m_lex[i] = { _Func_resolv_, result->second };

			else {
			    mis->mistake((uint64_t)(m_point_str[i + 1] - m_str), BODY_OF_FUNC_NOTFOUND);
			    terminate();
			    exit(BODY_OF_FUNC_NOTFOUND);
			    //проверить на ошибку в написании и существует ли вообще
			}   //обьявление (тело) функции
		}

		if (m_lex[i].flag == _CMD_ && m_lex[i].integer == CMD_begin) {

			if (sit_begin != 0) {
                mis->mistake((uint64_t)(m_point_str[i] - m_str), MULTIPLE_IDENTICAL_BEGIN);
                terminate();
                exit(MULTIPLE_IDENTICAL_BEGIN);
			}

			sit_begin = i + 1;
		}

		if (m_lex[i].flag == _CMD_ && m_lex[i].integer == CMD_end) {

			if (sit_end != 0) {
                mis->mistake((uint64_t)(m_point_str[i] - m_str), MULTIPLE_IDENTICAL_END);
                terminate();
                exit(MULTIPLE_IDENTICAL_END);
			}

			sit_end = i + 1;
		}
	}
	if (sit_begin == 0) {
        mis->mistake(0, BEGIN_NOTFOUND);
        terminate();
        exit(BEGIN_NOTFOUND);
	}

	if (sit_end == 0) {
        mis->mistake(0, END_NOTFOUND);
        terminate();
        exit(END_NOTFOUND);
    }
	if (sit_begin > sit_end) {
	    mis->mistake((uint64_t)(m_point_str[sit_begin - 1] - m_str), INVALID_LOCATION_BEGIN_END00);
	    mis->mistake((uint64_t)(m_point_str[sit_end - 1] - m_str), INVALID_LOCATION_BEGIN_END01);
	    terminate();
	    exit(INVALID_LOCATION_BEGIN_END00);
	}

	return 0;
}

int Compiler::semantic_analysis() {

	m_instr = new Instruction[m_len]();
	int k = 0;
	
	int cod = 0;

	for (uint64_t i = 0; i < m_len; i++) {

		if (m_lex[i].flag == _CMD_) {

			m_instr[k].CMD_flag = _CMD_;
			m_instr[k].CMD_code = cod = m_lex[i].integer;

			//комманды без параметра
			if (search_code(cod, CMD_with_NULL, NOM_CMD_WITH_NULL)) {

			    if (i == m_len - 1 || (i < m_len && m_lex[i + 1].flag == _CMD_))
                    m_instr[k].arg_flag = _NULL_;
			    else {
			        mis->mistake((uint64_t)(m_point_str[i] - m_str), INCORRECT_ARGUMENTS_NULL);
			        terminate();
			        exit(INCORRECT_ARGUMENTS_NULL);
			    }
            }

			//команды с параметром REG
			else if (search_code(cod, CMD_with_REG, NOM_CMD_WITH_REG)) {

                m_instr[k].arg_flag = _REG_;
                i++;
                assert(i < m_len);

                if (m_lex[i].flag == _REG_)
                    m_instr[k].integer = m_lex[i].integer;
                else {
                    mis->mistake((uint64_t) (m_point_str[i - 1] - m_str), INCORRECT_ARGUMENTS_REG);
                    terminate();
                    exit(INCORRECT_ARGUMENTS_REG);
                }
            }

			//команды с параметром REG или NUM
			else if (search_code(cod, CMD_with_REGandNUM, NOM_CMD_WITH_REGandNUM)) {

				i++;
				assert(i < m_len); //что то пошло не так...

				if (m_lex[i].flag == _REG_) {

					m_instr[k].arg_flag = _REG_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else if (m_lex[i].flag == _NUM_) {

					m_instr[k].arg_flag = _NUM_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else {
				    mis->mistake((uint64_t)(m_point_str[i - 1] - m_str), INCORRECT_ARGUMENTS_REG_or_NUM);
				    terminate();
				    exit(INCORRECT_ARGUMENTS_REG_or_NUM);  //после команды с параметром REG или NUM
				}
			}

			//Команды "прыжка" (jmp и тому подобное)
			else if (search_code(cod, CMD_with_Label, NOM_CMD_WITH_LABEL)) {

				i++;
				if (m_lex[i].flag == _Label_resolv_) {

					m_instr[k].arg_flag = _Label_resolv_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else {
				    mis->mistake((uint64_t)(m_point_str[i - 1] - m_str), INCORRECT_ARGUMENTS_LABEL);
				    terminate();
				    exit(INCORRECT_ARGUMENTS_LABEL); //после команды прыжка идет не метка! куда прыгать?
				}
			}
			k++;
		}
	
		else if (m_lex[i].flag == _Label_create_ || m_lex[i].flag == _SKIP_) {
			//здесь и должно быть пусто
			//
			//*пропускаются точки создания метки
			//(т.к. в команде прыжка уже прописано куда прыгвть)
			//
			//*пропускаются лексемы с названиями функций
			//(call уже связан с телом вызываемой функции func)
			//
			//*пропускаются другие лексемы, отмеченные как SKIP№
		}
		//инструкция по вызову функции
		else if (m_lex[i].flag == _Func_resolv_) {

			m_instr[k].CMD_flag = _CMD_;
			m_instr[k].CMD_code = CMD_call;
			m_instr[k].arg_flag = _Func_resolv_;

			m_instr[k].integer  = m_lex[i].integer;

			k++;
		}
		else {
		    //cout << (int)m_lex[i].flag << endl;
            assert(0); //встречен неизвестный флаг лексемы
        }
	}
	m_len = k;

	//for (int h = 0; h < m_len; h++)
		//cout << h << "  " << (int)m_instr[h].CMD_flag << "  " << (int)m_instr[h].CMD_code << "  " << (int64_t)m_instr[h].arg_flag << "  " << m_instr[h].integer << endl;

	return 0;
}

void Compiler::terminate() {
    if (m_str != NULL) {
        delete [] m_str;
        m_str = NULL;
    }
    if(m_point_str != NULL) {
        delete [] m_point_str;
        m_point_str = NULL;
    }
    if(m_lex != NULL) {
        delete [] m_lex;
        m_lex = NULL;
    }
    if(m_instr != NULL) {
        delete [] m_instr;
        m_instr = NULL;
    }
    if(mis != NULL) {
        delete mis;
        mis = NULL;
    }
    if (Label_cr != NULL) {
        delete Label_cr;
        Label_cr = NULL;
    }
    if (func != NULL) {
        delete func;
        func = NULL;
    }
}
#pragma once

#include <cassert>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>

#include "CmdFlagReg.h"

struct Lexem {
	int flag;

	union {
		int64_t integer;
		double floating;
	};
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

private: map <string, int> Label_cr;
private: map <string, int> func;

public: int read(const char* fname_in);
public: int processing();
public: int parse();
public: int semantic_analysis();
public: int output(const char* fname_out);

private: int search_code(const int code, const int* data, uint64_t nelem);
private: int search_comments();

public: Compiler() :
		   m_str(NULL),
		   m_point_str(NULL),
		   m_lex(NULL),
		   m_instr(NULL),
		   m_len(0),
	       m_progma_begin(0),
		   m_nom_label(0),
	       m_nom_cmd(0)
	   {};
};

int Compiler::read(const char* fname_in) {

	std::ifstream file(fname_in);
	
	file.seekg(0, std::ios::end);

	m_len = file.tellg();
	assert(m_len != 0);
	
	file.seekg(0, std::ios::beg);

	m_str = new char [(m_len + 1) * sizeof(char)]();
	assert(m_str != 0);

	file.getline(m_str, m_len, '\0');

	file.close();

	m_str[m_len] = '\0';

	return 0;
}

int Compiler::output(const char* fname_out) {

	std::ofstream out(fname_out);
	
	//out << "DMRY";
	time_t sec = time(NULL);

	m_progma_begin++;

	out.write((char*)&sec, sizeof(time_t));             //время компеляции
	out.write((char*)&m_len, sizeof(uint64_t));			//кол-во команд
	out.write((char*)&m_progma_begin, sizeof(uint64_t));//номер команды начала

	for (int i = 0; i < m_len; i++)
		out.write((char*)(m_instr + i), sizeof(Instruction));

	out.close();

	delete [] m_str;
	delete [] m_point_str;
	delete [] m_lex;
	delete [] m_instr;

	return 0;
}

int Compiler::search_comments() {

	int stat = 0;

	for (int i = 0; i < m_len; i++) {

		if (m_str[i] == '*' && stat == 1) {
			m_str[i] = ' ';
			stat = 0;
		}

		if (m_str[i] == '*' && stat == 0)
			stat = 1;

		if (stat == 1)
			m_str[i] = ' ';
	}

	if (stat == 1)
		assert(0); //не обнаружена закрывающая '*'

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
				delete[] m_point_str;
				assert(0); //ошибка realloc
			}
			m_point_str = tmp_str;
			}

		*(m_point_str + i) = tmp;
	}
	m_len = i;

	return 0;
}

int Compiler::search_code(const int code, const int* data, uint64_t nelem) {

	int stat = 0;

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

				func.emplace(m_point_str[i + 1], m_nom_cmd);
				m_lex[i].flag = SKIP№;
				m_lex[++i].flag = SKIP№;
			}
			//фиксация вызова функции
			else if (result->second == CMD_call) {

				m_lex[i] = { Func№, (int64_t)m_nom_cmd };
				m_lex[++i] = { SKIP№, 0 };

				m_nom_cmd++;
			}
			else {
				m_lex[i] = { CMD№, result->second };

				if (result->second == CMD_begin)
					m_progma_begin = m_nom_cmd;

				m_nom_cmd++;
			}
		}

		else if ((result = REG.find(m_point_str[i])) != REG.end())
			m_lex[i] = { REG№, result->second };

		else {
			char* mb_label = strstr(m_point_str[i], ":"); 
			if (mb_label == NULL) {
				
				if (isdigit(*(m_point_str[i]))) {

					int sum = 0;
					for (int k = 0; m_point_str[i][k] != '\0'; k++)
						sum = sum * 10 + m_point_str[i][k] - '0';

					m_lex[i] = { NUM№, sum };
					sum = 0;
				}
				else assert(0); //встретилась неизвестная команда
			}

			else if (*(mb_label - 1) == '\0') {

				m_lex[i] = { Label_arg№, i };
			}
			else if (*(mb_label + 1) == '\0') {
				*(mb_label) = '\0';
			
				Label_cr.emplace(m_point_str[i], m_nom_cmd);

				m_lex[i] = { Label_create№, (int64_t)(m_nom_cmd) };
				m_nom_label++;
			}
			else assert(0); //встретилось обозначение метки (":")
							//но в неправильном написании
							//до и после метки должен быть разделительный знак
		}
		
		//else assert(0);
		}

	uint64_t sit_begin = 0, sit_end = 0;

	for (int i = 0; i < m_len; i++) {
		if (m_lex[i].flag == Label_arg№) {

			auto result = Label_cr.find(m_point_str[i] + 1);

			if (result != Label_cr.end())
				m_lex[i] = { Label_resolv№, result->second };

			else assert(0); //проверить на ошибку в написании и существует ли вообще
			                //метка под таким именем 
		}
		else if (m_lex[i].flag == Func№) {

			auto result = func.find(m_point_str[i + 1]);
			//cout << i << "  " << m_point_str[i + 1] << endl;

			if (result != func.end())
				m_lex[i] = { Func_resolv№, result->second };

			else assert(0); //проверить на ошибку в написании и существует ли вообще
			                //обьявление (тело) функции
		}
		if (m_lex[i].flag == CMD№ && m_lex[i].integer == CMD_begin) {

			if (sit_begin != 0)
				assert(0); //недопустимо x2 и больше begin!

			sit_begin = i + 1;
		}

		if (m_lex[i].flag == CMD№ && m_lex[i].integer == CMD_end) {

			if (sit_end != 0)
				assert(0); //недопустимо х2 и больше end!

			sit_end = i + 1;
		}
	}
	if (sit_begin == 0)
		assert(0); //необнаружен begin

	if (sit_end == 0)
		assert(0); //необнаружен end

	if (sit_begin > sit_end)
		assert(0); //begin находится после end

	//for (int l = 0; l < m_len; l++)
		//cout << l << "   " << m_lex[l].flag << "  " << m_lex[l].integer << endl;

	return 0;
}

int Compiler::semantic_analysis() {

	m_instr = new Instruction[m_len]();
	int k = 0;
	
	int cod = 0;

	for (int i = 0; i < m_len; i++) {

		if (m_lex[i].flag == CMD№) {

			m_instr[k].CMD_flag = CMD№;
			m_instr[k].CMD_code = cod = m_lex[i].integer;

			//комманды без параметра
			if (search_code(cod, CMD_with_NULL, NOM_CMD_WITH_NULL))
				m_instr[k].arg_flag = NULL№;

			//команды с параметром REG
			else if (search_code(cod, CMD_with_REG, NOM_CMD_WITH_REG)) {

				m_instr[k].arg_flag = REG№;
				i++;
				assert(i < m_len);

				if (m_lex[i].flag == REG№)
					m_instr[k].integer = m_lex[i].integer;
				else 
					assert(0); //после команды ожидается лексема с флагом REG

			}
			//команды с параметром REG или NUM
			else if (search_code(cod, CMD_with_REGandNUM, NOM_CMD_WITH_REGandNUM)) {

				i++;
				assert(i < m_len); //что то пошло не так...

				if (m_lex[i].flag == REG№) {

					m_instr[k].arg_flag = REG№;
					m_instr[k].integer = m_lex[i].integer;
				}
				else if (m_lex[i].flag == NUM№) {

					m_instr[k].arg_flag = NUM№;
					m_instr[k].integer = m_lex[i].integer;
				}
				else assert(0); //после команды с параметром REG или NUM 
								//ни первое, ни второе
			}
			//Команды "прыжка" (jmp и тому подобное)
			else if (search_code(cod, CMD_with_Label, NOM_CMD_WITH_LABEL)) {

				i++;
				if (m_lex[i].flag == Label_resolv№) {

					m_instr[k].arg_flag = Label_resolv№;
					m_instr[k].integer = m_lex[i].integer;
				}
				else assert(0); //после команды прыжка идет не метка! куда прыгать?

			}
			k++;
		}
	
		else if (m_lex[i].flag == Label_create№ || m_lex[i].flag == SKIP№) {
			//здесь и должно быть пусто
			//
			//*пропускаются точки создания метки
			//(т.к. в команде прыжка уже прописано куда прыгвть)
			//
			//*пропускаются другие лексемы, отмеченные как SKIP№
		}
		//инструкция по вызову функции
		else if (m_lex[i].flag == Func_resolv№) {

			m_instr[k].CMD_flag = CMD№;
			m_instr[k].CMD_code = CMD_call;
			m_instr[k].arg_flag = Func_resolv№;

			m_instr[k].integer  = m_lex[i].integer;

			k++;
		}
		else
			assert(0); //встречен неизвестный флаг лексемы
	}
	m_len = k;

	//for (int h = 0; h < m_len; h++)
		//cout << h << "  " << (int)m_instr[h].CMD_flag << "  " << (int)m_instr[h].CMD_code << "  " << (int64_t)m_instr[h].arg_flag << "  " << m_instr[h].integer << endl;

	return 0;
}
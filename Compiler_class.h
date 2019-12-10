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

	m_str[m_len] = '\0';

	return 0;
}

int Compiler::output(const char* fname_out) {

	std::ofstream out(fname_out);
	
	//out << "DMRY";
	time_t sec = time(NULL);

	m_progma_begin++;

	out.write((char*)&sec, sizeof(time_t));             //����� ����������
	out.write((char*)&m_len, sizeof(uint64_t));			//���-�� ������
	out.write((char*)&m_progma_begin, sizeof(uint64_t));//����� ������� ������

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
		assert(0); //�� ���������� ����������� '*'

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
				assert(0); //������ realloc
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
			//�������� ���������� �������
			if (result->second == CMD_func) {

				func.emplace(m_point_str[i + 1], m_nom_cmd);
				m_lex[i].flag = _SKIP_;
				m_lex[++i].flag = _SKIP_;
			}
			//�������� ������ �������
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
				
				if (isdigit(*(m_point_str[i]))) {

					int sum = 0;
					for (int k = 0; m_point_str[i][k] != '\0'; k++)
						sum = sum * 10 + m_point_str[i][k] - '0';

					m_lex[i] = { _NUM_, sum };
					sum = 0;
				}
				else {assert(0); cout << i;} //����������� ����������� �������
			}

			else if (*(mb_label - 1) == '\0') {

				m_lex[i] = { _Label_arg_, i };
			}
			else if (*(mb_label + 1) == '\0') {
				*(mb_label) = '\0';
			
				Label_cr.emplace(m_point_str[i], m_nom_cmd);

				m_lex[i] = { _Label_create_, (int64_t)(m_nom_cmd) };
				m_nom_label++;
			}
			else assert(0); //����������� ����������� ����� (":")
							//�� � ������������ ���������
							//�� � ����� ����� ������ ���� �������������� ����
		}
		
		//else assert(0);
		}

	uint64_t sit_begin = 0, sit_end = 0;

	for (int i = 0; i < m_len; i++) {
		if (m_lex[i].flag == _Label_arg_) {

			auto result = Label_cr.find(m_point_str[i] + 1);

			if (result != Label_cr.end())
				m_lex[i] = { _Label_resolv_, result->second };

			else assert(0); //��������� �� ������ � ��������� � ���������� �� ������
			                //����� ��� ����� ������ 
		}
		else if (m_lex[i].flag == _Func_) {

			auto result = func.find(m_point_str[i + 1]);
			//cout << i << "  " << m_point_str[i + 1] << endl;

			if (result != func.end())
				m_lex[i] = { _Func_resolv_, result->second };

			else assert(0); //��������� �� ������ � ��������� � ���������� �� ������
			                //���������� (����) �������
		}
		if (m_lex[i].flag == _CMD_ && m_lex[i].integer == CMD_begin) {

			if (sit_begin != 0)
				assert(0); //����������� x2 � ������ begin!

			sit_begin = i + 1;
		}

		if (m_lex[i].flag == _CMD_ && m_lex[i].integer == CMD_end) {

			if (sit_end != 0)
				assert(0); //����������� �2 � ������ end!

			sit_end = i + 1;
		}
	}
	if (sit_begin == 0)
		assert(0); //����������� begin

	if (sit_end == 0)
		assert(0); //����������� end

	if (sit_begin > sit_end)
		assert(0); //begin ��������� ����� end

	//for (int l = 0; l < m_len; l++)
		//cout << l << "   " << m_lex[l].flag << "  " << m_lex[l].integer << endl;

	return 0;
}

int Compiler::semantic_analysis() {

	m_instr = new Instruction[m_len]();
	int k = 0;
	
	int cod = 0;

	for (int i = 0; i < m_len; i++) {

		if (m_lex[i].flag == _CMD_) {

			m_instr[k].CMD_flag = _CMD_;
			m_instr[k].CMD_code = cod = m_lex[i].integer;

			//�������� ��� ���������
			if (search_code(cod, CMD_with_NULL, NOM_CMD_WITH_NULL))
				m_instr[k].arg_flag = _NULL_;

			//������� � ���������� REG
			else if (search_code(cod, CMD_with_REG, NOM_CMD_WITH_REG)) {

				m_instr[k].arg_flag = _REG_;
				i++;
				assert(i < m_len);

				if (m_lex[i].flag == _REG_)
					m_instr[k].integer = m_lex[i].integer;
				else 
					assert(0); //����� ������� ��������� ������� � ������ REG

			}
			//������� � ���������� REG ��� NUM
			else if (search_code(cod, CMD_with_REGandNUM, NOM_CMD_WITH_REGandNUM)) {

				i++;
				assert(i < m_len); //��� �� ����� �� ���...

				if (m_lex[i].flag == _REG_) {

					m_instr[k].arg_flag = _REG_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else if (m_lex[i].flag == _NUM_) {

					m_instr[k].arg_flag = _NUM_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else assert(0); //����� ������� � ���������� REG ��� NUM 
								//�� ������, �� ������
			}
			//������� "������" (jmp � ���� ��������)
			else if (search_code(cod, CMD_with_Label, NOM_CMD_WITH_LABEL)) {

				i++;
				if (m_lex[i].flag == _Label_resolv_) {

					m_instr[k].arg_flag = _Label_resolv_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else assert(0); //����� ������� ������ ���� �� �����! ���� �������?

			}
			k++;
		}
	
		else if (m_lex[i].flag == _Label_create_ || m_lex[i].flag == _SKIP_) {
			//����� � ������ ���� �����
			//
			//*������������ ����� �������� �����
			//(�.�. � ������� ������ ��� ��������� ���� �������)
			//
			//*������������ ������ �������, ���������� ��� SKIP�
		}
		//���������� �� ������ �������
		else if (m_lex[i].flag == _Func_resolv_) {

			m_instr[k].CMD_flag = _CMD_;
			m_instr[k].CMD_code = CMD_call;
			m_instr[k].arg_flag = _Func_resolv_;

			m_instr[k].integer  = m_lex[i].integer;

			k++;
		}
		else
			assert(0); //�������� ����������� ���� �������
	}
	m_len = k;

	//for (int h = 0; h < m_len; h++)
		//cout << h << "  " << (int)m_instr[h].CMD_flag << "  " << (int)m_instr[h].CMD_code << "  " << (int64_t)m_instr[h].arg_flag << "  " << m_instr[h].integer << endl;

	return 0;
}
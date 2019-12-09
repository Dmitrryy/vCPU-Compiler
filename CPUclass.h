#pragma once

#include <cassert>
#include <cstring>
#include <cstdint>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <ctime> 

#include "StackF.h"
#include "CmdFlagReg.h"


class CPU {
public: uint64_t m_nom_cmd;
public: time_t m_time_compl;
public: time_t m_time_change;
public: uint64_t m_now_cmd;
public: const char* m_fname;

public: Instruction* instr;

private: double comp;

public: double ax, bx, cx, dx;

private: time_t time_change_file();

public: int preparer(const char* fname);
public: int executor();

	  CPU() :
		  m_nom_cmd(0),
		  m_time_compl(0),
		  m_time_change(0),
		  m_now_cmd(0),
		  m_fname(NULL),
		  instr(NULL),
		  comp(0),
		  ax(0), bx(0), cx(0), dx(0)
	  {};
};


time_t CPU::time_change_file() {

	struct _stat st = {};
	_stat("sourse.txt", &st);

	return st.st_mtime;
}

int CPU::preparer(const char* fname) {

	m_fname = fname;
	std::ifstream in(m_fname);

	if (!in.is_open())
		assert(0);

	in.read((char*)&m_time_compl, sizeof(time_t));
	in.read((char*)&m_nom_cmd, sizeof(uint64_t));
	in.read((char*)&m_now_cmd, sizeof(uint64_t));

	in.close();

	m_time_change = time_change_file();

	return 0;
}

int CPU::executor() {

	instr = new Instruction[m_nom_cmd]();
	if (instr == NULL)
		assert(0);

	std::ifstream in(m_fname);
	if (!in.is_open())
		assert(0);

	in.seekg(sizeof(time_t) + sizeof(uint64_t) * 2, std::ios::beg);

    in.read((char*)instr, sizeof(Instruction) * m_nom_cmd);

	CREATE_STACK(double, st, 4);
	CREATE_STACK(uint64_t, m_ret, 100);

	for (int stat = 0 ; stat != 26; m_now_cmd++) {

		if (instr[m_now_cmd].CMD_flag == CMD¹) {

			switch (instr[m_now_cmd].CMD_code) {
			case CMD_push: 
				if (instr[m_now_cmd].arg_flag == REG¹) {
					switch (instr[m_now_cmd].integer) {
					case REG_ax:
						st->Push(ax);
						break;
					case REG_bx:
						st->Push(bx);
						break;
					case REG_cx:
						st->Push(cx);
						break;
					case REG_dx:
						st->Push(dx);
						break;
					}
				}
				else
					st->Push(instr[m_now_cmd].integer);
				break;
			
			case CMD_pop: 
				switch (instr[m_now_cmd].integer) {
				case REG_ax:
					ax = st->Pop();
					break;
				case REG_bx:
					bx = st->Pop();
					break;
				case REG_cx:
					cx = st->Pop();
					break;
				case REG_dx:
					dx = st->Pop();
					break;
				}
				break;
		
			case CMD_sub:
				st->Push(-(st->Pop()) + st->Pop());
				break;

			case CMD_add:
				st->Push(st->Pop() + st->Pop());
				break;

			case CMD_mul:
				st->Push(st->Pop() * st->Pop());
				break;

			case CMD_div:
				st->Push((1.0 / st->Pop()) * st->Pop());
				break;

			case CMD_in:
				switch (instr[m_now_cmd].integer) {
				case REG_ax:
					std::cin >> ax;
					break;
				case REG_bx:
					std::cin >> bx;
					break;
				case REG_cx:
					std::cin >> cx;
					break;
				case REG_dx: 
					std::cin >> dx;
					break;
				}
				break;

			case CMD_out:
				if (instr[m_now_cmd].arg_flag == REG¹)
					switch (instr[m_now_cmd].integer) {
					case REG_ax:
						std::cout << ax << endl;
						break;
					case REG_bx:
						std::cout << bx << endl;
						break;
					case REG_cx:
						std::cout << cx << endl;
						break;
					case REG_dx:
						std::cout << dx << endl;
						break;
					}
				else
					std::cout << instr[m_now_cmd].integer << endl;
				break;

			case CMD_jmp:
				m_now_cmd = instr[m_now_cmd].integer - 1;
				break;

			case CMD_jaz:
				comp = st->Pop();
				if (comp > 0)
					m_now_cmd = instr[m_now_cmd].integer - 1;
				st->Push(comp);
				break;

			case CMD_jlz:
				comp = st->Pop();
				if (comp < 0)
					m_now_cmd = instr[m_now_cmd].integer - 1;
				st->Push(comp);
				break;

			case CMD_jz:
				comp = st->Pop();
				if (comp == 0)
					m_now_cmd = instr[m_now_cmd].integer - 1;
				st->Push(comp);
				break;

			case CMD_dump:
				st->Dump();
				break;

			case CMD_end:
				st->Destroy();
				stat = 26;
				break;

			case CMD_dup:
				comp = st->Pop();
				st->Push(comp);
				st->Push(comp);
				comp = 0;
				break;

			case CMD_sqrt:
				comp = st->Pop();
				comp = sqrt(comp);
				st->Push(comp);
				comp = 0;
				break;

			case CMD_call:
				m_ret->Push(m_now_cmd);
				m_now_cmd = instr[m_now_cmd].integer - 1;
				break;

			case CMD_ret:
				m_now_cmd = m_ret->Pop();
				break;

			//default:
				//assert(0);
				//break;
			} //end switch
		}
		else {
		
			//for (int h = 0; h < m_nom_cmd; h++)
				//cout << h << "  " << (int)instr[h].CMD_flag << "  " << (int)instr[h].CMD_code << "  " << (int64_t)instr[h].arg_flag << "  " << instr[h].integer << endl;

			assert(0);
		}
	}//end for( ; ; )
	return 0;
}
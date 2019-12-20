#pragma once

#include "vCPU_errors.h"

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
public: uint64_t m_now_cmd;
public: const char* m_fname;

public: Instruction* instr;

//public: void OK();
public: void dump();

private: double comp;

private: int m_stat;
public: double ax, bx, cx, dx;

public: int preparer(const char* fname);
public: int executor();

	  CPU() :
		  m_nom_cmd(0),
		  m_time_compl(0),
		  m_now_cmd(0),
		  m_fname(NULL),
		  instr(NULL),
		  comp(0),
		  m_stat(0),
		  ax(0), bx(0), cx(0), dx(0)
	  {};
};

//подготовка (считывание всех инструкций)
int CPU::preparer(const char* fname) {

	m_fname = fname;
	std::ifstream in(m_fname);

	if (!in.is_open()) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << endl << "failed to open a file with the name:   " << m_fname << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
        exit(FAILED_OPEN_FILE);
    }

	in.read((char*)&m_time_compl, sizeof(time_t));
	in.read((char*)&m_nom_cmd, sizeof(uint64_t));
	in.read((char*)&m_now_cmd, sizeof(uint64_t));

    instr = new Instruction[m_nom_cmd]();
    if (instr == NULL) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

        cout << endl << "failed to allocate memory for instructions" << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
        exit(F_ALLOCATE_MEMORY);
    }

    in.read((char*)instr, sizeof(Instruction) * m_nom_cmd);


	in.close();

	return 0;
}

int CPU::executor() {

	CREATE_STACK(double, st, 4);
	CREATE_STACK(uint64_t, m_ret, 100);

	for (int stat = 0 ; stat != 26; m_now_cmd++) {

		if (instr[m_now_cmd].CMD_flag == _CMD_) {

			switch (instr[m_now_cmd].CMD_code) {
			case CMD_push: 
				if (instr[m_now_cmd].arg_flag == _REG_) {
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
		
			case CMD_sub: {
                double a = st->Pop();
                double b = st->Pop();
                st->Push(b - a);
                break;
            }

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
				if (instr[m_now_cmd].arg_flag == _REG_)
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
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

		    cout << "runtime error" << endl;

		    dump();

			exit(1);
		}
	}//end for( ; ; )
	return 0;
}

void CPU::dump() {

    HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(cons, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);

    cout << endl;
    cout << "File name:  " << m_fname << endl;
    cout << "=========================================================" << endl;
    cout << "|| " << "time of the last compile          = " << setw(16) << left << m_time_compl << "||" << endl;
    cout << "|| " << "number of commands                = " << setw(16) << left << m_nom_cmd << "||" << endl;
    cout << "|| " << "the number of the current command = " << setw(16) << left << m_now_cmd << "||" << endl;
    cout << "||                                                     ||" << endl;
    cout << "|| " << "REG:                                                " << "||" << endl;
    cout << "|| " << "ax = " << setw(16) << left << ax << "                               ||" << endl;
    cout << "|| " << "bx = " << setw(16) << left << bx << "                               ||" << endl;
    cout << "|| " << "cx = " << setw(16) << left << cx << "                               ||" << endl;
    cout << "|| " << "dx = " << setw(16) << left << dx << "                               ||" << endl;
    cout << "=========================================================" << endl;

    if (m_nom_cmd > 0) {
        cout << "№       CMD_flag          " << "CMD_code          " << "arg_flag          " << "arg" << endl;

        for (int i = 0; i < m_nom_cmd; i++)
            cout << "[" << setw(4) << left << i << "]   " << instr[i] << endl;
    }
    cout << endl << endl;

    SetConsoleTextAttribute(cons, 0 | 15);

    system("pause");
}

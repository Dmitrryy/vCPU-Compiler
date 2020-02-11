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

#define SIZE_RAM 200


class CPU {
private: uint64_t m_nom_cmd;
private: time_t m_time_compl;
private: uint64_t m_current_cmd;
private: const char* m_fname;

private: Instruction* instr;

public: void OK(int stat);
public: void dump();

private: double m_reg[4];
private: double* m_RAM;

public: int preparer(const char* fname);
public: int executor();

	  CPU() :
	  m_nom_cmd(0),
	  m_time_compl(0),
	  m_current_cmd(0),
	  m_fname(NULL),
	  instr(NULL)
	  {
          for (int i = 0; i < 4; i++)
              m_reg[i] = 0;

          m_RAM = new double[SIZE_RAM]();
      };

	  ~CPU() {

	      //cout << endl << "KKKKDESTROYY" << endl;
	      if (instr != NULL) {
	          delete [] instr;
	          instr = NULL;

	          delete [] m_RAM;
	      }
	  }
};

//подготовка (считывание всех инструкций)
int CPU::preparer(const char* fname) {

    OK(BEFORE_PREPARER);

	m_fname = fname;
	std::ifstream in(m_fname);

	if (!in.is_open())
	    OK(FAILED_OPEN_FILE);

	in.read((char*)&m_time_compl, sizeof(time_t));
	in.read((char*)&m_nom_cmd, sizeof(uint64_t));
	in.read((char*)&m_current_cmd, sizeof(uint64_t));

    instr = new Instruction[m_nom_cmd]();
    if (instr == NULL)
        OK(F_ALLOCATE_MEMORY);

    in.read((char*)instr, sizeof(Instruction) * m_nom_cmd);

	in.close();

	return 0;
}

int CPU::executor() {

    uint32_t time_beg = clock();

    OK(BEFORE_EXECUTOR);

	CREATE_STACK(double, m_st, 4);
	CREATE_STACK(uint64_t, m_ret, 100);

	double comp = 0;

	for (int stat = 0 ; stat != 26; m_current_cmd++) {

		if (instr[m_current_cmd].CMD_flag == _CMD_) {

			switch (instr[m_current_cmd].CMD_code) {
			case CMD_push: 
				if (instr[m_current_cmd].arg_flag == _REG_) {
				    assert(instr[m_current_cmd].integer < 5 && instr[m_current_cmd].integer > 0);

				    m_st->Push(m_reg[instr[m_current_cmd].integer - 1]);
				}
				else
					m_st->Push(instr[m_current_cmd].integer);
				break;
			
			case CMD_pop:
                assert(instr[m_current_cmd].integer < 5 && instr[m_current_cmd].integer > 0);

                m_reg[instr[m_current_cmd].integer - 1] = m_st->Pop();
				break;
		
			case CMD_sub: {
                double a = m_st->Pop();
                double b = m_st->Pop();
                m_st->Push(b - a);
                break;
            }

			case CMD_add:
				m_st->Push(m_st->Pop() + m_st->Pop());
				break;

			case CMD_mul:
				m_st->Push(m_st->Pop() * m_st->Pop());
				break;

			case CMD_div:
				m_st->Push((1.0 / m_st->Pop()) * m_st->Pop());
				break;

			case CMD_in:
                assert(instr[m_current_cmd].integer < 5 && instr[m_current_cmd].integer > 0);

			    std::cin >> m_reg[instr[m_current_cmd].integer - 1];

				break;

			case CMD_out:
				if (instr[m_current_cmd].arg_flag == _REG_) {
                    assert(instr[m_current_cmd].integer < 5 && instr[m_current_cmd].integer > 0);

                    std::cout << m_reg[instr[m_current_cmd].integer - 1] << endl;
                }
				else
					std::cout << instr[m_current_cmd].integer << endl;
				break;

			case CMD_jmp:
                m_current_cmd = instr[m_current_cmd].integer - 1;
				break;

			case CMD_jaz:
                //cout << instr[m_current_cmd].integer << endl;
				if (m_st->Pop() > 0)
                    m_current_cmd = instr[m_current_cmd].integer - 1;
				//system("pause");
				break;

			case CMD_jlz:
				if (m_st->Pop() < 0)
                    m_current_cmd = instr[m_current_cmd].integer - 1;
				break;

			case CMD_jz:
				if (m_st->Pop() == 0)
                    m_current_cmd = instr[m_current_cmd].integer - 1;
				break;

			case CMD_dump:
				m_st->Dump();
				//system("pause");
				break;

			case CMD_end:
				m_st->Destroy();
				m_ret->Destroy();
				stat = 26;
				break;

			case CMD_dup:
				m_st->Push(m_st->Top());
				break;

			case CMD_sqrt:
			    m_st->Push(sqrt(m_st->Pop()));
				break;

			case CMD_call:
				m_ret->Push(m_current_cmd);
                    m_current_cmd = instr[m_current_cmd].integer - 1;
				break;

			case CMD_ret:
                m_current_cmd = m_ret->Pop();
				break;

			} //end switch
		}
		else if (instr[m_current_cmd].CMD_flag == _CMD_RAM_) {

            switch (instr[m_current_cmd].CMD_code) {
                case CMD_mpush:

                    assert(m_st->Top() < SIZE_RAM);

                    m_RAM[(int)m_st->Pop()] = m_st->Pop();

                    break;

                case CMD_mpop:

                    assert(m_st->Top() < SIZE_RAM);

                    m_st->Push(m_RAM[(int)m_st->Pop()]);

                    break;
            }
		}
		else {
            m_st->Destroy();
            m_ret->Destroy();

            //system("pause");

            OK(ERROR_EXECUTE);
        }

	}//end for( ; ; )
	cout << endl << "time = " << (clock() - time_beg)/1000.0 << endl;
	system("pause");

	return 0;
}

ostream& operator << (ostream& out, Instruction str) {

    cout << setw(16) << left << (int)str.CMD_flag << "  " <<
         setw(16) << left << (int)str.CMD_code << "  " <<
         setw(16) << left << (int)str.arg_flag << "  " << str.integer;

    return out;
}

void CPU::dump() {

    HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(cons, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);

    cout << endl;
    cout << "File name:  " << m_fname << endl;
    cout << "=========================================================" << endl;
    cout << "|| " << "time of the last compile          = " << setw(16) << left << m_time_compl << "||" << endl;
    cout << "|| " << "number of commands                = " << setw(16) << left << m_nom_cmd << "||" << endl;
    cout << "|| " << "the number of the current command = " << setw(16) << left << m_current_cmd << "||" << endl;
    cout << "||                                                     ||" << endl;
    cout << "|| " << "REG:                                                " << "||" << endl;
    cout << "|| " << "ax = " << setw(16) << left << m_reg[0] << "                               ||" << endl;
    cout << "|| " << "bx = " << setw(16) << left << m_reg[1] << "                               ||" << endl;
    cout << "|| " << "cx = " << setw(16) << left << m_reg[2] << "                               ||" << endl;
    cout << "|| " << "dx = " << setw(16) << left << m_reg[3] << "                               ||" << endl;
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

void CPU::OK(int stat) {

    if (stat == 0) {


    }
    else {

        if (stat == FILE_NAME_NOT_PASSED) {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
            cout << endl << "the file name was not passed" << endl;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

            system("pause");
        }
        /*if (stat == MORE_ARG_COMMAND_LINE) {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
            cout << endl << "expected 1 argument on the command line, but the resulting = " << argc - 1 << endl;
            cout << "They are:" << endl;

            for (int i = 1; i < argc; i++)
                cout << argv[i] << endl;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

            system("pause");
        }*/
        if (stat == FAILED_OPEN_FILE) {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

            cout << endl << "failed to open a file with the name:   " << m_fname << endl;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
            exit(FAILED_OPEN_FILE);
        }
        if (stat == F_ALLOCATE_MEMORY) {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

            cout << endl << "failed to allocate memory for instructions" << endl;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
            exit(F_ALLOCATE_MEMORY);
        }
        if (stat == ERROR_EXECUTE) {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);

            cout << "runtime error" << endl;

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);

            delete [] instr;
            instr = NULL;

            dump();

            exit(ERROR_EXECUTE);
        }

    }

}

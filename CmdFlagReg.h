#pragma once

#include <map>
using namespace std;

struct Instruction {
	union {
		int64_t integer;
		double floating;
	};
	uint16_t CMD_flag;
	uint8_t CMD_code;
	uint8_t arg_flag;
};

//������ static ����� ���������� �� 2 �������!!!
//�� ������������ ��� == 26!!! ��� ������ �� ����!
static map <string, int> CMD = { {"push"  , 9 },
							     {"pop"   , 10},
							     {"sub"   , 11}, //���������
							     {"add"   , 12}, //��������
							     {"mul"   , 13}, //���������
							     {"div"   , 14}, //�������
							     {"in"    , 15}, //���� � ����������
							     {"out"   , 16},
							     {"dump"  , 17},
							     {"end"   , 18}, //����� ���������
								 {"jmp"   , 19}, //������ � ����� ������
								 {"jaz"   , 20}, //������, ���� ��������� �������� > 0
								 {"jlz"   , 21}, //������, ���� ��������� �������� < 0
								 {"jz"    , 22}, //������, ���� ��������� �������� = 0
								 {"dup"   , 23}, //��������� ��������� ������� � Stack
								 {"func"  , 24}, //���������� �������
								 /*{"prfunc", 25},*/
								 {"ret"   , 30}, //���������� ����� ����������� �������
								 {"call"  , 27}, 
								 {"begin" , 28}, //������ main
								 {"sqrt"  , 29}  //������ �� ���������� �����
};

static map <string, int> REG = { {"ax", 1},
								 {"bx", 2},
								 {"cx", 3},
								 {"dx", 4} 
};


/**
��� ���������� �������:
1. �������� ��� ������� � �� ��� � ������� �� ����� ��������� (CMD_name � CMD_num)
2. ++cmd_max
3. �������� ��� ������� � ������ ������ (� ����������� �� �� ����������)
	�) CMD_with_reg - ���� ������������ ��������
	�) CMD_with_NULL - ���� ��� ����������
	�) CMD_with_REGandNUM - ��� �������� ��� �����
4. ++��������������� define
5. �������� #define CMD_"��� �������"
*/

//#define cmd_max  10
#define flag_max 7
#define reg_max  4

#define CMD�          101
#define NUM�          102
#define REG�          103
#define NULL�         104
#define Label_create� 105
#define Label_arg�    106
#define Label_resolv� 107
#define Func�         108
#define Func_resolv�  109
#define SKIP�         110

#define CMD_push   9
#define CMD_pop   10
#define CMD_sub   11
#define CMD_add   12
#define CMD_mul   13
#define CMD_div   14
#define CMD_in    15
#define CMD_out   16
#define CMD_dump  17
#define CMD_end   18
#define CMD_jmp   19
#define CMD_jaz   20
#define CMD_jlz   21
#define CMD_jz    22
#define CMD_dup   23
#define CMD_func  24
#define CMD_ret   30
#define CMD_call  27
#define CMD_begin 28
#define CMD_sqrt  29



#define NOM_CMD_WITH_NULL 10		         //sub, add, mul, div, dump, end, dup, sqrt, ret, begin
static int CMD_with_NULL[NOM_CMD_WITH_NULL] = { 11,  12,  13,  14,   17,  18,  23,   29,  30, 28 };

#define NOM_CMD_WITH_REG 2                 //pop, in  
static int CMD_with_REG[NOM_CMD_WITH_REG] = { 10, 15 };

#define NOM_CMD_WITH_REGandNUM 2                       //push, out
static int CMD_with_REGandNUM[NOM_CMD_WITH_REGandNUM] = {   9, 16 };

#define NOM_CMD_WITH_LABEL 4				   //jmp, jaz, jmp, jz
static int CMD_with_Label[NOM_CMD_WITH_LABEL] = { 19,  20,  21, 22 };


#define REG_ax 1
#define REG_bx 2
#define REG_cx 3
#define REG_dx 4
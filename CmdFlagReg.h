
#include <map>
using namespace std;

struct Instruction {

    int64_t integer;

	uint16_t CMD_flag;
	uint8_t CMD_code;
	uint8_t arg_flag;
};


//не использовать код == 26!!! все пойдет не туда!
map <string, int> CMD = { {"push"  , 9 },
                          {"pop"   , 10},
                          {"sub"   , 11}, //вычитание
                          {"add"   , 12}, //сложение
                          {"mul"   , 13}, //умножение
                          {"div"   , 14}, //деление
                          {"in"    , 15}, //ввод с клавиатуры
                          {"out"   , 16},
                          {"dump"  , 17},
                          {"end"   , 18}, //конец программы
                          {"jmp"   , 19}, //прыжок в любом случае
                          {"jaz"   , 20}, //прыжок, если последний эллемент > 0
                          {"jlz"   , 21}, //прыжок, если последний эллемент < 0
                          {"jz"    , 22}, //прыжок, если последний эллемент = 0
                          {"dup"   , 23}, //дублирует последней элемент в Stack
                          {"func"  , 24}, //обьявление функции
                          /*{"prfunc", 25},*/
                          {"ret"   , 30}, //обозначает конец обьявленной функции
                          {"call"  , 27},
                          {"begin" , 28}, //аналог main
                          {"sqrt"  , 29}, //корень из последнего числа
                          {"mpop"  , 40},
                          {"mpush" , 41}
};

map <string, int> REG = { {"ax", 1},
                          {"bx", 2},
                          {"cx", 3},
                          {"dx", 4}
};

map <string, int> OPR = { {"[", 202},
                          {"]", 203},
                          {"+", 204},
                          {"-", 205}
};

//#define OPR_COLON             201
#define OPR_SQUARE_BRACKET_OP 202
#define OPR_SQUARE_BRACKET_CL 203
#define OPR_PLUS              204
#define OPR_MINUS             205


#define _OPR_          100
#define _CMD_          101
#define _NUM_          102
#define _REG_          103
#define _NULL_         104
#define _Label_create_ 105
#define _Label_arg_    106
#define _Label_resolv_ 107
#define _Func_         108
#define _Func_resolv_  109
#define _SKIP_         110
#define _CMD_RAM_      111

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
#define CMD_mpop  40
#define CMD_mpush 41



#define NOM_CMD_WITH_NULL     10	  //sub, add, mul, div, dump, end, dup, sqrt, ret, begin
int CMD_with_NULL[NOM_CMD_WITH_NULL] = { 11,  12,  13,  14,   17,  18,  23,   29,  30, 28 };

#define NOM_CMD_WITH_REG      2     //pop, in
int CMD_with_REG[NOM_CMD_WITH_REG] = { 10, 15 };

#define NOM_CMD_WITH_REGorNUM 2             //push, out
int CMD_with_REGandNUM[NOM_CMD_WITH_REGorNUM] = {9, 16 };

#define NOM_CMD_WITH_LABEL    4	        //jmp, jaz, jmp, jz
int CMD_with_Label[NOM_CMD_WITH_LABEL] = { 19,  20,  21, 22 };

#define NOM_CMD_WITH_RAM      2         //mpop, mpush
int CMD_with_RUM[NOM_CMD_WITH_RAM] = { 40, 41 };


#define REG_ax 1
#define REG_bx 2
#define REG_cx 3
#define REG_dx 4
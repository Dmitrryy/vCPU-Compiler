
#include "Compiler.h"
#include "CmdFlagReg.h"

char* m_strtok (char* str, const char* skp, unsigned int nom_lex_skip = 0);
int Skip (char** str, const char* skp);
void SkipNon (char** str, const char* skp);
int In (char s, const char* skp);


char* Compiler::read(const char* fname_in) {

	ifstream file(fname_in);

    if (!file.is_open())
        assert(0);
	
	file.seekg(0, ios::end);

	m_len = file.tellg();
	assert(m_len != 0);
	
	file.seekg(0, ios::beg);

	char* m_str = new char [(m_len + 1) * sizeof(char)]();
	assert(m_str != NULL);

	file.getline(m_str, m_len, '\0');

	file.close();

	mis->get_orig_str(m_str, m_len);

	m_str[m_len] = '\0';

	return m_str;
}


int Compiler::output(Instruction* m_instr, const char* fname_out) {

	std::ofstream out(fname_out);
	
	//out << "DMRY";
	time_t sec = time(NULL);

	m_progma_begin++;

	out.write((char*)&sec, sizeof(time_t));                //время компиляции
	out.write((char*)&m_len, sizeof(uint64_t));		   //кол-во команд
	out.write((char*)&m_progma_begin, sizeof(uint64_t));//номер кдманды начала

	for (int i = 0; i < m_len; i++)
		out.write((char*)(m_instr + i), sizeof(Instruction));

	out.close();

	terminate();

	return 0;
}


ptl* Compiler::preprocessor(char* m_str) {

    search_comments(m_str, m_len);

    const char skp[] = {" \n\t\r"};
    char* str_tmp = m_str;

    m_str_pr = new char[m_len](); //можно оптимизировать(уменьшить размер массива)************************************
    ptl* m_ptl = (ptl*)calloc(10, sizeof(ptl));

    uint64_t k = 0;
    m_ptl_len = 0;

    for (uint64_t i = 0; i < m_len; i++, str_tmp++) {

        Skip(&str_tmp, " \n\t\r");
        i = (uint64_t)(str_tmp - m_str);
        assert(i < m_len);

        if (!In(*str_tmp, " \n\t\r[]+-") && *str_tmp != '\0') {
            if (m_ptl_len % 10 == 0 && m_ptl_len != 0) {
                ptl *tmp_ptl = (ptl *) realloc(m_ptl, (m_ptl_len + 10) * sizeof(ptl));
                assert(tmp_ptl != NULL);
                m_ptl = tmp_ptl;
            }
            m_ptl[m_ptl_len].point = m_str_pr + k;
            m_ptl[m_ptl_len++].first_char = i;
        }

        for ( ; !In(*str_tmp, " \n\t\r[]+-") && i < m_len; i++, k++, str_tmp++) {
            if (*str_tmp == '\0')
                break;

            m_str_pr[k] = *str_tmp;

        }

        if (In(*str_tmp, "[]+-")) {
            m_str_pr[k++] = '\0';
            m_str_pr[k++] = *str_tmp;
            m_str_pr[k++] = '\0';

            if (m_ptl_len % 10 == 0 && m_ptl_len != 0) {
                ptl* tmp_ptl = (ptl*) realloc(m_ptl, (m_ptl_len + 10) * sizeof(ptl));
                assert(tmp_ptl != NULL);
                m_ptl = tmp_ptl;
            }
            m_ptl[m_ptl_len].point = m_str_pr + k -2;
            m_ptl[m_ptl_len++].first_char = i;

        }
        else
            m_str_pr[k++] = '\0';
    }

    m_str_pr[k] = '\0';
    m_len_pr = k;


    //for (int i = 0; i < m_ptl_len; i++)
      //  cout << m_ptl[i].point << "  " << m_ptl[i].first_char << endl;

    return m_ptl;
}


int Compiler::search_comments(char* str, uint64_t len) {

	int stat = 0;
	char* beg_comm = NULL;

	for (uint64_t i = 0; i < len; i++) {

        if (str[i] == '/' && i + 1 < len) // /*. . .
            if (str[i + 1] == '*') {
                if (stat == 0)
                    beg_comm = &(str[i]);

                if (stat == 1)
                    mis->mistake(i, WAR_IN_COMMENTS00); //встречен блок комментарий внутри уже открытого блока

                stat = 1;
            }

        if (str[i] == '*' && i + 1 < len) // . . .*/
            if (str[i + 1] == '/') {
                if (stat == 1) {
                    str[i] = str[i + 1] = ' ';
                    stat = 0;
                }
                else {
                    mis->mistake(i, COMMENT_ISNT_OPEN); //символ не закрывает ни одного комментария
                    terminate();
                    exit(COMMENT_ISNT_OPEN);
                }
            }

		if (stat == 1)
			str[i] = ' ';

		if (stat == 0 && i + 1 < len)                     //комментарии вида: // . . . \n
		    if (str[i] == '/' && str[i + 1] == '/') {

		        for ( ; i < len && str[i] != '\n' && str[i] != '\0'; i++)
		            str[i] = ' ';
		    }
	}


	if (stat == 1) {
        mis->mistake((uint64_t) (beg_comm - str), COMMENT_ISNT_CLOSED); // блок комментарий не закрыт
        terminate();

        exit(COMMENT_ISNT_CLOSED);
    }

	return 0;
}

int Compiler::search_code(int code, const int* data, uint64_t nelem) {

	for (uint64_t i = 0; i < nelem; i++) {
		if (code == data[i])
			return 1;
	}

	return 0;
}


Lexem* Compiler::parse(ptl* m_ptl) {

	Lexem* m_lex = new Lexem[m_ptl_len + 1]();
    char stat_br_op = 0;

	for (int i = 0; i < m_ptl_len; i++) {

		auto result = CMD.find(m_ptl[i].point);

		if (result != CMD.end()) {
			//фиксация обьявления функции
			if (result->second == CMD_func) {

			    if (i + 1 < m_ptl_len && func->find(m_ptl[i + 1].point) == func->end()) {
                    func->emplace(m_ptl[i + 1].point, m_nom_cmd);
                    m_lex[i].flag = _SKIP_;
                    m_lex[i].first_char = m_ptl[i].first_char;

                    m_lex[++i].flag = _SKIP_;
                    m_lex[i].first_char = m_ptl[i].first_char;
                }
			    else {
                    mis->mistake(m_ptl[i + 1].first_char, MULTIPLE_IDENTICAL_FUNC);
                    terminate();
                    exit(MULTIPLE_IDENTICAL_FUNC);
			    }
			}
			//фиксация вызова функции
			else if (result->second == CMD_call) {

				m_lex[i] = { _Func_, (int64_t)m_nom_cmd };
                m_lex[i].first_char = m_ptl[i].first_char;

				m_lex[++i] = { _SKIP_, 0 };
                m_lex[i].first_char = m_ptl[i].first_char;

				m_nom_cmd++;
			}
			else {
				m_lex[i] = { _CMD_, result->second };
                m_lex[i].first_char = m_ptl[i].first_char;

				if (result->second == CMD_begin)
					m_progma_begin = m_nom_cmd;

				m_nom_cmd++;
			}
		}
		else if ((result = REG.find(m_ptl[i].point)) != REG.end()) {
            m_lex[i] = {_REG_, result->second};
            m_lex[i].first_char = m_ptl[i].first_char;
        }
        else if ((result = OPR.find(m_ptl[i].point)) != OPR.end()) {
            m_lex[i] = {_OPR_, result->second};
            m_lex[i].first_char = m_ptl[i].first_char;

            if (result->second == OPR_SQUARE_BRACKET_OP) {                         // [ax]  --->  push ax
                m_nom_cmd++;
                stat_br_op = 1;
            }
            else if (result->second == OPR_SQUARE_BRACKET_CL) {

                if (stat_br_op == 0)
                    assert(0);
                else
                    stat_br_op = 0;
            }
            else if (result->second == OPR_PLUS || result->second == OPR_MINUS) {   // [ax + bx]  ---> push ax; push bx; add;

                if (stat_br_op == 1)
                    m_nom_cmd += 2;
                else
                    m_nom_cmd += 3;
            }
        }
		else {
			char* mb_label = strstr(m_ptl[i].point, ":");
			if (mb_label == NULL) {

			    int sum = 0;
			    for (int k = 0; (m_ptl[i].point)[k] != '\0'; k++) {

			        if (isdigit((m_ptl[i].point)[k]))
			            sum = sum * 10 + (m_ptl[i].point)[k] - '0';

			        else {
                        mis->mistake(m_ptl[i].first_char, UNDECLARED_IDENTIFIER);
                        terminate();
                        exit(UNDECLARED_IDENTIFIER);
                    }
			    }
			    m_lex[i] = { _NUM_, sum };
			    m_lex[i].first_char = m_ptl[i].first_char;
			}

			else if (*(mb_label - 1) == '\0' && *(mb_label + 1) != '\0') {

				m_lex[i] = { _Label_arg_, i };
                m_lex[i].first_char = m_ptl[i].first_char;
			}
			else if (*(mb_label + 1) == '\0' && *(mb_label - 1) != '\0') {
				*(mb_label) = '\0';

				if (Label_cr->find(m_ptl[i].point) == Label_cr->end()) {
                    Label_cr->emplace(m_ptl[i].point, m_nom_cmd);

                    m_lex[i] = {_Label_create_, (int64_t) (m_nom_cmd)};
                    m_lex[i].first_char = m_ptl[i].first_char;
                }
				else {
				    mis->mistake(m_ptl[i].first_char, MULTIPLE_IDENTICAL_LABELS_cr);
				    terminate();
				    exit(MULTIPLE_IDENTICAL_LABELS_cr);
				}
			}
			else {
			    mis->mistake(m_ptl[i].first_char, WRONG_CREATED_LABEL00); //встретилось обозначение метки (":")
			    terminate();                                                                        //но в неправильном написании
			    exit(WRONG_CREATED_LABEL00);                                                        //до и после метки должен быть разделительный знак
			}
		}
	}

	uint64_t sit_begin = 0, sit_end = 0;

    //второй обход
	for (uint64_t i = 0; i < m_ptl_len; i++) {

	    //разрешение меток
		if (m_lex[i].flag == _Label_arg_) {

			auto result = Label_cr->find(m_ptl[i].point + 1); //индекс i у лексемы и у указателя на на лексему (m_pointer_str) одинаков

			if (result != Label_cr->end())
				m_lex[i] = { _Label_resolv_, result->second };

			else {
			    mis->mistake(m_ptl[i].first_char, WRONG_CREATED_LABEL01);
			    terminate();
			    exit(WRONG_CREATED_LABEL01);                     //проверить на ошибку в написании и существует ли вообще
			}                                                    //метка под таким именем
		}
		//разрешение вызова функции
		else if (m_lex[i].flag == _Func_) {

			auto result = func->find(m_ptl[i + 1].point);

			if (result != func->end())
				m_lex[i] = { _Func_resolv_, result->second };

			else {
			    mis->mistake(m_ptl[i].first_char, BODY_OF_FUNC_NOTFOUND);
			    terminate();
			    exit(BODY_OF_FUNC_NOTFOUND);
			    //проверить на ошибку в написании и существует ли вообще
			}   //обьявление (тело) функции
		}

		if (m_lex[i].flag == _CMD_ && m_lex[i].integer == CMD_begin) {

			if (sit_begin != 0) {
                mis->mistake(m_ptl[i].first_char, MULTIPLE_IDENTICAL_BEGIN);
                terminate();
                exit(MULTIPLE_IDENTICAL_BEGIN);
			}

			sit_begin = i + 1;
		}

		if (m_lex[i].flag == _CMD_ && m_lex[i].integer == CMD_end) {

			if (sit_end != 0) {
                mis->mistake(m_ptl[i].first_char, MULTIPLE_IDENTICAL_END);
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
	    mis->mistake(m_ptl[sit_begin - 1].first_char, INVALID_LOCATION_BEGIN_END00);
	    mis->mistake(m_ptl[sit_end - 1].first_char, INVALID_LOCATION_BEGIN_END01);
	    terminate();
	    exit(INVALID_LOCATION_BEGIN_END00);
	}

	return m_lex;
}


Instruction* Compiler::semantic_analysis(Lexem* m_lex, ptl* m_ptl) {

	Instruction* m_instr = new Instruction[m_ptl_len]();
	int k = 0;
	
	int cod = 0;

	for (uint64_t i = 0; i < m_ptl_len; i++) {

		if (m_lex[i].flag == _CMD_) {

			m_instr[k].CMD_flag = _CMD_;
			m_instr[k].CMD_code = cod = m_lex[i].integer;

			//комманды без параметра
			if (search_code(cod, CMD_with_NULL, NOM_CMD_WITH_NULL)) {

			    if (i == m_ptl_len - 1 || (i < m_ptl_len && m_lex[i + 1].flag == _CMD_) || (i < m_ptl_len && m_lex[i + 1].flag == _SKIP_) || (i < m_ptl_len && m_lex[i + 1].flag == _Label_create_))
                    m_instr[k].arg_flag = _NULL_;
			    else {
			        mis->mistake(m_ptl[i].first_char, INCORRECT_ARGUMENTS_NULL);
			        terminate();
			        exit(INCORRECT_ARGUMENTS_NULL);
			    }
			    mis->check_line_style(m_ptl[i].first_char, 0, LINE_NULL);
            }

			//команды с параметром REG
			else if (search_code(cod, CMD_with_REG, NOM_CMD_WITH_REG)) {

                m_instr[k].arg_flag = _REG_;
                i++;
                assert(i < m_ptl_len);

                if (m_lex[i].flag == _REG_)
                    m_instr[k].integer = m_lex[i].integer;
                else {
                    mis->mistake(m_ptl[i - 1].first_char, INCORRECT_ARGUMENTS_REG);
                    terminate();
                    exit(INCORRECT_ARGUMENTS_REG);
                }
                mis->check_line_style(m_ptl[i - 1].first_char, m_ptl[i].first_char, LINE_ONE);
            }

			//команды с параметром REG или NUM
			else if (search_code(cod, CMD_with_REGandNUM, NOM_CMD_WITH_REGorNUM)) {

				i++;
				assert(i < m_ptl_len); //что то пошло не так...

				if (m_lex[i].flag == _REG_) {

					m_instr[k].arg_flag = _REG_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else if (m_lex[i].flag == _NUM_) {

					m_instr[k].arg_flag = _NUM_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else {
				    mis->mistake(m_ptl[i - 1].first_char, INCORRECT_ARGUMENTS_REG_or_NUM);
				    terminate();
				    exit(INCORRECT_ARGUMENTS_REG_or_NUM);  //после команды с параметром REG или NUM
				}
                mis->check_line_style(m_ptl[i - 1].first_char, m_ptl[i].first_char, LINE_ONE);
			}

			//Коианды по работе с оперативной памятью
			else if (search_code(cod, CMD_with_RUM, NOM_CMD_WITH_RAM)) {

			    i++;
			    assert(i < m_ptl_len);

			    if (m_lex[i].flag != _OPR_ || m_lex[i].integer != OPR_SQUARE_BRACKET_OP)
			        assert(0);

			    i++;
			    assert(i < m_ptl_len);

			    //int stat_opr = 0;

			    for ( ; i < m_ptl_len; i++) {
                    if (m_lex[i].flag == _OPR_ && m_lex[i].integer == OPR_SQUARE_BRACKET_CL)
                        break;

                    if (m_lex[i].flag == _REG_ || m_lex[i].flag == _NUM_) {

                        m_instr[k].CMD_flag = _CMD_;
                        m_instr[k].CMD_code = CMD_push;
                        m_instr[k].arg_flag = m_lex[i].flag;
                        m_instr[k++].integer  = m_lex[i].integer;
                    }

                    if (m_lex[i].flag == _OPR_ && (m_lex[i].integer == OPR_PLUS || m_lex[i].integer == OPR_MINUS)) {

                        i++;
                        assert(i < m_ptl_len);

                        if (m_lex[i].flag == _REG_ || m_lex[i].flag == _NUM_) {

                            m_instr[k].CMD_flag = _CMD_;
                            m_instr[k].CMD_code = CMD_push;
                            m_instr[k].arg_flag = m_lex[i].flag;
                            m_instr[k++].integer  = m_lex[i].integer;
                        }
                        else
                            assert(0);

                        m_instr[k].CMD_flag = _CMD_;

                        if (m_lex[i - 1].integer == OPR_PLUS)
                            m_instr[k++].CMD_code = CMD_add;
                        else if (m_lex[i - 1].integer == OPR_MINUS)
                            m_instr[k++].CMD_code = CMD_sub;
                        else assert(0);
                    }
			    }
                m_instr[k].CMD_flag = _CMD_RAM_;
                m_instr[k].CMD_code = cod;
                m_instr[k].arg_flag = _NULL_;
			}

			//Команды "прыжка" (jmp и тому подобное)
			else if (search_code(cod, CMD_with_Label, NOM_CMD_WITH_LABEL)) {

				i++;
				if (m_lex[i].flag == _Label_resolv_) {

					m_instr[k].arg_flag = _Label_resolv_;
					m_instr[k].integer = m_lex[i].integer;
				}
				else {
				    mis->mistake(m_ptl[i - 1].first_char, INCORRECT_ARGUMENTS_LABEL);
				    terminate();
				    exit(INCORRECT_ARGUMENTS_LABEL); //после команды прыжка идет не метка! куда прыгать?
				}
                mis->check_line_style(m_ptl[i - 1].first_char, m_ptl[i].first_char, LINE_ONE);
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
		else if (m_lex[i].flag == _NUM_ || m_lex[i].flag == _REG_ || m_lex[i].flag == _NULL_){
		    mis->mistake(m_ptl[i].first_char, UNKNOWN_ARGUMENT);
		    terminate();
		    exit(UNKNOWN_ARGUMENT);
        }
		else {assert(0);}
	}
	m_len = k;

	if (mis->m_nom_war != 0 || mis->m_nom_err) {
	    cout << endl << "warning: " << mis->m_nom_war << " and error: " << mis->m_nom_err << endl;
	    //system("pause");
	}
	for (int h = 0; h < m_len; h++)
		cout << h << "  " << (int)m_instr[h].CMD_flag << "  " << (int)m_instr[h].CMD_code << "  " << (int64_t)m_instr[h].arg_flag << "  " << m_instr[h].integer << endl;

	return m_instr;
}


void Compiler::terminate() {
    if (m_str_pr != NULL) {
        delete [] m_str_pr;
        m_str_pr = NULL;
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
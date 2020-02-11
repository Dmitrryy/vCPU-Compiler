#pragma once
#include "Stack.h"

using namespace std;

struct StPoint* fixations(LPVOID st, size_t n_page, bool mod);
void terminate(int signum);

/**
 * исользуется для создания Stack
 */
#define CREATE_STACK(T, name, size); \
 signal (SIGSEGV, terminate); \
 Stack <T>* name = (Stack <T>*) VirtualAlloc (NULL, size * sizeof(T) + sizeof(Stack<T>), MEM_COMMIT, PAGE_READONLY); \
 name->Create(size); \


 /**
  * Функция, создающая Stack
  * Инициализирует все поля класса(где это требуется) и фиксирует созданный Stack
  * в функции fixations
  * @tparam T - тип данных в Stack
  * @param size - размер Stack. Т.к. Stack на виртуальной памяти, то мин размер = 4кб
  * (один лист оперативной памяти), т.е если даже передать в функцию значение < 1024(для int)
  * то будет создан Stack на +-1024 (некоторое место займут поля класса)
  */
template <typename T>
void Stack<T>::Create(int size) {

	if (size <= 0)
		OK(NEG_SIZE_STACK);
	OK(0);

	unsigned long h = 0;
	VirtualProtect(this, size * sizeof(T) + sizeof(Stack<T>), PAGE_READWRITE, &h);
	if (h != PAGE_READONLY)
		OK(ERROR_CHANGE_PROTECT_IN_CR);

	if (m_data != NULL)
		OK(STACK_IS_CREATED);

	m_nom_page = ceil((size * sizeof(T) + sizeof(Stack<T>)) / PAGE_SIZE) + 1;
	m_size = (PAGE_SIZE * m_nom_page - sizeof(Stack<T>)) / sizeof(T);

	m_data = (T*)((char*)this + sizeof(Stack<T>));

	fixations(this, m_nom_page, true);

	OK(0);

#ifndef DISABLE_CHECK_PROTECT
	VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READONLY, &h);
	if (h != PAGE_READWRITE)
		OK(ERROR_CHANGE_PROTECT_IN_CR);
#endif
}

/**
 * Функция для удаления Stack
 * @tparam T - тип Stack
 */
template <typename T>
void Stack<T>::Destroy() {

#ifndef DISABLE_CHECK_PROTECT
	unsigned long h = 0;
	VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READWRITE, &h);
	if (h != PAGE_READONLY)
		OK(ERROR_CHANGE_PROTECT);
#endif
    
	fixations(this, 0, false);
	
	VirtualFree(this, 0, MEM_RELEASE); ///Удаление стэка из числа "активных"
}

/**
 * Функция, которая помещает число(или символ, указатель и т.д) в Stack
 *
 * @tparam T - тип данных
 * @param value - число, которое необходимо положить в Stack
 */
template <typename T>
void Stack<T>::Push(T value) {
#ifndef DISABLE_CHECK_PROTECT
	unsigned long h = 0;
	VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READWRITE, &h);
	if (h != PAGE_READONLY)
		OK(ERROR_CHANGE_PROTECT);
#endif

	OK(0);

	if (m_n_now == m_size)
		OK(REACH_MAX);

	m_data[m_n_now++] = value;
	OK(0);

#ifndef DISABLE_CHECK_PROTECT
	VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READONLY, &h);
	if (h != PAGE_READWRITE)
		OK(ERROR_CHANGE_PROTECT);
#endif
}

/**
 * Возвращает число, лежащее наверху Stack (удаляя его из Stack)
 *
 * @tparam T - тип данных
 * @return число, лежащее на верху Стэк'а
 */
template <typename T>
T Stack<T>::Pop() {

#ifndef DISABLE_CHECK_PROTECT
	unsigned long h = 0;
	VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READWRITE, &h);
	if (h != PAGE_READONLY)
		OK(ERROR_CHANGE_PROTECT);
#endif

	OK(0);

	if (m_n_now - 1 < 0)
		OK(REACH_MIN);

	T ret = m_data[--m_n_now];
	m_data[m_n_now] = 0;

	OK(0);

#ifndef DISABLE_CHECK_PROTECT
	VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READONLY, &h);
	if (h != PAGE_READWRITE)
		OK(ERROR_CHANGE_PROTECT);
#endif

	return ret;
}

template <typename T>
T Stack<T>::Top() {

#ifndef DISABLE_CHECK_PROTECT
    unsigned long h = 0;
    VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READWRITE, &h);
    if (h != PAGE_READONLY)
        OK(ERROR_CHANGE_PROTECT);
#endif

    OK(0);

    if (m_n_now <= 0)
        OK(STACK_IS_EMPTY);

    T ret = m_data[m_n_now - 1];

    OK(0);

#ifndef DISABLE_CHECK_PROTECT
    VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READONLY, &h);
    if (h != PAGE_READWRITE)
        OK(ERROR_CHANGE_PROTECT);
#endif

    return ret;
}

/**
 * функция, проверяющая правильность работы Stack
 *
 * @tparam T - тип данных
 * @param stat - код ошибки (если все нормально, то stat == 0)
 */
template <typename T>
void Stack<T>::OK(int stat) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	if (stat == 0) {
		if (this == NULL) {
			cout << endl << "The pointer to Stack is zero (it may not have been created)." << endl
				<< "Try calling the \"Create\" function" << endl;
			terminate(STACK_NOT_FOUND);
		}
	}
	else {
	    if (stat == STACK_IS_EMPTY) {
            cout << endl << "\"Top\" function is used when Stack is empty" << endl;
            Dump();
            terminate(STACK_IS_EMPTY);
	    }
		if (stat == NEG_SIZE_STACK) {
			cout << endl << "In the stack creation function (CREATE_STACK(T, name, size);) pass a negative size" << endl
				<< "Cannot create a stack of negative size" << endl;
			Dump();
			terminate(NEG_SIZE_STACK);
		}
		if (stat == STACK_IS_CREATED) {
			cout << endl << "Stack is created several times" << endl
				<< "Remove the old before creating a new it" << endl;
			Dump();
			terminate(STACK_IS_CREATED);
		}
		if (stat == REACH_MIN) {
			cout << endl << "\"Pop\" function is used when Stack is empty" << endl;
			Dump();
			terminate(REACH_MIN);
		}
		if (stat == ERROR_CHANGE_PROTECT) {
			cout << endl << "failed to change protection mode" << endl;
			Dump();
			terminate(ERROR_CHANGE_PROTECT);
		}
		if (stat == ERROR_CHANGE_PROTECT_IN_CR) {
			cout << endl << "failed to change protection mode when creating stack" << endl;
			Dump();
			terminate(ERROR_CHANGE_PROTECT_IN_CR);
		}
		if (stat == REACH_MAX) {
			cout << endl << "\"Push\" function is used when Stack is full" << endl
				<< "it is not possible to put an item in a full Stack" << endl;
			Dump();
			terminate(REACH_MAX);
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 | 15);
}

template <typename T>
void Stack<T>::Dump() {
	//ofstream fout;
	//fout.open("ferror.txt");

	/*#ifndef DISABLE_CHECK_PROTECT
		unsigned long h = 0;
		VirtualProtect(this, PAGE_SIZE * m_nom_page, PAGE_READONLY, &h);
		if (h != PAGE_READONLY)
			OK(ERROR_CHANGE_PROTECT);
	#endif*/

	HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(cons, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	cout << "***************************************" << endl;

	if(this == NULL)
		SetConsoleTextAttribute(cons, FOREGROUND_RED);
	cout << "|| this          = " << setw(16) << left << this << "  ||" << endl;

	SetConsoleTextAttribute(cons, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	cout << "|| data          = " << setw(16) << left << m_data << "  ||" << endl;

	if(m_size == m_n_now)
		SetConsoleTextAttribute(cons, FOREGROUND_RED);
	cout << "|| size          = " << setw(16) << left << m_size << "  ||" << endl;

	SetConsoleTextAttribute(cons, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	cout << "|| type (sizeof) = " << setw(16) << left << sizeof(T) << "  ||" << endl;

	if (m_size == m_n_now || m_n_now == 0)
		SetConsoleTextAttribute(cons, FOREGROUND_RED);
	cout << "|| nom elements  = " << setw(16) << left << m_n_now << "  ||" << endl;

	SetConsoleTextAttribute(cons, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	cout << "|| nom page      = " << setw(16) << left << m_nom_page << "  ||" << endl;
	cout << "***************************************" << endl;

	if (m_n_now > 0) {
		SetConsoleTextAttribute(cons, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		cout << "content data: " << endl;

		for (int i = 0; i < m_n_now; i++)
			cout << "[" << i << "]  " << m_data[i] << endl;
	}
	
	SetConsoleTextAttribute(cons, 0 | 15);
}


/**
 * структура, хранящая необходимою информацию о Stack для его удаления:
 * ptr - указатель на Stack
 * page - кол-во страниц в Stack
 */
struct StPoint {
	LPVOID ptr;
	size_t page;
};

/**
 * Освобождает память, которую занимаю все созданные стэки за время работы программы
 * Вызывается автоматически при возникновении ошибки
 *
 * @param signum - код сигнала об ошибке
 */
void terminate(int signum) {

	if (signum == 11) {
		cout << "an unauthorized attempt to change the contents of Stack was recorded" << endl;
		cout << "Interrupt signal (" << signum << ") received.\n" << endl;
	}

	StPoint* del = {};
	for (int i = 0; (del = fixations(NULL, 0, false)) != NULL; i++) {
		unsigned long h = 0;
		VirtualProtect(del->ptr, PAGE_SIZE * del->page, PAGE_READWRITE, &h);
		if (del->ptr != NULL)
			VirtualFree(del->ptr, 0, MEM_RELEASE);
	}
    system("pause");
	exit(signum);
}

void my_realloc(StPoint** arr, size_t elem) {

	StPoint* tmp = new StPoint[elem + 10];
	memcpy(tmp, *arr, elem * sizeof(StPoint));
	delete[](*arr);
	*arr = tmp;
}

/**
 * фиксирует созданные (и не удаленные ранее) структуры Stack
 * Два режима работы:
 * при mod == true:
 * кладет в конец статического массива структур структуру о st
 *
 * при mod == false:
 * удаляет из массива структуру с указателем st
 * если st == NULL - вытаскивает последнюю в массиве структуру и возвращает на нее указатель (удалив ее из массива)
 *
 * @param st - указатель на Stack
 * @param n_page - кол-во страниц в Stack
 * @param mod - режим работы программы(флаг)
 * @return указатель на структуру с информацией о Stack
 */
struct StPoint* fixations(LPVOID st, size_t n_page, bool mod) {

	static struct StPoint* add = new StPoint[10];
	static int i = 0; // указывает на следующий свободный элемент массива

	if (mod == true) {
		if (i % 10 == 9)
			my_realloc(&add, i);

		add[i].page = n_page;
		add[i++].ptr = st;
		return NULL;
	}
	else {
		if (st == NULL && i > 0) //если st == NULL возвращается последний лежащий элемент
			return add + (--i);
		else
			for (int k = i - 1; k >= 0; k--) { //иначе ищется переданная структура в базе данных и вытаскивается от туда
				if (add[k].ptr == st) {
					memmove(add + k, add + k + 1, (i - k) * sizeof(StPoint*)); //Закрытие "дырки" после удаления
					--i;
					add[i].page = 0;
					add[i].ptr = NULL;
					return NULL;
				}
			}
	}
	return NULL; //возвращается NULL если в базе пусто (илиесли в базу добавили элемент)
}
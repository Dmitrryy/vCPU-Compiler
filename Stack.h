
#ifndef STACK_2_STACK_H
#define STACK_2_STACK_H

#include "error.h"

///размер страници
#define PAGE_SIZE 4096

///отключает защиту
#define DISABLE_CHECK_PROTECT

#include <cstddef>
#include <iostream>
#include <cctype>
#include <functional>
#include <ctime>
#include <windows.h>
#include <fstream>
#include <cmath>
#include <csignal>
#include <iomanip>

/**
 * класс по работе с Stack
 *
 * @tparam T - тип данных
 * @param m_data - указатель на первый элемент
 * @param m_size - макс кол-во элементов в Stack
 * @param m_n_now - кол-во лежащих в Stack элементов
 * @param m_nom_page - кол-во страниц в Stack
 */
template <typename T>
class Stack {
private: T* m_data = NULL;
private: int m_size = 0;
private: int m_n_now = 0;
private: int m_nom_page = 0;

public: void OK(int stat);

public: void Dump();

public: void Create(int size);
public: void Destroy();
public: void Push(T value);
public: T Pop();
};


#endif //STACK_2_STACK_H

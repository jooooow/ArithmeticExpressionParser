/***********************************************
Copyright (c) 2017 jooooow All rights reserved
Author : jooooow
Date : 2017-08-21
File : Main.cpp
Description : a test of the arithmetic expression parser class
************************************************/

#define DEBUG
#include <iostream>
#include "Calculator.h"

#ifdef DEBUG
inline void enableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
#endif

bool input(Calculator& calc, const char* source)
{
	if (strlen(source) == 0)						/*if source is empty, reinput*/
		return true;
	if (!calc.input(source))						/*input the expression to calculator*/
	{
		std::cout << "\n\n[press any key to exit]";
		getchar();
		return false;
	}
	if (!calc.isError())
		calc.printResult();
	else
		calc.printError(calc.getErrorInformtion());
	return true;
}

int main(int argc, char* argv[])
{
#ifdef DEBUG
	std::cout << "debug :\n";
	enableMemLeakCheck();
#endif
	Calculator calc;
	char source_arr[MAX_SIZE];
	std::cout << "..........CALCULATOR..........\n\n";
	if (argc > 1)											/*input from console parameter*/
	{
		std::cout << "Input an expression ください : " << argv[1] << '\n';
		bool isQuit = !input(calc, source_arr);
		if (isQuit)
			return 0;
	}
	while (1)
	{
		std::cout << "Input an expression ください : ";
		std::cin.getline(source_arr, MAX_SIZE);				/*input the expression*/
		bool isQuit = !input(calc, source_arr);
		if (isQuit)
			return 0;
	}
	return 0;
}
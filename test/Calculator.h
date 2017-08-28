/***********************************************
Copyright (c) 2017 jooooow All rights reserved
Author : jooooow
Date : 2017-08-21
File : Calculator.h
Description : an arithmetic expression parser class
************************************************/

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define MAX_SIZE 100										/*max array capacity*/
#include <list>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>    
#include <crtdbg.h> 
#include <iostream>
#include <string>

typedef std::string stl_string;
typedef std::list<char> char_list;
typedef std::list<stl_string> string_list;
typedef std::vector<double> double_vector;

class Calculator
{	
	enum COMMAND
	{
		/****************************************
		COMMAND		action
		-q			quit
		-h			return the previous result
		-s			accumulate previous results
		-a          return all history results
		*****************************************/
		GENERAL = -1,
		QUIT = 0,
		GET_HISTORY = 1,
		ACC = 2,
		ALL = 3
	};
	struct CalcState
	{
		int mode;
		int source_length;
		int result_dev;
		bool is_error;
		int error_code;
		int error_position;
	};

private:
	char* output_buffer;									/*buffer to store output*/
	char* buffer_ptr;										/*pointer of buffer element*/
	const char* source;										/*source array*/
	CalcState state;										/*the state of calculator*/
	char_list stack;										/*list to store symbol temporarily*/
	string_list postfix;									/*list to store postfix expression*/
	double_vector result;									/*vector to store historical result*/

private:
	const stl_string legal_chars = "+-*/0123456789().";	/*legal characters*/

public:
	Calculator();											/*default constructor*/
	Calculator(const Calculator& calc);
	Calculator(const char* source);							
	~Calculator();											/*destructor*/
	bool input(const char* source);							/*store the source array and check the mode*/
	void initCalc();										/*initializate calculator(clear lists, reset ints)*/
	void calculate();										/*check legality, convert to postfix expression, process result*/
	char* c_getResult();									/*return the result of c style*/
	stl_string getResult();									/*return the result of stl style*/
	double getResultAcc();									/*accumulate all result*/
	const char* getErrorInformtion() const;					/*return the error information*/
	bool isError() const;									/*return true if error occur*/
	void printResult();										/*print result or error information*/
	void printError(const char* error);						/*formatted print error*/

private:
	/////////////////////////////////////
	
	bool checkCommandLegality();
	//bool checkExpressionLegality();
	//bool checkNumberLegality();

	/////////////////////////////////////
	int checkMode();										/*check mode*/
	bool checkLegality();									/*check the barcket matching, check the symbols*/
	bool convertToPostfixExpression();						/*convert infix expression to postfix expression*/
	bool processResult();									/*calculate result from postfix expression, push into result stack*/

private:
	stl_string convertDoubleToString(double& val);			/*convert double to string*/
	void storeDoubleInCharArray(double& val);
	bool isSymbolsLegal();									/*check if symbols legal*/
	bool isBarcketMatch();									/*check if barcket matching*/
	bool isNumber(const char& ch);							/*check if the char is number( 0123456789. )*/
	bool isSymbol(const stl_string& str);					/*check if the char is symbol ( +-/*)( )*/
	bool isNumberLegal(const stl_string& str);				/*check if the number legal*/
	bool isPrior(const char& ch1, const char& ch2);			/*check if ch1 is prior to ch2*/
};
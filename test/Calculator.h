/***********************************************
Copyright (c) 2017 jooooow All rights reserved
Author : jooooow
Date : 2017-08-21
File : Calculator.h
Description : an arithmetic expression parser class
************************************************/

#pragma once
#include <list>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>    
#include <crtdbg.h> 
#include <iostream>
#define MAX_SIZE 100										/*max array capacity*/

class Calculator
{
	/*	
		COMMAND		action
		-q			quit
		-h			show the previous result
		-s			accumulate previous results
	*/
	enum COMMAND
	{
		GENERAL = -1,
		QUIT = 0,
		GET_HISTORY = 1,
		ACC = 2
	};
private:
	int error_code;											/*error code*/
	int error_pos;											/*error position*/
	int source_length;										/*the length of source array*/
	double current_result;									/*current result*/
	int result_dev;											/*dev to get history result*/
	int mode;												/*mode*/
	const char* source;										/*source array*/
	std::list<char> stack;									/*list to store symbol temporarily*/
	std::list<std::string> postfix;							/*list to store postfix expression*/
	std::vector<double> result;								/*vector to store historical result*/

private:
	const std::string legal_chars = "+-*/0123456789().";	/*legal characters*/

public:
	Calculator();											/*default constructor*/
	~Calculator();											/*destructor*/
	bool input(const char* source);							/*store the source array and check the mode*/
	void initCalc();										/*initializate calculator(clear lists, reset ints)*/
	void calculate();										/*check legality, convert to postfix expression, process result*/
	double getResult();										/*return the result*/
	double getResultAcc();									/*accumulate all result*/
	const char* getErrorInformtion() const;					/*return the error information*/
	bool isError() const;									/*return true if error occur*/
	void printResult();										/*print result or error information*/

private:
	int checkMode();										/*check mode*/
	bool checkLegality();									/*check the barcket matching, check the symbols*/
	bool convertToPostfixExpression();						/*convert infix expression to postfix expression*/
	bool processResult();									/*calculate result from postfix expression, push into result stack*/

private:
	std::string convertDoubleToString(double& val);			/*convert double to string*/
	void printError(const char* error);						/*formatted print error*/
	bool isSymbolsLegal();									/*check if symbols legal*/
	bool isBarcketMatch();									/*check if barcket matching*/
	bool isNumber(const char& ch);							/*check if the char is number( 0123456789. )*/
	bool isSymbol(const std::string& str);					/*check if the char is symbol ( +-/*)( )*/
	bool isNumberLegal(const std::string& str);				/*check if the number legal*/
	bool isPrior(const char& ch1, const char& ch2);			/*check if ch1 is prior to ch2*/
};
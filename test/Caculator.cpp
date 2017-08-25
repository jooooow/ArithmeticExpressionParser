/***********************************************
Copyright (c) 2017 jooooow All rights reserved
Author : jooooow
Date : 2017-08-21
File : Calculator.cpp
Description : an arithmetic expression parser class
************************************************/

#include "Calculator.h"

#pragma region Calculator functions
/*******************************
Calculator : public
********************************/
Calculator::Calculator() : source_length(0), error_code(-1), error_pos(-1)
{

}
Calculator::~Calculator()
{
	source = nullptr;
}
bool Calculator::input(const char* source)
{
	initCalc();

	this->source = source;									/*store source*/
	source_length = strlen(source);							/*get the length of expression*/

															/******************************
															0.general calculate
															1.get history result
															*******************************/
	mode = checkMode();
	if (mode == QUIT)
		return false;
	if (mode == GET_HISTORY ||
		mode == ACC)
		return true;
	calculate();
	result_dev = 0;
	return true;
}
void Calculator::initCalc()
{
	source_length = 0;
	error_code = -1;
	error_pos = -1;
	postfix.clear();						/*reset posfix expression*/
	stack.clear();							/*reset stack*/
}
void Calculator::calculate()
{
	bool isLegal = checkLegality();			/*check legality*/
	if (!isLegal)
		return;
	/*convert infix expression to postfix expression*/
	bool isConvertSucess = convertToPostfixExpression();
	if (!isConvertSucess)
		return;

	bool isProcessSuccess = processResult();/*process the postfix expression*/
	if (!isProcessSuccess)
		return;
}
double Calculator::getResultAcc()
{
	double sum = 0;
	for (auto ele : result)
		sum += ele;
	return sum;
}
double Calculator::getResult()
{
	return current_result;					/*return the top of result*/
}
const char* Calculator::getErrorInformtion() const
{
	switch (error_code)										/*return error information*/
	{
	case 0:
		return "valid character";
		break;
	case 1:
		return "valid number";
		break;
	case 2:
		return "missing symbol";
		break;
	case 3:
		return "missing number";
		break;
	case 4:
		return "extra symbol";
		break;
	case 5:
		return "extra number";
		break;
	case 6:
		return "barcket mismatching";
		break;
	case 7:
		return "math error";
		break;
	case 8:
		return "no more history";
		break;
	}
	return NULL;
}
bool Calculator::isError() const
{
	return error_code == -1 ? false : true;
}
void Calculator::printResult()
{
	if (mode == GET_HISTORY)
	{
		if (result_dev > result.size() - 1)
		{
			error_code = 8;
		}
		else
		{
			current_result = result.at(result.size() - result_dev - 1);
			result_dev++;
		}
	}
	else if (mode == GENERAL)
	{
		current_result = result.back();
	}
	else if (mode == ACC)
	{
		current_result = getResultAcc();
	}

	if (isError())
	{
		printError(getErrorInformtion());
	}
	else
	{
		double res = getResult();
		std::cout << "\nReuslt : " << res << "\n\n";		/*print result*/
	}
}
/*******************************
Calculator : private : general function
********************************/
int Calculator::checkMode()
{
	if (source_length == 2 &&								/*check if quit*/
		source[0] == '-' &&
		source[1] == 'q')
		return QUIT;
	if (source_length == 2 &&								/*check if get history*/
		source[0] == '-' &&
		source[1] == 'h')
		return GET_HISTORY;
	if (source_length == 2 &&								/*check if accumulate*/
		source[0] == '-' &&
		source[1] == 's')
		return ACC;
	return GENERAL;
}
bool Calculator::checkLegality()
{
	bool isMatch = isBarcketMatch();		/*check the matching of barcket*/
	if (!isMatch)
		return false;

	bool symbolLegalFlag = isSymbolsLegal();/*check the legality of symbols*/
	if (!symbolLegalFlag)
		return false;

	return true;
}
bool Calculator::convertToPostfixExpression()
{
	for (int i = 0; i < source_length; i++)
	{
		char current_char = source[i];		/*get current character*/
		if (isNumber(current_char))			/*if the character is number*/
		{
			std::string number = "";
			while (isNumber(current_char))	/*joint next character if it is a number*/
			{
				number += current_char;
				if (i == source_length - 1)
					break;
				current_char = source[++i];
			}
			if (!isNumberLegal(number))		/*cheack the legality of the number string*/
			{
				error_code = 1;				/*raise error*/
				return false;
			}
			postfix.push_back(number);		/*push the number string into postfix_expression*/
		}
		if (i == source_length - 1)
			break;
		if (stack.empty() || stack.back() == '(' && current_char != ')')
		{
			stack.push_back(current_char);
			continue;
		}
		/*if current character is prior to top, then push*/
		if (isPrior(current_char, stack.back()))
		{
			stack.push_back(current_char);
		}
		else if (current_char == ')')		/*if current character is ')', then pop all*/
		{
			while (stack.back() != '(')		/*pop until '('*/
			{
				postfix.push_back(std::string(1, stack.back()));
				stack.pop_back();
			}
			stack.pop_back();
		}
		else								/*if current character is not prior to top, then pop top*/
		{
			postfix.push_back(std::string(1, stack.back()));
			stack.pop_back();
			stack.push_back(current_char);
		}
	}
	while (!stack.empty())					/*pop all symbol*/
	{
		if (stack.back() != '(')			/*skip '('*/
			postfix.push_back(std::string(1, stack.back()));
		stack.pop_back();
	}
	return true;
}
bool Calculator::processResult()
{
	/*copy postfix to vector*/
	std::vector<std::string> vec(postfix.begin(), postfix.end());
	int index = 0;
	while (vec.size() != 1)					/*loop until get result*/
	{
		if (isSymbol(vec.at(index)))
		{
			double temp;
			char ch = vec.at(index).at(0);	/*choose the calculation*/
			switch (ch)
			{
				/**********************************
				1.convert string to double
				2.calculate
				3.convet double to string
				4.insert the result and delete used characters
				***********************************/
			case '+':
				temp = atof(vec.at(index - 2).c_str()) + atof(vec.at(index - 1).c_str());
				vec.insert(vec.begin() + index - 2, convertDoubleToString(temp));
				vec.erase(vec.begin() + index - 1, vec.begin() + index + 2);
				index = -1;
				break;
			case '-':
				temp = atof(vec.at(index - 2).c_str()) - atof(vec.at(index - 1).c_str());
				vec.insert(vec.begin() + index - 2, convertDoubleToString(temp));
				vec.erase(vec.begin() + index - 1, vec.begin() + index + 2);
				index = -1;
				break;
			case '*':
				temp = atof(vec.at(index - 2).c_str()) * atof(vec.at(index - 1).c_str());
				vec.insert(vec.begin() + index - 2, convertDoubleToString(temp));
				vec.erase(vec.begin() + index - 1, vec.begin() + index + 2);
				index = -1;
				break;
			case '/':
				/*if divisor is zero, then skip*/
				double divisor = atof(vec.at(index - 1).c_str());
				if (divisor == 0)
				{
					error_code = 7;
					error_pos = index;
					return false;
				}
				temp = atof(vec.at(index - 2).c_str()) / atof(vec.at(index - 1).c_str());
				vec.insert(vec.begin() + index - 2, convertDoubleToString(temp));
				vec.erase(vec.begin() + index - 1, vec.begin() + index + 2);
				index = -1;
				break;
			}
		}
		index++;
	}
	result.push_back(atof(vec.at(0).c_str()));/*store result*/
	return true;
}
/*******************************
Calculator : private : tool function
********************************/
std::string Calculator::convertDoubleToString(double& val)
{
	char buf[20];
	sprintf_s(buf, "%f", val);
	std::string str = buf;
	return str;
}
bool Calculator::isBarcketMatch()
{
	return true;
}
bool Calculator::isSymbolsLegal()
{
	for (int i = 0; i < source_length; i++)
	{
		char ch = source[i];
		/* 1@2+3 */
		if (legal_chars.find(ch) == std::string::npos)
		{
			error_code = 0;
			error_pos = i;
			return false;
		}
		/* 1(2+3) */
		if (ch == '(' && i != 0 && isNumber(source[i - 1]))
		{
			error_code = 2;
			error_pos = i;
			return false;
		}
		/*1+(2+3)4 */
		if (ch == ')' && i < source_length - 1 && isNumber(source[i + 1]))
		{
			error_code = 2;
			error_pos = i;
			return false;
		}
		/* 1+2+3+ */
		if (isSymbol(std::string(1, ch)) && i == source_length - 1)
		{
			error_code = 3;
			error_pos = i;
			return false;
		}
		/* 1++2+3 */
		if (isSymbol(std::string(1, ch)) && i < source_length - 1 && isSymbol(std::string(1, source[i + 1])))
		{
			error_code = 3;
			error_pos = i;
			return false;
		}
		/* 1+2+()+3 */
		if (ch == '(' && i < source_length - 1 && source[i + 1] == ')')
		{
			error_code = 3;
			error_pos = i;
			return false;
		}
		/* 1+((2+3)*)+4 */
		if (isSymbol(std::string(1, ch)) && i < source_length - 1 && source[i + 1] == ')')
		{
			error_code = 4;
			error_pos = i;
			return false;
		}
		/* 1+(*(2+3))+4 */
		if (isSymbol(std::string(1, ch)) && source[i - 1] == '(')
		{
			error_code = 4;
			error_pos = i;
			return false;
		}
	}
	return true;
}
void Calculator::printError(const char* error)
{
	std::cout << "\n  ERROR : " << error;
	if (error_pos != -1)
		std::cout << " : No." << error_pos << "\n\n";
	else
		std::cout << "\n\n";
}
bool Calculator::isNumber(const char& ch)
{
	if (ch >= '0' && ch <= '9' || ch == '.')
		return true;
	else
		return false;
}
bool Calculator::isSymbol(const std::string& str)
{
	if (str == "+" ||
		str == "-" ||
		str == "*" ||
		str == "/" ||
		str == "." ||
		str == "¡Ì")
		return true;
	else
		return false;
}
bool Calculator::isNumberLegal(const std::string& str)
{
	bool hasDot = false;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '.')
		{
			if (!hasDot)
				hasDot = true;
			else
				return false;
		}
		if (!isNumber(str[i]))
			return false;
	}
	return true;
}
bool Calculator::isPrior(const char& ch1, const char& ch2)
{
	if (ch1 == '(')							/*'(' is prior to all symbols*/
		return true;
	if ((ch1 == '*' || ch1 == '/') && (ch2 == '+' || ch2 == '-'))
		return true;
	else
		return false;
}
#pragma endregion
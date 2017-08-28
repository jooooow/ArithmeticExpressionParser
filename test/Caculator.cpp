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
Calculator::Calculator()
{
	initCalc();
	output_buffer = new char[MAX_SIZE * sizeof(double) + MAX_SIZE];
	buffer_ptr = output_buffer;
}
Calculator::Calculator(const Calculator& calc)
{
	//TODO
}
Calculator::Calculator(const char* source)
{
	//TODO
}
Calculator::~Calculator()
{
	delete output_buffer;
	source = nullptr;
	output_buffer = nullptr;
}
bool Calculator::input(const char* source)
{
	initCalc();

	this->source = source;											/*store source*/
	state.source_length = strlen(source);							/*get the length of expression*/
															/******************************
															0.general calculate
															1.get history result
															*******************************/
	state.mode = checkMode();
	if (state.mode == QUIT)
		return false;
	if (state.mode == GET_HISTORY ||
		state.mode == ACC ||
		state.mode == ALL)
	{
		checkCommandLegality();
		return true;
	}
		
	calculate();
	state.result_dev = 0;
	return true;
}
bool Calculator::checkCommandLegality()
{
	//temp ��
	if (state.mode == GET_HISTORY)
	{
		if (state.result_dev > result.size() - 1)
		{
			state.error_code = 8;
			return false;
		}
	}
	if (state.mode == ALL && result.size() == 0)
	{
		state.error_code = 8;
		return false;
	}
}
void Calculator::initCalc()
{
	state.source_length = 0;
	state.error_code = -1;
	state.error_position = -1;
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
char* Calculator::c_getResult()
{
	if (state.mode == ALL)
	{
		for (auto res : result)
		{
			storeDoubleInCharArray(res);
		}	
	}
	else if (state.mode == GENERAL)
	{
		storeDoubleInCharArray(result.back());
	}
	else if (state.mode == ACC)
	{
		double acc = getResultAcc();
		storeDoubleInCharArray(acc);
	}
	else if (state.mode == GET_HISTORY)
	{
		if (state.result_dev > result.size() - 1)
		{
			state.error_code = 8;
			printError(getErrorInformtion());
			return " ";
		}
		else
		{
			double tmp = result.at(result.size() - state.result_dev - 1);
			storeDoubleInCharArray(tmp);
			state.result_dev++;
		}
	}
	return output_buffer;					/*return the top of result*/
}
stl_string Calculator::getResult()
{
	//TODO
	stl_string str = "";
	return str;
}
const char* Calculator::getErrorInformtion() const
{
	switch (state.error_code)										/*return error information*/
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
	return state.error_code == -1 ? false : true;
}
void Calculator::printResult()
{
	char* res = c_getResult();
	std::cout << "\nReuslt : " << res << "\n\n";		/*print result*/
	buffer_ptr = output_buffer;
	memset(output_buffer, '\0', sizeof(output_buffer) / sizeof(char));
}
/*******************************
Calculator : private : general function
********************************/
int Calculator::checkMode()
{
	if (state.source_length == 2 &&								/*check if quit*/
		source[0] == '-' &&
		source[1] == 'q')
		return QUIT;
	if (state.source_length == 2 &&								/*check if get history*/
		source[0] == '-' &&
		source[1] == 'h')
		return GET_HISTORY;
	if (state.source_length == 2 &&								/*check if accumulate*/
		source[0] == '-' &&
		source[1] == 's')
		return ACC;
	if (state.source_length == 2 &&								/*check if accumulate*/
		source[0] == '-' &&
		source[1] == 'a')
		return ALL;
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
	for (int i = 0; i < state.source_length; i++)
	{
		char current_char = source[i];		/*get current character*/
		if (isNumber(current_char) || (current_char == '-' && (i == 0 || source[i-1] == '(')))			/*if the character is number*/
		{
			stl_string number = "";
			while (isNumber(current_char) || (current_char == '-' && (i == 0 || source[i - 1] == '(')))	/*joint next character if it is a number*/
			{
				number += current_char;
				if (i == state.source_length - 1)
					break;
				current_char = source[++i];
			}
			if (!isNumberLegal(number))		/*cheack the legality of the number string*/
			{
				state.error_position = 1;				/*raise error*/
				return false;
			}
			postfix.push_back(number);		/*push the number string into postfix_expression*/
		}
		if (i == state.source_length - 1)
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
				postfix.push_back(stl_string(1, stack.back()));
				stack.pop_back();
			}
			stack.pop_back();
		}
		else								/*if current character is not prior to top, then pop top*/
		{
			postfix.push_back(stl_string(1, stack.back()));
			stack.pop_back();
			stack.push_back(current_char);
		}
	}
	while (!stack.empty())					/*pop all symbol*/
	{
		if (stack.back() != '(')			/*skip '('*/
			postfix.push_back(stl_string(1, stack.back()));
		stack.pop_back();
	}
	return true;
}
bool Calculator::processResult()
{
	/*copy postfix to vector*/
	std::vector<stl_string> vec(postfix.begin(), postfix.end());
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
					state.error_code = 7;
					state.error_position = index;
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
stl_string Calculator::convertDoubleToString(double& val)
{
	char buf[20];
	sprintf_s(buf, "%f", val);
	stl_string str = buf;
	return str;
}
void Calculator::storeDoubleInCharArray(double& val)
{
	char temp[20];
	memset(temp, '\0', 20);
	if(val == (int)val)
		sprintf(temp, "%.0f", val);
	else
		sprintf(temp, "%f", val);
	strcpy(buffer_ptr, temp);
	buffer_ptr += strlen(temp);
	*buffer_ptr = ' ';
	buffer_ptr++;
	*buffer_ptr = '\0';
}
bool Calculator::isBarcketMatch()
{
	return true;
}
bool Calculator::isSymbolsLegal()
{
	for (int i = 0; i < state.source_length; i++)
	{
		char ch = source[i];
		/* 1@2+3 */
		if (legal_chars.find(ch) == stl_string::npos)
		{
			state.error_code = 0;
			state.error_position = i;
			return false;
		}
		/* 1(2+3) */
		if (ch == '(' && i != 0 && isNumber(source[i - 1]))
		{
			state.error_code = 2;
			state.error_position = i;
			return false;
		}
		/*1+(2+3)4 */
		if (ch == ')' && i < state.source_length - 1 && isNumber(source[i + 1]))
		{
			state.error_code = 2;
			state.error_position = i;
			return false;
		}
		/* 1+2+3+ */
		if (isSymbol(stl_string(1, ch)) && i == state.source_length - 1)
		{
			state.error_code = 3;
			state.error_position = i;
			return false;
		}
		/* 1++2+3 */
		if (isSymbol(stl_string(1, ch)) && i < state.source_length - 1 && isSymbol(stl_string(1, source[i + 1])))
		{
			state.error_code = 3;
			state.error_position = i;
			return false;
		}
		/* 1+2+()+3 */
		if (ch == '(' && i < state.source_length - 1 && source[i + 1] == ')')
		{
			state.error_code = 3;
			state.error_position = i;
			return false;
		}
		/* 1+((2+3)*)+4 */
		if (isSymbol(stl_string(1, ch)) && i < state.source_length - 1 && source[i + 1] == ')')
		{
			state.error_code = 4;
			state.error_position = i;
			return false;
		}
		/* 1+(*(2+3))+4 */
		if (isSymbol(stl_string(1, ch)) && source[i - 1] == '(' && ch != '-')
		{
			state.error_code = 4;
			state.error_position = i;
			return false;
		}
	}
	return true;
}
void Calculator::printError(const char* error)
{
	std::cout << "\n  ERROR : " << error;
	if (state.error_code != -1)
		std::cout << " : No." << state.error_position << "\n\n";
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
bool Calculator::isSymbol(const stl_string& str)
{
	if (str == "+" ||
		str == "-" ||
		str == "*" ||
		str == "/" ||
		str == "." ||
		str == "��")
		return true;
	else
		return false;
}
bool Calculator::isNumberLegal(const stl_string& str)
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
		if (!isNumber(str[i]) && str[i] != '-')
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
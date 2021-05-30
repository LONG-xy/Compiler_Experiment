#include "LexicalAnalyzer.h"
#include <iostream>

LexicalAnalyzer::LexicalAnalyzer() {
	_fin.open("factorial.pas", std::ios_base::in);
	_fout.open("result.dyd", std::ios_base::out);
	_ferr.open("error.err", std::ios_base::out);

	if (!_fin.is_open() || !_fout.is_open() || !_ferr.is_open()) {
		std::cerr << "Could not open the file" << std::endl;
		_fin.clear();
	}

	CreatTable();
}

LexicalAnalyzer::~LexicalAnalyzer() {

}

void LexicalAnalyzer::initial()
{
	DPair binseq;
	while (_fin.get(_character)) {
		binseq = LexAnalyze();
		if (binseq.second > 0)
			output(binseq);
		else {
			error(binseq);
		}
	}

	output(DPair("EOF", 25));

	_fin.close();
	_fout.close();
	_ferr.close();
}

DPair LexicalAnalyzer::LexAnalyze()
{
	_fin.unget();

	std::string token;

	while (_fin.get(_character)) {
		if (_character == ' ')
			continue;
		else
			break;
	}
	switch (_character)
	{
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
		while (isalpha(_character) || isdigit(_character)) {
			token += _character;
			_fin.get(_character);
		}
		_fin.unget();
		_character = ' ';

		if (reserve(token) != 0) {
			return DPair(token, reserve(token));
		}
		else {
			return DPair(token, _CodeTable.find("$SYMBOL")->second);
		}
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		while (isdigit(_character)) {
			token += _character;
			_fin.get(_character);
		}
		_fin.unget();
		_character = ' ';
		return DPair(token, _CodeTable.find("$CONSTANT")->second);
		break;
	case '<':
		_fin.get(_character);
		if (_character == '=') {
			return DPair("<=", _CodeTable.find("<=")->second);
		}
		else if (_character == '>') {
			return DPair("<>", _CodeTable.find("<>")->second);
		}
		else {
			_fin.unget();
			_character = ' ';
			return DPair("<", _CodeTable.find("<")->second);
		}
		break;
	case '>':
		_fin.get(_character);
		if (_character == '=') {
			return DPair(">=", _CodeTable.find(">=")->second);
		}
		else {
			_fin.unget();
			_character = ' ';
			return DPair(">", _CodeTable.find(">")->second);
		}
		break;

	case ':':
		_fin.get(_character);
		if (_character == '=') {
			return DPair(":=", _CodeTable.find(":=")->second);
		}
		else {
			return DPair(std::string(1, _character), -COLONMATCHERR);
		}
		break;
	case '=':
		return DPair("=", _CodeTable.find("=")->second);
		break;
	case '-':
		return DPair("-", _CodeTable.find("-")->second);
		break;
	case '*':
		return DPair("*", _CodeTable.find("*")->second);
		break;
	case '(':
		return DPair("(", _CodeTable.find("(")->second);
		break;
	case ')':
		return DPair(")", _CodeTable.find(")")->second);
	case ';':
		return DPair(";", _CodeTable.find(";")->second);
		break;
	case '\n':
		line++;
		return DPair("EOLN", 24);
		break;
	case ' ':
		break;
	default:
		return DPair(std::string(1, _character), -CHARACTERERR);
		break;
	}
	return DPair();
}

void LexicalAnalyzer::CreatTable()
{
	_CodeTable["begin"] = 1;
	_CodeTable["end"] = 2;
	_CodeTable["integer"] = 3;
	_CodeTable["if"] = 4;
	_CodeTable["then"] = 5;
	_CodeTable["else"] = 6;
	_CodeTable["function"] = 7;
	_CodeTable["read"] = 8;
	_CodeTable["write"] = 9;
	_CodeTable["$SYMBOL"] = 10;
	_CodeTable["$CONSTANT"] = 11;
	_CodeTable["="] = 12;
	_CodeTable["<>"] = 13;
	_CodeTable["<="] = 14;
	_CodeTable["<"] = 15;
	_CodeTable[">="] = 16;
	_CodeTable[">"] = 17;
	_CodeTable["-"] = 18;
	_CodeTable["*"] = 19;
	_CodeTable[":="] = 20;
	_CodeTable["("] = 21;
	_CodeTable[")"] = 22;
	_CodeTable[";"] = 23;
}

int LexicalAnalyzer::reserve(const std::string & token)
{
	std::map<std::string, int>::iterator it;
	it = _CodeTable.find(token);
	if (it != _CodeTable.end())
		return it->second;
	else {
		return 0;
	}

}

void LexicalAnalyzer::error(const DPair errinfo)
{
	switch (-errinfo.second)
	{
	case CHARACTERERR:
		_ferr << "***LINE: " << line << ' ' << ' ' << "Invalid _character \'" << errinfo.first << "\'" << std::endl;
		break;
	case COLONMATCHERR:
		_ferr << "***LINE: " << line << ' ' << ' ' << "Character \'" << errinfo.first << "\' doesn't math \':\' " << std::endl;
		break;
	default:
		break;
	}
}

void LexicalAnalyzer::output(const DPair binseq)
{
	int blanknum = 16 - binseq.first.length();
	for (int i = 0; i < blanknum; i++)
		_fout << ' ';
	_fout << binseq.first << ' ' << binseq.second << std::endl;
}

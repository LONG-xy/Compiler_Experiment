#pragma once
#ifndef LEXICALANALYZER_H_
#define LEXICALANALYZER_H_

#include <fstream>
#include <string>
#include <map>

#define COLONMATCHERR 1
#define CHARACTERERR 2

typedef std::pair<std::string, int> DPair;

class LexicalAnalyzer {
private:
	std::map<std::string, int> _CodeTable;	//符号表
	char _character;			//当前读取的字符
	int line = 1;				//当前读取到的行数
	std::ifstream _fin;			//读入文件流
	std::ofstream _fout;		//输出二元式结果流
	std::ofstream _ferr;		//输出错误文件流
public:
	LexicalAnalyzer();
	~LexicalAnalyzer();

	void start();							//启动分析器
	DPair LexAnalyze();						//词法分析函数
	void CreatTable();						//构造符号表
	int reserve(const std::string &);		//判断是否是保留字
	void error(int type);					//报错函数
	void output(const DPair binseq);		//输出结果

};

#endif // !LEXICALANALYZER_H_

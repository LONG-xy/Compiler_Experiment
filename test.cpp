#include "RDParser.h"
#include "LexicalAnalyzer.h"
#include <iostream>

int main()
{
	LexicalAnalyzer analyzer;
	analyzer.start();
	std::cout << "词法分析完成！\n";
	RDParser parser;
	parser.start();
	std::cout << "语法分析完成！\n";
	system("pause");
	return 0;
}
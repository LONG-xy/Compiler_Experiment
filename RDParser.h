#pragma once
#ifndef RDPARSER_H_
#define RDPARSER_H_

#include <fstream>
#include <string>
#include <map>
#include <vector>

#define SYMBOL 10
#define CONSTANT 11

#define REPEATDEFINE 0
#define MISSINGEND 1
#define MISSINGBEGIN 2
#define MISSINGSEMICOLON 3
#define MISSINGTYPE 4
#define MISSINGVAR 5
#define MISSINGFUNC 6
#define MISSSINGFUNCID 7
#define MISSINGLEFTBR 8
#define MISSINGRIGHTBR 9
#define MISSINGARGUMENT 10
#define UNDEDINEVAR 11
#define MISSINGEUQ 12
#define MISSINGCOLON 13
#define MISSINGIF 15
#define MISSINGTHEN 16
#define MISSINGELSE 17
#define MISSINGOPERATE 18

typedef enum { integers } types;

//变量名表成员
struct Value {
	std::string _vname;
	std::string _vproc;
	bool _vkind;
	types _vtype;
	int _vlev;
	int _vadr;
};

//过程名表成员
struct Process {
	std::string _pname;
	types _ptype;
	int _plev; 
	int _fadr = -1;
	int _ladr;
};

typedef std::pair<std::string, Process> PPair;

class RDParser {
private:
	std::map<std::string, Process> _ProcessTable;	//过程名表，用map容器存储
	std::vector<Value> _ValueTable;					//变量名表，用vector容器存储

	//所需文件流
	std::ifstream _fin;			//读取dyd文件流
	std::ofstream _fvalt;		//输出变量名表文件流
	std::ofstream _fprot;		//输出过程名表文件流
	std::ofstream _ferr;		//输出错误文件流
	int _val_len;			//当前过程名表的长度，为了确定变量位置
	int _level;				//当前层次
	int _linecount;			//当前行数
	std::string _sym;		//当前待匹配的字符串
	int _id;				//当前待匹配的字符串的种别，用于识别标识符和常数
	std::string _current_process_id;	//当前过程名
	std::string _last_process_id;		//上一级过程名

	void advance();			//往下读取一个字符串到_sym中
	//根据文法的语法分析函数
	void S();
	void A();
	void A_s();
	void B();
	void B_s();
	void C();
	void D();
	void E();
	void F();
	void G();
	void H();
	void I();
	void I_s();
	void J();
	void J_s();
	void K();
	void L();
	void M();

public:
	RDParser();
	~RDParser();

	void AddToValueTable(std::string vname, bool vkind);	//添加变量到变量名表中
	void AddToProcessTable(std::string pname);				//添加过程到过程名表中
	bool Is_in_ValueTable(std::string vname);				//判断是否在变量名表中
	bool Is_in_ProcessTable(std::string pname);				//判断是否在过程名表中

	void error(int type);		//报错函数
	void start();				//开始函数
	void PTGetValAdd();			//分析结束后根据变量名表完善过程名表中的变量位置信息
	void ShowValueTable();		//输出变量名表
	void ShowProcessTable();	//输出过程名表
};
#endif // !RDPARSER_H_

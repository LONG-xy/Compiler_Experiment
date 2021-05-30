#include "RDParser.h"
#include <iostream>
#include <vector>

//构造函数
RDParser::RDParser()
{
	//打开所有文件
	_fin.open("result.dyd");
	_ferr.open("error.err", std::ios_base::app);
	_fvalt.open("Value table.table", std::ios_base::out);
	_fprot.open("Process table.table", std::ios_base::out);
	if (!_fin.is_open() || !_ferr.is_open() || !_fvalt.is_open() || !_fprot.is_open()) {
		std::cout << "Cannot open file!\n";
	}

	//初始化部分参数
	_val_len = 0;
	_level = 0;
	_linecount = 1;
	_current_process_id = "main";
	AddToProcessTable(_current_process_id);
}

RDParser::~RDParser()
{
}

//添加变量到变量名表中
void RDParser::AddToValueTable(std::string vname, bool vkind)
{
	if (!Is_in_ValueTable(vname)) {
		Value new_value;
		new_value._vname = vname;
		new_value._vproc = _current_process_id;
		new_value._vkind = vkind;
		new_value._vlev = _level+1;
		new_value._vadr = _val_len;
		_ValueTable.push_back(new_value);
		_val_len++;
	}
	else {
		error(REPEATDEFINE);
	}
}

//添加过程到过程名表中
void RDParser::AddToProcessTable(std::string pname)
{
	Process new_process;
	new_process._pname = pname;
	new_process._plev = _level+1;

	_ProcessTable[pname] = new_process;
}

//判断是否在变量名表中
bool RDParser::Is_in_ValueTable(std::string vname)
{
	int size = _ValueTable.size();
	for (int i = 0; i < size; i++) {
		if (_ValueTable[i]._vname == vname && _ValueTable[i]._vproc == _current_process_id)
			return true;
	}
	return false;
}

//判断是否在过程名表中
bool RDParser::Is_in_ProcessTable(std::string pname)
{
	std::map<std::string, Process>::iterator it;
	it = _ProcessTable.find(pname);
	if (it != _ProcessTable.end()) {
		return true;
	}
	return false;
}

//报错函数，根据传递信息进行报错
void RDParser::error(int type)
{
	switch (type)
	{
	case REPEATDEFINE:
		_ferr << "***LINE:" << _linecount << "  " << _sym << "重复定义\n";
		break;
	case MISSINGEND:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关键字end\n";
		break;
	case MISSINGBEGIN:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关键字begin\n";
		break;
	case MISSINGSEMICOLON:
		_ferr << "***LINE:" << _linecount << "  " << "缺少分号\';\'\n";
		break;
	case MISSINGTYPE:
		_ferr << "***LINE:" << _linecount << "  " << "缺少类型说明，" << _sym << "不是关键字\n";
		break;
	case MISSINGVAR:
		_ferr << "***LINE:" << _linecount << "  " << "缺少变量\n";
		break;
	case MISSINGFUNC:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关键字function\n";
		break;
	case MISSSINGFUNCID:
		_ferr << "***LINE:" << _linecount << "  " << "缺少函数名\n";
		break;
	case MISSINGLEFTBR:
		_ferr << "***LINE:" << _linecount << "  " << "缺少左括号\'(\'\n";
		break;
	case MISSINGRIGHTBR:
		_ferr << "***LINE:" << _linecount << "  " << "缺少左括号\')\'\n";
		break;
	case MISSINGARGUMENT:
		_ferr << "***LINE:" << _linecount << "  " << "缺少参数\n";
		break;
	case UNDEDINEVAR:
		_ferr << "***LINE:" << _linecount << "  " << "变量 " << _sym << " 未定义\n";
		break;
	case MISSINGEUQ:
		_ferr << "***LINE:" << _linecount << "  " << "缺少等号\'=\'\n";
		break;
	case MISSINGCOLON:
		_ferr << "***LINE:" << _linecount << "  " << "缺少赋值号\':=\'\n";
		break;
	case MISSINGIF:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关键字if\n";
		break;
	case MISSINGTHEN:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关键字then\n";
		break;
	case MISSINGELSE:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关键字else\n";
		break;
	case MISSINGOPERATE:
		_ferr << "***LINE:" << _linecount << "  " << "缺少关系运算符\n";
		break;
	default:
		break;
	}
}

//开始分析
void RDParser::start()
{
	advance();
	S();
	PTGetValAdd();
	ShowValueTable();		//输出变量名表
	ShowProcessTable();		//输出过程名表

	//关闭文件
	_fin.close();
	_ferr.close();
	_fvalt.close();
	_fprot.close();
}

//分析结束后根据变量名表完善过程名表中的变量位置信息
void RDParser::PTGetValAdd()
{
	int size = _ValueTable.size();
	for (int i = 0; i < size; i++) {
		std::string pid = _ValueTable[i]._vproc;
		if (_ProcessTable[pid]._fadr < 0) {
			_ProcessTable[pid]._fadr = _ValueTable[i]._vadr;
		}
		if (_ProcessTable[pid]._ladr < _ValueTable[i]._vadr) {
			_ProcessTable[pid]._ladr = _ValueTable[i]._vadr;
		}
	}
}

//输出变量名表
void RDParser::ShowValueTable()
{
	_fvalt << "name" << '\t' << "address" << '\t' << "kind" << '\t' << "level" << '\t' << "process" << '\n';

	int size_1 = _ValueTable.size();
	for (int i = 0; i < size_1; i++) {
		
			std::string kind = (_ValueTable[i]._vkind == true) ? "形参" : "实参";
			_fvalt << _ValueTable[i]._vname << '\t' << _ValueTable[i]._vadr << '\t' << kind << '\t' << _ValueTable[i]._vlev << '\t' << _ValueTable[i]._vproc<<'\n';
	}
}

//输出过程名表
void RDParser::ShowProcessTable()
{
	_fprot << "name" <<  '\t' << "level" << '\t'  << "faddress" << '\t' << "laddress" << '\n';

	std::map<std::string, Process>::iterator it;
	for (it = _ProcessTable.begin(); it != _ProcessTable.end(); it++) {
		_fprot << it->second._pname << '\t' << it->second._plev << '\t' << it->second._fadr << '\t' << it->second._ladr << '\n';

	}
}

//继续往下读取一个字符串到_sym中
void RDParser::advance()
{
	std::string line;
 	if (!_fin.eof()) {
		std::getline(_fin, line);
		line.erase(0, line.find_first_not_of(" "));		//去掉空白字符

		//分割字符串，提取二元式信息
		std::string::size_type pos, end;
		pos = line.find(" ");
		end = line.find("\n");
		_sym = line.substr(0, pos);
		_id = atoi(line.substr(pos + 1, end).c_str());

		//如果读到换行符则行数+1，继续往下读
		if (_id == 24 || _sym == "EOLN") {
			_linecount++;
			advance();
		}
	}
}

/*
改进文法G'如下：
S→begin A B end
A→C;A'
A'→C;A'|ε
C→integer D
D→<标识符> | function <标识符>（<标识符>）；E
E→begin A B end
B→FB'
B'→;FB'|ε
F→read(<标识符>)│write(<标识符>)│G│H
G→<标识符>:=I
I→JI'
I'→-JI'|ε
J→KJ'
J'→*KJ' | ε
K→<标识符>│<常数>│<函数调用>
H→if L then F else F
L→IMI
M→<│<=│>│>=│=│<>

其中：
S:<程序>
A:<说明语句表>
A':<说明语句表>'
B:<执行语句表>
B':<执行语句表>'
C:<说明语句>
D:<标识符或函数说明>
E:<函数体>
F:<执行语句>
G:<赋值语句>
H:<条件语句>
I:<算术表达式>
I':<算术表达式>'
J:<项>
J':<项>'
K:<因子>
L:<条件表达式>
M:<关系运算符>
*/

//<程序>→begin <说明语句表><执行语句表> end
//S→begin A B end
void RDParser::S()
{
	if (_sym != "begin") {
		error(MISSINGBEGIN);
	}
	advance();
	A();
	B();
	if (_sym == "end") {
		advance();
	}
	else {
		error(MISSINGEND);
	}
}

//<说明语句表>→<说明语句>；<说明语句表'>
//A→C;A'
void RDParser::A()
{
	C();
	if (_sym == ";") {
		advance();
		A_s();
	}
	else {
		error(MISSINGSEMICOLON);
	}
}

//<说明语句表'>→<说明语句>；<说明语句表'>|ε
//A'→C;A'|ε
void RDParser::A_s()
{
	C();
	if (_sym == ";") {
		advance();
		A_s(); 
	}
	else if (_sym == "if" || _sym == "read" || _sym == "write" || _id == CONSTANT) {
		//如果';'后是执行语句，则跳过
	}
	else {
		error(MISSINGSEMICOLON);
	}
}

//<执行语句表>→<执行语句><执行语句表'>
//B→FB'
void RDParser::B()
{
	F();
	B_s();
}

//<执行语句表'>→; <执行语句><执行语句表'> | ε
//B'→;FB'|ε
void RDParser::B_s()
{
	if (_sym == ";") {
		advance();
		F();
		B_s();
	}
	else if (_sym == "EOF" || _sym == "end") {
		//如果是换行符或者一个过程结尾，跳过
	}
	else
	{
		error(MISSINGSEMICOLON);
	}
}

//<说明语句>→integer <标识符或函数说明>
//C→integer D
void RDParser::C()
{
	if (_sym == "integer") {
		advance();
		D();
	}
	else if (_sym == "if" || _sym == "read" || _sym == "write" || _id == CONSTANT ) {
	
	}
	else {
		error(MISSINGTYPE);
		advance();
		D();
	}
}

//标识符或函数说明>→<标识符> | function <标识符>（<标识符>）；<函数体>
//D→<标识符> | function <标识符>（<标识符>）；E
void RDParser::D()
 {
	if (_id == SYMBOL) {
		AddToValueTable(_sym, false);	
		advance();
	}
	else if (_sym == "function") {
		advance();
		if (_id == SYMBOL) {
			//将过程名添加到过程名表中，此时层次+1，更新当前过程名，并保存上一级过程名
			_level++;
			AddToProcessTable(_sym);
			_last_process_id = _current_process_id;
			_current_process_id = _sym;
			//还原层次
			_level--;
			advance();
			if (_sym == "(") {
				advance();
				if (_id == SYMBOL){
					_level++;
					AddToValueTable(_sym, true);	//将形参保存到变量名表中
					_level--;
					advance();
					if (_sym == ")") {
						advance();
						if (_sym == ";") {
							advance();
							E();
							_level--;				//还原层次
							_current_process_id = _last_process_id;	//还原当前过程名
						}
						else error(MISSINGSEMICOLON);
					}
					else error(MISSINGRIGHTBR);
				}
				else error(MISSINGARGUMENT);
			}
			else error(MISSINGLEFTBR);
		}
		else error(MISSSINGFUNCID);
	}
	else error(MISSINGVAR);
}

//<函数体>→begin <说明语句表><执行语句表> end
//E→begin A B end
void RDParser::E()
{
	if (_sym != "begin") {
		error(MISSINGBEGIN);
	}
	_level++;	//层次+1
	advance();
	A();
	B();
	if (_sym == "end") {
		advance();
	}
	else {
		error(MISSINGEND);
	}
}

//<执行语句>→read(<标识符>)│write(<标识符>)│<赋值语句>│<条件语句>
//F→read(<标识符>)│write(<标识符>)│G│H
void RDParser::F()
{
	if (_sym == "read" || _sym == "write") {
		advance();
		if (_sym == "(") {
			advance();
			if (_id == SYMBOL) {
				if (!Is_in_ValueTable(_sym))
					error(UNDEDINEVAR);
				advance();
				if (_sym == ")") {
					advance();
				}
				else error(MISSINGRIGHTBR);
			}
			else error(MISSINGVAR);
		}
		else error(MISSINGLEFTBR);
	}
	else if (_id == SYMBOL) {
		G();
	}
	else if (_sym == "if") {
		H();
	}
}

//<赋值语句>→<标识符>:=<算术表达式>
//G→<标识符>:=I
void RDParser::G()
{
	if (_id == SYMBOL) {
		if (!Is_in_ValueTable(_sym) && !Is_in_ProcessTable(_sym))
			error(UNDEDINEVAR);
		advance();
		if (_sym == ":=") {
			advance();
			I();
		}
		else {
			error(MISSINGCOLON);
			advance();
			I();
		}
	}
	else error(MISSINGVAR);
}


//<条件语句>→if<条件表达式>then<执行语句>else <执行语句>
//H→if L then F else F
void RDParser::H()
{
	if (_sym == "if") {
		advance();
		L();
		if (_sym == "then") {
			advance();
			F();
			if (_sym == "else") {
				advance();
				F();
			}
			else error(MISSINGELSE);
		}
		else error(MISSINGTHEN);
	}
	else error(MISSINGIF);
}

//<算术表达式>→<项><算术表达式>'
//I→JI'
void RDParser::I()
{
	J();
	I_s();
}

//<算术表达式>'→-<项><算术表达式>'
//I'→-JI'|ε
void RDParser::I_s()
{
	if (_sym == "-") {
		advance();
		J();
		I_s();
	}
}

//<项>→<因子><项'>
//J→KJ'
void RDParser::J()
{
	K();
	J_s();
}

//<项'>→*<因子><项'> | ε
//J'→*KJ' | ε
void RDParser::J_s()
{
	if (_sym == "*") {
		advance();
		K();
		J_s();
	}
}

//<因子>→<标识符>│<常数>│<函数调用>
//K→<标识符>│<常数>│<函数调用>
void RDParser::K()
{
	if (_id == SYMBOL) {
		if (!Is_in_ValueTable(_sym) && !Is_in_ProcessTable(_sym))
			error(UNDEDINEVAR);
		advance();
		if (_sym == "(") {
			advance();
			I();
			if (_sym == ")")
				advance();
			else
				error(MISSINGRIGHTBR);
		}
	}
	else if (_id == CONSTANT) {
		advance();
	}
}

//<条件表达式>→<算术表达式><关系运算符><算术表达式>
//L→IMI
void RDParser::L()
{
	I();
	M();
	I();
}

//<关系运算符> →<│<=│>│>=│=│<>
//M→<│ <= │>│ >= │ = │<>
void RDParser::M()
{
	if (_sym == "<" || _sym == "<=" || _sym == ">" || _sym == ">=" || _sym == "=" || _sym == "<>") {
		advance();
	}
	else error(MISSINGOPERATE);
}



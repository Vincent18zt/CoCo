#include "ir.h"
#include "tree.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

// ----------------------------------------------------------------------
// 构造函数
// ----------------------------------------------------------------------

ir_tree::ir_tree()
{
	next = NULL;
}
ir_tree::ir_tree(string result, string label)
{
	this->op = "";
	this->arg1 = "";
	this->arg2 = "";
	this->result = result;
	this->label = label;
	next = NULL;
}
ir_tree::ir_tree(string op, string arg1, string arg2, string result)
{
	this->op = op;
	this->arg1 = arg1;
	this->arg2 = arg2;
	this->result = result;
	this->label = "";
	next = NULL;
}
ir_tree::ir_tree(string op, string arg1, string arg2, string result, string label)
{
	this->op = op;
	this->arg1 = arg1;
	this->arg2 = arg2;
	this->result = result;
	this->label = label;
	next = NULL;
}
ir_tree::ir_tree(string op, string arg1, string arg2, string result, string label, string type)
{
	this->op = op;
	this->arg1 = arg1;
	this->arg2 = arg2;
	this->result = result;
	this->label = label;
	this->type = type;
	next = NULL;
}

// ----------------------------------------------------------------------
// 变量声明
// ----------------------------------------------------------------------

int extra_count = 0;
int ir_tree::node_count = 0;
int ir_tree::label_count = 0;


map<string, string> map_goto;
set<string> save_goto;
map<string, string> symbol_table;
set<string> output_label;


void quadruples_print(ir_tree *node);
void translate(ir_tree *node);
ir_tree *strip_extra(ir_tree *node);
void output(ir_tree *node);
string only_for_def(parser_tree *node);
void get_label(ir_tree *);
void pass_symbol_table(map<string, string> table)
{
	symbol_table = table;
}
string check_type(string value)
{
	for (char ch : value)
	{
		if (ch == '\'')
			return "char";
		if (ch == '.')
			return "double";
	}
	return "int";
}

ir_tree *traver_ir(parser_tree *node)
{

	if (node == NULL)
	{
		return NULL;
	}
	ir_tree *head = NULL;
	ir_tree *before = NULL;
	for (int i = node->sibling.size() - 1; i >= 0; --i)
	{
		ir_tree *current = traver_ir((node->sibling)[i]);
		if (!current)
			continue;
		if (head == NULL)
		{
			head = current;
		}
		else
		{
			while (before->next)
				before = before->next;
			before->next = current;
		}
		before = current;
	}

	string _type(node->node_type);
	ir_tree *current = NULL;
	if (_type == "Program")
	{
		current = traver_ir(node->right);
	}
	else if (_type == "complete_statement")
	{
		int before_count = ir_tree::node_count;
		ir_tree *left = new ir_tree("", "", "", "", "", "{");
		current = traver_ir(node->left);
		if (current == NULL)
			current = new ir_tree("", "No label");
		ir_tree *temp_node = current;
		while (temp_node->next)
			temp_node = temp_node->next;
		ir_tree *right = new ir_tree("", "", "", "", "", "}");
		left->next = current;
		temp_node->next = right;
		if (current != NULL)
			current = left;
		ir_tree::node_count = before_count;
	}
	else if (_type == "statement")
	{
		string value(node->node_value);
		// if (value != "definition_statement") {
		if (value == "if_statement" || value == "while_statement" || value == "for_statement")
		{
			ir_tree *left = new ir_tree("", "", "", "", "", "{");
			current = traver_ir(node->left);
			if (current == NULL)
				current = new ir_tree("", "No label");
			ir_tree *temp_node = current;
			while (temp_node->next)
				temp_node = temp_node->next;
			ir_tree *right = new ir_tree("", "", "", "", "", "}");
			ir_tree *label = new ir_tree("", "label_" + to_string(ir_tree::label_count++));

			left->next = current;
			temp_node->next = right;
			if (right == NULL)
				right = new ir_tree("", "No label");
			temp_node = right;
			while (temp_node->next)
				temp_node = temp_node->next;
			temp_node->next = label;
			if (current != NULL)
				current = left;
		}
		else
		{
			ir_tree *label = new ir_tree("", "LABEL_" + to_string(extra_count++));
			current = traver_ir(node->left);
			label->next = current;
			current = label;
		}
	}
	else if (_type == "definition_statement")
	{
		string def_stmt = only_for_def(node);
		current = new ir_tree("", "", "", "", "", def_stmt);
	}
	else if (_type == "write_statement")
	{
		current = traver_ir(node->left);
		if (current == NULL)
			current = new ir_tree("", "No label");
		ir_tree *temp_node = current;
		while (temp_node->next)
			temp_node = temp_node->next;
		string write = "cout << " + temp_node->result + " << endl;";
		ir_tree *left = new ir_tree("", "", "", "", "", write);
		temp_node->next = left;
		current = strip_extra(current);
		quadruples_print(current);
	}
	else if (_type == "read_statement")
	{
		current = traver_ir(node->left);
		string write = "cin >> " + current->result + ";";
		ir_tree *left = new ir_tree("", "", "", "", "", write);
		current->next = left;
	}
	else if (_type == "variable" || _type == "Val")
	{
		current = new ir_tree(node->node_value, "No label");
		return current;
	}
	else if (_type == "bool_expression")
	{
		current = traver_ir(node->left);
	}
	else if (_type == "(expression)")
	{
		current = traver_ir(node->left);
	}
	else if (_type == "for_expression_1")
	{
		current = traver_ir(node->left);
	}
	else if (_type == "for_expression_2")
	{
		current = traver_ir(node->left);
	}
	else if (_type == "for_expression_3")
	{
		current = traver_ir(node->left);
	}
	else if (_type == "for_expression_1 NULL")
	{
		// current = traver_ir(node->left);
	}
	else if (_type == "for_expression_2 NULL")
	{
		// current = traver_ir(node->left);
	}
	else if (_type == "for_expression_3 NULL")
	{
	}
	else if (_type == "expression")
	{
		string value(node->node_value);
		if (value == "(expression)")
		{
			current = traver_ir(node->left);
		}
		else if (value == "Add" || value == "MINUS" || value == "Mul" || value == "Div" || value == "Mod" || value == "Cmp_EL" || value == "Cmp_LT" || value == "Cmp_GT" || value == "Cmp_EQ" || value == "Cmp_EG" || value == "Cmp_and" || value == "Cmp_OR" || value == "Cmp_EN" || value == "Bit_right" || value == "Bit_left" || value == "Bit_and" || value == "Bit_or" || value == "Bit_xor")
		{
			ir_tree *left = traver_ir(node->left);
			ir_tree *right = traver_ir(node->right);
			ir_tree *left_last = left, *right_last = right;
			ir_tree *temp_node = nullptr;
			temp_node = left;
			while (temp_node->next)
				temp_node = temp_node->next;
			left_last = temp_node;
			temp_node = right;
			while (temp_node->next)
				temp_node = temp_node->next;
			right_last = temp_node;

			string op;
			if (value == "Add")
				op = "+";
			else if (value == "Mul")
				op = "*";
			else if (value == "MINUS")
				op = "-";
			else if (value == "Div")
				op = "/";
			else if (value == "Mod")
				op = "%";
			else if (value == "Cmp_EL")
				op = "<=";
			else if (value == "Cmp_LT")
				op = "<";
			else if (value == "Cmp_GT")
				op = ">";
			else if (value == "Cmp_EQ")
				op = "==";
			else if (value == "Cmp_EG")
				op = ">=";
			else if (value == "Cmp_EN")
				op = "!=";
			else if (value == "Cmp_and")
				op = "&&";
			else if (value == "Cmp_OR")
				op = "||";
			else if (value == "Bit_left")
				op = "<<";
			else if (value == "Bit_right")
				op = ">>";
			else if (value == "Bit_and")
				op = "&";
			else if (value == "Bit_or")
				op = "|";
			else if (value == "Bit_xor")
				op = "^";
			string type1 = symbol_table[left_last->result], type2 = symbol_table[right_last->result];
			if (type1 == "")
				type1 = check_type(left_last->result);
			if (type2 == "")
				type2 = check_type(right_last->result);
			string type;
			if (type1 == type2)
				type = type1;
			else if (type1 == "int" && type2 == "float")
				type = "float";
			else if (type1 == "float" && type2 == "int")
				type = "float";
			else if (type1 == "int" && type2 == "double")
				type = "double";
			else if (type1 == "double" && type2 == "int")
				type = "double";
			else if (type1 == "float" && type2 == "double")
				type = "double";
			else if (type1 == "double" && type2 == "float")
				type = "double";
			else if (type1 == "int" && type2 == "char")
				type = "int";
			else if (type1 == "char" && type2 == "int")
				type = "int";

			ir_tree *temp = new ir_tree(op, left_last->result, right_last->result, "tempVar" + to_string(ir_tree::node_count++),
										"label_" + to_string(ir_tree::label_count++), type);
			symbol_table["tempVar" + to_string(ir_tree::node_count - 1)] = type;
			current = left;
			left_last->next = right;
			right_last->next = temp;
		}
		else if (value == "Assign")
		{
			if (node->left->node_type == "variable" && node->right->node_type == "expression" && (string(node->right->node_value) == "back++" || string(node->right->node_value) == "back--") && string(node->left->node_value) == string(node->right->left->node_value))
			{
				current = new ir_tree(string(node->right->node_value) == "back++" ? "+" : "-", string(node->left->node_value),
									  "1", string(node->left->node_value), "label_" + to_string(ir_tree::label_count++));
			}
			else
			{
				ir_tree *left = traver_ir(node->left);
				ir_tree *right = traver_ir(node->right);
				ir_tree *left_last = left, *right_last = right;
				ir_tree *temp_node = nullptr;
				temp_node = left;
				while (temp_node->next)
					temp_node = temp_node->next;
				left_last = temp_node;
				temp_node = right;
				while (temp_node->next)
					temp_node = temp_node->next;
				right_last = temp_node;

				string result_left = left_last->result, result_right = right_last->result;
				ir_tree *temp = new ir_tree("=", result_right, "", result_left, "label_" + to_string(ir_tree::label_count++));
				current = left;
				left_last->next = right;
				right_last->next = temp;
			}
		}
		else if (value == "++front" || value == "--front")
		{
			ir_tree *left = traver_ir(node->left);
			ir_tree *temp = new ir_tree(value == "++front" ? "+" : "-", left->result, "1", left->result, "label_" + to_string(ir_tree::label_count++));
			ir_tree *temp2 = new ir_tree(left->result, "No label");
			temp->next = temp2;
			current = temp;
		}
		else if (value == "back++" || value == "back--")
		{
			ir_tree *left = traver_ir(node->left);
			string type = symbol_table[left->result];
			ir_tree *temp2 = new ir_tree("=", left->result, "", "tempVar" + to_string(ir_tree::node_count++),
										 "label_" + to_string(ir_tree::label_count++), type);
			symbol_table["tempVar" + to_string(ir_tree::node_count - 1)] = type;
			ir_tree *temp = new ir_tree(value == "back++" ? "+" : "-", left->result, "1", left->result,
										"label_" + to_string(ir_tree::label_count++));
			ir_tree *temp3 = new ir_tree("tempVar" + to_string(ir_tree::node_count - 1), "No label");
			current = temp2;
			temp2->next = temp;
			temp->next = temp3;
		}
	}
	else if (_type == "while_statement")
	{
		ir_tree *left = traver_ir(node->left);
		// left = strip_extra(left);
		if (left == NULL)
			left = new ir_tree("", "label_" + to_string(ir_tree::label_count++));
		if (left->label == "No label")
			left->label = "label_" + to_string(ir_tree::label_count++);
		ir_tree *left_last = left, *right_last = NULL;
		ir_tree *temp_node = nullptr;
		temp_node = left;
		while (temp_node->next)
			temp_node = temp_node->next;
		left_last = temp_node;
		ir_tree *mid = new ir_tree("IF-NOT", left_last->result, "", "", "label_" + to_string(ir_tree::label_count++));
		ir_tree *right = traver_ir(node->right);
		right = strip_extra(right);
		if (right == NULL)
			right = new ir_tree("", "No label");
		temp_node = right;
		while (temp_node->next)
			temp_node = temp_node->next;
		right_last = temp_node;

		ir_tree *go_back = new ir_tree("", "", "", "goto " + left->label, "label_" + to_string(ir_tree::label_count++));
		mid->result = "goto label_" + to_string(ir_tree::label_count);
		current = left;
		left_last->next = mid;
		mid->next = right;
		right_last->next = go_back;
	}
	else if (_type == "if_statement")
	{
		string value(node->node_value);
		if (value == "if(...) ...")
		{
			ir_tree *left = traver_ir(node->left);
			// left = strip_extra(left);
			if (left == NULL)
				left = new ir_tree("", "No label");
			ir_tree *left_last = left, *right_last = NULL;
			ir_tree *temp_node = nullptr;
			temp_node = left;
			while (temp_node->next)
				temp_node = temp_node->next;
			left_last = temp_node;
			ir_tree *mid = new ir_tree("IF-NOT", left_last->result, "", "", "label_" + to_string(ir_tree::label_count++));
			ir_tree *right = traver_ir(node->right);
			right = strip_extra(right);
			if (right == NULL)
				right = new ir_tree("", "No label");
			temp_node = right;
			while (temp_node->next)
				temp_node = temp_node->next;
			right_last = temp_node;

			mid->result = "goto label_" + to_string(ir_tree::label_count);
			current = left;
			left_last->next = mid;
			mid->next = right;
		}
		else if (value == "if(...) .. else ...")
		{
			ir_tree *left = traver_ir(node->left);
			// left = strip_extra(left);
			if (left == NULL)
				left = new ir_tree("", "No label");
			ir_tree *left_last = left, *right_last = NULL;
			ir_tree *temp_node = nullptr;
			temp_node = left;
			while (temp_node->next)
				temp_node = temp_node->next;
			left_last = temp_node;
			ir_tree *mid = new ir_tree("IF-NOT", left_last->result, "", "", "label_" + to_string(ir_tree::label_count++));
			ir_tree *right = traver_ir(node->right);
			right = strip_extra(right);
			if (right == NULL)
				right = new ir_tree("", "No label");
			temp_node = right;
			while (temp_node->next)
				temp_node = temp_node->next;
			right_last = temp_node;

			mid->result = "goto label_" + to_string(ir_tree::label_count + 1);
			current = left;
			left_last->next = mid;
			mid->next = right;
			ir_tree *go_to = new ir_tree("", "", "", "",
										 "label_" + to_string(ir_tree::label_count++));
			ir_tree *else_branch = traver_ir(node->temp1);
			map_goto[mid->result] = else_branch->label;
			save_goto.insert(else_branch->label);
			go_to->result = "goto label_" + to_string(ir_tree::label_count);
			right_last->next = go_to;
			go_to->next = else_branch;
			current = strip_extra(current);
		}
	}
	else if (_type == "for_statement")
	{
		parser_tree *forlist = node->left;
		ir_tree *temp_node = nullptr;
		ir_tree *for1_last = NULL, *for2_last = NULL, *for3_last = NULL, *stmt_last = NULL;
		ir_tree *for1 = traver_ir(forlist->left);
		if (for1 == NULL)
			for1 = new ir_tree("", "No label");
		temp_node = for1;
		while (temp_node->next)
			temp_node = temp_node->next;
		for1_last = temp_node;
		int cnt = ir_tree::label_count;
		ir_tree *for2 = traver_ir(forlist->right);
		if (for2 == NULL)
			for2 = new ir_tree("", "No label");
		temp_node = for2;
		while (temp_node->next)
			temp_node = temp_node->next;
		for2_last = temp_node;
		ir_tree *mid = new ir_tree("IF-NOT", for2_last->result, "", "", "label_" + to_string(ir_tree::label_count++));
		ir_tree *stmt = traver_ir(node->right);
		if (stmt == NULL)
			stmt = new ir_tree("", "No label");
		temp_node = stmt;
		while (temp_node->next)
			temp_node = temp_node->next;
		stmt_last = temp_node;
		ir_tree *for3 = traver_ir(forlist->temp1);
		if (for3 == NULL)
			for3 = new ir_tree("", "No label");
		temp_node = for3;
		while (temp_node->next)
			temp_node = temp_node->next;
		for3_last = temp_node;
		ir_tree *go_back = new ir_tree("", "", "", "goto label_" + to_string(cnt), "label_" + to_string(ir_tree::label_count++));
		mid->result = "goto label_" + to_string(ir_tree::label_count);

		current = for1;
		for1_last->next = for2;
		for2_last->next = mid;
		mid->next = stmt;
		stmt_last->next = for3;
		for3_last->next = go_back;
		current = strip_extra(current);
	}

	if (before)
	{
		while (before->next)
			before = before->next;
		before->next = current;
		return head;
	}
	if (_type == "Program")
	{
		if (current == NULL)
			current = new ir_tree("", "No label");
		ir_tree *temp_node = current;
		while (temp_node->next)
			temp_node = temp_node->next;
		temp_node->next = new ir_tree("End", "label_" + to_string(ir_tree::label_count++));
		current = strip_extra(current);
		quadruples_print(current);
		cout << "-----------------------------------x86 Assembly Code-------------------" << endl;
		translate(current);
		// get_label(current);
		// cout << "labels should be output" << endl;
		// cout << output_label.size() << endl;
		// for (string fuck : output_label)cout << fuck << endl;
	}
	return current;
}

// ----------------------------------------------------------------------
// 四元式打印
// ----------------------------------------------------------------------

string indenter(string s)
{
	if (s.size() >= 16)
		return s + "\t";
	if (s.size() >= 8)
		return s + "\t\t";
	return s + "\t\t\t";
}

void quadruples_print(ir_tree *node)
{
	while (node)
	{
		if (node->label.size() >= 5 && string(node->label.begin(), node->label.begin() + 5) == "label")
			cout << indenter(node->op) + indenter(node->arg1) + indenter(node->arg2) + indenter(node->result) + indenter(node->label) << endl;
		node = node->next;
	}
}

// ----------------------------------------------------------------------
// 汇编生成
// ----------------------------------------------------------------------

// 假设都是int -> 4 bytes

int rbp_current = 0;
string get_reg()
{
	rbp_current += 4;
	return "[rbp-" + to_string(rbp_current) + "]";
}

// 判断是否是 "11" -> int
inline bool is_integer(const std::string &s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
		return false;

	char *p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}

map<string, string> symbol_reg;
void translate(ir_tree *node)
{
	vector<string> goto_labels;

	ir_tree *save_node_state = node;

	while (node)
	{
		if (node->label.size() >= 5 && string(node->label.begin(), node->label.begin() + 5) == "label")
		{
			if (string(node->result.begin(), node->result.begin() + 4) == "goto")
			{
				goto_labels.push_back(string(node->result.begin() + 5, node->result.end()));
			}
		}
		node = node->next;
	}

	node = save_node_state;

	cout << "main:" << endl;

	while (node)
	{
		ir_tree *next_node = node->next;
		if (node->label.size() >= 5 && string(node->label.begin(), node->label.begin() + 5) == "label")
		{
			if (find(goto_labels.begin(), goto_labels.end(), node->label) != goto_labels.end() || node->label == "label_0")
			{
				cout << node->label << ":" << endl;
			}

			// intel syntax
			if (node->op == "=")
			{
				// 如果是const赋值
				if (is_integer(node->arg1))
				{
					// =			5						a			label_0
					// 检查一下是否有这个变量
					string temp_reg;
					if (symbol_reg.count(node->result))
					{
						temp_reg = symbol_reg[node->result];
					}
					else
					{
						temp_reg = get_reg();
					}
					cout << "\t"
						 << "mov DWORD PTR " << temp_reg << ", " << node->arg1 << endl;
					symbol_reg[node->result] = temp_reg;
				}
				else
				{
					// 如果是变量赋值
					cout << "\t"
						 << "mov edx, DWORD PTR " << symbol_reg[node->arg1] << endl;
					string temp_reg;
					if (symbol_reg.count(node->result))
					{
						temp_reg = symbol_reg[node->result];
					}
					else
					{
						temp_reg = get_reg();
					}
					cout << "\t"
						 << "mov DWORD PTR " << temp_reg << ", eax" << endl;
					symbol_reg[node->result] = temp_reg;
				}
			}
			else if (node->op == "+")
			{
				// + a b tempVar1
				// 把 a b的值从reg中取出来，给tempVar分配一个地址
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov edx, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t"
					 << "mov edx, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				cout << "\t"
					 << "add eax, edx" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			}
			else if (node->op == "-")
			{
				// + a b tempVar1
				// 把 a b的值从reg中取出来，给tempVar分配一个地址
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg1 << endl;
				}
				else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "sub eax, DWORD PTR " << node->arg2 << endl;
				} else {
				cout << "\t"
					 << "sub eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			}
			else if (node->op == "*")
			{

				// *			a			b			tempVar1		label_4
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "imul eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t"
					 << "imul eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			}
			else if (node->op == "/")
			{
				///			a			b			tempVar2		label_6
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				cout << "\t"
					 << "cdq" << endl;

				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "idiv DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t"
					 << "idiv DWORD PTR " << symbol_reg[node->arg2] << endl;
				}

				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", eax";
				symbol_reg[node->result] = temp_reg;
			}
			else if (node->op == "%")
			{
				// %			a			b			tempVar3		label_8
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				cout << "\t"
					 << "cdq" << endl;
				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "idiv DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t"
					 << "idiv DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", edx";
				symbol_reg[node->result] = temp_reg;
			}

			else if (node->op == ">")
			{
				// >			a			b			tempVar2
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "cmp eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t"
					 << "cmp eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				cout << "\t"
					 << "setg al" << endl;
				cout << "\t"
					 << "movzx eax, al" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			}
			else if (node->op == "<")
			{
				// >			a			b			tempVar2
				if(is_integer(node->arg1)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t"
					 << "mov eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t"
					 << "mov eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				cout << "\t"
					 << "setl al" << endl;
				cout << "\t"
					 << "movzx eax, al" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t"
					 << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			}
			else if (node->op == "IF-NOT")
			{
				// IF-NOT			tempVar2					goto label_10		label_7
				cout << "\t"
					 << "cmp DWORD PTR " << symbol_reg[node->arg1] << ", 0" << endl;
				cout << "\t"
					 << "je " << string(node->result.begin() + 5, node->result.end()) << endl;
			} else if (node->op == "==") {
				// ==			a			b			tempVar0		label_2
				if(is_integer(node->arg1)) {
					cout << "\t" << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t" << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t" << "cmp eax, DWORD PTR " << node->arg2 << endl;
				}
				else {
					cout << "\t" << "cmp eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				cout << "\t" << "sete al" << endl;
				cout << "\t" << "movzx eax, al" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t" << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			} else if (node->op == "<=") {
				// <=			a			b			tempVar1		label_4
				if(is_integer(node->arg1)) {
					cout << "\t" << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t" << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t" << "cmp eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t" << "cmp eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				cout << "\t" << "setle al" << endl;
				cout << "\t" << "movzx eax, al" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t" << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			} else if (node->op == ">=") {
				if(is_integer(node->arg1)) {
					cout << "\t" << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t" << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg2)) {
					cout << "\t" << "cmp eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t" << "cmp eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
			
				cout << "\t" << "setge al" << endl;
				cout << "\t" << "movzx eax, al" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t" << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			} else if (node->op == "!=") {
				if(is_integer(node->arg1)) {
					cout << "\t" << "mov eax, DWORD PTR " << node->arg1 << endl;
				} else {
					cout << "\t" << "mov eax, DWORD PTR " << symbol_reg[node->arg1] << endl;
				}
				if(is_integer(node->arg1)) {
					cout << "\t" << "cmp eax, DWORD PTR " << node->arg2 << endl;
				} else {
					cout << "\t" << "cmp eax, DWORD PTR " << symbol_reg[node->arg2] << endl;
				}
				
				cout << "\t" << "setne al" << endl;
				cout << "\t" << "movzx eax, al" << endl;
				string temp_reg;
				if (symbol_reg.count(node->result))
				{
					temp_reg = symbol_reg[node->result];
				}
				else
				{
					temp_reg = get_reg();
				}
				cout << "\t" << "mov DWORD PTR " << temp_reg << ", eax" << endl;
				symbol_reg[node->result] = temp_reg;
			}

			// if(node->op == "+") {
			// 	cout << "\t" << "mov eax, [" << node->arg1 << "]" << endl;
			// 	cout << "\t" << "mov [" << node->result << "], eas" << endl;
			// 	cout << "\t" << "mov eax, [" + node->arg2 << "]" << endl;
			// 	cout << "\t" << "add [" << node->result << "], eax" << endl;
			// } else if(node->op == "-") {
			// 	cout << "\t" << "mov eax, [" << node->arg1 << "]" << endl;
			// 	cout << "\t" << "mov [" << node->result << "], eas" << endl;
			// 	cout << "\t" << "mov eax, [" + node->arg2 << "]" << endl;
			// 	cout << "\t" << "sub [" << node->result << "], eax" << endl;
			// } else if(node->op == "=") {
			// 	cout << "\t" << "mov eax, [" << node->arg1 << "]" << endl;
			// 	cout << "\t" << "mov [" << node->result << "], eax" << endl;
			// } else if(node->op == "<") {
			// 	cout << "\t" << "mov eax, [" << node->arg1 << "]" << endl;
			// 	cout << "\t" << "mov [" << node->result << "], eax" << endl;
			// 	cout << "\t" << "mov eax, [" << node->arg2 << "]" << endl;
			// } else if(node->op == "IF-NOT") {
			// 	cout << "\t" << "cmp [" << node->arg1 << "], eax" << endl;
			// 	cout << "\t" << "jge " << string(node->result.begin()+5, node->result.end()) << endl;
			// }
		}
		node = node->next;
	}
	cout << "\t"
		 << "END" << endl;
}

ir_tree *strip_extra(ir_tree *node)
{
	ir_tree *head = NULL, *pre = NULL;
	while (node)
	{
		if ((node->label.size() >= 5 && string(node->label.begin(), node->label.begin() + 5) == "label") || (node->type != "") || save_goto.find(node->label) != save_goto.end())
		{
			if (head == NULL)
				head = node;
			else
				pre->next = node;
			pre = node;
		}
		node = node->next;
	}
	return head;
}

string def_expr(parser_tree *node)
{
	if (node == NULL)
		return "";
	if (node->node_type == "variable" || node->node_type == "Val")
		return node->node_value;
	if (node->node_type == "expression")
	{
		string left = def_expr(node->left);
		string right = def_expr(node->right);
		if (node->node_value == "Add")
			return "(" + left + "+" + right + ")";
		else if (node->node_value == "MINUS")
			return "(" + left + "-" + right + ")";
		else if (node->node_value == "Mul")
			return "(" + left + "*" + right + ")";
		else if (node->node_value == "Div")
			return "(" + left + "/" + right + ")";
		else if (node->node_value == "Mod")
			return "(" + left + "%" + right + ")";
		else if (node->node_value == "Cmp_EL")
			return "(" + left + "<=" + right + ")";
		else if (node->node_value == "Cmp_LT")
			return "(" + left + "<" + right + ")";
		else if (node->node_value == "Cmp_GT")
			return "(" + left + ">" + right + ")";
		else if (node->node_value == "Cmp_EQ")
			return "(" + left + "==" + right + ")";
		else if (node->node_value == "Cmp_EG")
			return "(" + left + ">=" + right + ")";
		else if (node->node_value == "Cmp_and")
			return "(" + left + "&&" + right + ")";
		else if (node->node_value == "Cmp_OR")
			return "(" + left + "||" + right + ")";
		else if (node->node_value == "Cmp_EN")
			return "(" + left + "!=" + right + ")";
		else if (node->node_value == "Bit_right")
			return "(" + left + ">>" + right + ")";
		else if (node->node_value == "Bit_left")
			return "(" + left + "<<" + right + ")";
		else if (node->node_value == "Bit_and")
			return "(" + left + "&" + right + ")";
		else if (node->node_value == "Bit_or")
			return "(" + left + "|" + right + ")";
		else if (node->node_value == "Bit_xor")
			return "(" + left + "^" + right + ")";
		else if (node->node_value == "(expression)")
			return def_expr(node->left);
	}
}

void def_helper(parser_tree *node, vector<string> &hehe)
{
	if (node == NULL)
		return;
	for (int i = node->sibling.size() - 1; i >= 0; --i)
	{
		def_helper(node->sibling[i], hehe);
	}
	if (node->node_type == "variable")
		hehe.push_back(node->node_value);
	else
	{
		string left = node->left->node_value;
		string right = def_expr(node->right);
		hehe.push_back(left + "=" + right);
	}
}

string only_for_def(parser_tree *node)
{
	string type = node->left->node_value;
	vector<string> temp_str;
	def_helper(node->right, temp_str);
	string res = type + " ";
	for (int i = 0; i < temp_str.size(); ++i)
	{
		res += temp_str[i];
		if (i < temp_str.size() - 1)
		{
			res += ",";
		}
		else
		{
			res += ";";
		}
	}
	return res;
}

void get_label(ir_tree *node)
{
	if (node == NULL)
		return;
	while (node)
	{
		if (node->result.size() >= 5 && string(node->result.begin(), node->result.begin() + 5) == "goto ")
		{
			output_label.insert(string(node->result.begin() + 5, node->result.end()));
		}
		node = node->next;
	}
}


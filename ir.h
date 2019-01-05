#include <string>
using namespace std;

struct ir_tree {
	static int node_count;
	static int label_count;
	string op, arg1, arg2, result, label, type;
	ir_tree *next;
	ir_tree();
	ir_tree(string result, string label);
	ir_tree(string op, string arg1, string arg2, string result);
	ir_tree(string op, string arg1, string arg2, string result, string label);
	ir_tree(string op, string arg1, string arg2, string result, string label, string type);
};

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
#include"tree.h"

int i;

struct parser_tree *node(char *_type, char *value, char *_helper)
{
	struct parser_tree *root = new parser_tree();
	if (!root)
	{
		printf("Out of space! Program failed. ");
		exit(0);
	}
	root->node_type = _type;
	root->node_value = value;
	root->node_helper = _helper;
	root->node_var = NULL;
	root->left = NULL;
	root->right = NULL;
	root->temp1 = NULL;
	root->temp2 = NULL;
	return root;
}

struct parser_tree *node(char* v, char *_type, char *value, char *_helper)
{
	struct parser_tree *root = new parser_tree();
	//struct parser_tree *root = (struct parser_tree*)malloc(sizeof(struct parser_tree));
	if (!root)
	{
		printf("Out of space! Program failed. ");
		exit(0);
	}
	root->node_type = _type;
	root->node_value = value;
	root->node_helper = _helper;
	root->node_var = v;
	root->left = NULL;
	root->right = NULL;
	root->temp1 = NULL;
	root->temp2 = NULL;
	return root;
}

struct parser_tree *node(parser_tree *a, char *_type, char *value, char *_helper)
{
	struct parser_tree *root = new parser_tree();
	if (!root)
	{
		printf("Out of space! Program failed. ");
		exit(0);
	}
	root->node_type = _type;
	root->node_value = value;
	root->node_helper = _helper;
	root->node_var = NULL;
	root->left = a;
	root->right = NULL;
	root->temp1 = NULL;
	root->temp2 = NULL;
	return root;
}



struct parser_tree *node(parser_tree *a, parser_tree *b, char *_type, char *value, char *_helper)
{
	struct parser_tree *root = new parser_tree();
	if (!root)
	{
		printf("Out of space! Program failed. ");
		exit(0);
	}
	root->node_type = _type;
	root->node_value = value;
	root->node_helper = _helper;
	root->node_var = NULL;
	root->left = a;
	root->right = b;
	root->temp1 = NULL;
	root->temp2 = NULL;
	return root;
}

struct parser_tree *node(parser_tree *a, parser_tree *b, parser_tree *c, char *_type, char *value, char *_helper)
{
	struct parser_tree *root = new parser_tree();
	if (!root)
	{
		printf("Out of space! Program failed. ");
		exit(0);
	}
	root->node_type = _type;
	root->node_value = value;
	root->node_helper = _helper;
	root->node_var = NULL;
	root->left = a;
	root->right = b;
	root->temp1 = c;
	root->temp2 = NULL;
	return root;
}

void eval(struct parser_tree*root, int level)
{
	if (root != NULL)
	{
		for (int index = root->sibling.size() - 1; index >= 0; --index)
			eval(root->sibling[index], level);
		for (i = 0; i < level; ++i)
		{
			printf("  ");
		}
		if (root->node_type != NULL)
		{
			printf("%s  ", root->node_type);
		}
		if (root->node_value != NULL)
		{
			printf("%s  ", root->node_value);
		}
		if (root->node_helper != NULL)
		{
			printf("%s  ", root->node_helper);
		}
		printf("\n");
		eval(root->left, level + 1);
		eval(root->right, level + 1);
		eval(root->temp1, level + 1);
		eval(root->temp2, level + 1);

	}
}

bool is_val(struct parser_tree* root)
{
	if (root->node_type == "Val")
		return true;
	else
		return false;
}


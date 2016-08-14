#ifndef PARSER_H_
#define PARSER_H_

#include<unordered_set>
#include<unordered_map>
#include<vector>
#include<iostream>
#include "Automaton.h"

using std::unordered_set;
using std::unordered_map;
using std::vector;
using std::cin;
using std::stoi;

static unordered_map <char, char> bracket{
	std::make_pair('{', '}'),
	std::make_pair('[', ']'),
	std::make_pair('(', ')')
};

static unordered_map <char, char> escape_char{
	std::make_pair('f','\f'),
	std::make_pair('n','\n'),
	std::make_pair('r','\r'),
	std::make_pair('t','\t'),
	std::make_pair('v','\v'),
};

static unordered_set<char> space{ ' ', '\t', '\n', '\r' };
static unordered_set<char> num{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
static unordered_set<char> digits{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', \
'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', \
'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', \
'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_'
};
enum op {bind, or , not, repeat, start, end, none_of, any_of};

class token
{
protected:
	bool is_single;
public:
	token();
	~token();
	token(bool single_node);
	bool single();
	virtual void insert(token* node);
	virtual void set_max(int maximum);
	virtual automaton* eval();
};

class single_char : public token
{
protected:
	unordered_set<char> match;
public:
	single_char();
	single_char(char ch);
	single_char(unordered_set<char> chs);
	~single_char();
	void insert(char ch);
	void insert(unordered_set<char> chs);
	automaton* eval();
};

class tree : public token
{
protected:
	op func;
	int min;
	int max;
	vector<token*> nodes;
public:
	tree();
	tree(op function);
	tree(op function, token* node);
	tree(op function, token* node, int min_count, int max_count);
	tree(op function, vector<token*> node);
	~tree();
	void insert(token* node);
	void insert(vector<token*> branches);
	void set_func(op function);
	void set_max(int maximum);
	vector<token*>* get_nodes();
	automaton* eval();
};

tree* parse();
token* escape(char ch);

#endif

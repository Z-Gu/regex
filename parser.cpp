#include "parser.h"

token::token() {};
token::~token() {};
token::token(bool single_node)
{
	is_single = single_node;
}

bool token::single()
{
	return is_single;
}

single_char::single_char() : token(true) {};
single_char::single_char(char ch) : token(true)
{
	match.insert(ch);
}
single_char::single_char(unordered_set<char> chs) : token(true)
{
	match = chs;
}

single_char::~single_char() {};

void single_char::insert(char ch)
{
	match.insert(ch);
}

void single_char::insert(unordered_set<char> chs)
{
	match.insert(chs.begin(), chs.end());
}

tree::tree() : token(false) {};
tree::tree(op function) : token(false)
{
	func = function;
}
tree::tree(op function, token* node) : token(false)
{
	func = function;
	nodes.push_back(node);
}
tree::tree(op function, token* node, int min_count, int max_count) : token(false)
{
	func = function;
	nodes.push_back(node);
	min = min_count;
	max = max_count;
}
tree::tree(op function, vector<token*> node) : token(false)
{
	func = function;
	nodes = node;
}
tree::~tree() {};
void tree::insert(token* tree)
{
	if (tree)
	{
		nodes.push_back(tree);
	}
}
void tree::insert(vector<token*> tree)
{
	for (auto it = tree.begin(); it != tree.end(); ++it)
	{
		nodes.push_back(*it);
	}
}
void tree::set_func(op function)
{
	func = function;
}

void tree::set_max(int max_count)
{
	max = max_count;
}

void token::insert(token* node) {};
void token::set_max(int max_count) {};
automaton* token::eval() { return new automaton(); };

vector<token*>* tree::get_nodes()
{
	return &nodes;
}

token* escape(char ch)
{
	switch (ch)
	{
	case 'b':
		return new single_char(space);
	case 'B':
		return new tree(not, new single_char(space));
	case 'd':
		return new single_char(num);
	case 'D':
		return new tree(not, new single_char(num));
	case 'w':
		return new single_char(digits);
	case 'W':
		return new tree(not, new single_char(digits));
	default:
		if (escape_char.count(ch))
		{
			return new single_char(escape_char[ch]);
		}
		return new single_char(ch);
	}
}

tree* parse()
{
	tree* output = new tree(bind);
	vector<tree*> stack;
	token* buffer = NULL;
	stack.push_back(output);
	bool is_escape = false;
	string number;
	string regex;
	int min, max;
	char prev = 0;
	getline(cin, regex);
	char current;
	int i = 0;
	while (i < regex.size() && stack.size())
	{
		current = regex[i];
		++i;
		if (is_escape)
		{
			buffer = escape(current);
			is_escape = false;
			continue;
		}
		switch (current)
		{
		case '\\':
			is_escape = true;
			stack.back()->insert(buffer);
			buffer = NULL;
			break;
		case '*':
			if (buffer)
			{
				stack.back()->insert(new tree(repeat, buffer, 0, -1));
			}
			buffer = NULL;
			break;
		case '?':
			if (buffer)
			{
				stack.back()->insert(new tree(repeat, buffer, 0, 1));
			}
			buffer = NULL;
			break;
		case '+':
			if (buffer)
			{
				stack.back()->insert(new tree(repeat, buffer, 1, -1));
			}
			buffer = NULL;
			break;
		case '^':
			stack.back()->insert(buffer);
			stack.back()->insert(new tree(start));
			buffer = NULL;
			break;
		case '$':
			stack.back()->insert(buffer);
			stack.back()->insert(new tree(end));
			buffer = NULL;
			break;
		case '.':
			stack.back()->insert(buffer);
			buffer = new tree(not, new single_char('\n'));
			break;
		case '(':
			stack.back()->insert(buffer);
			buffer = NULL;
			stack.push_back(new tree(bind));
			break;
		case ')':
			stack.back()->insert(buffer);
			buffer = stack.back();
			stack.pop_back();
			break;
		case '[':
			stack.back()->insert(buffer);
			current = regex[i];
			++i;
			if (current == '^')
			{
				buffer = new tree(none_of);
				current = regex[i];
				++i;
			}
			else
			{
				buffer = new tree(any_of);
			}
			while (is_escape || current != ']')
			{
				if (is_escape)
				{
					buffer->insert(escape(current));
					is_escape = false;
					prev = 0;
				}
				switch (current)
				{
				case '-':
					current = regex[i];
					++i;
					if (!(digits.count(prev) && digits.count(current) && current > prev))
					{
						throw;
					}
					for (char tmp = prev; tmp <= current; ++tmp)
					{
						buffer->insert(new single_char(tmp));
					}
					prev = 0;
					break;
				case '\\':
					if (prev != '\0')
					{
						buffer->insert(new single_char(prev));
					}
					is_escape = true;
					break;
				default:
					if (prev != '\0')
					{
						buffer->insert(new single_char(prev));
					}
					prev = current;
				}
				current = regex[i];
				++i;
			}
			if (prev != '\0')
			{
				buffer->insert(new single_char(prev));
			}
			prev = 0;
			break;
		case ']':
			throw;
		case '{':
			if (!buffer)
			{
				throw;
			}
			current = regex[i];
			++i;
			while (num.count(current))
			{
				number.push_back(current);
				current = regex[i];
				++i;
			}
			min = stoi(number);
			buffer = new tree(repeat, buffer, min, min);
			switch (current)
			{
			case '}':
				break;
			case ',':
				number.clear();
				current = regex[i];
				++i;
				while (num.count(current))
				{
					number.push_back(current);
					current = regex[i];
					++i;
				}
				if (current != '}')
				{
					throw;
				}
				if (number.size() == 0)
				{
					buffer->set_max(-1);
					break;
				}
				buffer->set_max(stoi(number));
				break;
			default:
				throw;
			}
			stack.back()->insert(buffer);
			buffer = NULL;
			break;
		case '}':
			throw;
		case '|':
			stack.back()->insert(buffer);
			buffer = stack.back();
			stack.pop_back();
			stack.push_back(new tree(or , buffer));
			buffer = NULL;
			break;
		default:
			stack.back()->insert(buffer);
			buffer = new single_char(current);
			break;
		}
	}
	if (i != regex.size() || stack.size() != 1)
	{
		throw;
	}

	stack.back()->insert(buffer);
	return stack.back();
	
}

automaton* single_char::eval()
{
	return new state(match, true, true, false, NULL, NULL);
}

automaton* tree::eval()
{
	vector<automaton*> states;
	int i = 0;
	automaton* tmp = NULL;
	switch (func)
	{
	case bind:
		if (nodes.size() == 1)
		{
			return nodes[0]->eval();
		}
		for(i = 0; i < nodes.size(); ++i)
		{
			states.push_back(nodes[i]->eval());
		}
		for (i = 0; i < nodes.size() - 1; ++i)
		{
			states[i]->set_pass(states[i + 1]);
			states[i]->set_fail(NULL);
			states[i]->set_p_out(false);
			states[i]->set_n_out(false);
		}
		states[i]->set_pass(NULL);
		states[i]->set_fail(NULL);
		states[i]->set_p_out(true);
		states[i]->set_n_out(false);
		return new automata(states, true, true, false, NULL, NULL);
	case or:
		if (nodes.size() == 1)
		{
			return nodes[0]->eval();
		}
		tmp = nodes[0]->eval();
		tmp->set_retry(true);
		states.push_back(tmp);
		nodes.erase(nodes.begin());
		states.push_back(tree(bind, nodes).eval());
		states[0]->set_pass(NULL);
		states[0]->set_fail(states[1]);
		states[0]->set_p_out(true);
		states[0]->set_n_out(false);
		states[1]->set_pass(NULL);
		states[1]->set_fail(NULL);
		states[1]->set_p_out(true);
		states[1]->set_n_out(false);
		return new automata(states, true, true, false, NULL, NULL);
	case start:
		return new state(true);
	case end:
		return new state(false);
	case not:
		tmp = nodes[0]->eval();
		tmp->set_accept(false);
		return tmp;
	case any_of:
		if (nodes.size() == 1)
		{
			return nodes[0]->eval();
		}
		for (i = 0; i < nodes.size(); ++i)
		{
			states.push_back(nodes[i]->eval());
			states[i]->set_single(false);
		}
		for (i = 0; i < nodes.size() - 1; ++i)
		{
			states[i]->set_pass(NULL);
			states[i]->set_fail(states[i + 1]);
			states[i]->set_p_out(true);
			states[i]->set_n_out(false);
		}
		states[i]->set_pass(NULL);
		states[i]->set_fail(NULL);
		states[i]->set_p_out(true);
		states[i]->set_n_out(false);
		tmp = new automata(states, true, true, false, NULL, NULL);
		tmp->set_single(true);
		return tmp;
	case repeat:
		return new automata(nodes[0]->eval(), min, max);
	case none_of:
		if (nodes.size() == 1)
		{
			tmp = nodes[0]->eval();
			tmp->set_accept(false);
			return tmp;
		}
		for (i = 0; i < nodes.size(); ++i)
		{
			states.push_back(nodes[i]->eval());
			states.back()->set_accept(false);
			states[i]->set_single(false);
		}
		for (i = 0; i < nodes.size() - 1; ++i)
		{
			states[i]->set_pass(NULL);
			states[i]->set_fail(states[i + 1]);
			states[i]->set_p_out(true);
			states[i]->set_n_out(false);
		}
		states[i]->set_pass(NULL);
		states[i]->set_fail(NULL);
		states[i]->set_p_out(true);
		states[i]->set_n_out(false);
		tmp = new automata(states, true, true, false, NULL, NULL);
		tmp->set_single(true);
		return tmp;
	}
}
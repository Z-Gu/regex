#include "Automaton.h"
#include<iostream>


state s_w(unordered_set<char> { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', \
'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', \
'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o','p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',\
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'}, true, true, false, NULL, NULL);

state s_W(unordered_set<char> { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', \
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', \
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', \
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_'}, false, false, true, NULL, NULL);

automaton::automaton()
{
	accept = true;
	p_out = false;
	n_out = false;
	positive = NULL;
	negative = NULL;
}

automaton::automaton(bool accept_positive, bool p_output, bool n_output, automaton * pass_next, automaton * fail_next)
{
	accept = accept_positive;
	p_out = p_output;
	n_out = n_output;
	positive = pass_next;
	negative = fail_next;
}

automaton::automaton(const automaton &machine)
{
	accept = machine.accept;
	p_out = machine.p_out;
	n_out = machine.n_out;
	positive = machine.positive;
	negative = machine.negative;
	match_single = machine.match_single;
}

void automaton::set_p_out(bool p_output)
{
	p_out = p_output;
}

void automaton::set_n_out(bool n_output)
{
	n_out = n_output;
}

void automaton::set_accept(bool accept_positive)
{
	accept = accept_positive;
}

void automaton::set_pass(automaton * pass_next)
{
	if (accept)
	{
		positive = pass_next;
	}
	else
	{
		negative = pass_next;
	}
}
void automaton::set_fail(automaton * fail_next)
{
	if (accept == true)
	{
		negative = fail_next;
	}
	else
	{
		positive = fail_next;
	}
}

automaton & automaton::operator=(const automaton machine)
{
	accept = machine.accept;
	p_out = machine.p_out;
	n_out = machine.n_out;
	positive = machine.positive;
	negative = machine.negative;
	match_single = machine.match_single;
	return *this;
	// TODO: insert return statement here
}

automaton::~automaton()
{
	positive = NULL;
	negative = NULL;
}

bool automaton::run(string input,int &n, automaton* &next)
{
	n++;
	return true;
}



bool automaton::is_single()
{
	return match_single;
}

state::state() : automaton()
{
	is_start = false;
	is_end = false;
	match_single = true;
	unordered_set<char> match_set;
}

state::state(unordered_set<char> matched_set, bool accept, bool p_output, bool n_output,\
	automaton* pass_next, automaton* fail_next) \
	: automaton(accept, p_output, n_output, pass_next, fail_next)
{
	match_set = matched_set;
	match_single = true;
}

state::state(const state& machine) : automaton(machine)
{
	match_set = machine.match_set;
	match_single = true;
}

state::~state()
{
	automaton::~automaton();
	match_set.clear();
}

state& state::operator=(const state machine) 
{
	automaton::operator=(machine);
	match_set = machine.match_set;
	return *this;
}


bool state::run(string input, int &n, automaton* &next)
{
	if (n >= input.size())
	{
		next = NULL;
		return false;
	}
	if (match_set.count(input[n]) == accept \
		|| (is_start && n == 0) \
		|| (is_end && n == input.size() -1))
	{
		if (n < input.size())
		{
			next = positive;
		}
		else
		{
			next = NULL;
		}
		if (match_single)
		{
			n++;
			if (n == input.size())
			{
				next = NULL;
			}
		}
		return p_out;
	}
	if (n < input.size())
	{
		next = negative;
	}
	else
	{
		next = NULL;
	}
	return n_out;
}

automata::automata() : automaton()
{
	states.clear();
}

automata::automata(vector<automaton*> expressions, \
	bool accept_positive, bool p_output, bool n_output, automaton* pass_next, automaton* fail_next)
	: automaton(accept_positive, p_output, n_output, pass_next, fail_next)
{
	states = expressions;
}

automata::automata(const automata &machine): automaton(machine)
{
	states = machine.states;
}

automata::automata(automaton* target, int min, int max)
{
	int i = 0;
	if (max < 0)
	{
		states.resize(min + 1);
		for (i = 0; i < min + 1; ++i)
		{
			states[i] = target->clone();
			states[i]->set_fail(NULL);
			states[i]->set_p_out(false);
			states[i]->set_n_out(false);
		}
		if (min > 0)
		{
			states[min - 1]->set_p_out(true);
		}
		for (i = 0; i < min; ++i)
		{
			states[i]->set_pass(states[i + 1]);
		}
		states.back()->set_pass(states.back());
		states.back()->set_fail(NULL);
		states.back()->set_p_out(true);
		states.back()->set_n_out(true);
	}
	else
	{
		states.resize(max);
		for (i = 0; i < max; ++i)
		{
			states[i] = target->clone();
			states[i]->set_fail(NULL);
			states[i]->set_p_out(false);
			states[i]->set_n_out(false);
		}
		for (i = 0; i < max - 1; ++i)
		{
			states[i]->set_pass(states[i + 1]);
		}

		states.back()->set_pass(NULL);
		states[min - 1]->set_p_out(true);
		states[min - 1]->set_n_out(false);
		for (i = min; i < max; ++i)
		{
			states[i]->set_p_out(true);
			states[i]->set_n_out(true);
		}
	}
	p_out = true;
	n_out = false;
	positive = NULL;
	negative = NULL;
}

automata::~automata()
{
	automaton::~automaton();
	states.clear();
}

bool automata::run(string input, int &n, automaton* &next)
{
	acculumator = -1;
	if (retry)
	{
		acculumator++;
	}
	automaton* current = states[0];
	int b = n;
	bool output = current->run(input, n, next);
	while (next != NULL && n < input.size())
	{
		if (current->is_single())
		{
			acculumator++;
		}
		current = next;
		output = next->run(input, n, next);
	}
	if (output == accept)
	{
		if (n < input.size())
		{
			next = positive;
		}
		else
		{
			next = NULL;
		}
		if (match_single)
		{
			n++;
		}
		return p_out;
	}
	if (!match_single)
	{
		n -= acculumator;
	}
	if (n < input.size())
	{
		next = negative;
	}
	else
	{
		next = NULL;
	}
	
	return n_out;

}

automata& automata::operator=(const automata machine)
{
	automaton::operator=(machine);
	states = machine.states;
	return *this;
}


automaton* automaton::clone() const
{
	return new automaton(*this);
}

state* state::clone() const
{
	return new state(*this);
}

automata* automata::clone() const
{
	return new automata(*this);
}

state::state(bool start_or_end)
{
	start_or_end ? is_start = true : is_end = true;
	match_single = false;
}

void automaton::set_single(bool is)
{
	match_single = is;
}
#include "Automaton.h"
#include "parser.h"
#include<iostream>
#include<fstream>
#include<string>

int main()
{
	using std::cout;
	using std::cin;
	using std::endl;
	using std::ifstream;

	std::string regex;
	A:
	cout << "Enter your regular expression\n";
	/*
	std::getline(std::cin, regex);
	automata fsa(regex);
	fsa.set_pass(&fsa);
	fsa.set_fail(&fsa);
	//fsa.init();
	*/
	tree* syntax_tree = parse();
	automaton* fsa = syntax_tree->eval();
	if (!(fsa->is_automata()))
	{
		fsa = new automata(fsa, 1,1);
	}
	delete syntax_tree;
	fsa->set_pass(fsa);
	fsa->set_fail(fsa);
	std::string text;
	cout << "Enter your text\n";
	std::getline(std::cin, text);

	int n = 0;
	int count = 0;
	int b = 0;
	automaton* current;
	vector<int> matched;
	do
	{
		b = n;
		if (fsa->run(text, n, current))
		{
			count++;
			//cout << n << endl;
			matched.push_back(b);
			matched.push_back(n - b);
		}
		cout << n << endl;
	}
	while (current != NULL);

	cout << "Total match found " << count << endl;
	for (int i = 0; i < matched.size(); i += 2)
	{
		cout << matched[i] + 1 << ' ' << text.substr(matched[i], matched[i + 1]) << endl;
	}
	goto A;
	return 0;
}


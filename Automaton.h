#ifndef STATE_H_
#define STATE_H_


#include<vector>
#include<unordered_set>
#include<vector>
#include<string>


using std::unordered_set;
using std::vector;
using std::string;
/*
Positive/Negative corresponds to whether the char is recognized by the state/ the string is recognized by the automata, pass/fail
corresponds to whether the automaton test is passed or not, the connection between positive/negative and pass/fail controlled by accept.
e.g. '[K]' will respond positive and pass to 'K';
     '[^K]' will respond positive and fail to 'K';
*/

class automaton 
{
protected:
	bool accept;
	bool p_out;
	bool n_out;
	bool match_single = false;
	automaton* positive;
	automaton* negative;
public:
	automaton();
	automaton(bool accept_positive, bool p_output, bool n_output, automaton* pass_next, automaton* fail_next);
	automaton(const automaton &machine);
	void set_accept(bool accept_positive);
	void set_p_out(bool p_output);
	void set_n_out(bool n_output);
	void set_pass(automaton* fail_next);
	void set_fail(automaton* fail_next);
	void set_single(bool is);
	virtual ~automaton();
	virtual bool run(string input, int &n, automaton* &next);
	virtual automaton& automaton::operator=(const automaton machine);
	virtual inline void set_retry(bool is_retry) {};
	bool is_single();
	virtual inline bool is_automata() { return false; };
	virtual automaton* clone() const;
};

class state: public automaton
{
private:
	unordered_set<char> match_set;
	bool is_start;
	bool is_end;
public:
	state();
	state(bool start_or_end);
	state(unordered_set<char> matched_set, bool accept_positive, bool p_output, bool n_output,\
		automaton* pass_next, automaton* fail_next);
	state(const state& machine);
	~state();
	state& state::operator=(const state machine);
	bool run(string input, int &n, automaton* &next);
	state* clone() const;
};
 
class automata: public automaton
{
private:
	int acculumator;
	bool retry = false;
	vector<automaton*> states;
public:
	automata();
	automata(vector<automaton*> expressions, \
		bool accept_positive, bool p_output, bool n_output, automaton* pass_next, automaton* fail_next);
	automata(const automata &machine);
	automata(automaton* target, int min, int max);
	~automata();
	automata& automata::operator=(const automata machine);
	bool run(string input, int &n, automaton* &next);
	inline void set_retry(bool is_retry) { retry = is_retry; };
	automata* clone() const;
	inline bool is_automata() { return true; };
};

#endif

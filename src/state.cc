/*
 * state.cpp
 *
 *  Created on: May 22, 2013
 *      Author: lpzun
 */

#include "state.hh"

namespace STATE {
ushort Global_State::s;
ushort Global_State::l;

Utilities::Utilities() {
	// TODO Auto-generated constructor stub

}

Utilities::~Utilities() {
	// TODO Auto-generated destructor stub
}

/**
 *
 * @param s
 * @param delim
 * @param elems
 * @return
 */
vector<string> Utilities::split(const string &s, char delim, vector<string> &elems) {
	std::stringstream ss(s);
	string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/**
 * @brief
 * @param s
 * @param delim
 * @return
 */
vector<string> Utilities::split(const string &s, const char& delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

/**
 *
 * @param s_ts
 * @param delim
 * @return
 */
Thread_State Utilities::create_thread_state_from_str(const string& s_ts, const char& delim) {
	vector<string> vs_ts = split(s_ts, delim);
	if (vs_ts.size() != 2) {
		throw CONTROL::Error("The format of thread state is wrong.");
	}
	Thread_State ts(atol(vs_ts[0].c_str()), atol(vs_ts[1].c_str()));
	return ts;
}

/**
 *
 * @param s_ts
 * @param delim
 * @return
 */
Global_State Utilities::create_global_state_from_str(const string& s_gs, const char& delim) {
	vector<string> vs_ts = split(s_gs, delim);
	if (vs_ts.size() != 2) {
		throw CONTROL::Error("The format of thread state is wrong.");
	}
	vector<string> vs_locals = split(vs_ts[1], ',');
	map<ushort, ushort> locals;
	for (ushort i = 0; i < vs_locals.size(); i++) {
		ushort local = atol(vs_locals[i].c_str());
		if (locals.find(local) != locals.end())
			locals[local]++;
		else
			locals[local] = 1;
	}
	Global_State gs(atol(vs_ts[0].c_str()), locals);
	//cout << " Target state" << endl;
	return gs;
}

/**
 * @brief print all of the transitions in the thread-state transition diagram
 * @param adjacency_list
 * @param out
 */
void Utilities::print_adj_list(const map<Thread_State, list<Thread_State> >& adjacency_list, ostream& out) {
	out << Global_State::s << " " << Global_State::l << endl;
	for (map<Thread_State, list<Thread_State> >::const_iterator pair = adjacency_list.begin(), end =
			adjacency_list.end(); pair != end; ++pair) {
		const Thread_State& t = pair->first;
		const list<Thread_State>& successors = pair->second;
		__SAFE_ASSERT__ (! successors.empty());
		for (list<Thread_State>::const_iterator succ = successors.begin(), end = successors.end(); succ != end;
				++succ) {
			out << t << " -> " << (*succ) << endl;
		}
	}
}
} /* namespace STATE */

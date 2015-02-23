/*
 * state.h
 *
 *  Created on: May 22, 2013
 *      Author: lpzun
 */

#ifndef STATE_H_
#define STATE_H_

#include  "functions.hh"

namespace STATE {

/**
 *@brief Thread State class
 */
class Thread_State {

public:
	ushort shared;
	ushort local;

	inline Thread_State();
	inline Thread_State(const Thread_State& ts);
	inline Thread_State(const cushort& shared, const cushort& local);

	ostream& to_stream(ostream& out = cout) const;
};

/**
 * @brief constructor with thread state
 * @param ts
 */
inline Thread_State::Thread_State(const Thread_State& ts) :
		shared(ts.shared), local(ts.local) {

}

/**
 * @brief constructor with a shared state and a local state
 * @param shared
 * @param local
 */
inline Thread_State::Thread_State(const cushort& shared, const cushort& local) :
		shared(shared), local(local) {
}

/**
 *
 * @param out
 * @return ostream
 */
inline ostream& Thread_State::to_stream(ostream& out) const {
	out << "(" << shared << "|" << local << ")";
	return out;
}

/**
 *
 * @param out
 * @param ts
 * @return ostream
 */
inline ostream& operator <<(ostream& out, const Thread_State& ts) {
	return ts.to_stream(out);
}

/**
 * @brief thread state comparator
 * @param ts1
 * @param ts2
 * @return bool
 */
inline bool operator <(const Thread_State& ts1, const Thread_State& ts2) {
	if (ts1.shared != ts2.shared) {
		return ts1.shared < ts2.shared;
	}
	return ts1.local < ts2.local;
}

/**
 *
 * @param ts1
 * @param ts2
 * @return
 */
inline bool operator >(const Thread_State& ts1, const Thread_State& ts2) {
	return ts2 < ts1;
}

/**
 *
 * @param ts1
 * @param ts2
 * @return
 */
inline bool operator ==(const Thread_State& ts1, const Thread_State& ts2) {
	if (ts1.shared == ts2.shared && ts1.local == ts2.local)
		return true;
	return false;
}

/**
 *
 * @param ts1
 * @param ts2
 * @return
 */
inline bool operator !=(const Thread_State& ts1, const Thread_State& ts2) {
	return !(ts1 == ts2);
}

class Transition {
public:
	Thread_State src;
	Thread_State dest;
	bool is_expanded;

	inline Transition();
	inline Transition(const Thread_State& src, const Thread_State& dest);
	inline Transition(const Thread_State& src, const Thread_State& dest, const bool& exp);

	ostream& to_stream(ostream& out = cout) const;
};

inline Transition::Transition() :
		is_expanded(false) {

}
inline Transition::Transition(const Thread_State& src, const Thread_State& dest) {
	this->src = src;
	this->dest = dest;
	this->is_expanded = false;
}

inline Transition::Transition(const Thread_State& src, const Thread_State& dest, const bool& exp) {
	this->src = src;
	this->dest = dest;
	this->is_expanded = exp;
}

inline ostream& Transition::to_stream(ostream& out) const {
	if (!is_expanded) {
		out << src << " -> " << dest;
	} else {
		out << src << " ~> " << dest;
	}
	return out;
}

/**
 *
 * @param e1
 * @param e2
 * @return
 */
inline bool operator <(const Transition& e1, const Transition& e2) {
	if (e1.src < e2.src) {
		return true;
	}
	if (e1.src == e2.src && e1.dest < e2.dest) {
		return true;
	}
	return false;
}

/**
 *
 * @param e1
 * @param e2
 * @return
 */
inline bool operator >(const Transition& e1, const Transition& e2) {
	return e2 < e1;
}

/**
 * @brief operator == for transitions
 * @param e1
 * @param e2
 * @return bool: if equal, then return true
 */
inline bool operator ==(const Transition& e1, const Transition& e2) {
	if (e1.src == e2.src && e1.dest == e2.dest) {
		return true;
	}
	return false;
}

/**
 * @brief operator != for transitions
 * @param e1
 * @param e2
 * @return bool:  if not equal, then return true
 */
inline bool operator !=(const Transition& e1, const Transition& e2) {
	return !(e1 == e2);
}

/**
 *
 * @param out
 * @param gs
 * @return
 */
inline ostream& operator <<(ostream& out, const Transition& t) {
	return t.to_stream(out);
}

/**
 *@brief Global State class
 */
class Global_State {

public:
	ushort shared;
	map<ushort, ushort> locals;
	ushort depth; //used to output the BWS tree;

	uint ID;
	Transition transition;
	uint succ_ID;

	static ushort s;
	static ushort l;

	inline Global_State(const Thread_State& ts);
	inline Global_State(const Global_State& gs);
	inline Global_State(const Global_State& gs, cushort& depth);
	inline Global_State(cushort& shared, const map<ushort, ushort> locals);
	inline Global_State(cushort& shared, const map<ushort, ushort> locals, cushort& depth);
	inline Global_State(cushort& shared, const map<ushort, ushort> locals, cushort& depth, const uint& ID,
			const Transition& transition, const uint& succ_ID);

	ostream& to_stream(ostream& out = cout, const string& sep = "|") const;
};

/**
 * @brief constructor with a thread state
 * @param ts
 */
inline Global_State::Global_State(const Thread_State& ts) :
		ID(0), succ_ID(0) {
	this->shared = ts.shared;
	this->locals[ts.local] = 1;
	this->depth = 0;
}

/**
 * @brief constructor with a global state
 * @param gs
 */
inline Global_State::Global_State(const Global_State& gs) :
		shared(gs.shared), locals(gs.locals), depth(gs.depth), ID(gs.ID), transition(gs.transition), succ_ID(gs.succ_ID) {
}

/**
 * @brief constructor with a global state and the depth
 * @param gs
 * @param depth
 */
inline Global_State::Global_State(const Global_State& gs, cushort& depth) :
		shared(gs.shared), locals(gs.locals), depth(depth), ID(0), succ_ID(0) {

}

/**
 * @brief constructor with a shared state, local states and the depth
 * @param shared
 * @param locals
 */
inline Global_State::Global_State(cushort& shared, const map<ushort, ushort> locals) :
		shared(shared), locals(locals), depth(0), ID(0), succ_ID(0) {
}

/**
 * @brief constructor with a shared state, local states and the depth
 * @param shared
 * @param locals
 * @param depth
 */
inline Global_State::Global_State(cushort& shared, const map<ushort, ushort> locals, cushort& depth) :
		shared(shared), locals(locals), depth(depth), ID(0), succ_ID(0) {
}

/**
 *
 * @param shared
 * @param locals
 * @param depth
 * @param ID
 * @param transition
 * @param successor
 */
inline Global_State::Global_State(cushort& shared, const map<ushort, ushort> locals, cushort& depth, const uint& ID,
		const Transition& transition, const uint& succ_ID) :
		shared(shared), locals(locals), depth(depth), ID(ID), transition(transition), succ_ID(succ_ID) {
}
/**
 * @brief stream which prints global state
 * @param out
 * @param sep
 * @return ostream
 */
inline ostream& Global_State::to_stream(ostream& out, const string& sep) const {
	out << "<" << shared << sep;
	for (map<ushort, ushort>::const_iterator iter = locals.begin(), end = locals.end(); iter != end; iter++) {
		out << "(" << iter->first << "," << iter->second << ")";
	}
	out << ">";
	return out;
}

/**
 *
 * @param out
 * @param gs
 * @return
 */
inline ostream& operator <<(ostream& out, const Global_State& gs) {
	return gs.to_stream(out);
}

/**
 *
 * @param s1
 * @param s2
 * @return bool
 */
inline bool operator<(const Global_State& s1, const Global_State& s2) {
	if (s1.shared == s2.shared) {
		short result = COMPARE::compare_map(s1.locals, s2.locals);
		return result == -1;
	} else {
		return s1.shared < s2.shared;
	}
}

/**
 *
 * @param s1
 * @param s2
 * @return
 */
inline bool operator>(const Global_State& s1, const Global_State& s2) {
	return s2 < s1;
}

/**
 * @brief overload comparator ==
 * @param s1
 * @param s2
 * @return bool
 */
inline bool operator==(const Global_State& s1, const Global_State& s2) {
	if (s1.shared == s2.shared) {
		if (s1.locals.size() == s2.locals.size()) {
			map<ushort, ushort>::const_iterator s1_iter = s1.locals.begin(), s1_end = s1.locals.end();
			map<ushort, ushort>::const_iterator s2_iter = s2.locals.begin();
			while (s1_iter != s1_end) {
				if ((s1_iter->first != s2_iter->first) || (s1_iter->second != s2_iter->second)) {
					return false;
				}
				s1_iter++, s2_iter++;
			}
			return true;
		}
	}
	return false;
}

/**
 *
 * @param s1
 * @param s2
 * @return bool
 */
inline bool operator!=(const Global_State& s1, const Global_State& s2) {
	return !(s1 == s2);
}

/**
 * @brief default constructor
 */
inline Thread_State::Thread_State() :
		shared(Random::integer(0, Global_State::s - 1)), local(Random::integer(0, Global_State::l - 1)) {
}

class Utilities {
public:
	Utilities();
	virtual ~Utilities();

	vector<string> split(const string &s, char delim, vector<string> &elems);
	vector<string> split(const string &s, const char& delim);
	Thread_State create_thread_state_from_str(const string& s_ts, const char& delim = '|');
	Global_State create_global_state_from_str(const string& s_gs, const char& delim = '|');
	void print_adj_list(const map<Thread_State, list<Thread_State> >& adjacency_list, ostream& out = cout);
};

} /* namespace STATE */
#endif /* STATE_H_ */

/******************************************************************************
 * bwra.cc This is under Git control
 *
 *  Created on: Jan 31, 2014
 *      Author: lpzun
 ******************************************************************************/

#include "bwra.hh"

BWS::BWS() {
	// TODO Auto-generated constructor stub
}

BWS::~BWS() {
	// TODO Auto-generated destructor stub
}

/**
 * @brief determine whether s equals initial state
 * @param s
 * @param init_ts
 * @return bool
 * 					true: s <= init_ts
 * 					false: s > or incomparable init_ts
 */
bool BWS::is_less_or_eq_init(const Global_State& s, const Thread_State& init_ts) {
	if (s.shared == init_ts.shared) {
		if (s.locals.size() == 1) {
			if (s.locals.begin()->first == init_ts.local) {
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief determine whether s1 <= s2
 * @param s1
 * @param s2
 * @return bool
 * 					true: s1 <= s2
 * 					false: s1 > s2 or incomparable
 */
bool BWS::is_less_or_eq(const Global_State& s1, const Global_State& s2) {
	if (s1.shared == s2.shared) {
		for (map<ushort, ushort>::const_iterator iter = s1.locals.begin(), end = s1.locals.end(); iter != end; iter++) {
			if (s2.locals.find(iter->first) == s2.locals.end()) { // s2 doesn't contain a specific local
				return false;
			} else {
				ushort s2_count = s2.locals.find(iter->first)->second;
				if (iter->second > s2_count) { // s1.counter > s2.counter
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

/**
 * @brief Abdulla's backward reachability analysis
 *
 * @param init_ts: the initial state of Boolean program
 * @param target: the set of thread states which we want to determine if there exist reachable ones.
 * @param inverse_adj_list
 * @return bool
 *              true: if target is reachable from s, s \in upperward(init)
 *              false: if target is unreachable
 */
bool BWS::backward_search(const Thread_State& init_ts, const Global_State& target,
		const map<Thread_State, list<Thread_State> >& inverse_adj_list) {
	queue<Global_State, list<Global_State> > to_explore_states; // the set of to explore state: NOTE: use list
	set<Global_State> explored_states; //the set of already explored state
	ushort curr_depth = 0;

	Global_State bws_start_state(target);
	to_explore_states.push(bws_start_state);

	//the following piece of code is only for output the bws tree
	multimap<ushort, Global_State> to_explores_tree;
	to_explores_tree.insert(std::pair<ushort, Global_State>(curr_depth, bws_start_state));

//	cout << endl;
//	cout << "backward search starts from: " << bws_start_state << endl;
	if (is_less_or_eq_init(bws_start_state, init_ts)) { // if current state is <= \upperward(init)
		return true;
	}

	ushort solns = 0;
	while (!to_explore_states.empty()) {
		Global_State explore = to_explore_states.front();
		to_explore_states.pop();

		bool is_exist_less = false;
		for (set<Global_State>::const_iterator iter = explored_states.begin(), end = explored_states.end(); iter != end;
				iter++) {
			if (is_less_or_eq(*iter, explore)) {
				is_exist_less = true; //if exists a smaller state
				break;
			}
		}

		if (is_exist_less) {
			continue;
		} else {
			curr_depth = explore.depth + 1; // current depth increases by 1
			set<Global_State> cov_pre_states;
			cov_pre_states = compute_cov_predecessors(explore, inverse_adj_list);

			for (set<Global_State>::const_iterator iter = cov_pre_states.begin(), end = cov_pre_states.end();
					iter != end; iter++) {
				const Global_State& state = *iter;
				to_explore_states.push(state);

				if (IS_BWS_TREE || IS_ALL) { //insert a new to_explore into output container
					to_explores_tree.insert(std::pair<ushort, Global_State>(curr_depth, state));
				}

				if (is_less_or_eq_init(state, init_ts)) { //if one of the explore's predecessors is less than upperward(init)
					solns++;
					if (IS_BWS_TREE || IS_ALL) { //output BWS tree. Note: not a good design?
						//output_to_explores_tree(to_explores_tree, curr_depth);
						output_bws_witness_path(to_explores_tree, state, solns);
					}
					if (!is_all_solns)
						return true;
				}
			}

			//remove the "state =< explore" from explored_states
			for (set<Global_State>::const_iterator iter = explored_states.begin(), end = explored_states.end();
					iter != end; iter++) {
				if (is_less_or_eq(*iter, explore)) {
					explored_states.erase(iter);
				}
			}
			// insert "explore" to explored_states
			std::pair<set<Global_State>::iterator, bool> result = explored_states.insert(explore);
			if (!result.second) {
				//cout << explore << " inserts failed!" << endl;
			}
		}
	}

//	if (is_bws_tree || is_all) { //output BWS tree. Note: not a good design?
//		output_to_explores_tree(to_explores_tree, curr_depth);
//	}

	if (is_all_solns && solns > 0) {
		cout << "There are " << solns << " solutions" << endl;
		return true;
	}
	return false;
}

/**
 * @brief compute the cover predecessors of a given global state
 * @param state
 * @param inverse_adj_list
 * @return set<Global_State>
 * 					 cover predecessors
 */
set<Global_State> BWS::compute_cov_predecessors(const Global_State& state,
		const map<Thread_State, list<Thread_State> >& inverse_adj_list) {
	set<Global_State> cov_pre_states; //some container declarations to store cover predecessors

	for (ushort local = 0; local < Global_State::l; local++) {
		Thread_State curr_ts(state.shared, local);
		map<Thread_State, list<Thread_State> >::const_iterator ifind = inverse_adj_list.find(curr_ts);
		if (ifind != inverse_adj_list.end()) {
			list<Thread_State> predecessors = ifind->second;
			if (predecessors.size() >= 1) {
				for (list<Thread_State>::const_iterator liter = predecessors.begin(), lend = predecessors.end();
						liter != lend; liter++) {
					const Thread_State& pre_ts = *liter;
					const Global_State& cov_pre = this->update_counter(curr_ts, pre_ts, state);
					std::pair<set<Global_State>::iterator, bool> p = cov_pre_states.insert(cov_pre);
					if (!p.second) {
					}
				}
			}
		}
	}

	return cov_pre_states;
}

/**
 * @brief Compute the preceding states
 * @param curr_ts: the current thread state
 * @param pre_ts: the preceding thread state
 * @param locals: the list of local states with explicit
 * 				  counter abstraction representation
 * @return a State after update the local states' counters
 */
Global_State BWS::update_counter(const Thread_State & curr_ts, const Thread_State & pre_ts, const Global_State& state) {
	map<ushort, ushort> ilocals = state.locals;

	map<ushort, ushort>::iterator ifind_c = ilocals.find(curr_ts.local);
	if (ifind_c != ilocals.end()) {
		ilocals[curr_ts.local]--;
		if (ilocals[curr_ts.local] == 0) {
			ilocals.erase(ifind_c);
		}
	}

	map<ushort, ushort>::iterator ifind_p = ilocals.find(pre_ts.local);
	if (ifind_p != ilocals.end()) {
		ilocals[pre_ts.local]++;
	} else {
		ilocals.insert(std::pair<ushort, ushort>(pre_ts.local, 1));
	}

	Transition t(pre_ts, curr_ts);
	Global_State cov_pre(pre_ts.shared, ilocals, state.depth + 1, ++state_ID, t, state.ID);
	return cov_pre;
}

/**
 * @brief print the BWS states as a tree
 * @param output_explores_tree
 * @param curr_depth
 */
void BWS::output_to_explores_tree(const multimap<ushort, Global_State>& output_explores_tree,
		const ushort& curr_depth) {
	cout << endl;
	cout << PPRINT::plural(output_explores_tree.size(), "state") << " backward analyzed" << (true ? ":" : "") << endl;
	for (ushort d = 0; d <= curr_depth; ++d) {
		bool depth_found = false;
		for (multimap<ushort, Global_State>::const_iterator cc = output_explores_tree.begin(), end =
				output_explores_tree.end(); cc != end; ++cc) {
			if (cc->first == d) {
				depth_found = true;
				cout << "(depth = " << std::setw(3) << d << ")" << std::setw(3) << cc->second << endl;
			}
			if (!depth_found) {
				//cout << setw(13) << "    (depth = " << setw(0) << d << ")" << endl;
			}
		}
	}
}

/**
 * @brief output the witness path
 * @param output_explores_tree
 * @param initial
 * @param path
 */
void BWS::output_bws_witness_path(const multimap<ushort, Global_State>& output_explores_tree,
		const Global_State& initial, const ushort& path) {
	cout << endl;
	Global_State state = initial;
	list<Transition> transitions;

	while (state.ID > 0) {
		cout << PPRINT::widthify(state, 20, PPRINT::LEFTJUST, ' ') << std::setw(30) << state.transition << endl;
		Transition t = state.transition;
		transitions.push_back(state.transition);
		for (multimap<ushort, Global_State>::const_iterator cc = output_explores_tree.begin(), end =
				output_explores_tree.end(); cc != end; ++cc) {
			if (cc->second.ID == state.succ_ID) {
				state = cc->second;
				break;
			}
		}
		if (t.dest != state.transition.src) {
			Transition expansion(t.dest, state.transition.src, true);
			transitions.push_back(expansion);
		}
	}
	string filename = FILE_NAME_PREFIX + "_path" + COMPARE::num2string(path) + ".dot";
	output_ettd_to_dot_file(filename, transitions);
}

/**
 *
 * @param filename
 * @param transitions
 */
void BWS::output_ettd_to_dot_file(const string& filename, const list<Transition>& transitions) {
	ofstream fout;
	fout.open(filename.c_str());
	if (!fout.is_open()) {
		cerr << " create file " << filename << " failed!" << endl;
	} else {
		fout << "// This file is generated from Press, which aims to visualize the Expanded TTD" << endl;
		fout << "// NOTE: convert .dot to .ps using the following command:" << endl;
		fout << "//\t\t neato -Tps -n2 -s1 <.dot file> -o <.ps file>" << endl;
		fout << "// You can also convert to other format, say fig, with corresponding commands" << endl;
		fout << endl;

		fout << "digraph ttd {" << endl;
		fout << "\t pagedir=BL; nodesep =.5; rankdir=BT; overlap=false; splines = spline;" << endl;
		fout << "\t node[shape=plaintext];" << endl;
		fout << "\t edge [arrowhead=vee, arrowsize=0.5];" << endl;

		float x = 0.0, y = 0.0;
		for (ushort s = 0; s < Global_State::s; s++) {
			for (ushort l = 0; l < Global_State::l; l++) {
				fout << "\t \"(" << s << "," << l << ")\"" << "[pos=\"" << x << "," << y << "\"];" << endl;
				x += 80.0;
			}
			fout << endl;
			y += 50.0;
			x = 0.0;
		}

		fout << endl;
		for (list<Transition>::const_iterator itran = transitions.begin(), end = transitions.end(); itran != end;
				itran++) {
			fout << "\t \"(" << itran->src.shared << "," << itran->src.local << ")\" -> \"(" << itran->dest.shared
					<< "," << itran->dest.local << ")\"";
			if (itran->is_expanded) {
				fout << "[style=dotted, color=red]";
			}
			fout << ";" << endl;
		}

		fout << "}" << endl;
		fout.flush();
		fout.close();
	}
}


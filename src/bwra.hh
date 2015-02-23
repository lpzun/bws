/*
 * bwra.hh
 *
 *  Created on: Jan 31, 2014
 *      Author: lpzun
 */

#ifndef BWRA_HH_
#define BWRA_HH_
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>

#include "state.hh"
#include "refs.hh"

using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;

using namespace STATE;
class BWS {
private:

public:
	BWS();
	virtual ~BWS();

	bool backward_search(const Thread_State& init_ts, const Global_State& target,
			const map<Thread_State, list<Thread_State> >& inverse_adj_list);

	bool is_less_or_eq_init(const Global_State& s, const Thread_State& init_ts);
	bool is_less_or_eq(const Global_State& s1, const Global_State& s2);
	Global_State update_counter(const Thread_State & curr_ts, const Thread_State & pre_ts, const Global_State& state);
	set<Global_State> compute_cov_predecessors(const Global_State& state,
			const map<Thread_State, list<Thread_State> >& inverse_adj_list);
	void output_to_explores_tree(const multimap<ushort, Global_State>& output_explores_tree, const ushort& curr_depth);
	void output_bws_witness_path(const multimap<ushort, Global_State>& output_explores_tree,
			const Global_State& initial, const ushort& path);
	void output_ettd_to_dot_file(const string& filename, const list<Transition>& transitions);
};

#endif /* BWRA_HH_ */

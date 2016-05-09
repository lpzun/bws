//============================================================================
// Name       : bws.cc
// Author      : Peizun Liu
// Date         :  Sep. 1, 2013
// Version     :
// Copyright   : The copyright belongs to CAV group of CCIS at NEU
// Description : backward reachability analysis in C++, Ansi-style
//============================================================================
#include <stdlib.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>

#include "state.hh"
#include "input.hh"
#include "bwra.hh"
#include "refs.hh"

using std::setw;
using std::endl;
using std::cout;
using namespace std;
using namespace STATE;

/**
 * @brief
 * @param t
 * @param successors
 */
void add_successor(const Thread_State& t, list<Thread_State>& successors) {
    while (true) {
        Thread_State succ; // random thread state, i.e., call the default constructor "Thread_State()"
        if (succ != t) { //find if there are some "succ" in successors, return 0 if not
            list<Thread_State>::const_iterator ifind = std::find(
                    successors.begin(), successors.end(), succ);
            if (ifind == successors.end()) {
                successors.push_back(succ);
                break;
            }
        }
    }
}

/**
 * @brief main function: the entry of this program
 * @param argc
 * @param argv
 * @return int
 * 					 0: successful
 */
int main(const int argc, const char * const * const argv) {
    try {
        Input ins;
        //"X" is the default filename, means there are no input file
        ins.add_argument("-f", "input file (if given, don't specify s,l,e)", "",
                "X");
        ins.add_argument("s",
                "number of shared states (i.e. 1 shared variable with range [0..(s-1)])",
                "", "0");
        ins.add_argument("l", "number of local states", "", "0");
        ins.add_argument("e",
                "number of thread transitions (no self loops; enter 0 to get info on maximum value)",
                "", "0");
        ins.add_argument("--initial", "the initial thread state", "", "0|0");
        ins.add_argument("--target", "the target state or target state file",
                "", "0|0");

        ins.add_switch("--adj-list", "whether to print the adjacency list");
        ins.add_switch("--cmd-line", "whether to print the command line");
        ins.add_switch("--bwstree",
                "whether to print the backward search tree");
        ins.add_switch("--all-paths",
                "whether to find all of the bws reachable path");
        ins.add_switch("--all", "whether to print all of the above");

        try {
            ins.get_command_line(argc, argv);
        } catch (Input::Help) {
            return 0;
        }

        IS_ALL = ins.arg2bool("--all");
        if (ins.arg2bool("--cmd-line") || IS_ALL) {
            ins.print_command_line(0);
        }
        IS_BWS_TREE = ins.arg2bool("--bwstree");
        is_all_solns = ins.arg2bool("--all-paths");

        map<Thread_State, list<Thread_State> > adj_list;
        map<Thread_State, list<Thread_State> > inverse_adj_list; //the reverse transition

        const string filename = ins.arg_value("-f");
        FILE_NAME_PREFIX = filename.substr(0, filename.find_last_of("."));
        const string s_initial = ins.arg_value("--initial");
        const string s_target = ins.arg_value("--target");

        Utilities util;
        Thread_State init_ts = util.create_thread_state_from_str(s_initial);
        Global_State target(init_ts);
        if (s_target.find('|') != std::string::npos) {
            target = util.create_global_state_from_str(s_target);
        } else {
            ifstream in(s_target.c_str());
            string s_ts;
            std::getline(in, s_ts);
            target = util.create_global_state_from_str(s_ts);
//			while (in >> s_ts) {
//				target_ts = util.create_thread_state_from_str(s_ts);
//			}
            in.close();
        }
        if (filename == "X") { // make random thread-state transition diagram
            Global_State::s = atoi(ins.arg_value("s").c_str());
            Global_State::l = atoi(ins.arg_value("l").c_str());
            ushort e = atol(ins.arg_value("e").c_str());
            culong T = Global_State::s * Global_State::l; // number of thread states

            culong e_max = T * (T - 1);
            if (e == 0) { // if there is no transition, print the following message and return 0
                cout
                        << "maximum number of thread transitions in this configuration: "
                        << e_max << endl;
                return 0;
            }
            assert(e <= e_max);
            Random::set_seed();
            add_successor(init_ts, adj_list[init_ts]); // initial state must have a successor
            for (ulong i = 1; i < e; ++i) {
                __CERR__("currently adding edge " << i + 1 << endl);
                while (true) {
                    Thread_State t; // random thread state
                    list<Thread_State>& successors = adj_list[t]; // must be created unless exists
                    list<Thread_State>::const_iterator ifind = std::find(
                            successors.begin(), successors.end(), t);
                    if (ifind == successors.end()) {
                        add_successor(t, successors);
                        break;
                    } else if (successors.size() < T - 1) { // t does not already point to every other thread state
                        add_successor(t, successors);
                        break;
                    }
                }
            }
            ofstream last("Examples/last.ttd");
            util.print_adj_list(adj_list, last);
        } else {
            ifstream orig(filename.c_str());
            CONTROL::remove_comments(orig, "/tmp/tmp.ttd.no_comment", "#");
            orig.close();
            ifstream in("/tmp/tmp.ttd.no_comment");
            in >> Global_State::s >> Global_State::l;
            ushort s1, l1, s2, l2;
            string sep;
            while (in >> s1 >> l1 >> sep >> s2 >> l2) {
                //in >> s1 >> l1 >> sep >> s2 >> l2;
                __SAFE_ASSERT__(sep == "->");
                adj_list[Thread_State(s1, l1)].push_back(Thread_State(s2, l2));
                inverse_adj_list[Thread_State(s2, l2)].push_back(
                        Thread_State(s1, l1));
            }
            in.close();
        }

        if (ins.arg2bool("--adj-list") || IS_ALL) {
            cout << "Adjacency list:" << endl;
            util.print_adj_list(adj_list);
            cout << "Inverse adjacency list:" << endl;
            util.print_adj_list(inverse_adj_list);
        }

        //cout << endl;
        cout << "The initial state is: " << init_ts << endl;
        cout << "The backward search starts from: " << target << endl;
        //Global_State target(target_ts); // the initial state

        BWS bws;
        bool is_reachable = bws.backward_search(init_ts, target,
                inverse_adj_list);
        cout << "======================================================"
                << endl;
        cout << " " << target;
        if (is_reachable) {
            cout << " is reachable: verification failed!" << endl;
        } else {
            cout << " is unreachable: verification successful!" << endl;
        }
        cout << "======================================================"
                << endl;
        return 0;
    }

    catch (const CONTROL::Error& error) {
        error.print_exit();
    } catch (...) {
        CONTROL::Error("main: unknown exception occurred").print_exit();
    }
}

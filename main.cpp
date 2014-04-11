/*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 *	This program is free software; you can redistribute it and/or modify         *
 *  it under the terms of the GNU General Public License as published by         *
 *  the Free Software Foundation; either version 2 of the License, or            *
 *  (at your option) any later version.                                          *
 *                                                                               *
 *  This program is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
 *  GNU General Public License for more details.                                 *
 *                                                                               *
 *  You should have received a copy of the GNU General Public License            *
 *  along with this program; if not, write to the Free Software                  *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                               *
 *  Created by Yuxuan He on 2014.4.5 (E-mail: max.yuxuan.he[at]gmail.com)        *
 *	Collaborator: Gongshen Liu                                                   *
 *	Project: Simulation of Dynamic Graphs                                        *
 *                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */


#include <iostream>
#include "DynamicGraph.h"
#include "functions.h"
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

#define unlikely -1

int num_nodes = unlikely;
int avg_degree = unlikely;
int max_degree = unlikely;
double tau = unlikely;
double comm_tau = unlikely;

double mixing_parm = unlikely;

int min_comm_size = unlikely;
int max_comm_size = unlikely;

int snapshots = unlikely;


bool readConf(string path) {
    ifstream f(path);
    string line;
    for (int i = 0; i < 100; i++) {
        if (!getline(f, line))
            break;
        
        line = line.substr(0, line.find("#"));
        if (line.length() == 0) {
            i--;
            continue;
        }
        if (i == 0)
            num_nodes = stoi(line);
        else if (i == 1)
            avg_degree = stoi(line);
        else if (i == 2)
            max_degree = stoi(line);
        else if (i == 3)
            tau = stod(line);
        else if (i == 4)
            comm_tau = stod(line);
        else if (i == 5)
            mixing_parm = stod(line);
        else if (i == 6)
            min_comm_size = stoi(line);
        else if (i == 7)
            max_comm_size = stoi(line);
        else if (i == 8)
            snapshots = stoi(line);
        else
            break;
    }
    if (num_nodes == unlikely || avg_degree == unlikely ||
        max_degree == unlikely || tau == unlikely ||
        comm_tau == unlikely || mixing_parm == unlikely ||
        min_comm_size == unlikely || max_comm_size == unlikely ||
        snapshots == unlikely) {
        return false;
    }

    return true;
}

int main(int argc, const char * argv[])
{
    clock_t start_time, finish_time;
    start_time = clock();
    srand((unsigned int)time(NULL));
    if (!readConf("param.conf")) {
        cerr << "Something wrong with the param.conf" << endl
             << "Please double check the configurations file, or try with default settings." << endl;
        return 1;
    }
    
    int min_degree = solve_dmin(max_degree, avg_degree, -tau);
    cout << "====================== Parameters ======================" << endl;
    cout << "Num of nodes: " << num_nodes << endl;
    cout << "Average degree: " << avg_degree << endl;
    cout << "Max degree: " << max_degree << endl;
    cout << "Min degree(calculated): " << min_degree << endl;
    cout << "Tau: " << tau << endl;
    cout << "Tau of community size: " << comm_tau << endl;
    cout << "Mixing parameter: " << mixing_parm << endl;
    cout << "min_comm_size: " << min_comm_size << endl;
    cout << "max_comm_size: " << max_comm_size << endl;
    cout << "Snapshots: " << snapshots << endl;
    cout << "========================================================" << endl;


    deque <double> cumulative;
    vector<degree_t> internal_degree_seq(num_nodes) ; //  degree sequence of verteces
    vector<degree_t> external_degree_seq(num_nodes) ; //  degree sequence of verteces
    
	powerlaw(max_degree, min_degree, tau, cumulative);
    for (int i = 0; i < num_nodes; i++) {
		degree_t nn = lower_bound(cumulative.begin(), cumulative.end(), rand01()) - cumulative.begin() + min_degree;
		external_degree_seq[i] = floor(nn * mixing_parm + 0.5) ;
        internal_degree_seq[i] = nn - external_degree_seq[i];
	}
    
    powerlaw(max_comm_size, min_comm_size, comm_tau, cumulative);
    
    deque<community_index> membership_seq;
    node_index total_members = 0;
    while (total_members < num_nodes) {
        node_index nn = (node_index)(lower_bound(cumulative.begin(), cumulative.end(), rand01()) - cumulative.begin() + min_comm_size);
        total_members += nn;
        membership_seq.push_back(total_members);
    }
    
    
    DynamicGraph g(num_nodes, (community_index)membership_seq.size());
    for (int i = 1; i < num_nodes + 1; ++i) {
        community_index comm = (community_index)(upper_bound(membership_seq.begin(), membership_seq.end(), rand() % total_members) - membership_seq.begin());
        g.add_vertex(comm);
        
        // Take snapshots
        if (i % (num_nodes / snapshots) == (num_nodes % (num_nodes / snapshots))) {
            g.iterate_graph(internal_degree_seq, external_degree_seq);

            stringstream stream;
            stream << "network_" << i << ".dat";
            string filename = stream.str();
            ofstream outfile(filename);
            g.output_edge(outfile);
            outfile.close();

            stream.str("");
            stream.clear();
            stream << "community_" << i << ".dat";
            filename = stream.str();
            
            outfile.open(filename);
            g.output_community(outfile);
            outfile.close();
        }
    }
    
    cout << "Done." << endl;
    finish_time = clock();
    cout << "Time elasped: " << double(finish_time - start_time) / CLOCKS_PER_SEC << "s." << endl;

    return 0;
}


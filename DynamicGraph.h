//
//  DynamicGraph.h
//  DynamicGraph
//
//  Created by He Yuxuan on 14-4-2.
//  Copyright (c) 2014 laohyx. All rights reserved.
//

#ifndef __DynamicGraph__DynamicGraph__
#define __DynamicGraph__DynamicGraph__

#include <iostream>
#include <set>
#include <vector>
#include <queue>
#include <fstream>
#include <stdint.h>
#include <cmath>
#include <algorithm>

typedef uint32_t node_index;
typedef uint32_t community_index;
typedef uint16_t degree_t;
using namespace std;

struct vertex {
    node_index index;
    community_index membership;
    degree_t internal_degree;
    degree_t external_degree;
    set<node_index> internal_neighbors;
    set<node_index> external_neighbors;
};


class DynamicGraph {
    vector<vertex> verteces;
    vector<set<node_index> > member_matrix;
    node_index index;
public:
    DynamicGraph(node_index verteces_num, community_index community_num){
        verteces.resize(verteces_num);
        member_matrix.resize(community_num);
        index = 0;
    }
    node_index add_vertex(community_index membership);
    bool is_neighbor(node_index v1, node_index v2) {
        if (verteces[v1].membership == verteces[v2].membership) {
            return verteces[v1].internal_neighbors.find(v2) != verteces[v1].internal_neighbors.end();
        }else {
            return verteces[v1].external_neighbors.find(v2) != verteces[v1].external_neighbors.end();
        }
    }

    bool add_edge(node_index v1, node_index v2) {
        if (v1 == v2 || is_neighbor(v1, v2)) {
            return false;
        }
        if (verteces[v1].membership == verteces[v2].membership) {
            verteces[v1].internal_neighbors.insert(v2);
            verteces[v2].internal_neighbors.insert(v1);
            verteces[v1].internal_degree++;
            verteces[v2].internal_degree++;
        }else {
            verteces[v1].external_neighbors.insert(v2);
            verteces[v2].external_neighbors.insert(v1);
            verteces[v1].external_degree++;
            verteces[v2].external_degree++;
        }
        if (verteces[v1].internal_degree != verteces[v1].internal_neighbors.size() ||
            verteces[v1].external_degree != verteces[v1].external_neighbors.size() ||
            verteces[v2].internal_degree != verteces[v2].internal_neighbors.size() ||
            verteces[v2].external_degree != verteces[v2].external_neighbors.size() ) {
            cout << "ERROROROR" << endl;
        }
        return true;
    }
    bool remove_edge(node_index v1, node_index v2) {
        if (!is_neighbor(v1, v2)) {
            // v1 and v2 are not connected at all.
            return false;
        }
        if (verteces[v1].membership == verteces[v2].membership) {
            verteces[v1].internal_neighbors.erase(v2);
            verteces[v2].internal_neighbors.erase(v1);
            verteces[v1].internal_degree--;
            verteces[v2].internal_degree--;
        }else {
            verteces[v1].external_neighbors.erase(v2);
            verteces[v2].external_neighbors.erase(v1);
            verteces[v1].external_degree--;
            verteces[v2].external_degree--;
        }
        if (verteces[v1].internal_degree != verteces[v1].internal_neighbors.size() ||
            verteces[v1].external_degree != verteces[v1].external_neighbors.size() ||
            verteces[v2].internal_degree != verteces[v2].internal_neighbors.size() ||
            verteces[v2].external_degree != verteces[v2].external_neighbors.size() ) {
            cout << "ERROROROR" << endl;
        }

        return true;
    }
//    degree_t vertex_degree(node_index v, bool is_internal) {
//        return is_internal ? verteces[v].internal_degree : verteces[v].external_degree;
//    }

    degree_t vertex_internal_degree(node_index v) {
        return verteces[v].internal_degree;
    }
    degree_t vertex_external_degree(node_index v) {
        return verteces[v].external_degree;
    }
//    queue<node_index> vertex_common_neighbors(node_index v1, node_index v2) {
//        queue<node_index> common;
//        for (set<node_index>::iterator it = ajacency_list[v1].begin(); it != ajacency_list[v1].end(); ++it) {
//            if (ajacency_list[v2].find(*it) != ajacency_list[v2].end()) {
//                common.push(*it);
//            }
//        }
////        set_intersection(ajacency_list[v1].begin(), ajacency_list[v1].end(), ajacency_list[v2].begin(), ajacency_list[v2].end(), inserter(common, common.begin()));
//        return common;
//    }

    // find neighbors in the same community
    degree_t inline internal_common_neighbors(node_index v1, node_index v2) {
        degree_t common = 0;
        for (set<node_index>::iterator it = verteces[v1].internal_neighbors.begin(); it != verteces[v1].internal_neighbors.end(); ++it) {
            if (verteces[v2].internal_neighbors.find(*it) != verteces[v2].internal_neighbors.end()) {
                common++;
            }
        }
        return common;
    }
    const set<node_index> & vertex_neighbors(node_index v, bool is_interal) {
        if (is_interal) {
            return verteces[v].internal_neighbors;
        }else {
            return verteces[v].external_neighbors;
        }
    }
    const vector<vertex> & get_all_verteces(){
        return verteces;
    }
    const set<node_index> & get_community_verteces(community_index comm) {
        return member_matrix.at(comm);
    }
    
    void output_edge(ofstream &f) {
        set<node_index>::iterator iter;
        for (int i = 0; i < index; ++i) {
            // print internal edges
            for (iter = verteces[i].internal_neighbors.begin(); iter != verteces[i].internal_neighbors.end(); ++iter) {
                f << i << "\t" << *iter << endl;
            }
            // print external edges
            for (iter = verteces[i].external_neighbors.begin(); iter != verteces[i].external_neighbors.end(); ++iter) {
                f << i << "\t" << *iter << endl;
            }
        }
    }
    void output_community(ofstream &f) {
        for (int i = 0; i < index; ++i) {
            f << verteces[i].index << "\t" << verteces[i].membership << endl;
        }
    }
    void iterate_graph(const vector<degree_t> &internal_degree_seq, const vector<degree_t> &external_degree_seq);

private:
    void iterate_internal_community(community_index community_number, const vector<degree_t> &degree_seq);
    void iterate_external_community(const vector<degree_t> &degree_seq);
};



#endif /* defined(__DynamicGraph__DynamicGraph__) */

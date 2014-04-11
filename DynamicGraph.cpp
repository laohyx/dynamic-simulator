//
//  DynamicGraph.cpp
//  DynamicGraph
//
//  Created by He Yuxuan on 14-4-2.
//  Copyright (c) 2014 laohyx. All rights reserved.
//

#include "DynamicGraph.h"
    
node_index DynamicGraph::add_vertex(community_index membership) {
    struct vertex v;
    v.index = index;
    v.membership = membership;
    v.internal_degree = 0;
    v.external_degree = 0;
    verteces[index] = v;
    member_matrix[membership].insert(index);
    index++;
    return v.index;
}

bool inline pairCompare(const pair<node_index, degree_t>& firstElem, const pair<node_index, degree_t> secondElem) {
    return firstElem.second > secondElem.second;
}

void DynamicGraph::iterate_graph(const vector<degree_t> &internal_degree_seq, const vector<degree_t> &external_degree_seq) {
    for (int i = 0; i < member_matrix.size(); ++i) {
        iterate_internal_community(i, internal_degree_seq);
    }
    iterate_external_community(external_degree_seq);
}

void DynamicGraph::iterate_internal_community(community_index community_number, const vector<degree_t> &degree_seq) {
    set<node_index> vertices = get_community_verteces(community_number);
    vector<pair<node_index, degree_t> > sorted_degree;
    for (set<node_index>::iterator vit = vertices.begin(); vit != vertices.end(); ++vit) {
        sorted_degree.push_back(pair<node_index, degree_t>(*vit, vertex_internal_degree(*vit)));
    }
    sort(sorted_degree.begin(), sorted_degree.end(), pairCompare);
    for (set<node_index>::iterator vit = vertices.begin(); vit != vertices.end(); ++vit) {
        // remove edges randomly
        degree_t target_degree = degree_seq.at(*vit);
        //        cout << "remove: " <<  vertex_internal_degree(*vit) - target_degree << endl;

        while (target_degree < vertex_internal_degree(*vit)) {
            set<node_index> neighbors = vertex_neighbors(*vit, true);
            set<node_index>::iterator randIt = neighbors.begin();
            advance(randIt, rand() % neighbors.size());
            remove_edge(*vit, *randIt);
        }
        // add edges with high-degree verteces
        //        cout << "target: " <<target_degree << ", now: " << vertex_internal_degree(*vit) << endl;;
        degree_t num_to_add = floor((target_degree - vertex_internal_degree(*vit)) / 2 + 0.5) ;
        if (num_to_add > 0) {
            //            cout << "add1: " << num_to_add << endl;
            set<node_index> candidates;
            for (int i = 0; i < sorted_degree.size() / 5; ++i) {
                if (!is_neighbor(sorted_degree.at(i).first, *vit)) {
                    candidates.insert(sorted_degree.at(i).first);
                }
            }
            if (candidates.size() > 0) {
                for (int i = 0; i < num_to_add; ++i) {
                    set<node_index>::iterator randIt = candidates.begin();
                    advance(randIt, rand() % candidates.size());
                    add_edge(*vit, *randIt);
                }
            }
        }

        // add edges with neighbours' neighbours (randomly pick up second degree neighbors)
        num_to_add = target_degree - vertex_internal_degree(*vit);
        if (num_to_add > 0) {
            //            cout << "add2: " << num_to_add << endl;
            set<node_index> neighbors = vertex_neighbors(*vit, true);
            set<node_index> candidates;
            for (set<node_index>::iterator nbr = neighbors.begin(); nbr != neighbors.end(); ++nbr) {
                set<node_index> second_degree_neighbors = vertex_neighbors(*nbr, true);
                for (set<node_index>::iterator nbr2 = second_degree_neighbors.begin(); nbr2 != second_degree_neighbors.end(); ++nbr2) {
                    if (*nbr == *nbr2 || *nbr2 == *vit || is_neighbor(*vit, *nbr2))
                        continue;
                    candidates.insert(*nbr2);
                }
            }
            while (num_to_add > 0 && candidates.size() > 0) {
                set<node_index>::iterator randIt = candidates.begin();
                advance(randIt, rand() % candidates.size());
                add_edge(*vit, *randIt);
                candidates.erase(randIt);
                num_to_add--;
            }
        }
    }
}
bool flag = false;

void DynamicGraph::iterate_external_community(const vector<degree_t> &degree_seq) {
    for (node_index v_idx = 0; v_idx < index; ++v_idx) {
        degree_t target_degree = degree_seq[v_idx];
        // delete external neighbors randomly
        while (verteces[v_idx].external_degree > target_degree ) {
            set<node_index>::iterator randIt = verteces[v_idx].external_neighbors.begin();
            advance(randIt, rand() % verteces[v_idx].external_neighbors.size());
            remove_edge(v_idx, *randIt);
        }
        degree_t counter = 0;
        // add external neighbors randomly
        while (verteces[v_idx].external_degree < target_degree) {
            node_index nbr = rand() % index;
            if (verteces[nbr].membership != verteces[v_idx].membership) {
                add_edge(nbr, v_idx);
            }
            if (counter++ > target_degree * 2) {
                break;
            }
        }
    }
}


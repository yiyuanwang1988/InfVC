#include "preprocess.h"
using namespace std;
Preprocess::Preprocess(string origin_graph_filename) : origin_graph_filename(origin_graph_filename) {
	initialize(origin_graph_filename);
}

void Preprocess::initialize(string origin_graph_filename) {
	ifstream in_file(origin_graph_filename);
	if (! in_file.is_open()) {
		cout << "in_file error" << endl;
		exit(1);
	}

	size_t vertex_count, edge_count;

	//get vertex_count
	string line;
	istringstream is;
	string p, tmp;
	do {
		getline(in_file, line);
		is.clear();
		is.str(line);
		is >> p >> tmp >> vertex_count >> edge_count;
	} while (p != "p");
	//cout << "c origin graph: " << line << endl;

	adjacency_list.resize(vertex_count + 1);
	neighbor_indicator.resize(vertex_count + 1, false);
	addable_dom.resize(vertex_count + 1, false);
	addable_fold.resize(vertex_count + 1, true);
	long v1, v2;

	for (size_t i = 0; i < edge_count; ++i) {
		in_file >> tmp >> v1 >> v2;
		adjacency_list[v1].push_back(v2);
		adjacency_list[v2].push_back(v1);
	}
	in_file.close();
}

void Preprocess::reduce(long v) {
	for (auto neighbor : adjacency_list[v]) {
		vector<long>::size_type i = 0;
		for (; i < adjacency_list[neighbor].size(); ++i) {
			if (adjacency_list[neighbor][i] == v) {
				break;
			}
		}
		adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
		adjacency_list[neighbor].pop_back();
	}
	adjacency_list[v].clear();
}
void Preprocess::determine(const std::vector<long>& vertices) {
	for (auto v : vertices) {
		for (auto u : adjacency_list[v]) neighbor_indicator[u] = true;
		for (auto u : adjacency_list[v]) {
			for (auto w : adjacency_list[u]) {
				if (!neighbor_indicator[w] && addable_dom[w]) {
					dom_v.push(w); addable_dom[w] = false;
				}
			}
			if (addable_fold[u]) {
				fold_v.push(u); addable_fold[u] = false;
			}
		}
		for (auto u : adjacency_list[v]) neighbor_indicator[u] = false;
	}
	for (auto v : vertices) {
		reduce(v);
	}
	Inf_P record;
	record.flag = 'd';
	record.data.assign(vertices.begin(), vertices.end());
	S_Inf.push(record);
}
bool Preprocess::strong_foldable(long v) {
	anti_set.clear();
	std::vector<long> D_set;
	long anti_edge = 0;
	for (auto u : adjacency_list[v]) {
		bool fg = true;
		neighbor_indicator[u] = true;
		for (auto w : adjacency_list[u]) neighbor_indicator[w] = true;
		for (auto u1 : adjacency_list[v]) {
			if (!neighbor_indicator[u1]) {
				fg = false;
				if (u < u1) { anti_set[u].insert(u1); anti_edge++; }
			}
		}
		neighbor_indicator[u] = false;
		for (auto w : adjacency_list[u]) neighbor_indicator[w] = false;
		if (fg) D_set.push_back(u);
	}
	if (!D_set.empty()) determine(D_set);

	if (anti_edge == 0 || anti_edge >= adjacency_list[v].size() + 1) return false;
	bool fg = true;
	for (auto x : anti_set) { 
		long u = x.first;
		for (auto u1 : anti_set[u]) { // check every anti_edge <u,u1> 
			if (anti_set.count(u1)) {
				for (auto u2 : anti_set[u1]) { //for each <u1,u2>, there is no edge<u,u2> 
					if (anti_set[u].count(u2)) {
						fg = false;
						break;
					}
				}
			}
		}
	}
	return fg;
}
std::set<long> Preprocess::get_neighbor(const std::vector<long>& vertices) {
	std::set<long> new_neighbors;
	for (auto v : vertices) {
		for (auto w : adjacency_list[v]) new_neighbors.insert(w);
	}
	return new_neighbors;
}
void Preprocess::Fold(long v) {
	std::vector<std::set<long>> anti_E;
	long anti_edge = 0;
	Inf_P record;
	record.flag = 'f'; record.vertex = v;
	std::vector<long> neighbor_of_v(adjacency_list[v].begin(), adjacency_list[v].end());
	for (auto x : anti_set) {
		long u1 = x.first;
		for (auto u2 : anti_set[u1]) {
			record.data.push_back(anti_edge); record.data.push_back(u1); record.data.push_back(u2);
			anti_edge++;
			std::vector<long> vertices; vertices.push_back(u1); vertices.push_back(u2);
			std::set<long> new_neighbors = get_neighbor(vertices); new_neighbors.erase(v);
			for (auto w : neighbor_of_v) new_neighbors.erase(w);
			anti_E.push_back(new_neighbors);
		}
	}
	for (auto u : neighbor_of_v) reduce(u);
	long cur = 0;
	while (cur != anti_edge) {
		record.data[cur * 3] = neighbor_of_v[cur];
		adjacency_list[neighbor_of_v[cur]].assign(anti_E[cur].begin(), anti_E[cur].end());
		cur++;
	}
	S_Inf.push(record);
	neighbor_of_v.resize(anti_edge);
	for (auto u : neighbor_of_v) {
		for (auto w : adjacency_list[u]) adjacency_list[w].push_back(u);
	}
	/*for (auto u1 : neighbor_of_v) {
		if (adjacency_list[u1].size() == 0) {
			std::vector<long> tmp;
			for (auto u2 : neighbor_of_v) {
				if (u1 != u2) tmp.push_back(u2);
			}
			determine(tmp);
		}
	}*/
	for (auto u1 : neighbor_of_v) {
		for (auto u2 : neighbor_of_v) {
			if (u2 != u1) adjacency_list[u1].push_back(u2);
		}
	}

	for (auto vij : neighbor_of_v) {
		// add dom_v
		neighbor_indicator[vij] = true;
		for (auto u : adjacency_list[vij]) neighbor_indicator[u] = true;
		for (auto u : adjacency_list[vij]) {
			for (auto w : adjacency_list[u]) {
				if (addable_dom[w] && neighbor_indicator[w]) {
					dom_v.push(w); addable_dom[w] = false;
				}
			}
		}
		neighbor_indicator[vij] = false; for (auto u : adjacency_list[vij]) neighbor_indicator[u] = false;
		// add fold_v
		for (auto u : adjacency_list[vij]) {
			if (addable_fold[u] && adjacency_list[u].size() < search_thre) {
				fold_v.push(u); addable_fold[u] = false;
			}
		}
		if (addable_fold[vij] && adjacency_list[vij].size() < search_thre) {
			fold_v.push(vij); addable_fold[vij] = false;
		}
	}
}
void Preprocess::rm_acnodes() {
	for (vector<vector<long>>::size_type v = 1; v < adjacency_list.size();) {
		if (adjacency_list[v].empty()) {
			adjacency_list[v].swap(adjacency_list[adjacency_list.size() - 1]);
			vertice_map[v] = adjacency_list.size() - 1;
			adjacency_list.pop_back();
		}
		else {
			if (vertice_map.find(v) != vertice_map.end()) {
				long origin_vertex = vertice_map[v];
				for (auto neighbor : adjacency_list[v]) {
					vector<long>::size_type i = 0;
					for (; i < adjacency_list[neighbor].size(); ++i) {
						if (adjacency_list[neighbor][i] == origin_vertex) {
							break;
						}
					}
					adjacency_list[neighbor][i] = v;
				}
			}
			++v;
		}
	}
	if (vertice_map.find(adjacency_list.size()) != vertice_map.end()) { //ending
		vertice_map.erase(adjacency_list.size());
	}
}
bool Preprocess::is_adjacent(long v1, long v2) {
	if (adjacency_list[v1].size() > adjacency_list[v2].size()) {
		swap(v1, v2);
	}
	for (auto neighbor : adjacency_list[v1]) {
		if (v2 == neighbor) {
			return true;
		}
	}
	return false;
}

void Preprocess::simplify() {
	std::vector<long> pending_vertex;
	for (vector<vector<long>>::size_type v = 1; v < adjacency_list.size(); v++) {
		if (adjacency_list[v].size() == 1 || adjacency_list[v].size() == 2) pending_vertex.push_back(v);
	}
	while (pending_vertex.size() != 0) {
		long processing_vertice = pending_vertex[pending_vertex.size() - 1];
		pending_vertex.pop_back();
		long degree = adjacency_list[processing_vertice].size();
		long u1, u2;
		if (degree == 1) {
			//the neighbor of processing_vertice(u1) dominates processing_vertice
			u1 = adjacency_list[processing_vertice][0];
			for (auto neighbor : adjacency_list[u1]) {
				vector<long>::size_type i = 0;
				for (; i < adjacency_list[neighbor].size(); ++i) {
					if (adjacency_list[neighbor][i] == u1) {
						break;
					}
				}
				adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
				adjacency_list[neighbor].pop_back();

				long degree_u = adjacency_list[neighbor].size();
				if (degree_u == 1 || degree_u == 2) {
					pending_vertex.push_back(neighbor);
				}
			}
			adjacency_list[u1].clear();
			Inf_P record;
			record.flag = 'd';
			record.data.push_back(u1);
			S_Inf.push(record);
		}
		else if (degree == 2) {
			u1 = adjacency_list[processing_vertice][0]; u2 = adjacency_list[processing_vertice][1];
			//if (adjacency_list[u1].size() == 0 || adjacency_list[u2].size() == 0) std::cout << "degree error" << std::endl;
			if (is_adjacent(u1, u2)) {
				// u1,u2 dominate processing_vertice
				std::set<long> new_neighbors;
				for (auto neighbor : adjacency_list[u1]) {
					vector<long>::size_type i = 0;
					for (; i < adjacency_list[neighbor].size(); ++i) {
						if (adjacency_list[neighbor][i] == u1) {
							break;
						}
					}
					adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
					adjacency_list[neighbor].pop_back();

					new_neighbors.insert(neighbor);
				}
				adjacency_list[u1].clear();
				for (auto neighbor : adjacency_list[u2]) {
					vector<long>::size_type i = 0;
					for (; i < adjacency_list[neighbor].size(); ++i) {
						if (adjacency_list[neighbor][i] == u2) {
							break;
						}
					}
					adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
					adjacency_list[neighbor].pop_back();

					new_neighbors.insert(neighbor);
				}
				adjacency_list[u2].clear();
				new_neighbors.erase(processing_vertice); new_neighbors.erase(u1); new_neighbors.erase(u2);
				for (auto w : new_neighbors) {
					long degree_w = adjacency_list[w].size();
					if (degree_w == 1 || degree_w == 2) {
						pending_vertex.push_back(w);
					}
				}
				Inf_P record;
				record.flag = 'd';
				record.data.push_back(u1); record.data.push_back(u2);
				S_Inf.push(record);
			}
			else {
				// u1,u2,processing_vertice is foldable
				std::set<long> new_neighbors(adjacency_list[u1].begin(), adjacency_list[u1].end());
				for (auto neighbor : adjacency_list[u2]) new_neighbors.insert(neighbor); 
				new_neighbors.erase(processing_vertice);
				for (auto neighbor : adjacency_list[u1]) {
					vector<long>::size_type i = 0;
					for (; i < adjacency_list[neighbor].size(); ++i) {
						if (adjacency_list[neighbor][i] == u1) {
							break;
						}
					}
					adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
					adjacency_list[neighbor].pop_back();		
				}
				adjacency_list[u1].clear();
				for (auto neighbor : adjacency_list[u2]) {
					vector<long>::size_type i = 0;
					for (; i < adjacency_list[neighbor].size(); ++i) {
						if (adjacency_list[neighbor][i] == u2) {
							break;
						}
					}
					adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
					adjacency_list[neighbor].pop_back();
				}
				adjacency_list[u2].clear();
				adjacency_list[u1].assign(new_neighbors.begin(), new_neighbors.end());
				for (auto neighbor : adjacency_list[u1]) {
					adjacency_list[neighbor].push_back(u1);
					long degree_neighbor = adjacency_list[neighbor].size();
					if (degree_neighbor == 1 || degree_neighbor == 2) pending_vertex.push_back(neighbor);
				}
				long degree_u1 = adjacency_list[u1].size();
				if (degree_u1 == 1 || degree_u1 == 2) pending_vertex.push_back(u1);
				Inf_P record;
				record.flag = 'f';
				record.vertex = processing_vertice;
				//if (record.data.size() != 0) std::cout << "record_data isn't empty" << std::endl;
				record.data.push_back(u1); record.data.push_back(u1); record.data.push_back(u2);
				S_Inf.push(record);
			}
		}
	}
}

void Preprocess::Prep() {
	simplify();
	for (vector<vector<long>>::size_type v = 1; v < adjacency_list.size(); v++) {
		long degree = adjacency_list[v].size();
		if (degree != 0) {
			dom_v.push(v);
			if (degree < search_thre) {
				fold_v.push(v); addable_fold[v] = false;
			}
		}
	}
Dom:while (!dom_v.empty()) {
		long v = dom_v.front();dom_v.pop();
		if (adjacency_list[v].size() == 0) continue;
		neighbor_indicator[v] = true;
		for (auto u : adjacency_list[v]) {
			neighbor_indicator[u] = true;
		}

		bool is_dominated = false;
		for (auto u : adjacency_list[v]) {
			if (adjacency_list[v].size() < adjacency_list[u].size()) continue;
			is_dominated = true;
			for (auto w : adjacency_list[u]) {
				if (!neighbor_indicator[w]) {
					is_dominated = false;
					break;
				}
			}
			if (is_dominated) {
				break;
			}
		}

		neighbor_indicator[v] = false;
		for (auto u : adjacency_list[v]) {
			neighbor_indicator[u] = false;
		}
		if (is_dominated) {
			std::vector<long> tmp; tmp.push_back(v);
			determine(tmp);
		}
		addable_dom[v] = true;
	}
	while (!fold_v.empty()) {
		long u1 = fold_v.front(); fold_v.pop();
		long degree = adjacency_list[u1].size();
		if (degree >= search_thre || degree == 0) continue;
		if (strong_foldable(u1)) {
			Fold(u1);
		}
		addable_fold[u1] = true;
	}
	if (!dom_v.empty()) goto Dom;
	std::queue<long>().swap(fold_v); std::queue<long>().swap(dom_v);
	rm_acnodes();
}

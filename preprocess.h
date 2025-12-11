#ifndef PREPROCESS_H_NC8VRLR1
#define PREPROCESS_H_NC8VRLR1

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
#include <stack>
#include <map>
#include <iterator>
#include <algorithm>

class Preprocess {
	public:
		Preprocess (std::string origin_graph_filename);
		void initialize(std::string origin_graph_filename);
		std::vector<std::vector<long>> & get_adjacency_list() { return adjacency_list; }
		void shrink_memory() { adjacency_list.clear(); adjacency_list.shrink_to_fit(); }
		void Prep();
		void determine(const std::vector<long>& vertices);
		void reduce(long v);
		bool strong_foldable(long v);
		void Fold(long v);
		void simplify();
		bool is_adjacent(long v1, long v2);
		std::set<long> get_neighbor(const std::vector<long>& vertices);
		template<typename ITERATOR> void out_solution(ITERATOR begin, ITERATOR end);
		template<typename ITERATOR> bool verify(ITERATOR begin, ITERATOR end);
		int ans = 0;
		std::string origin_graph_filename;

	private:
		void rm_acnodes();
		std::vector<std::vector<long>> adjacency_list;
		std::vector<bool> neighbor_indicator;
		std::queue<long> dom_v, fold_v;
		std::vector<long> cmp_v;
		std::vector<bool> addable_dom;
		std::vector<bool> addable_fold;
		std::map<long, long> vertice_map;
		std::map<long, std::set<long>> anti_set;
		struct Inf_P {
			char flag;
			long vertex;
			std::vector<long> data;
		};
		std::stack<Inf_P> S_Inf;
		long search_thre = 22;
};

template<typename ITERATOR>
void Preprocess::out_solution(ITERATOR begin, ITERATOR end) {
	for (ITERATOR iter = begin; iter != end; ++iter) {
		auto map_iter = vertice_map.find(*iter);
		if (map_iter != vertice_map.end()) {
			std::cout << map_iter->second << ' ';
		}
		else {
			std::cout << *iter << ' ';
		}
	}
}

template<typename ITERATOR>
bool Preprocess::verify(ITERATOR begin, ITERATOR end) {
	std::ifstream origin_graph_file(origin_graph_filename);
	if (!origin_graph_file.is_open()) {
		std::cout << "cant open file when verfing" << std::endl;
		exit(1);
	}
	std::string line;
	getline(origin_graph_file, line);
	size_t vertex_count, edge_count;
	std::string tmp;
	std::istringstream is(line);
	is >> tmp >> tmp >> vertex_count >> edge_count;
	//std::cout << "all vertice =" << vertex_count << " ";
	int *solution_bit_array = new int[vertex_count + 1];
	std::set<long> answer;
	//if (!answer.empty()) std::cout << " answer isn't empty" << std::endl;
	for (size_t i = 1; i <= vertex_count; ++i) {
		solution_bit_array[i] = 0;
	}
	for (ITERATOR iter = begin; iter != end; ++iter) {
		auto map_iter = vertice_map.find(*iter);
		if (map_iter != vertice_map.end()) { //被更换过
			answer.insert(map_iter->second);
		}
		else { //未被更换过
			answer.insert(*iter);
		}
	}
	//Inv_Prep
	long remove_arr[3];
	while (!S_Inf.empty()) {
		Inf_P record = S_Inf.top(); S_Inf.pop();
		if (record.flag == 'd') {
			for (auto v : record.data) {
				answer.insert(v);
			}
		}
		else {
			bool marked = false;
			for (int index = 0; index < record.data.size(); index += 3) {
				if (!answer.count(record.data[index])) {
					remove_arr[0] = record.data[index]; remove_arr[1] = record.data[index + 1]; remove_arr[2] = record.data[index + 2];
					marked = true;
				}
				else {
					answer.erase(record.data[index]);
				}
			}
			for (int index = 0; index < record.data.size(); index += 3) {
				answer.insert(record.data[index + 1]); answer.insert(record.data[index + 2]);
			}
			if (marked) {
				answer.erase(remove_arr[1]); answer.erase(remove_arr[2]);
				answer.insert(record.vertex);
			}
		}
	}

	for (auto v : answer) solution_bit_array[v] = 1;

	while (getline(origin_graph_file, line)) {
		long v1, v2;
		char e;
		is.clear();
		is.str(line);
		is >> e >> v1 >> v2;
		if (solution_bit_array[v1] != 1 && solution_bit_array[v2] != 1) {
			return false;
		}
	}
	delete []solution_bit_array;
	origin_graph_file.close();
	ans = answer.size();
	return true;
}

// =====================================================================================
//
//      the simplified_graph_file maybe empty after Preprocess
//
// =====================================================================================


#endif /* end of include guard: PREPROCESS_H_NC8VRLR1 */


#include "my_heap.h"
#include <set>

/* functions declaration */
int build_instance(vector<vector<long>> &adjacency_matrix);
void add(int v);
void remove(int v);
int check_solution();

int countbbbbbb = 0;

void remove_v_in_array(int v,int* arr,int* index_of_arr,int& length) {
	int index = index_of_arr[v];
	if (index == -1) cout << "it is error" << endl;
	int last_v = arr[--length];
	index_of_arr[last_v] = index;
	arr[index] = last_v;
	index_of_arr[v] = -1;
}
int find_only_v(int v, int tabu) {
	//if (dscore[v] != -1) cout << "find_only_v error" << endl;
	int u;
	int edge_count = v_degree[v];
	for (int i = 0; i < edge_count; i++) {
		u = v_adj[v][i];
		if (!v_in_c[u] && u != tabu) return u;
	}
	cout << "find_only_v error" << endl;
	return 0;
}
void add_criticals(int v) {
	critical_count[v]++; critical_check[v] = 0;
	if (critical_count[v] == 2) {
		//if (index_critical2[v] != -1) cout << "111" << endl;
		index_critical2[v] = size_of_critical;
		critical2[size_of_critical++] = v;
	}
}

void update_best_sol(int cur_block)
{
	int i;
	best_c_size = c_size;
	int block_num = block_size[cur_block];
	int v;
	for (i = 0; i < block_num; i++) {
		v = connect_block[cur_block][i];
		best_v_in_c[v] = v_in_c[v];
	}

	times(&finish);
	best_comp_time = double(finish.tms_utime - start.tms_utime + finish.tms_stime - start.tms_stime) / sysconf(_SC_CLK_TCK);
	best_comp_time = round(best_comp_time * 100) / 100.0;
	best_step = step;
}

bool is_bigraph(int cur_block) {
	X_num = Y_num = 0;
	memset(visit, 0, sizeof(int) * (v_num + 1));
	int front1, front2;
	front1 = front2 = 0;
	int u;
	int v = connect_block[cur_block][0];
	X[X_num++] = v;
	visit[v] = 1;
	while (front1 < X_num || front2 < Y_num) {
		if (front1 < X_num) {
			v = X[front1];
			int adj_count = v_degree[v];
			for (int i = 0; i < adj_count; i++) {
				u = v_adj[v][i];
				if (visit[u] == 1) return false;
				else if (visit[u] == 0) {
					visit[u] = 2;
					Y[Y_num++] = u;
				}
			}
			front1++;
		}
		if (front2 < Y_num) {
			v = Y[front2];
			int adj_count = v_degree[v];
			for (int i = 0; i < adj_count; i++) {
				u = v_adj[v][i];
				if (visit[u] == 2) return false;
				else if (visit[u] == 0) {
					visit[u] = 1;
					X[X_num++] = u;
				}
			}
			front2++;
		}
	}
	return true;
}
bool match(int v) {
	int u;
	int edge_count = v_degree[v];
	for (int i = 0; i < edge_count; i++) {
		u = v_adj[v][i];
		if (!visit[u]) {
			visit[u] = 1;
			if (p[u] == 0 || match(p[u])) {
				p[u] = v;
				p[v] = u;
				return true;
			}
		}
	}
	return false;
}
int Hungary() {
	memset(p, 0, sizeof(int) * (v_num + 1));
	int cnt = 0;
	for (int i = 0; i < X_num; i++) {
		memset(visit, 0, sizeof(int) * (v_num + 1));
		if (match(X[i])) cnt++;
	}
	return cnt;
}
int check_pipei(int cur_block) {
	int cnt = 0;
	int u, v, w;
	int v_count = block_size[cur_block];
	for (int i = 0; i < v_count; i++) {
		v = connect_block[cur_block][i];
		if (p[v] == 0) continue;
		u = p[v];
		cnt++;
		if (p[u] != v) cout << "p[u]!= v" << endl;
	}
	return cnt;
}
void dfs_path(int v) {
	int edge_count = v_degree[v];
	int u;
	remarked[v] = 1;
	int j;
	for (j = 0; j < edge_count; j++) {
		u = v_adj[v][j];
		if (!visit[u] && p[u] != 0) {
			remarked[u] = visit[u] = 1;
			dfs_path(p[u]);
		}
	}
	return;
}
bool BGVC(int cur_block) {
	//times(&bitime1);
	if (is_bigraph(cur_block)) {
		/* X,Y is bipartite*/
		int cnt_of_pipei = Hungary();
		// if (check_pipei(cur_block) != cnt_of_pipei * 2) cout << "error" << endl;
		int v;
		for (int i = 0; i < X_num; i++) {
			v = X[i];
			//Î´Åä¶ÔµÄx
			if (p[v] == 0) {
				memset(visit, 0, sizeof(int) * (v_num + 1));
				dfs_path(v);
			}
		}
		// update 
		int init_cnt = 0;
		int v_count = block_size[cur_block];
		for (int i = 0; i < v_count; i++) {
			v = connect_block[cur_block][i];
			if (v_in_c[v]) {
				init_cnt++;
				v_in_c[v] = 0;
				c_size--;
			}
		}
		int cnt = 0;
		for (int i = 0; i < X_num; i++) {
			v = X[i];
			if (!remarked[v]) {
				v_in_c[v] = 1;
				c_size++;
				cnt++;
			}
		}
		for (int i = 0; i < Y_num; i++) {
			v = Y[i];
			if (remarked[v]) {
				v_in_c[v] = 1;
				c_size++;
				cnt++;
			}
		}
		//cout << init_cnt << " " << X_num << " " << Y_num << " " << cnt << " " << cnt_of_pipei << endl;
		update_best_sol(cur_block);
		/*times(&bitime2);
		sum_bitime = double(bitime2.tms_utime - bitime1.tms_utime + bitime2.tms_stime - bitime1.tms_stime) / sysconf(_SC_CLK_TCK);
		sum_bitime = round(sum_bitime * 10000) / 10000.0;*/
		return true;
	}
	else return false;
}
void check_add_cand() {
		for (int v = 1; v <= v_num; v++)
		{
			if (v_in_c[v] == 1)
			{
				if (index_in_add_cand[v] != -1)
					cout << file_name1 << " " << seed << " add_cand_size[v] != -1 " << endl;
			}
			else {
				if (index_in_add_cand[v] == -1) {
					cout << file_name1 << " " << seed << " (add_cand_size[v] == -1) " << endl;
				}
				if (v != add_cand[index_in_add_cand[v]]) {
					cout << file_name1 << " " << seed << " v != add_cand[index_in_add_cand[v]] " << endl;
				}
			}
		}
}



int build_instance(vector<vector<long>> &adjacency_matrix) {
	int v,e;

	int	v1,v2;

	if (adjacency_matrix.size() == 1) {
#ifndef NDEBUG
		cout << "the simplified_graph is empty" << endl;
#endif
		return 0;
	}

	/*** build problem data structures of the instance ***/
	v_num = adjacency_matrix.size() - 1;

	e_num = 0;
	for (vector<vector<long>>::size_type v = 1; v < adjacency_matrix.size(); ++v) {
		e_num += adjacency_matrix[v].size();
	}
	e_num /= 2;

	edge = new Edge [e_num];						//be initialized here
	conf_change = new int[v_num + 1];
	for (int i = 0; i <= v_num; i++) {
		conf_change[i] = 1;
	}
	uncov_stack = new int [e_num];					//only need to initialized uncov_stack_fill_pointer, has been done in init_sol()
	index_in_uncov_stack = new int [e_num];			//the same as above
	dscore = new int [v_num + 1];					//be initialized in init_sol()
	time_stamp = new long long [v_num + 1];			//be initialized in init_sol()
	v_edges = new int* [v_num + 1];					//be initialized here
	v_adj = new int* [v_num + 1];					//the same as above
	v_degree = new int [v_num + 1];					//the same as above
	v_in_c = new bool [v_num + 1];					//be initialized in init_sol()
	tmp_v_in_c = new bool [v_num + 1];				//be initialized in init_sol()
	remove_cand = new int [v_num + 1];				//be initialized in reset_remove_cand() in init_sol()
	index_in_remove_cand = new int [v_num + 1];		//the same as above
	best_v_in_c = new bool [v_num + 1];				//be initialized in update_best_sol() in init_sol()

	my_heap = new int [v_num + 1];
	pos_in_my_heap = new int [v_num + 1];
	my_heap_count = 0;

	dscore_1 = new int[v_num + 1];
	index_dscore_1 = new int[v_num + 1];

	dscore_0 = new int[v_num + 1];
	index_dscore_0 = new int[v_num + 1];

	add_cand = new int[v_num + 1];
	index_in_add_cand = new int[v_num + 1];
	connect_id = new int[v_num + 1];

	X = new int[v_num + 1];
	Y = new int[v_num + 1];
	visit = new int[v_num + 1];
	remarked = new int[v_num + 1];
	memset(remarked, 0, sizeof(int) * (v_num + 1));
	p = new int[v_num + 1];
	sum_bitime = 0;
	neighbor_indicator = new int[v_num + 1];
	memset(neighbor_indicator, 0, sizeof(int) * (v_num + 1));
	cri_relate = new int[v_num + 1];
	critical_count = new int[v_num + 1];
	memset(critical_count, 0, sizeof(int) * (v_num + 1));
	critical_check = new int[v_num + 1];
	memset(critical_check, 0, sizeof(int) * (v_num + 1));
	critical2 = new int[v_num + 1];
	index_critical2 = new int[v_num + 1];
	memset(index_critical2, -1, sizeof(int) * (v_num + 1));
	size_of_critical = 0;
	for (vector<vector<long>>::size_type v = 1, e = 0; v < adjacency_matrix.size(); ++v) {
		v_degree[v] = adjacency_matrix[v].size();
		for (auto u : adjacency_matrix[v]) {
			if (v < (vector<vector<long>>::size_type)u) {
				edge[e].v1 = v;
				edge[e].v2 = u;
				++e;
			}
		}
	}

	/* build v_adj and v_edges arrays */
	for (v=1; v<=v_num; v++)
	{
		v_adj[v] = new int[v_degree[v]];
		v_edges[v] = new int[v_degree[v]];
	}

	int* v_degree_tmp = new int [v_num + 1];
	memset(v_degree_tmp, 0, sizeof(int) * (v_num + 1));

	for (e=0; e<e_num; e++)
	{
		v1=edge[e].v1;
		v2=edge[e].v2;
		
		v_edges[v1][v_degree_tmp[v1]] = e;
		v_edges[v2][v_degree_tmp[v2]] = e;
		v_adj[v1][v_degree_tmp[v1]] = v2;
		v_adj[v2][v_degree_tmp[v2]] = v1;
		v_degree_tmp[v1]++;
		v_degree_tmp[v2]++;
	}
	delete[] v_degree_tmp;
	//adjacency_matrix.clear();
	//adjacency_matrix.shrink_to_fit();
	return 1;
}

void free_memory()
{
	for (int v=1; v<=v_num; v++)
	{
		delete[] v_adj[v];
		delete[] v_edges[v];
	}
	delete[] best_v_in_c;
	delete[] index_in_remove_cand;
	delete[] remove_cand;
	delete[] v_in_c;
	delete[] tmp_v_in_c;
	delete[] v_degree;
	delete[] v_adj;
	delete[] v_edges;
	delete[] time_stamp;
	delete[] dscore;
	delete[] index_in_uncov_stack;
	delete[] uncov_stack;
	delete[] edge;

	delete[] dscore_0;
	delete[] dscore_1;

	delete[] my_heap;
	delete[] pos_in_my_heap;
	delete[] add_cand;
	delete[] visit;
	delete[] X;
	delete[] Y;
	delete[] p;
	delete[] connect_block;
	delete[] block_size;
}

void reset_remove_cand(int cur_block)
{
	int v, j;

	int count_num = block_size[cur_block];
	remove_cand_size = add_cand_size = 0;
	for (int i = 0; i < count_num; i++) {
		v = connect_block[cur_block][i];
		if (v_in_c[v] == 1) {
			index_in_remove_cand[v] = remove_cand_size;
			remove_cand[remove_cand_size++] = v;
		}
		else {
			index_in_add_cand[v] = add_cand_size;
			add_cand[add_cand_size++] = v;
		}
	}
}

void update_target_size_0()
{
	c_size--;

	int v, i;
	int best_dscore;
	int best_remove_v;//vertex with the highest improvement in C

	best_remove_v = remove_cand[0];
	best_dscore = dscore[best_remove_v];

	if (dscore[best_remove_v] != 0)
	{
		for (i = 1; i < remove_cand_size; ++i)
		{
			v = remove_cand[i];

			if (dscore[v] == 0) break;

			if (dscore[v] > dscore[best_remove_v])
				best_remove_v = v;
		}
	}

	remove(best_remove_v);

	//remove best_remove_v from remove_cand, and move the last vertex in remove_cand to the position
	int last_remove_cand_v = remove_cand[--remove_cand_size];
	int index = index_in_remove_cand[best_remove_v];
	remove_cand[index] = last_remove_cand_v;
	index_in_remove_cand[last_remove_cand_v] = index;
	index_in_remove_cand[best_remove_v] = -1;
	//reset_remove_cand();
}

void remove_cand_vec(int remove_v) {
	int index = index_in_remove_cand[remove_v];
	int last_vec = remove_cand[--remove_cand_size];
	remove_cand[index] = last_vec;
	index_in_remove_cand[last_vec] = index;
	index_in_remove_cand[remove_v] = -1;
}

void add_cand_vec(int add_v) {
	int index = index_in_add_cand[add_v];
	int last_vec = add_cand[--add_cand_size];
	add_cand[index] = last_vec;
	index_in_add_cand[last_vec] = index;
	index_in_add_cand[add_v] = -1;
}

void add_cand_add(int add_v) {
	index_in_add_cand[add_v] = add_cand_size;
	add_cand[add_cand_size++] = add_v;
}

int cover_all(set<int> & remove_set) {
	int add_cnt = 0;
	int fg = 0;
	int v1, v2, e;
	int add_v;
	while (uncov_stack_fill_pointer != 0) {
		if (!fg) {
			int best_v, max_dscore = -1;
			for (int i = 0; i < uncov_stack_fill_pointer; i++) {
				e = uncov_stack[i];
				v1 = edge[e].v1;
				v2 = edge[e].v2;
				if (dscore[v1] > max_dscore) {
					best_v = v1; max_dscore = dscore[v1];
				}
				if (dscore[v2] > max_dscore) {
					best_v = v2; max_dscore = dscore[v2];
				}
			}
			if (max_dscore < 2) fg = 1;
			add(best_v);
			add_cnt++;
			c_size++;
			index_in_remove_cand[best_v] = remove_cand_size;
			remove_cand[remove_cand_size++] = best_v;
		}
		else {
			e = uncov_stack[rand() % uncov_stack_fill_pointer];
			v1 = edge[e].v1;
			v2 = edge[e].v2;

			if (remove_set.count(v1) == 0 && remove_set.count(v2) == 1) {
				add_v = v1;
			}
			else if (remove_set.count(v1) == 1 && remove_set.count(v2) == 0) {
				add_v = v2;
			}
			else {
				if (rand() % 10 < 5) {
					add_v = v1;
				}
				else {
					add_v = v2;
				}
			}

			add(add_v);
			add_cnt++;
			c_size++;
			index_in_remove_cand[add_v] = remove_cand_size;
			remove_cand[remove_cand_size++] = add_v;
		}
	}
	return add_cnt;
}
bool update_target_size_1() {
	int remove_cnt = 0;
	int remove_v;
	while (dscore_0_size != 0) {
		remove_v = dscore_0[dscore_0_size - 1];
		remove(remove_v);
		remove_cnt++;
		c_size--;
		remove_cand_vec(remove_v);
	}
	set<int> remove_set;
	while (dscore_1_size != 0) {
		remove_v = dscore_1[dscore_1_size - 1];
		remove(remove_v);
		remove_cnt++;
		c_size--;
		remove_cand_vec(remove_v);
		remove_set.insert(remove_v);
	}
	int add_cnt = cover_all(remove_set);
	return (remove_cnt == add_cnt);
}


//update the best vertex in C
int cand_count;

int choose_remove_v()
{
	int i,v;

	if(flag_can == 1)
		if (rand() % 100000 < base11) {
			return remove_cand[rand() % remove_cand_size];
		}

	int best_v = remove_cand[rand()%remove_cand_size];

	if (flag_can == 1) {
		if (remove_cand_size > 750) {
			for (i = 1; i < 750; ++i)
			{
				v = remove_cand[rand() % remove_cand_size];
				if (v == tabu_add)
					continue;
				if (dscore[v] < dscore[best_v])
					continue;
				else if (dscore[v] > dscore[best_v])
					best_v = v;
				else if (time_stamp[v] < time_stamp[best_v])
					best_v = v;
			}
		}
		else {
			for (i = 0; i < remove_cand_size; ++i)
			{
				v = remove_cand[i];
				if (v == tabu_add)
					continue;
				if (dscore[v] < dscore[best_v])
					continue;
				else if (dscore[v] > dscore[best_v])
					best_v = v;
				else if (time_stamp[v] < time_stamp[best_v])
					best_v = v;
			}
		}
	}
	else {
		if (dscore_0_size > 0) {
			for (i = 0; i < dscore_0_size; ++i)
			{
				v = dscore_0[i];
				if (dscore[v] < dscore[best_v])
					continue;
				else if (dscore[v] > dscore[best_v])
					best_v = v;
				else if (time_stamp[v] < time_stamp[best_v])
					best_v = v;
			}
		}
		else if (dscore_1_size > 0) {
			if (dscore_1_size > 750) {
				for (i = 1; i < 750; ++i)
				{
					v = dscore_1[rand() % dscore_1_size];
					if (v == tabu_add)
						continue;
					if (dscore[v] < dscore[best_v])
						continue;
					else if (dscore[v] > dscore[best_v])
						best_v = v;
					else if (time_stamp[v] < time_stamp[best_v])
						best_v = v;
				}
			}
			else {
				for (i = 0; i < dscore_1_size; ++i)
				{
					v = dscore_1[i];
					if (v == tabu_add)
						continue;
					if (dscore[v] < dscore[best_v])
						continue;
					else if (dscore[v] > dscore[best_v])
						best_v = v;
					else if (time_stamp[v] < time_stamp[best_v])
						best_v = v;
				}
			}
		}
		else {
			if (remove_cand_size > 750) {
				for (i = 1; i < 750; ++i)
				{
					v = remove_cand[rand() % remove_cand_size];
					if (v == tabu_add)
						continue;
					if (dscore[v] < dscore[best_v])
						continue;
					else if (dscore[v] > dscore[best_v])
						best_v = v;
					else if (time_stamp[v] < time_stamp[best_v])
						best_v = v;
				}
			}
			else {
				for (i = 0; i < remove_cand_size; ++i)
				{
					v = remove_cand[i];
					if (v == tabu_add)
						continue;
					if (dscore[v] < dscore[best_v])
						continue;
					else if (dscore[v] > dscore[best_v])
						best_v = v;
					else if (time_stamp[v] < time_stamp[best_v])
						best_v = v;
				}
			}
		}
	}

	return best_v;
}


int choose_remove_v1()
{
	int i, v;

	int best_v = add_cand[rand() % add_cand_size];
	if (add_cand_size > 750) {
		for (i = 0; i < 750; ++i)
		{
			v = add_cand[rand() % add_cand_size];
			if (v_degree[v] < v_degree[best_v])
				continue;
			else if (v_degree[v] > v_degree[best_v])
				best_v = v;
			else if (time_stamp[v] < time_stamp[best_v])
				best_v = v;
		}
	}
	else {
		for (i = 0; i < add_cand_size; ++i)
		{
			v = add_cand[i];
			if (v_degree[v] < v_degree[best_v])
				continue;
			else if (v_degree[v] > v_degree[best_v])
				best_v = v;
			else if (time_stamp[v] < time_stamp[best_v])
				best_v = v;
		}
	}
	return best_v;
}

	inline
void uncover(int e)
{
	index_in_uncov_stack[e] = uncov_stack_fill_pointer;
	push1(e,uncov_stack);
}


	inline
void cover(int e)
{
	int index,last_uncov_edge;

	//since the edge is satisfied, its position can be reused to store the last_uncov_edge
	last_uncov_edge = pop1(uncov_stack);
	index = index_in_uncov_stack[e];
	uncov_stack[index] = last_uncov_edge;
	index_in_uncov_stack[last_uncov_edge] = index;
}



bool is_adjacent(long v1, long v2, vector<vector<long>>& adjacency_list) {
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
void fix(long v, vector<long>& pending_vertice, vector<vector<long>>& adjacency_list) {
	//decrease degree of neighbooring vertice, and update pending_vertice
	for (auto neighbor : adjacency_list[v]) {
		vector<long>::size_type i = 0;
		for (; i < adjacency_list[neighbor].size(); ++i) {
			if (adjacency_list[neighbor][i] == v) {
				break;
			}
		}
		adjacency_list[neighbor][i] = *adjacency_list[neighbor].rbegin();
		adjacency_list[neighbor].pop_back();
		long degree = adjacency_list[neighbor].size();
		if (degree == 1 || degree == 2) {
			pending_vertice.push_back(neighbor);
		}
	}
	adjacency_list[v].clear();
	dscore[v] = 0;
}
int get_rand_in_myheap() {
	long v = my_heap[0];
	long temp_arr[52];
	int front = 0;
	int rear = 1;
	temp_arr[0] = 0;
	int degree = dscore[v];
	while (front != rear && rear < 48) {
		int pos = temp_arr[front];
		int left_child = my_heap_left_child(pos);
		if (left_child < my_heap_count && dscore[my_heap[left_child]] == degree) {
			temp_arr[rear++] = left_child;
		}
		int right_child = my_heap_right_child(pos);
		if (right_child < my_heap_count && dscore[my_heap[right_child]] == degree) {
			temp_arr[rear++] = right_child;
		}
		front++;
	}
	return temp_arr[rand() % rear];
}
std::vector<long> initVC(vector<vector<long>>& adjacency_list) {
	std::vector<long> init_vertice;
	std::vector<long> pending_vertice;
	for (vector<vector<long>>::size_type v = 1; v < adjacency_list.size(); ++v) {
		if (adjacency_list[v].size() == 1 || adjacency_list[v].size() == 2) {
			pending_vertice.push_back(v);
		}
	}
	//init heap
	for (int v = 1; v < adjacency_list.size(); v++) {
		dscore[v] = adjacency_list[v].size();
		my_heap_insert(v);
	}
	while (my_heap_count != 0) {
		if (pending_vertice.size() == 0) {
			//select v remove
			long v = my_heap[get_rand_in_myheap()];
			my_heap_remove(pos_in_my_heap[v]);
			for (auto neighbor : adjacency_list[v]) {
				my_heap_remove(pos_in_my_heap[neighbor]);
				dscore[neighbor]--;
				if (dscore[neighbor] != 0) my_heap_insert(neighbor);
			}
			fix(v, pending_vertice, adjacency_list);
			init_vertice.push_back(v);
			continue;
		}
		long processing_vertice = pending_vertice[pending_vertice.size() - 1];
		pending_vertice.pop_back();

		long degree = adjacency_list[processing_vertice].size();
		if (degree == 1) {
			long u = adjacency_list[processing_vertice][0];
			init_vertice.push_back(u);
			for (auto neighbor : adjacency_list[u]) {
				my_heap_remove(pos_in_my_heap[neighbor]);
				dscore[neighbor]--;
				if (dscore[neighbor] != 0) my_heap_insert(neighbor);
			}
			fix(u, pending_vertice, adjacency_list);
			my_heap_remove(pos_in_my_heap[u]);
		}
		else if (degree == 2) {
			long u1 = adjacency_list[processing_vertice][0], u2 = adjacency_list[processing_vertice][1];
			if (is_adjacent(u1, u2, adjacency_list)) {
				init_vertice.push_back(u1); init_vertice.push_back(u2);
				for (auto neighbor : adjacency_list[u1]) {
					my_heap_remove(pos_in_my_heap[neighbor]);
					dscore[neighbor]--;
					if (dscore[neighbor] != 0) my_heap_insert(neighbor);
				}
				fix(u1, pending_vertice, adjacency_list);
				my_heap_remove(pos_in_my_heap[u1]);
				for (auto neighbor : adjacency_list[u2]) {
					my_heap_remove(pos_in_my_heap[neighbor]);
					dscore[neighbor]--;
					if (dscore[neighbor] != 0) my_heap_insert(neighbor);
				}
				fix(u2, pending_vertice, adjacency_list);
				my_heap_remove(pos_in_my_heap[u2]);
			}
			// add judge 
			else {
				if (adjacency_list[u1].size() > adjacency_list[u2].size()) swap(u1, u2);
				for (auto neighbor : adjacency_list[u1]) {
					if (adjacency_list[neighbor].size() == 2 && neighbor != processing_vertice) {
						if ((adjacency_list[neighbor][0] == u1 && adjacency_list[neighbor][1] == u2) || (adjacency_list[neighbor][0] == u2 && adjacency_list[neighbor][1] == u1)) {
							init_vertice.push_back(u1); init_vertice.push_back(u2);
							for (auto neighbor : adjacency_list[u1]) {
								my_heap_remove(pos_in_my_heap[neighbor]);
								dscore[neighbor]--;
								if (dscore[neighbor] != 0) my_heap_insert(neighbor);
							}
							fix(u1, pending_vertice, adjacency_list);
							my_heap_remove(pos_in_my_heap[u1]);
							for (auto neighbor : adjacency_list[u2]) {
								my_heap_remove(pos_in_my_heap[neighbor]);
								dscore[neighbor]--;
								if (dscore[neighbor] != 0) my_heap_insert(neighbor);
							}
							fix(u2, pending_vertice, adjacency_list);
							my_heap_remove(pos_in_my_heap[u2]);
							break;
						}
					}
				}
			}

		}
	}
	return init_vertice;
}
void init_sol_merge(const vector<long>& init_v) {
	int i, v, e;

	/*** build solution data structures of the instance ***/
	memset(dscore, 0, sizeof(int) * (v_num + 1));
	memset(time_stamp, 0, sizeof(long long) * (v_num + 1));
	memset(v_in_c, 0, sizeof(bool) * (v_num + 1));

	//init vertex cover

	init_method = "heap";

	for (int i = 0; i < init_v.size(); i++) {
		v_in_c[init_v[i]] = 1;
	}
	c_size = init_v.size();
	for (int e = 0; e < e_num; e++) {
		int v1 = edge[e].v1;
		int v2 = edge[e].v2;
	}
	//calculate dscore
	for (int v = 1; v <= v_num; v++)
	{
		dscore[v] = 0;
	}
	for (int e = 0; e < e_num; e++)
	{
		int v1 = edge[e].v1;
		int v2 = edge[e].v2;
		if (v_in_c[v1] == 1 && v_in_c[v2] == 0)
			dscore[v1]--;
		else if (v_in_c[v2] == 1 && v_in_c[v1] == 0)
			dscore[v2]--;
	}

	for (v = 1; v <= v_num; v++)
	{
		time_stamp[v] = 0; // to break ties
	}

	//remove redundent vertices
	for (int v = 1; v <= v_num; v++)
	{
		if (v_in_c[v] == 1 && dscore[v] == 0)
		{
			v_in_c[v] = 0;

			int edge_count = v_degree[v];
			for (int i = 0; i < edge_count; ++i)
			{
				int n = v_adj[v][i];
				dscore[n]--;
			}
			c_size--;
		}
	}

	update_best_sol(0);

	//	std::cout << "c initial method = " << init_method << std::endl;
	//	times(&finish);
	//	double init_sol_time = double(finish.tms_utime - start.tms_utime + finish.tms_stime - start.tms_stime)/sysconf(_SC_CLK_TCK);
	//	init_sol_time = round(init_sol_time * 100)/100.0;
	//	cout << "c initial solution size = " << c_size << endl;
	//	cout << "c initial solution time = " << init_sol_time << endl;

	//check_add_cand();
}

void remove_dscore(int v, int fg) {
	if (!fg) {
		assert(index_dscore_0[v] != -1);
		int index = index_dscore_0[v];
		int last_vec = dscore_0[--dscore_0_size];
		index_dscore_0[last_vec] = index;
		dscore_0[index] = last_vec;
		index_dscore_0[v] = -1;
	}
	else {
		assert(dscore[v] == -1 && index_dscore_1[v] != -1);
		int index = index_dscore_1[v];
		int last = dscore_1[--dscore_1_size];
		index_dscore_1[last] = index;
		dscore_1[index] = last;
		index_dscore_1[v] = -1;
	}
}
void check_criticals(int cur_block) {
	int v_count = block_size[cur_block], v, u;
	for (int i = 0; i < v_count; i++) {
		v = connect_block[cur_block][i];
		if (!v_in_c[v]) {
			int edge_count = v_degree[v];
			int cnt = 0;
			for (int i = 0; i < edge_count; i++) {
				u = v_adj[v][i];
				if (dscore[u] == -1) {
					if (cri_relate[u] != v) cout << "criticals error3" << endl;
					cnt++;
				}
			}
			if(critical_count[v]!=cnt)  cout << "criticals error1" << endl;
		}
	}
}

void add(int v)
{
	v_in_c[v] = 1;
	if (critical_count[v] > 1) {
		remove_v_in_array(v, critical2, index_critical2, size_of_critical);
	}
	critical_count[v] = 0; critical_check[v] = 0;
	conf_change[v] = 0;
	bool is_cri = false;
	if (dscore[v] == 0) {
		index_dscore_0[v] = dscore_0_size;
		dscore_0[dscore_0_size++] = v;
	}
	else if (dscore[v] == 1) {
		index_dscore_1[v] = dscore_1_size;
		dscore_1[dscore_1_size++] = v;
		is_cri = true;
	}
	dscore[v] = -dscore[v];

	int i,e,n;

	int edge_count = v_degree[v];

	for (i=0; i<edge_count; ++i)
	{
		e = v_edges[v][i];// v's i'th edge
		n = v_adj[v][i];//v's i'th neighbor
		conf_change[n] = 1;
		if (v_in_c[n]==0)//this adj isn't in cover set
		{
			if (is_cri) {
				cri_relate[v] = n;
				add_criticals(n);
			}
			dscore[n]--;
			cover(e);
		}
		else
		{
			if (dscore[n] == -2 ) {
				index_dscore_1[n] = dscore_1_size;
				dscore_1[dscore_1_size++] = n;
				int w = find_only_v(n, -1);
				add_criticals(w);
				cri_relate[n] = w;
			}
			else if (dscore[n] == -1 ) {
				cri_relate[n] = -1;
				remove_dscore(n, 1);
				index_dscore_0[n] = dscore_0_size;
				dscore_0[dscore_0_size++] = n;
			}
			dscore[n]++;
		}
	}
	add_cand_vec(v);
}


void remove(int v)
{
	v_in_c[v] = 0;
	bool is_cri = false;
	if (dscore[v] == -1) {
		remove_dscore(v, 1);
		cri_relate[v] = -1;
		is_cri = true;
	}
	else if (dscore[v] == 0) {
		remove_dscore(v, 0);
	}
	dscore[v] = -dscore[v];
	critical_check[v] = 0; critical_count[v] = 0;
	conf_change[v] = 0;

	int i,e,n;

	int edge_count = v_degree[v];
	for (i=0; i<edge_count; ++i)
	{
		e = v_edges[v][i];
		n = v_adj[v][i];
		conf_change[n] = 1;
		if (v_in_c[n]==0)//this adj isn't in cover set
		{
			if (is_cri) {
				critical_count[n]--;
				if (critical_count[n] == 1) remove_v_in_array(n, critical2, index_critical2, size_of_critical);
			}
			dscore[n]++;
			uncover(e);
		}
		else
		{
			if (dscore[n] == -1) {
				remove_dscore(n, 1);
				int w = cri_relate[n];
				critical_count[w]--;
				cri_relate[n] = -1;
				if (critical_count[w] == 1) remove_v_in_array(w, critical2, index_critical2, size_of_critical);
			}
			else if (dscore[n] == 0 ) {
				add_criticals(v);
				cri_relate[n] = v;
				index_dscore_1[n] = dscore_1_size;
				dscore_1[dscore_1_size++] = n;
				remove_dscore(n, 0);
			}
			dscore[n]--;
		}
	}
	add_cand_add(v);
}

/*On solution*/

void print_solution()
{
	int mis_vertex_count=0;

	for (int i=1; i<=v_num; i++)
	{
		if (best_v_in_c[i]!=1)
			mis_vertex_count++;
	}

	if(mis_vertex_count+best_c_size!=v_num)
		cout<<"The size of independent set + the size of vertex cover is not equal to |V(G)|!"<<endl;

	for (int i=1; i<=v_num; i++)
	{
		if (best_v_in_c[i]!=1)//output max independent set
			cout<<i<<'\t';
	}
	cout<<endl;

}

//check whether the solution found is a proper solution
int check_solution()
{
	int e;

	for(e=0; e<e_num; ++e)
	{
		if(best_v_in_c[edge[e].v1]!=1 && best_v_in_c[edge[e].v2]!=1)
		{
			cout<<"uncovered edge "<<e<<endl;
			return 0;
		}
	}

	return 1;
}


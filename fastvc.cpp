#include "fastvc.h"
#include "preprocess.h"
#include <map>
#include <algorithm>

using namespace std;

int try_step=100;


int edge_cand;

void check_dscore_01(){
	//检查是否douwei-1
	for (int i = 0; i < dscore_1_size; i++) {
		if (index_dscore_1[dscore_1[i]] != i) cout << "index error" << endl;
		if (dscore[dscore_1[i]] != -1) {
			int v = dscore_1[i];
			cout << v << " :" << dscore[v] << endl;
			cout << "dscore_1 has not -1" << endl;
			break;
		}
	}
	for (int v = 1; v <= v_num; v++) {
		if (dscore[v] == -1 && (v != dscore_1[index_dscore_1[v]])) {
			cout << "dscore = -1 but not in dscore_1 " << v << endl;
			break;
		}
		if (dscore[v] != -1 && index_dscore_1[v] != -1) {
			cout << "index is error" << endl;
		}
	}
	for (int i = 0; i < dscore_0_size; i++) {
		if (dscore[dscore_0[i]] != 0) {
			cout << "dscore_0 has not 0" << endl;
			break;
		}
	}
}
int choose_add() {
	int best_v = edge[uncov_stack[0]].v1;
	int e, v1, v2, add_v;
	for (int i = 0; i < uncov_stack_fill_pointer; i++) {
		e = uncov_stack[rand() % uncov_stack_fill_pointer];
		v1 = edge[e].v1;
		v2 = edge[e].v2;

		if (dscore[v1] > dscore[v2] || (dscore[v1] == dscore[v2] && time_stamp[v1] < time_stamp[v2]))
			add_v = v1;
		else add_v = v2;
		if (dscore[add_v] > dscore[best_v]) best_v = add_v;
	}
	return best_v;
}

void random_change(int step) {
	int remove_v;
	int add_v;
	int cnt = sqrt(dscore_1_size);
	for (int i = 0; i < cnt && remove_cand_size != 0; i++) {
		remove_v = remove_cand[rand() % remove_cand_size];
		remove(remove_v);
		remove_cand_vec(remove_v);
		time_stamp[remove_v] = step;
		c_size--;
	}
	for (int i = 0; i < cnt && uncov_stack_fill_pointer != 0; i++) {
		add_v = choose_add();
		add(add_v);
		c_size++;
		index_in_remove_cand[add_v] = remove_cand_size;
		remove_cand[remove_cand_size++] = add_v;
		time_stamp[add_v] = step;
	}
}

bool compare_degree(int v1, int v2)
{
	return v_degree[v1] > v_degree[v2];
}

int choose_remove_v3()
{
	int i, v;

	//if (rand() % 10 < 5) {
	//	return  remove_cand[rand() % remove_cand_size];
	//}

	int best_v = remove_cand[rand() % remove_cand_size];

	for (i = 1; i < 750; ++i)
	{
		v = remove_cand[rand() % remove_cand_size];
		if (time_stamp[v] < time_stamp[best_v] && v_degree[v] < v_degree[best_v])
			best_v = v;
	}


	return best_v;
}

void random_change1(int step) {
	vector<int> rem_cand;
	//vector<int> vector_vertex_not_in_cover;
	int count = 0;
	int disturb_v = 85;
	for (int i = 0; i < disturb_v; i++) {
		if (remove_cand_size == 0)
			break;
		int remove_v = choose_remove_v3();
		remove(remove_v);
		remove_cand_vec(remove_v);
		time_stamp[remove_v] = step;
		c_size--;
		count++;
		
	}
	

	for (int i = 0; i < count; i++) {
		int vvv = choose_remove_v1();
		add(vvv);
		c_size++;
		index_in_remove_cand[vvv] = remove_cand_size;
		remove_cand[remove_cand_size++] = vvv;
	}

}


int check_clique() {
	bool flag = true;
	int u, w;
	int reduce_v = 0;
	while (flag) {
		flag = false;
		vector<int> tmp(critical2, critical2 + size_of_critical);
next:	for (auto v : tmp) {
			//if (index_critical2[v] != -1 && critical_count[v] < 2) cout << "error in citicals" << endl;
			if (critical_check[v] || index_critical2[v] == -1) continue;
			critical_check[v] = 1;
			vector<int> vs;
			int edge_count = v_degree[v];
			for (int i = 0; i < edge_count; i++) {
				u = v_adj[v][i];
				if (dscore[u] == -1) vs.push_back(u);
			}
			for (auto u1 : vs) {
				for (auto u2 : vs) neighbor_indicator[u2] = 1;
				int edge_count_u1 = v_degree[u1];
				for (int i = 0; i < edge_count_u1; i++) {
					w = v_adj[u1][i];
					neighbor_indicator[w] = 0;
				}
				for (auto u2 : vs) {
					if (neighbor_indicator[u2] && u2 != u1) {
						// u1,u2 is not adjacency
						flag = true;
						remove(u1); remove(u2); add(v);
						int index = index_in_remove_cand[u1];
						index_in_remove_cand[u1] = -1;
						remove_cand[index] = v;
						index_in_remove_cand[v] = index;
						time_stamp[v] = time_stamp[u1] = time_stamp[u2] = step;
						remove_cand_vec(u2);
						c_size--; reduce_v++;
						countbbbbbb++;
						goto next;
					}
				}
			}
		}
	}
	//if (block != 0) improve_in_check_clique += reduce_v;
	return reduce_v;
}
void reset_criticals(int cur_block) {
	for (int i = 0; i < size_of_critical; i++) {
		index_critical2[critical2[i]] = -1;
	}
	//memset(index_critical2, -1, sizeof(int) * (v_num + 1));
	size_of_critical = 0;
	int v_count = block_size[cur_block], v, u;
	memset(cri_relate, -1, sizeof(int) * (v_num + 1));
	for (int i = 0; i < v_count; i++) {
		v = connect_block[cur_block][i];
		if (dscore[v] == -1) {
			u = find_only_v(v, -1);
			add_criticals(u);
			cri_relate[v] = u;
		}
	}
	/*set<int> test;
	for (int i = 0; i < size_of_critical; i++) {
		if (test.count(critical2[i])) cout << "111" << endl;
		else test.insert(critical2[i]);
	}*/
}
void reset_dscore01(int cur_block) {
	int v;
	dscore_1_size = dscore_0_size = 0;
	int v_count = block_size[cur_block];
	for (int i = 0; i < v_count; i++) {
		v = connect_block[cur_block][i];
		if (dscore[v] == -1) {
			index_dscore_1[v] = dscore_1_size;
			dscore_1[dscore_1_size++] = v;
		}
		else if (dscore[v] == 0 && v_in_c[v] == 1) {
			index_dscore_0[v] = dscore_0_size;
			dscore_0[dscore_0_size++] = v;
		}
	}
}
void check_dscore(int cur_block) {
	int v_count = block_size[cur_block];
	int v;
	for (int i = 0; i < v_count; i++) {
		v = connect_block[cur_block][i];
		if (dscore[v] == -1) {
			if (index_dscore_1[v] == -1) {
				cout << "dscore 1 error" << endl;
			}
		}
		else if (dscore[v] == 0 && v_in_c[v]) {
			if (index_dscore_0[v] == -1) {
				cout << "dscore 0 error" << endl;
			}
		}
	}
}
bool is_bipartite_graph(int cur_block) {
	int* color = new int[v_num + 1];
	int* q = new int[v_num + 1];
	memset(color, -1, sizeof(int) * (v_num + 1));
	int v = connect_block[cur_block][0], u;
	int front = 0, rear = 1;
	q[0] = v;
	color[v] = 0;
	while (front < rear) {
		v = q[front];
		int edge_count = v_degree[v];
		for (int i = 0; i < edge_count; i++) {
			u = v_adj[v][i];
			if (color[u] == -1) {
				color[u] = 1 - color[v];
				q[rear++] = u;
			}
			else if (color[u] == color[v]) {
				return false;
			}
		}
		front++;
	}
	delete[] color;
	delete[] q;
	return true;
}
void reback() {
	local_best_size = best_c_size;
	uncov_stack_fill_pointer = 0;
	for (int v = 1; v <= v_num; v++) {
		v_in_c[v] = best_v_in_c[v];
		dscore[v] = 0;
	}
	c_size = best_c_size;
	for (int e = 0; e < e_num; e++)
	{
		int v1 = edge[e].v1;
		int v2 = edge[e].v2;
		if (v_in_c[v1] == 1 && v_in_c[v2] == 0)
			dscore[v1]--;
		else if (v_in_c[v2] == 1 && v_in_c[v1] == 0)
			dscore[v2]--;
	}
	int v, u;
	memset(index_dscore_1, -1, sizeof(int) * (v_num + 1));
	memset(index_dscore_0, -1, sizeof(int) * (v_num + 1));
	dscore_1_size = dscore_0_size = 0;
	memset(index_in_remove_cand, -1, sizeof(int) * (v_num + 1));
	memset(index_in_add_cand, -1, sizeof(int) * (v_num + 1));
	remove_cand_size = add_cand_size = 0;
	memset(index_critical2, -1, sizeof(int) * (v_num + 1));
	size_of_critical = 0;
	memset(critical_count, 0, sizeof(int) * (v_num + 1));
	memset(cri_relate, -1, sizeof(int) * (v_num + 1));
	for (int i = 1; i <= connect_block_size; i++) {
		for (int j = 0; j < block_size[i]; j++) {
			v = connect_block[i][j];
			if (v_in_c[v]==1) {
				index_in_remove_cand[v] = remove_cand_size;
				remove_cand[remove_cand_size++] = v;
				if (dscore[v] == -1) {
					u = find_only_v(v, -1);
					add_criticals(u);
					cri_relate[v] = u;
					index_dscore_1[v] = dscore_1_size;
					dscore_1[dscore_1_size++] = v;
				}
				else if (dscore[v] == 0) {
					index_dscore_0[v] = dscore_0_size;
					dscore_0[dscore_0_size++] = v;
				}
			}
			else {
				index_in_add_cand[v] = add_cand_size;
				add_cand[add_cand_size++] = v;
			}
		}
	}
	//cout << dscore_1_size << " " << dscore_0_size << " " << remove_cand_size << " " << add_cand_size << endl;
}
int find_valid_block(int cur_blcok) {
	while (block_size[cur_blcok] == 0 && cur_blcok < connect_block_size) cur_blcok++;
	return cur_blcok;
}


int cover_LS(int reduce_size)
{
	memset(index_dscore_1, -1, sizeof(int) * (v_num + 1));
	memset(index_dscore_0, -1, sizeof(int) * (v_num + 1));
	memset(index_in_remove_cand, -1, sizeof(int) * (v_num + 1));
	memset(index_in_add_cand, -1, sizeof(int) * (v_num + 1));
	int search_basic = v_num - reduce_size;
	int cur_block = find_valid_block(1);
	double given_time = (cutoff_time - 20) * block_size[cur_block] / search_basic;
	uncov_stack_fill_pointer = 0;
	cri_count1 = cri_count2 = 0;
	if (block_size[cur_block] == 0 && cur_block == connect_block_size) {
		local_best_size = best_c_size;
		cur_block = 0;
		return cur_block;
	}
	reset_dscore01(cur_block);
	reset_remove_cand(cur_block);
	reset_criticals(cur_block);
	//check_dscore(cur_block);
	int unimprove_cri = 0;
	int unimprove = 0;
	int unimprove_clique = 0;
	int		remove_v, add_v;
	int		e,v1,v2;
	step = 1;
	int unimprove_thre = 38000;
UP:	while(1)
	{
		if (uncov_stack_fill_pointer == 0)//update best solution if needed 删除关联性最差的一个顶点
		{
			update_best_sol(cur_block);
			if (check_clique() != 0) continue;
			update_target_size_0();
			tabu_add = -1;
			continue;
		}

		if (step % try_step == 0) //check cutoff 每循环100次检查一下时间
		{
			times(&finish);
			double elap_time = (finish.tms_utime + finish.tms_stime - start_time) / sysconf(_SC_CLK_TCK);
			if (elap_time >= cutoff_time) return cur_block;
			if (elap_time > given_time && cur_block != 0) {
				if (cur_block == connect_block_size) {
					cur_block = 0;
					local_best_size = best_c_size;
					reback();
					step++;
					continue;
				}
				
				c_size = best_c_size;
				uncov_stack_fill_pointer = 0;
				cur_block++;
				while (block_size[cur_block] == 0 && cur_block < connect_block_size) cur_block++;
				if (block_size[cur_block] == 0 && cur_block == connect_block_size) {
					cur_block = 0; local_best_size = best_c_size;
					reback();
					step++;
					continue;
				}
				given_time = elap_time + (cutoff_time - 20) * block_size[cur_block] / search_basic;
				//update remove_cand  ,cur_dscore_1,cur_dscore_0 
				flag_can = 1;
				/*update remove_cand remove_cand_size*/
				reset_remove_cand(cur_block);
				reset_criticals(cur_block);
				reset_dscore01(cur_block);
				//check_criticals(cur_block);
				countttt = 0;
				countttt2 = 0;
				step++;
				
				continue;
			}
		}
		if (unimprove > unimprove_thre && (flag_can == 1 || 2 * countttt2 <= countttt )) {
			perturb_fill_pointer = uncov_stack_fill_pointer;
			if(block_size[cur_block] > 3000)
				countttt++;
			unimprove = 0;
			random_change1(step);
			tabu_add = -1;
			base11 = 50;
			flag_can = 0;
			continue;
		}

		if (flag_can == 0 && uncov_stack_fill_pointer == perturb_fill_pointer) {
			flag_can = 1;
			if (unimprove > 19000 && block_size[cur_block] > 3000) {
				countttt2++;
			}

		}

		remove_v = choose_remove_v();
		
		remove(remove_v);
		int cnt = sqrt(uncov_stack_fill_pointer);
		if (cnt == 0)
			cnt = 1;
		
		if(flag_can == 1){
			e = uncov_stack[rand()%uncov_stack_fill_pointer];
			v1 = edge[e].v1;
			v2 = edge[e].v2;
			if (conf_change[v1] == 0) {
				add_v = v2;
			}
			else if (conf_change[v2] == 0) {
				add_v = v1;
			}
			else {

				if (dscore[v1] == dscore[v2]) {
					if (time_stamp[v1] < time_stamp[v2])
						add_v = v1;
					else
						add_v = v2;
				}
				else {
					if (dscore[v1] > dscore[v2])
						add_v = v1;
					else add_v = v2;
				}
			}
		}
		else {
			int best_score = -1;
			int best_v = -1;
			int best_time_step = step;
			for (int i = 0; i < cnt; i++) {
				e = uncov_stack[rand() % uncov_stack_fill_pointer];
				v1 = edge[e].v1;
				v2 = edge[e].v2;
				if (conf_change[v1] == 0) {
					if (dscore[v2] > best_score) {
						add_v = v2;
						best_score = dscore[v2];
						best_time_step = time_stamp[v2];
					}
					else if (dscore[v2] == best_score) {
						if (time_stamp[v2] < best_time_step)
						{
							add_v = v2;
							best_time_step = time_stamp[v2];
						}
					}
				}
				else if (conf_change[v2] == 0) {
					if (dscore[v1] > best_score) {
						add_v = v1;
						best_score = dscore[v1];
						best_time_step = time_stamp[v1];
					}
					else if (dscore[v1] == best_score) {
						if (time_stamp[v1] < best_time_step)
						{
							add_v = v1;
							best_time_step = time_stamp[v1];
						}
					}
				}
				else {
					if (dscore[v1] > best_score) {
						add_v = v1;
						best_score = dscore[v1];
						best_time_step = time_stamp[v1];
					}
					else if (dscore[v1] == best_score) {
						if (time_stamp[v1] < best_time_step)
						{
							add_v = v1;
							best_time_step = time_stamp[v1];
						}
					}

					if (dscore[v2] > best_score) {
						add_v = v2;
						best_score = dscore[v2];
						best_time_step = time_stamp[v2];
					}
					else if (dscore[v2] == best_score) {
						if (time_stamp[v2] < best_time_step)
						{
							add_v = v2;
							best_time_step = time_stamp[v2];
						}
					}
					
				}
			}
		}

		add(add_v);
		tabu_add = add_v;
		// 删除顶点和加入顶点后在这里更新 候选解
		int index = index_in_remove_cand[remove_v];
		index_in_remove_cand[remove_v] = -1;

		remove_cand[index] = add_v;
		index_in_remove_cand[add_v] = index;

		time_stamp[add_v]=time_stamp[remove_v]=step;

		step++;
		unimprove++;



		/*if (uncov_stack_fill_pointer == 1) {
			e = uncov_stack[0];
			v1 = edge[e].v1;
			v2 = edge[e].v2;
			if (CK_critical(v1) || CK_critical(v2)) cri_count2++;
			else {
				cri_count1 += swap_critical_vertices() + 1;
			}
		}*/
		//check_dscore(cur_block);
		//check_add_cand();
	}

}
void connect_bfs(int v) {
	int sum_edge = 0;
	int* my_queue;
	my_queue = new int[v_num + 1];
	int front = 0;
	int rear = 1;
	my_queue[0] = v;
	connect_id[v] = connect_block_size;
	int u, w;
	int edge_count;
	while (front != rear) {
		u = my_queue[front];
		edge_count = v_degree[u];
		sum_edge += edge_count;
		for (int i = 0; i < edge_count; i++) {
			w = v_adj[u][i];
			if (connect_id[w] == 0) {
				connect_id[w]= connect_block_size;
				my_queue[rear++] = w;
			}
		}
		front++;
	}
	//cout << rear << " " << sum_edge / 2 << endl;
	delete[] my_queue;
}
void init_connect_block() {
	memset(connect_id, 0, sizeof(int) * (v_num + 1));
	connect_block_size = 0;
	for (int v = 1; v <= v_num; v++) {
		if (connect_id[v] != 0) continue;
		connect_block_size++;
		connect_bfs(v);
	}
	
	// connnect_block start at 1,end with connect_block_size;
	connect_block = new int* [connect_block_size + 2];
	block_size = new int[connect_block_size + 1];
	edge_size = new int[connect_block_size + 1];
	memset(block_size, 0, sizeof(int) * (connect_block_size + 1));
	memset(edge_size, 0, sizeof(int) * (connect_block_size + 1));
	for (int v = 1; v <= v_num; v++) {
		block_size[connect_id[v]]++;
	}
	
	for (int block = 1; block <= connect_block_size; block++) {
		connect_block[block] = new int[block_size[block]];
		block_size[block] = 0;
	}

	int block;
	for (int v = 1; v <= v_num; v++) {
		block = connect_id[v];
		connect_block[block][block_size[block]++] = v;
		edge_size[block] += v_degree[v];
	}

	for (int block = 1; block <= connect_block_size; block++) {
		edge_size[block] = edge_size[block] / 2;
	}
	//cout << v_num << " " << e_num << " " << connect_block_size << endl;
	block_size[0] = 0;
	connect_block[0] = new int[v_num + 1];
	for (int v = 1; v <= v_num; v++) {
		connect_block[0][block_size[0]++] = v;
	}
}
int pre_search() {
	int reduce_size = 0;
	int improve_size = 0;
	int* add_cplex_flag = new int[v_num + 1];
	int* cplex_tore = new int[v_num + 1];
	int* graph_cplex_tore = new int[v_num + 1];
	cplex_ans = new int[connect_block_size + 1];
	cplex_time = new double[connect_block_size + 1];
	ls_ans = new int[connect_block_size + 1];
	//cout << connect_block_size << endl;
	for (int block = 1; block <= connect_block_size; block++) {
		/*if (BGVC(block)) {
			reduce_size += block_size[block];
		}
		else*/
		//cout << block_size[block] << " " << edge_size[block] << " " << edge_size[block] / block_size[block] << endl;
		if (block_size[block] < 300 || (edge_size[block] * 2.0 / block_size[block] < 7 && block_size[block] < 1e6)) {
			//reset 
			int v_count = block_size[block];
			int init_size = 0;
			for (int i = 0; i < v_count; i++) {
				int v = connect_block[block][i];
				if (v_in_c[v]) {
					v_in_c[v] = 0;
					c_size--; init_size++;
				}
			}
			cplex_ans[block] = v_num; cplex_time[block] = 0;
			times(&cplex_start);

			int u, v, i, j, k;

			IloEnv env;
			IloNumVarArray Xc(env);
			IloExpr obj(env);
			IloModel model(env);
			IloCplex cplex(model);
			cplex.setOut(env.getNullStream());
			cplex.setParam(IloCplex::EpGap, 0);

			for (i = 0; i < v_num + 1; i++)  add_cplex_flag[i] = 0;
			for (i = 0; i < v_num + 1; i++)  cplex_tore[i] = 0;
			for (i = 0; i < v_num + 1; i++)  graph_cplex_tore[i] = -1;
			int in_clpex_index = 0;

			for (i = 0; i < block_size[block]; i++)//给block中所有的顶点编号
			{
				u = connect_block[block][i];
				if (add_cplex_flag[u] == 0)//未编号
				{
					Xc.add(IloNumVar(env, 0, 1, ILOBOOL));
					obj += Xc[in_clpex_index];
					cplex_tore[in_clpex_index] = u;
					graph_cplex_tore[u] = in_clpex_index;
					add_cplex_flag[u] = 1;
					in_clpex_index++;//X编号++
				}
			}

			int const_num = 0;
			for (i = 0; i < block_size[block]; i++)
			{
				u = connect_block[block][i];
				for (j = 0; j < v_degree[u]; j++)
				{
					v = v_adj[u][j];//u的邻居
					if (u > v)//只看（u，v）且u > v的情况
					{
						const_num++;
						IloExpr conDum(env);//建立约束
						conDum += Xc[graph_cplex_tore[u]];
						conDum += Xc[graph_cplex_tore[v]];
						IloRange dumConst(env, 1, conDum, 2);//1<=conDum<=2，每个边至少有一个顶点在解集中
						model.add(dumConst);//添加约束
					}
				}
			}

			model.add(IloMinimize(env, obj));
			cplex.setParam(IloCplex::Param::TimeLimit, cutoff_time * block_size[block] / (v_num * 2.0));//设置时间
			cplex.setParam(IloCplex::Threads, 1);//设置线程数
			cplex.solve();

			//std::ofstream outFile1;//输出流
			//outFile1.open("cplex_result1.txt", std::ios::app);
			//exit(0);

			// best solution
			if (cplex.getStatus() != IloAlgorithm::Status::Optimal) {
				//cout << 22222 << endl;
				for (i = 0; i < v_count; i++) {
					int w = connect_block[block][i];
					v_in_c[w] = best_v_in_c[w];
				}
				c_size = best_c_size;

			}
			else {
				double eps = 0.000001;
				int ans = 0;
				double val_ans = 0;
				for (i = 0; i < in_clpex_index; i++) {
					//outFile1 << cplex.getValue(Xc[i]) << "\n";
					val_ans += cplex.getValue(Xc[i]);
					if ((int)(cplex.getValue(Xc[i]) + eps) == 1) {
						u = cplex_tore[i];//这个分支说明u为1，否则u为0
						v_in_c[u] = 1;
						c_size++; ans++;
					}
				}

				reduce_size += block_size[block];
				update_best_sol(block); block_size[block] = 0;
				improve_size += init_size - ans;
				cplex_ans[block] = ans;

			}

			times(&finish);
			cplex_time[block] = double(finish.tms_utime - cplex_start.tms_utime + finish.tms_stime - cplex_start.tms_stime) / sysconf(_SC_CLK_TCK);
			cplex_search_time += cplex_time[block];
		}
	}
	//cout << 111111111111 << endl;
	return reduce_size;
}
int main(int argc, char* argv[])
{
	if (argc != 4) {
		cout << "usage: " << endl;
		return 0;
	}
	int i;

	//cout<<"c This is NuMVC, a local search solver for the Minimum Vertex Cover (and also Maximum Independent Set) problem."<<endl;

	//preprocessing
	Preprocess preprocess(argv[1]);
	file_name1 = argv[1];
	i = 2;
	sscanf(argv[i++], "%d", &seed);
	sscanf(argv[i++], "%lf", &cutoff_time);
	srand(seed);


	times(&start);
	preprocess.Prep();
	start_time = start.tms_utime + start.tms_stime;
	times(&finish);
	double preprocess_time = double(finish.tms_utime + finish.tms_stime - start_time)/sysconf(_SC_CLK_TCK);

	//simplified_graph is empty
	if(build_instance(preprocess.get_adjacency_list()) != 1){
		times(&start);
		start_time = start.tms_utime + start.tms_stime;
		if (preprocess.verify((long*) 0, (long*) 0)) {
			ofstream outputfile("final.txt", std::ios::app);
			outputfile << preprocess.origin_graph_filename << " " << preprocess.ans << " " << seed << " " << preprocess_time << endl;
			outputfile.close();
		}
		else {
			cout<<"the solution is wrong."<<endl;
		}
		return 0;
	}
	times(&start);
	start_time = start.tms_utime + start.tms_stime;

	optimal_size=0;
	cutoff_time -= preprocess_time;


	
	init_connect_block();
	vector<long> init_v = initVC(preprocess.get_adjacency_list());
	//	init_sol();
	init_sol_merge(init_v);
	times(&finish);
	double init_time = double(finish.tms_utime + finish.tms_stime - start_time) / sysconf(_SC_CLK_TCK);
	//c_plex and bipatite search
	int reduce_size = pre_search();
	cutoff_time -= cplex_search_time;
	int final_block = cover_LS(reduce_size);

	//check solution
	long *solution = new long[best_c_size];
	for (long i = 1, j = 0; i <= v_num; ++i) {
		if (best_v_in_c[i] == 1) {
			solution[j++] = i;
		}
	}

	if (preprocess.verify(solution, solution + best_c_size)) {
		//cout << v_num << " " << reduce_size << " " << cplex_search_time << endl;
		ofstream outputfile("final.txt", std::ios::app);
		//outputfile << preprocess.origin_graph_filename << " " << preprocess.ans << " " << seed << " " << final_block << " " << local_best_size - best_c_size << " " << preprocess_time << endl;
		outputfile << preprocess.origin_graph_filename << " " << seed << " " << preprocess.ans <<  endl;
		//outputfile << preprocess.origin_graph_filename << " " << init_time << endl;
		outputfile.close();
	}
	else
	{
		cout<<"the solution is wrong."<<endl;
	}
	delete []solution;

	free_memory();

	return 0;
}

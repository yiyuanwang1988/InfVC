/************************************************
 ** This is a local search solver for Minimum Vertex Cover.                                                       
 ************************************************/


/************************************************
 ** Date:	2015.2.2  
 ** FastVC
 ** Author: Shaowei Cai, caisw@ios.ac.cn   
 **		   Key Laboratory of Computer Science,
 **		   Institute of Software, Chinese Academy of Sciences, 
 **		   Beijing, China                                                                        
 ************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <set>
#include <assert.h>
#include <sys/times.h>
#include <cmath>
#include "/home/wyy/ibm/ILOG/CPLEX_Studio201/cplex/include/ilcplex/cplex.h"
#include "/home/wyy/ibm/ILOG/CPLEX_Studio201/cplex/include/ilcplex/ilocplex.h"

using namespace std;
int countttt = 0;
int countttt2 = 0;

int flag_can = 1;
int perturb_fill_pointer;



#define pop1(stack) stack[--stack ## _fill_pointer]
#define push1(item, stack) stack[stack ## _fill_pointer++] = item

tms start, finish, internal_start, cplex_start;
double sum_bitime, internal_search_time = 0, cplex_search_time = 0;
int start_time, internal_start_time;

int base11 = 18;
int flagyy = 1;

int* edge_size;

struct Edge{
	int v1;
	int v2;
};
int*	conf_change;
/*parameters of algorithm*/
long long	max_steps;			//step limit
double		cutoff_time;			//time limit
long long	step;
int			optimal_size;			//terminate the algorithm before step limit if it finds a vertex cover of optimal_size

// connect_block
int** connect_block;
int* block_size;
int* connect_id;
int connect_block_size;
/*parameters of the instance*/
int		v_num;//|V|: 1...v
int		e_num;//|E|: 0...e-1

/*structures about edge*/
Edge *edge;

/*structures about vertex*/
int*	dscore;						//dscore of v
long long*	time_stamp;


//from vertex to it's edges and neighbors
int**	v_edges;		//edges related to v, v_edges[i][k] means vertex v_i's k_th edge
int**	v_adj;			//v_adj[v_i][k] = v_j(actually, that is v_i's k_th neighbor)
int*	v_degree;		//amount of edges (neighbors) related to v


/* structures about solution */
//current candidate solution
int		c_size;						//cardinality of C
bool*	v_in_c;						//a flag indicates whether a vertex is in C
int		tmp_c_size;						//cardinality of C
bool*	tmp_v_in_c;						//a flag indicates whether a vertex is in C
int*	remove_cand;				//remove candidates, an array consists of only vertices in C, not including tabu_remove
int*	index_in_remove_cand;
int		remove_cand_size;

int* dscore_1;
int* index_dscore_1;
int dscore_1_size;

int* dscore_0;
int* index_dscore_0;
int dscore_0_size;

int* critical2;
int* index_critical2;
int size_of_critical;
int* cri_relate;
int* critical_count;
int* critical_check;
//best solution found
int		best_c_size;
bool*	best_v_in_c;				//a flag indicates whether a vertex is in best solution
double  best_comp_time;
long    best_step;

int local_best_size = 0;

//uncovered edge stack
int*	uncov_stack;				//store the uncov edge number
int		uncov_stack_fill_pointer;
int*	index_in_uncov_stack;		//which position is an edge in the uncov_stack

string file_name1;
//CC and taboo
//int		tabu_remove=0;
int seed;
string init_method;

int tabu_add;
int flagff = 1;

int*	add_cand;
int*	index_in_add_cand;
int		add_cand_size;

//int* dscore_0;
//int* index_dscore_0;
//int dscore_0_size;
int* visit;
int* remarked;
int* X;
int X_num;
int* Y;
int Y_num;
int* p;
int sum_bigraph_size;
int* neighbor_indicator;

int cri_count1, cri_count2;

//c_plex
int* cplex_ans;
int* ls_ans;
double* cplex_time;
int improve_in_check_clique = 0;
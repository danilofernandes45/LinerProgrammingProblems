#include <stdio.h>
#include<bits/stdc++.h>
#include <ilcplex/ilocplex.h>
using namespace std;

void add_restriction(vector<int> W, IloEnv &env, IloModel &steiner, int graph[][2], int num_edges, IloIntVarArray &y)
{
	IloExpr sum(env);

	for (int i = 0; i < num_edges; i++) {
		int v1 = graph[i][0];
		int v2 = graph[i][1];
		if( W[v1] != W[v2] )
			sum += y[i];
	}
	steiner.add(sum >= 1);
}

void generate_W(vector<int> W, int terminals[], int num_terminals_in_W, int num_terminals, int positon, IloEnv &env, IloModel &steiner, int graph[][2], int num_edges, IloIntVarArray &y)
{
	if( positon >= W.size() )
		return;

	generate_W(W, terminals, num_terminals_in_W, num_terminals, positon + 1, env, steiner, graph, num_edges, y);

	W[positon] = 1;
	num_terminals_in_W += terminals[positon];

	if( num_terminals_in_W == num_terminals )
		return;

	if( num_terminals_in_W >= 1 )
		add_restriction(W, env, steiner, graph, num_edges, y);

	generate_W(W, terminals, num_terminals_in_W, num_terminals, positon + 1, env, steiner, graph, num_edges, y);
}

void generate_connect_constraints(int terminals[], int num_terminals, int num_vertices, IloEnv &env, IloModel &steiner, int graph[][2], int num_edges, IloIntVarArray &y)
{
	std::vector<int> W(num_vertices, 0);
	generate_W(W, terminals, 0, num_terminals, 0, env, steiner, graph, num_edges, y);
}

int main()
{

	printf("Teste\n");

	IloEnv env;
	IloModel steiner(env, "Problema de Steiner em grafos com limites de elo e links");
	IloCplex cplex(steiner);

	// Statement Data:
	int num_vertices, num_edges, l, r;
	scanf("%d %d %d %d", &num_vertices, &num_edges, &l, &r);

	int graph[num_edges][2] = {};
	int cost_edge[num_edges] = {};

	int vert1, vert2, cost;
	for (int i = 0; i < num_edges; i ++)
	{
		scanf("%d %d %d", &vert1, &vert2, &cost);
		graph[i][0] = vert1 - 1 ;
		graph[i][1] = vert2 - 1;
		cost_edge[i] = cost;
	}

	int num_terminals;
	scanf("%d", &num_terminals);

	int terminals[num_vertices] = {};
	int term;
	for (int i = 0; i < num_terminals; i++)
	{
		scanf("%d", &term);
		terminals[term-1] = 1;
	}

	int num_non_terminals = num_vertices - num_terminals;
	int non_terminals[num_non_terminals] = {};
	int id = 0;

	for (int i = 0; i < num_vertices; i++)
	{
		if( terminals[i] == 0 )
		{
			non_terminals[id] = i;
			id++;
		}
	}


	// Decision Variables:
	IloIntVarArray y(env, num_edges, 0, 1);

	IloIntVarArray p(env, num_non_terminals, 0, 1);
	IloIntVarArray q(env, num_non_terminals, 0, 1);
	IloIntVarArray s(env, num_non_terminals, 0, 1);

	// Restrictions:

	//For every pair of terminal vertices, there must exist in the solution a path connecting them
	generate_connect_constraints(terminals, num_terminals, num_vertices, env, steiner, graph, num_edges, y);

	//Steiner vertex with degree 2 or more
	for (int i = 0; i < num_non_terminals; i++) {
		int vertex = non_terminals[i];
		for (int j = 0; j < num_edges; j++)
		{
			if( graph[j][0] == vertex || graph[j][1] == vertex )
			{
				for (int k = j+1; k < num_edges; k++)
				{
					if( graph[k][0] == vertex || graph[k][1] == vertex )
						steiner.add( p[i] >= ( y[j] + y[k] - 1 ) );

				}
			}
		}
	}

	for (int i = 0; i < num_non_terminals; i++) {
		int vertex = non_terminals[i];
		int num_neighbors = 0;

		for (int e = 0; e < num_edges; e++) {
			if( graph[e][0] == vertex || graph[e][1] == vertex )
				num_neighbors++;
		}

		for (int n = 0; n < num_neighbors; n++) {
			int count = 0;
			IloExpr sum(env);
			for (int e = 0; e < num_edges; e++) {
				if( ( graph[e][0] == vertex || graph[e][1] == vertex ) )
				{
				 	if( count != n )
						sum += y[e];

					count++;
				}
			}
			steiner.add( p[i] <= sum );
		}
	}

	//Steiner vertex with degree 3 or more
	for (int i = 0; i < num_non_terminals; i++) {
		int vertex = non_terminals[i];
		for (int j = 0; j < num_edges; j++)
		{
			if( graph[j][0] == vertex || graph[j][1] == vertex )
			{
				for (int k = j+1; k < num_edges; k++)
				{
					if( graph[k][0] == vertex || graph[k][1] == vertex )
					{
						for (int m = k+1; m < num_edges; m++)
						{
							if( graph[m][0] == vertex || graph[m][1] == vertex )
								steiner.add( q[i] >= ( y[j] + y[k] + y[m] - 2 ) );
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < num_non_terminals; i++) {
		int vertex = non_terminals[i];
		int num_neighbors = 0;

		for (int e = 0; e < num_edges; e++) {
			if( graph[e][0] == vertex || graph[e][1] == vertex )
				num_neighbors++;
		}

		for (int n_2 = 0; n_2 < num_neighbors; n_2++) {
			for (int n_1 = n_2+1; n_1 < num_neighbors; n_1++) {
				int count = 0;
				IloExpr sum(env);
				for (int e = 0; e < num_edges; e++) {
					if( ( graph[e][0] == vertex || graph[e][1] == vertex ) )
					{
					 	if( count != n_1 && count != n_2)
							sum += y[e];

						count++;
					}
				}
				steiner.add( q[i] <= sum );
			}
		}
	}

	//Steiner vertex with degree 4 or more
	for (int i = 0; i < num_non_terminals; i++) {
		int vertex = non_terminals[i];
		for (int j = 0; j < num_edges; j++)
		{
			if( graph[j][0] == vertex || graph[j][1] == vertex )
			{
				for (int k = j+1; k < num_edges; k++)
				{
					if( graph[k][0] == vertex || graph[k][1] == vertex )
					{
						for (int m = k+1; m < num_edges; m++)
						{
							if( graph[m][0] == vertex || graph[m][1] == vertex )
							{
								for (int n = m+1; n < num_edges; n++)
								{
									if( graph[n][0] == vertex || graph[n][1] == vertex )
										steiner.add( s[i] >= ( y[j] + y[k] + y[m] + y[n] - 3 ) );
								}
							}
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < num_non_terminals; i++) {
		int vertex = non_terminals[i];
		int num_neighbors = 0;

		for (int e = 0; e < num_edges; e++) {
			if( graph[e][0] == vertex || graph[e][1] == vertex )
				num_neighbors++;
		}

		for (int n_3 = 0; n_3 < num_neighbors; n_3++) {
			for (int n_2 = n_3+1; n_2 < num_neighbors; n_2++) {
				for (int n_1 = n_2+1; n_1 < num_neighbors; n_1++) {
					int count = 0;
					IloExpr sum(env);
					for (int e = 0; e < num_edges; e++) {
						if( ( graph[e][0] == vertex || graph[e][1] == vertex ) )
						{
						 	if( count != n_1 && count != n_2 && count != n_3)
								sum += y[e];

							count++;
						}
					}
					steiner.add( s[i] <= sum );
				}
			}
		}
	}

	//limit on the number of steiner vertices of degree 2
	IloExpr sum_l(env);
	for (int i = 0; i < num_non_terminals; i++) {
		sum_l += p[i] - q[i];
	}
	steiner.add( sum_l <= l );

	//limit on the number of steiner vertices of degree greater than 3
	IloExpr sum_r(env);
	for (int i = 0; i < num_non_terminals; i++) {
		sum_r += s[i];
	}
	steiner.add( sum_r <= r );

	// Objective Function:
	IloExpr objective(env);
	for (int i = 0; i < num_edges; i++)
		objective += cost_edge[i] * y[i];

	steiner.add(IloMinimize(env, objective));

	// Get Solution:
	if ( cplex.solve() )
	{
		cout << "Custo da solução: " << cplex.getObjValue() << endl;
		cout << "Aresta pertencentes à solução - Custo da aresta" << endl;

		IloNumArray sol_y(env,  num_edges);
		cplex.getValues(sol_y, y);

		for (int i = 0; i < num_edges; i ++)
		{
			if( int(sol_y[i]) == 1 )
					printf("%d %d - %d\n", graph[i][0]+1, graph[i][1]+1, cost_edge[i]);
		}

	}

	env.end();
	return(0);
}

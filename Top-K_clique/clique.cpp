#include <stdio.h>
#include <ilcplex/ilocplex.h>
using namespace std;

int main()
{
	IloEnv env;
	IloModel clique(env, "Diversified Top-K Clique Problem");
	IloCplex cplex(clique);

	// Statement Data:
	int k, num_edges, num_vertices;
	scanf("%d", &k);
	scanf("%d %d", &num_vertices, &num_edges);

	int graph[num_vertices][num_vertices] = {};

	//Considering that input will present 1 as the initial vertex
	int vert1, vert2;
	for (int i = 0; i < num_edges; i ++)
	{
			scanf("%d %d", &vert1, &vert2);
			vert1--;
			vert2--;
			graph[vert1][vert2] = 1;
			graph[vert2][vert1] = 1;
	}

	// Decision Variables:

	// y[i] = 1, if the vertex i belongs to at least one of the maximal cliques
	// y[i] = 0, otherwise
	IloIntVarArray y(env, num_vertices, 0, 1);

	// y[i][j] = 1, if the vertex i belongs to the j maximal clique
	// y[i][j] = 0, otherwise
	IloArray<IloIntVarArray> x(env, num_vertices);
	for (int i = 0; i < num_vertices; i ++)
		x[i] = IloIntVarArray(env, k, 0, 1);

	// Restrictions:

	//Clique restriction to the k cliques
	for (int i = 0; i < num_vertices; i ++)
	{
		for (int l = (i+1); l < num_vertices; l ++)
		{
			if( graph[i][l] == 0 )
			{
				for (int j = 0; j < k; j++)
					clique.add( ( x[i][j] + x[l][j] ) <= 1);
			}
		}
	}

	//Cover restriction => y[i] must be 0 if x[i][j] = 0 for all j = 0, ..., k-1
	for (int i = 0; i < num_vertices; i++) {
		IloExpr sum(env);
		for (int j = 0; j < k; j++)
			sum += x[i][j];

		clique.add( y[i] <= sum );
	}

	// Objective Function:
	// Maximize the clique sizes (one independently of the others)
	IloExpr objective(env);
	for (int i = 0; i < num_vertices; i ++)
	{
		for (int j = 0; j < k; j++)
			objective += x[i][j];
	}

	// Maximize the graph coverage
	for (int i = 0; i < num_vertices; i++)
		objective += k * num_vertices * y[i];

	clique.add(IloMaximize(env, objective));

	// Get Solution:
	if ( cplex.solve() )
	{
		// Maximal coverage
		IloNumArray sol_y(env,  num_vertices);
		cplex.getValues(sol_y, y);

		int coverage = 0;
		for(int i = 0; i < num_vertices; i++)
			coverage += int( sol_y[i] );

		cout << "Cobertura do grafo: " << coverage << endl;

		//Maximal cliques
		printf("\n Cliques: \n\n");

		IloArray<IloNumArray> sol_x(env, num_vertices);
		for (int i = 0; i < num_vertices; i ++)
		{
			sol_x[i] = IloNumArray(env, k);
			cplex.getValues(sol_x[i], x[i]);
		}

		for (int j = 0; j < k; j++)
		{
			for (int i = 0; i < num_vertices; i++)
			{
				if( sol_x[i][j] == 1)
					printf("%d ", i+1);
			}
			printf("\n");
		}

	}

	env.end();
	return(0);
}

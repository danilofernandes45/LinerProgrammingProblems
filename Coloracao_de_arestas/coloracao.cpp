#include <stdio.h>
#include <ilcplex/ilocplex.h>
using namespace std;

int main()
{
	IloEnv env;
	IloModel color(env, "Problema da coloração de arestas com custo mínimo");
	IloCplex cplex(color);

	// Statement Data:
	int num_vertices, num_edges;
	scanf("%d %d", &num_vertices, &num_edges);
	int max_num_colors = num_edges;

	int graph[num_edges][2] = {};

	int vert1, vert2;
	for (int i = 0; i < num_edges; i ++)
	{
			scanf("%d %d", &vert1, &vert2);
			graph[i][0] = vert1;
			graph[i][1] = vert2;
	}

	// Decision Variables:

	// x[i][c] = 1, if the edge i has been colored with the color c
	// x[i][c] = 0, otherwise
	IloArray<IloIntVarArray> x(env, num_edges);
	for (int i = 0; i < num_edges; i ++)
		x[i] = IloIntVarArray(env, max_num_colors, 0, 1);

	// Restrictions:

	//Each edge must be colored with only one color
	for (int i = 0; i < num_edges; i ++)
	{
		IloExpr sum(env);
		for (int c = 0; c < max_num_colors; c++)
			sum += x[i][c];
		color.add(sum == 1);
	}

	//Adjacent edges must have different colors
	for (int i = 0; i < num_edges; i ++)
	{
		for (int j = (i+1); j < num_edges; j ++)
		{
			//If the edges i and j have a common vertex (That is, i and j are adjacent)
			if( graph[i][0] == graph[j][0] || graph[i][0] == graph[j][1] || graph[i][1] == graph[j][0] || graph[i][1] == graph[j][1])
			{
				for (int c = 0; c < max_num_colors; c++)
					color.add( (x[i][c] + x[j][c]) <= 1);
			}
		}
	}

	// Objective Function:
	IloExpr objective(env);
	for (int i = 0; i < num_edges; i++)
	{
		for (int c = 0; c < max_num_colors; c ++)
			objective += (c + 1) * x[i][c];
	}

	color.add(IloMinimize(env, objective));

	// Get Solution:
	if ( cplex.solve() )
	{
		cout << "Custo de colorir as arestas do grafo: " << cplex.getObjValue() << endl;
		cout << "Aresta - Cor (Custo)" << endl;

		IloArray<IloNumArray> solutions(env, num_edges);
		for (int i = 0; i < num_edges; i ++)
		{
			solutions[i] = IloNumArray(env, max_num_colors);
			cplex.getValues(solutions[i], x[i]);
			for (int c = 0; c < max_num_colors; c ++)
			{
				if( int(solutions[i][c]) == 1 )
				{
					printf("%d %d - %d\n", graph[i][0], graph[i][1], (c+1));
					break;
				}
			}
		}
	}

	env.end();
	return(0);
}

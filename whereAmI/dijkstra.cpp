#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include "graphQS.h"

using namespace std;
/*
int main(){
	FILE *input = fopen("input.txt", "r");
	FILE *output = fopen("hw5_prob1.txt", "w");
	FILE *sop = fopen("hw5_prob2.txt", "w");
	int chk = 0;
	int f, i, j = 0;
	int v = 0, w = 0;
	int V = 5;
	
	pGraph g = newGraph(V);
	GNode n;
	


	if (input == NULL)
		printf("file open failed!\n");

	while (chk != EOF){
		chk = 0;
		fscanf(input, "%d", &f);
		chk = fgetc(input);
		newGNode(f, NULL);
		if (chk == EOF) addEdgeFromTo(g, f, NULL, NULL);
		while (chk != '\n' && chk != EOF){
			fscanf(input, "%d,%d", &v, &w);
			chk = fgetc(input);
			addEdgeFromTo(g, f, v, w);
		}

	}
	
	fclose(input);
	printGraph(g);

	//S <- null
	//Q <- V[G]
	Dijkstra(g, 1);
	
	printGraph(g);

	fprintf(output, "In predecessor, 0 means NIL\n\n");
	fprintf(output, "vertex\tdistance\tpredecessor\n");
	for (i = 1; i <= g->V; i++)
		fprintf(output, "%d\t%d\t\t%d\n", i, g->distTo[i], g->compId[i]);

	fclose(output);

	PrintTable(g, sop);

	fclose(sop);
	system("pause");
	
	return 0;

}
*/
pGNode newGNode(int item, double weight) {
	pGNode node = (pGNode)malloc(sizeof(GNode));
	if (node == NULL) return NULL;
	node->item = item;
	node->weight = weight;
	node->next = NULL;
	return node;
}

// create a graph of V vertices
pGraph newGraph(int V) {
	pGraph g = (pGraph)malloc(sizeof(struct Graph));
	if (g == NULL) return NULL;
	g->V = V;
	g->Edge = 0;

	// create an array of adjacency list. size of array will be V
	g->adj = (pGNode)malloc(V * sizeof(GNode));
	if (g->adj == NULL) return NULL;

	// initialize each adjacency list as empty by making head as NULL;
	for (int i = 0; i < V; i++)
		g->adj[i].next = NULL;

	g->marked = NULL;	// allocate it when used for the first time
	g->edgeTo = NULL;	// allocate it when used for the first time
	g->compId = NULL;	// allocate it when used for the first time
	g->distTo = NULL;
	return g;
}

// return the number of vertices in the graph.
int V(pGraph g) { return g->V; }

// return the number of edges in the graph.
double Edge(pGraph g) { return g->Edge; }

// add an edge (from v to w) to an undirected graph
// A new node is added to the adjacency list of v. 
// The node is added at the beginning
void addEdgeFromTo(pGraph g, int v, int u, double w) {
	// instantiate a node with w and 
	// add it at the beginning of adj[v]
	pGNode node = newGNode(u, w);
	if (u != -1 && v != -1){
	node->next = g->adj[v].next;
	g->adj[v].next = node;
	g->Edge++;
	}
	//else node = NULL;
}

// add an edge to an undirected graph
void addEdge(pGraph g, int v, int u, double w) {
addEdgeFromTo(g, v, u, w);	// add an edge from v to w.
addEdgeFromTo(g, u, v, w);	// if graph is undirected, add both
}

// free graph data structure
void freeGraph(pGraph g) {
	pGNode curr, next;
	int i;
	if (g == NULL) return;
	if (g->adj == NULL) return;

	// free graph's adjacency list

	for (i = V(g); i >= 1; i--){
		curr = g->adj[i].next;
		while (curr != NULL){
			if (curr->item == NULL) {
				free(curr);
				break;
			}
			next = curr->next;
			free(curr);
			curr = next;
		}
		
	}

	free(g);
}

// print graph's contents 
void printGraph(pGraph g){

	// print the adjacency list of graph
	pGNode node;

	for (int i = 0; i < g->V; i++){
		printf("%d\n", g->V);
		printf("Adj.List V[%d]:", i);
		node = g->adj[i].next;
		while (node != NULL){	//  && node->item != NULL
			if (node->next == NULL){
				printf(" %d, %f", node->item, node->weight);
			}
			else
				printf(" %d, %f ->", node->item, node->weight);
			node = node->next;
		}
		printf("\n");


	}
	/*
	// print graph operation's reaults
	if (g->edgeFromTo) {
		printf(" edgeTo[0..%d] = ", V(g) - 1);
		for (int v = 1; v <= V(g); v++){
			for (int w = 1; w <= V(g); w++){
				printf("%d ", g->edgeFromTo[v][w]);
			}
			printf("\n");
		}
		printf("\n");
	}
	if (g->edgeTo) {
		printf(" edgeTo[0..%d] = ", V(g) - 1);
		for (int v = 1; v <= V(g); v++)
			printf("%d ", g->edgeTo[v]);
		printf("\n");
	}
	if (g->distTo) {
		printf(" distTo[0..%d] = ", V(g) - 1);
		for (int v = 1; v <= V(g); v++)
			printf("%d ", g->distTo[v]);
		printf("\n");
	}
	if (g->compId) {
		printf(" compId[0..%d] = ", V(g) - 1);
		for (int v = 1; v <= V(g); v++)
			printf("%d ", g->compId[v]);
		printf("\n");
	}
	*/
}

void Dijkstra(pGraph g, int u) {
	pQueue q = newQueue();
	pGNode node;
	int N = V(g);
	int i = 1, j = 1;

	if (g->marked == NULL) g->marked = (short *)malloc(N * sizeof(short));
	if (g->edgeTo == NULL) g->edgeTo = (int   *)malloc(N * sizeof(int));
	if (g->distTo == NULL) g->distTo = (double   *)malloc(N * sizeof(double));
	if (g->marked == NULL || g->distTo == NULL || g->edgeTo == NULL) return;	//
	if (g->compId == NULL) g->compId = (int   *)malloc(V(g) * sizeof(int));

	for (i = 1; i <= N; i++){
		for (j = 1; j <= N; j++){
			g->edgeFromTo[i][j] = 0;
		}
	}

	for (i = 0; i < N; i++) {
		g->marked[i] = FALSE;
		g->edgeTo[i] = -1;
		g->distTo[i] = 99.;
		enqueue(q, i);
		g->compId[i] = NULL;
		cout << g->distTo[i] << endl;
	}
	//g->distTo[1] = 0;	// 출발점을 0로. 나중엔 startIdx로 수정 필요
	cout << "     " << g->distTo[0] << endl;
	
	cout << g->distTo[0] << endl;
	printf("Dijkstra: "); printf("%d \n", u);

	
	while (q->front != NULL) {	// while Q != NULL
		u = extractMin(q, g);	// u = extract min
		printf("%d dequeue!\n", u);
		g->marked[u] = TRUE;
		pGNode w = g->adj[u].next;
		if (w != NULL){
			for (w; w; w = w->next) {
				if (!g->marked[w->item]) {	//false이면 (not makred)
					printf("from %d to %d\n", u, w->item);
					printf("before: %d\n", g->distTo[w->item]);
					relax(g, u, w->item, distTo(g, u, w->item));
					printf("after: %d, compid = %d\n", g->distTo[w->item], g->compId[w->item]);
					
				}
				printf("\n");
			}
		}
	}
	printf("\n");
	freeQueue(q);
}


void relax(pGraph g, int u, int v, double w)
{
	if (u == 1){
		if (g->distTo[v] > g->distTo[u] + w){
			g->distTo[v] = g->distTo[u] + w;
			g->compId[v] = u;
		}
		else;
	}
	else{
		if (g->distTo[v] > w){
			g->distTo[v] = w;
			g->compId[v] = u;
		}
		else;
	}

}

/*
void PrintTable(pGraph g, FILE *sop){
	int i, j;
	int k = 1;
	int m = 0;
	int Dist[MAX_VERTEX][MAX_VERTEX];
	int pre[MAX_VERTEX][MAX_VERTEX];

	for (i = 1; i <= g->V; i++){
		for (j = 1; j <= g->V; j++){
			Dist[i][j] = 99;
		}
	}

	for (i = 1; i <= g->V; i++){
		for (j = 1; j <= g->V; j++){
			pGNode w = g->adj[i].next;
			if (i == j)
				Dist[i][j] = 0;
			else if (w != NULL && w->item != NULL && w->item != j)
			{
				do{
					w = w->next;
					if (w == NULL) break;
					
					if (w->item == j){
						Dist[i][j] = min(g->distTo[w->item], w->weight);	// i와 j가 연결되어 있을 경우
						g->edgeFromTo[i][j] = g->compId[w->item];
					}
					else{	// i와 j가 연결되어 있지 않은 경우
						Dist[i][j] = 99;
						g->edgeFromTo[i][j] = 0;
						// i의 adj vertex중에 j가 있는지 확인	// w 대신 w->next?
					}
				} while (w->item != j);// && w->next != NULL

			}
			else if (w->item == j)
			{
				Dist[i][j] = g->distTo[w->item];
				if (i == 1) g->edgeFromTo[i][j] = g->compId[w->item];
				else g->edgeFromTo[i][j] = i;
			}
			else	// i와 j가 연결되어 있지 않은 경우
				Dist[i][j] = 99;
		}
	}
	/*
	fprintf(sop, "first'Distance' table\n");
	for (i = 1; i <= g->V; i++){
	for (j = 1; j <= g->V; j++){
	fprintf(sop, "%d ", Dist[i][j]);	//distTo(g, i, j));
	}
	fprintf(sop, "\n");
	}
	fprintf(sop, "\nfirst'predecessor' table\n");
	for (i = 1; i <= g->V; i++){
	for (int j = 1; j <= g->V; j++){
	fprintf(sop, "%d ", g->edgeFromTo[i][j]);
	}
	fprintf(sop, "\n");
	}
	
	for (k = 1; k <= g->V; k++){
		for (i = 1; i <= g->V; i++){
			for (j = 1; j <= g->V; j++){
				pGNode w = g->adj[i].next;
				pGNode y = g->adj[k].next;

				if (i == j)
					Dist[i][j] = 0;
				else if (i == k){
					while (y != NULL && y->item != NULL && y->item != j)
						y = y->next;
					if (y != NULL && y->item != NULL && y->item == j){
						m = min(g->distTo[y->item], y->weight);
						if (Dist[i][j] != m){
							Dist[i][j] = min(Dist[i][j], m);
							if (Dist[i][j] == m)
								g->edgeFromTo[i][j] = i;
						}

					}
				}
				else if (k == j){
					while (w != NULL && w->item != NULL && w->item != k){	// 일단 w->item = k일때까지 루프
						w = w->next;
					}
					if (w != NULL && w->item != NULL && w->item == k){
						m = min(g->distTo[w->item], w->weight);	// i와 j가 연결되어 있을 경우
						if (Dist[i][j] != m){
							Dist[i][j] = min(Dist[i][j], m);
							if (Dist[i][j] == m)
								g->edgeFromTo[i][j] = k;
						}
					}
				}


				//	else if (w != NULL && w->item != NULL && w->item != k && y != NULL && y->item != NULL && y->item != j)
				else {
					while (w != NULL && w->item != NULL && w->item != k){	// 일단 w->item = k일때까지 루프
						w = w->next;
					}
					if (w != NULL && w->item != NULL && w->item == k)
						m = min(g->distTo[w->item], w->weight);	// i와 j가 연결되어 있을 경우
					while (y != NULL && y->item != NULL && y->item != j)
						y = y->next;
					if (y != NULL && y->item != NULL && y->item == j){
						m = min(m + g->distTo[y->item], m + y->weight);
						if (Dist[i][j] != m){
							Dist[i][j] = min(Dist[i][j], m);
							if (Dist[i][j] == m)
								g->edgeFromTo[i][j] = k;
						}
					}
					else{
						m = 99;
						Dist[i][j] = min(Dist[i][j], m);
						if (Dist[i][j] == m)
							g->edgeFromTo[i][j] = 0;
					}
				}
				/*
				else	// i와 j가 연결되어 있지 않은 경우
				Dist[i][j] = 99;
				
			}
		}

	}

	fprintf(sop, "99 means INFINITY and 0 means NIL\n\n");
	fprintf(sop, "\nfinal 'Distance' table\n");
	for (i = 1; i <= g->V; i++){
		for (j = 1; j <= g->V; j++){
			fprintf(sop, "%d ", Dist[i][j]);
		}
		fprintf(sop, "\n");
	}


	fprintf(sop, "\nfinal 'predecessor' table\n");
	for (i = 1; i <= g->V; i++){
		for (int j = 1; j <= g->V; j++){
			fprintf(sop, "%d ", g->edgeFromTo[i][j]);
		}
		fprintf(sop, "\n");
	}

	freeGraph(g);
}
*/
//
// distTo() returns the number of edges in a shortest path between the source 
// vertex s and vertex v
// vertex s에서 v로 가는 distance
int distTo(pGraph g, int s, int v) {
	//if (g->distTo == NULL) Dijkstra(g, s);
	int d = 0;

	pGNode t = &g->adj[s];
	if (s == v) return d;
	else{
		while (t->item != NULL && t->item != v && t->next != NULL)
		{
			t = t->next;
		}
	}

	d = t->weight + g->distTo[s];

	return d;
	//return g->distTo[v];
}
//////////////////// Graph.cpp ///////////////////////////////////

///////////// helper function for Queue and Stack ///////////////
void freeNode(pNode curr) {
	pNode next;
	while (curr != NULL){
		next = curr->next;
		free(curr);
		curr = next;
	}
}

/////////////////////////// Queue ///////////////////////////////
pQueue newQueue() {
	pQueue q = (pQueue)malloc(sizeof(Queue));
	q->front = q->rear = NULL;
	q->size = 0;
	return q;
}

int sizeQueue(pQueue q) {
	return q->size;
}

void enqueue(pQueue q, Key key) {
	pNode t = (pNode)malloc(sizeof(Node));
	if (t == NULL) return;

	t->key = key;
	t->next = NULL;
	if (q->front == NULL)
		q->front = t;
	else
		q->rear->next = t;
	q->rear = t;
	q->size++;
}

int extractMin(pQueue q, pGraph g) {
	pNode t = q->front;
	pNode f;
	pNode tmp;
	int i = 0, newmin;
	double min;
	int u = t->key;
	int flag = 0;
	if (t == NULL) return NULL;


	min = g->distTo[u];
	newmin = g->distTo[u];
	for (t; t; t = t->next){
		f = q->front;
		if (!g->marked[t->key])
		{
			if (g->distTo[t->key] < min){
				newmin = g->distTo[t->key];
				u = t->key;
				tmp = t;
				swap(f, tmp);
				flag = 1;
			}
			else{
				min = min;
				u = u;
			}
		}
	}

	if (flag = 1 && min == newmin)
		q->front = q->front->next;

	free(t);
	q->size--;
	return u;
}

void swap(pNode f, pNode t){
	pNode tmp;

	tmp = t;
	f->next = tmp->next;
	t->next = f;
}

void freeQueue(pQueue q) {
	if (q == NULL) return;
	if (q->size == 0) return free(q);
	freeNode(q->front);
	free(q);
}

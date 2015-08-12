#include <stdio.h>
#include <stdlib.h>
#include "dijkstraHeader.h"


pq newPQ(int capacity) {
	pq p = (pq)malloc(sizeof(PQ));

	p->N = 0;
	p->capacity = capacity < 2 ? 2 : capacity;
	p->node = (char *)malloc(sizeof(char)* p->capacity);
	return p;
}

void minSwim(pq p, int k) {
	while (k > 1 && !less(p, k / 2, k)) {
		swap(p, k / 2, k);
		k = k / 2;
	}
}

void minSink(pq p, int k) {
	while (2 * k <= p->N) {
		int j = 2 * k;
		if (j < p->N && !less(p, j, j + 1)) j++;
		if (less(p, k, j)) break;
		swap(p, k, j);
		k = j;
	}
}

int resize(pq p, int newSize) {
	int memorySize = sizeof(char)* newSize;

	p->node = (char*)realloc(p->node, memorySize);
	p->capacity = newSize;
	if (p->node == NULL) { exit; }
	return 0;
}

int insert(pq p, char key){
	if (p->N == p->capacity - 1) resize(p, p->capacity * 2);

	p->node[++p->N] = key;
	minSwim(p, p->N);
	return key;
}

int deleting(pq p) {
	if (p->N == 0) return NULL;

	int max = p->node[1];
	swap(p, 1, p->N--);
	minSink(p, 1);

	return max;
}

pQueue newQueue() {
	pQueue q = (pQueue)malloc(sizeof(Queue));
	q->front = q->rear = NULL;
	q->size = 0;
	return q;
}

void enqueue(pQueue q, int key) {
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

pGNode newGNode(int item, double weight) {
	pGNode node = (pGNode)malloc(sizeof(GNode));
	if (node == NULL) return NULL;
	node->item = item;
	node->weight = weight;
	node->next = NULL;
	return node;
}

pGraph newGraph(int V) {
	pGraph g = (pGraph)malloc(sizeof(struct Graph));
	if (g == NULL) return NULL;
	g->V = V;
	g->Edge = 0;
	g->adj = (pGNode)malloc(V * sizeof(GNode));

	if (g->adj == NULL) return NULL;

	for (int i = 0; i < V; i++)
		g->adj[i].next = NULL;

	for (int i = 1; i < V; i++) {
		for (int j = 1; j < V; j++) {
			g->fd[i][j] = 99;
		}
	}
	g->S = NULL;
	g->p = NULL;
	g->d = NULL;
	return g;
}

void addEdge(pGraph g, int v, int k, double w) {
	pGNode node = newGNode(k, w);
	node->next = g->adj[v].next;
	g->adj[v].next = node;
	g->Edge++;
}

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

void printGraph(pGraph g){

	pGNode node;

	for (int i = 0; i < g->V; i++){
		printf("Adj.List V[%d]:", i);
		node = g->adj[i].next;

		while (node != NULL){	//  && node->item != NULL
			if (node->next == NULL){
				printf(" %d, %f", node->item, node->weight);
			}
			else
				printf(" %d,%f ->", node->item, node->weight);
			node = node->next;
		}
		printf("\n");
	}
}

//implement
void relax(pGraph G, int u, int v, double w){
	if (G->d[v] > G->d[u] + w){
		G->d[v] = G->d[u] + w;
		G->p[v] = u;
	}
}

//using priority queue ->loop
double dijkstra(pGraph G, pq Q, pq S, int s, int endIdx) {
	int u = 0;

	if (G->d == NULL) G->d = (double *)malloc(G->V * sizeof(double));
	if (G->p == NULL) G->p = (int *)malloc(G->V * sizeof(int));
	if (G->d == NULL || G->p == NULL) return 0;

	for (int i = 0; i < G->V; i++) {
		G->d[i] = 99.;
		G->p[i] = i;
	}

	G->d[s] = 0.;
	G->p[s] = 0;

	while (S->N != 0) {
		u = deleting(S);
	}
	u = 0;

	while (Q->N != 0){
		u = deleting(Q);
		insert(S, u);
		for (int i = 0; i < G->V; i++){
			if (G->adj[i].next != NULL){
				pGNode node = G->adj[i].next;
				for (node = G->adj[i].next; node->next != NULL; node = node->next){
					relax(G, i, node->item, node->weight);
				}
				relax(G, i, node->item, node->weight);
			}
		}
	}
	FILE *f1 = fopen("hw5_prob1.txt", "w");

	fprintf(f1, "vertex\tdistance\tproducessor\n");
	for (int i = 0; i < G->V; i++){
		fprintf(f1, "%d\t%f\t\t%d\n", i, G->d[i], G->p[i]);
		printf("%d\t%f\t\t%d\n", i, G->d[i], G->p[i]);
	}
	printf("Complete hw5_prob1.txt!\n\n");
	fclose(f1);
	
	return G->d[endIdx];
	//return S;
}

pGraph makeGraph(pGraph g){
	int fread = 0;
	int v;
	int k;
	int w;

	FILE *fin = fopen("input.txt", "r");
	if (fin == NULL){ printf("file open error!\n"); }

	while (fread != EOF){
		fread = 0;
		fscanf(fin, "%d", &v);
		fread = fgetc(fin);
		newGNode(v, NULL);

		if (fread == EOF) addEdge(g, v, NULL, NULL);

		while (fread != '\n' && fread != EOF) {
			fscanf(fin, "%d,%d", &k, &w);
			fread = fgetc(fin);
			addEdge(g, v, k, w);
		}
	}
	printf("Complete file open!\n\n");
	fclose(fin);

	return g;
}

int less(pq p, int i, int j) {
	return p->node[i] < p->node[j] ? 1 : 0;
}

void swap(pq p, int i, int j) {
	char t = p->node[i];
	p->node[i] = p->node[j];
	p->node[j] = t;
}

void swim(pq p, int k) {
	while (k > 1 && less(p, k / 2, k)) {
		swap(p, k / 2, k);
		k = k / 2;
	}
}

void sink(pq p, int k) {
	while (2 * k <= p->N) {
		int j = 2 * k;
		if (j < p->N && less(p, j, j + 1)) j++;
		if (!less(p, k, j)) break;
		swap(p, k, j);
		k = j;
	}
}
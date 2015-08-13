
//
// graphQS.h - a simple queue and stack to support Graph.cpp
// 
// 2014/05/01	YSK	Creation
//

#ifndef graphQS_h

#define MAX_VERTEX 10

#define graphQS_h
#define TRUE 1
#define FALSE 0


typedef int Key;				// for flexibility

typedef struct Node *pNode;		// a generic node for queue and stack
typedef struct Node {
	Key		key;
	pNode	next;
	pNode	before;
} Node;

typedef struct Queue *pQueue;
typedef struct Queue {
	pNode front;
	pNode rear;
	int size;
} Queue;

typedef struct GNode *pGNode;
typedef struct GNode {
	int item;
	double weight;			// for weighted graph - network
	pGNode next;
} GNode;

typedef struct Graph *pGraph;
typedef struct Graph {
	int V;				// number of vertices in the graph
	int Edge;				// number of edges in the graph
	double W;
	pGNode adj;			// an array of adjacency lists 

	// the following DS's are allocated when used for the first time
	// These are set by depthFirstSearch(), 
	// breadthFirstSearch(), and connectedComponent()
	short *marked;		// marked[v] = is there an s->v path?
	int   *edgeTo;		// edgeTo[v] = last edge on shortest s->v path
	int edgeFromTo[MAX_VERTEX][MAX_VERTEX];
	double	  *distTo;		// distTo[v] = length of shortest s->v path
	int   *compId;		// Connected Component id
}Graph;



void relax(pGraph g, int u, int v, double w);

pGNode newGNode(int item, double weight);				// create a new adjacency list node
pGraph newGraph(int V);					// create a graph of V vertices
int V(pGraph g);						// return the number of vertices in graph.
double Edge(pGraph g);						// return the number of edges in graph.
void addEdgeFromTo(pGraph g, int v, int u, double w);	// add an edge to an undirected graph
void addEdge(pGraph g, int v, int u, double w);	// add two edges, v-w & w-v, to the graph
void freeGraph(pGraph g);				// free graph
void printGraph(pGraph g);				// print the adjacency list of graph

double distTo(pGraph g, int s, int v);		// the number of edges in a shortest path from s to v

void Dijkstra(pGraph g, int u, int startIdx);
void PrintTable(pGraph g, FILE *sop);

// define the function prototypes for queue and stack
void	freeNode(pNode curr);			// helper function to free generic nodes

// Queue operations
pQueue	newQueue();
int		sizeQueue(pQueue q);

void	enqueue(pQueue q, Key key);
int dequeue(pQueue q);
int extractMin(pQueue q, pGraph g);
void swap(pNode f, pNode t);

void	freeQueue(pQueue q);

#endif graph_h


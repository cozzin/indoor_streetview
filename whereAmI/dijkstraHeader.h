#define TRUE 1 
#define FALSE 0
#define max_v 99

typedef struct Node *pNode;
typedef struct Node {
	int      key;
	pNode   next;
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
	double weight;         // for weighted graph - network
	pGNode next;
} GNode;

typedef struct Graph *pGraph;
typedef struct Graph {
	int V;
	double Edge;
	pGNode adj;
	short *S;
	double     *d;
	int   *p;
	int fd[max_v][max_v];
	//int count[max][max];

}Graph;

typedef struct PQ *pq;
typedef struct PQ {
	char *node;
	int capacity;
	int N;
} PQ;

void addEdgeFromTo(pGraph g, int v, int u, double w);
int less(pq p, int i, int j);
void swap(pq p, int i, int j);
void swim(pq p, int k);
void sink(pq p, int k);
int deleting(pq p);
pGraph makeGraph(pGraph g);
pq newPQ(int capacity);
void minSwim(pq p, int k);
void minSink(pq p, int k);
int resize(pq p, int newSize);
int insert(pq p, char key);
int deleting(pq p);
pQueue newQueue();
void enqueue(pQueue q, int key);
pGNode newGNode(int item, double weight);
pGraph newGraph(int V);
void addEdge(pGraph g, int v, int k, double w);
void printGraph(pGraph g);
void relax(pGraph G, int u, int v, double w);
double dijkstra(pGraph G, pq Q, pq S, int s, int endIdx, int* path);
pGraph makeGraph(pGraph g);

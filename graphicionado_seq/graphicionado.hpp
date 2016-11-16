// Graphicionado data types etc.
#ifndef graphicionado_hpp
#define graphicionado_hpp graphicionado_hpp

struct Graph; 
struct VertexProperty;
struct Vertex;
struct Edge;

/* -----------------------------------------------
--- Graphicionado global variables ---------------
----------------------------------------------- */

extern Vertex* vertices; // All vertices in the graph
extern Vertex* activeVertex; 

extern Edge* edges; // All edges in the graph
extern unsigned int* edgeIDTable;

extern VertexProperty* vProperty;
extern VertexProperty* vTempProperty;
extern VertexProperty* vConst; // TODO: Implement this at a later time if its ever is needed. It is a constant vertex property array VConst associated with all vertices. Have not seen it been used anywhere yet.
extern unsigned int totalVertexCount; 
extern unsigned int activeVertexCount; 

extern std::string graphAlgorithm; // Algorithm used e.g. BFS. This is set by e.g. bfs.cpp file or the algorithm file used during compilation.

/* -----------------------------------------------
--- Graphicionado structs ------------------------
----------------------------------------------- */

// A struct representing a graph
struct Graph {
	void* vertexList;	// A list of all vertices
	void* edgeList;		// A list of all edges
};

// A struct representing vertex property
struct VertexProperty {
	double property;
};

// A struct representing a vertex in the graph
struct Vertex {
	unsigned int ID;
	VertexProperty prop;
};

// A struct representing an edge in the graph
struct Edge {
	unsigned int srcID;
	unsigned int dstID;
	double weight;
};

/* -----------------------------------------------
--- Graphicionado functions ----------------------
----------------------------------------------- */

// User defined functions
VertexProperty processEdge(double weight, VertexProperty srcProp, VertexProperty dstProp);
VertexProperty reduce(VertexProperty temp, VertexProperty result);
VertexProperty apply(VertexProperty vprop, VertexProperty temp, VertexProperty vconst);

#endif

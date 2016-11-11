// Graphicionado data types etc.
#ifndef graphicionado_hpp
#define graphicionado_hpp graphicionado_hpp

/* -----------------------------------------------
--- Graphicionado functions ----------------------
----------------------------------------------- */



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
--- Graphicionado global variables ---------------
----------------------------------------------- */

Vertex* vertices; // All verticies in the graph
Vertex* activeVertex; 

Edge* edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty* vProperty;
VertexProperty* vTempProperty;
VertexProperty* vConst;


#endif

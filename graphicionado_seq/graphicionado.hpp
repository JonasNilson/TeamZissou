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

#endif

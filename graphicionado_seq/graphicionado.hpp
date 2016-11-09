// Graphicionado data types etc.

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
}

// A struct representing a vertex in the graph
struct Vertex {
	unsigned int id;
	VertexProperty prop;
};

// A struct representing an edge in the graph
struct Edge {
	unsigned int srcId;
	unsigned int dstId;
	unsigned int weight;
};

// A struct representing vertex property
struct VertexProperty {
	double property;
};
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
	int id;
	void* prop;
};

// A struct representing an edge in the graph
struct Edge {
	int srcId;
	int dstId;
	int weight;
};

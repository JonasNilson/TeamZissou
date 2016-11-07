// Graphicionado data types etc.


// A struct representing a vertex in the graph
struct Vertex {
	int Id;
	double VertexProperty;
};

// A struct representing an edge in the graph
struct Edge {
	int SrcId;
	int DstId;
	int Weight;
};

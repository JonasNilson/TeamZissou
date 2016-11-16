#ifndef test_functions_hpp
#define test_functions_hpp test_functions_hpp

#include "graphicionado.hpp"

void printEdges(unsigned int numEdges, Edge* edges);
void printVertices(unsigned int numVertices, Vertex* vertices);
void printEdgeIDTable(unsigned int numVertices, unsigned int* edgeIDTable, Vertex* vertices);
void printVerticesProperties(unsigned int numVertices, Vertex* vertices, VertexProperty* verticeProperties);

#endif

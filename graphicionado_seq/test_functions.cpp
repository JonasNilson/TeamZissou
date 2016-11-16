#include "test_functions.hpp"
#include <iostream>

void printEdges(unsigned int numEdges, Edge* edges){
  std::cout << "##########EDGES###########" << std::endl;
  for(unsigned int e=0; e<numEdges;e++){
    std::cout << edges[e].srcID << "->" << edges[e].dstID << std::endl;
  }
}

void printVertices(unsigned int numVertices, Vertex* vertices){
  std::cout << "##########VERTICES###########" << std::endl;
  for(unsigned int v=0; v<numVertices;v++){
    std::cout << vertices[v].ID << std::endl;
  }
}

void printEdgeIDTable(unsigned int numVertices,unsigned int *edgeIDTable, Vertex* vertices){
  std::cout << "##########EdgeIDTable########" << std::endl;
  for(unsigned int eit = 0; eit<numVertices; eit++)
    {
      std::cout << vertices[eit].ID << ": " << 
	edgeIDTable[vertices[eit].ID] << std::endl; 
    }
}

void printVerticesProperties(unsigned int numVertices, Vertex* vertices, VertexProperty* verticeProperties){
  std::cout << "##########VERTICE PROPERTIES###########" << std::endl;
  for(unsigned int v=0; v<numVertices;v++){
    std::cout << vertices[v].ID << ": " << verticeProperties[v].property << std::endl;
  }
}


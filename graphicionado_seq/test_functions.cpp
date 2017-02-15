#include "test_functions.hpp"
#include <iostream>
#include <fstream>

void printEdges(unsigned int numEdges, Edge* edges){
  std::cout << "##########EDGES###########" << std::endl;
  for(unsigned int e=0; e<numEdges;e++){
    std::cout << edges[e].srcID << "->" << edges[e].dstID << " : "<< edges[e].weight<< std::endl;
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

/*
 * Write the one dimensional arrays containing vertex id's and 
 * their corresponding properties to a file.
 */
void writeVerticesProperties(unsigned int numVertices, Vertex* vertices, VertexProperty* verticesProperties) {	
	std::cout << "######## Writing the vProperty array to file ###########" << std::endl;
	
	// Create a file stream
	std::ofstream filestream;
	filestream.open("result_serial.txt");
	
	// Write the vector pair to file
	for(unsigned int i = 0; i < numVertices; ++i) {
		filestream << vertices[i].ID << ":" << verticesProperties[i].property << std::endl;
	}

	// Close the file
	filestream.close();
}

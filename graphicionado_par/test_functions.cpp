#include "test_functions.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

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

/* 
 * Writes the two dimensional arrays containing vertex id's and 
 * their corresponding properties to a file.
 */
void writeTwoDimensionalVerticesProperties(unsigned int numberOfNodes, unsigned int* numVertices, Vertex** vertices, VertexProperty** verticeProperties) {
	std::cout << "######## Writing the vProperty array to file ###########" << std::endl;
	
	// Count the total number of vertices
    int totalNumberOfVertices = 0;
    for(unsigned int i = 0; i < numberOfNodes; ++i) {
		totalNumberOfVertices += numVertices[i];
	}
	
	// Create a vector containing pairs of vertex ID and vertex property
	std::vector<std::pair<unsigned int, unsigned int>> order(totalNumberOfVertices);
	int k = 0;
	for (unsigned int i = 0; i < numberOfNodes; ++i){
		for(unsigned int j = 0; j < numVertices[i]; ++j) {
			// Save the vertex id and vertex property in the vector as a pair
			order[k++] = std::make_pair(vertices[i][j].ID, verticeProperties[i][j].property);
		}
	}

	// Sort the vector based on the vertex ID
	std::sort(order.begin(), order.end(), 
			  [](std::pair<unsigned int, unsigned int> a, 
				 std::pair<unsigned int, unsigned int> b) {
				  return a.first < b.first; // Compare the first element of the pairs
			  });
	
	// Create a file stream
	std::ofstream filestream;
	filestream.open("result_parallel.txt");
	
	// Write the vector pair to file
	for(unsigned int i = 0; i < order.size(); ++i) {
		filestream << order[i].first << ":" << order[i].second << std::endl;
	}

	// Close the file
	filestream.close();
}

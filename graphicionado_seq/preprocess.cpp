#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include <fstream> // Used for file reading
#include <array>
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include <algorithm>

Vertex* vertices; // All verticies in the graph
Vertex* activeVertex; 

Edge* edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty* vProperty;
VertexProperty* vTempProperty;
VertexProperty* vConst;

/*
 * Collective allocations for the argoDSM system.
 * numVertices: total number of vertices in graph
 * numEdges: total number of edges in graph
 */
void setupDSM(unsigned int numVertices, unsigned int numEdges){
	vertices = argo::conew_array<Vertex>(numVertices); 
	activeVertex = argo::conew_array<Vertex>(numVertices); 
	
	edges = argo::conew_array<Edge>(numEdges); 
    edgeIDTable = argo::conew_array<unsigned int>(numVertices); // make it of size number of vertices

	vProperty = argo::conew_array<VertexProperty>(numVertices);
	vTempProperty = argo::conew_array<VertexProperty>(numVertices);
	vConst = argo::conew_array<VertexProperty>(numVertices);
}


/*
 * Compare edges with srcID then dstID
 */
bool edgeIDCompare(Edge e1, Edge e2){
	// Sort by srcID
	if(e1.srcID < e2.srcID || (e1.srcID == e2.srcID && e1.dstID < e2.dstID)){
		return true;
	}
	return false;
}

/*
 * Compare vertex with ID
 */
bool vertexIDCompare(Vertex v1, Vertex v2){
	if(v1.ID < v2.ID){
		return true;
	}
	return false;
}

/*
 * Organize already collective allocated space in the argoDSM system 
 * with data like graphicionado's model.
 * numVertices: total number of vertices in graph
 * numEdges: total number of edges in graph
 */
void initializeDSM(unsigned int numVertices, unsigned int numEdges){
	// Sort edges after srcID and then dstID with function edgeCompare
	std::sort(edges,&edges[numEdges],edgeIDCompare); // if this one does not work use std::sort(edges,edges+numEdges,edgeCompare);

	// Sort vertices after ID to make sure the edge ID table correspond to correct node.
	std::sort(vertices,&vertices[numVertices],vertexIDCompare);

  	// TODO: Init ActiveVertices
	// root node? where we start with. Maybe an argument what we take in

  	// Init EdgeIDTable
	unsigned int j = 0;
	for(unsigned int i = 0; i < numVertices; ++i){
		unsigned int vertex_ID = vertices[vertex_index].ID;	
		for(j; j < numEdges; ++j){
			if(vertex_ID <= edges[j].srcID){
				break;
			}
		}
		edgeIDTable[i] = j; 
	}


  	// Init VProperty
	for(unsigned int i =0; i < numVertices; ++i) {
    	vProperty[i] = vertices[i].prop;
  	}

  	// TODO: Init VTempProperty
  	//WARNING might not be needed to initialized 
  	for(unsigned int i =0; i < numVertices; ++i) {
      	vProperty[i] = vertices[i].prop;
   	}

  	// TODO: Init VConst
   	// Get more info what this is? Do we get this from the data?
}


/*
 * Read graph input data from a text file.
 */
void readTextFile(char * filename){
	// Local variable declaration
	std::ifstream file;
	std::string line;
	unsigned int numVertices;
	unsigned int numEdges;

	file.open(filename); // Open file with filename.
	getline(file,line); // get first line in file.
	numVertices = std::stoll(line);
	getline(file,line); // get second line in file.
	numEdges = std::stoll(line);

	setupDSM(numVertices,numEdges); // Make system ready to store data.

	for(unsigned int i = 0; i < numVertices; ++i){
		getline(file,line); // Saves the line in line.
		vertices[i].ID = std::stoll(line);
		getline(file,line);
		vertices[i].prop.property = std::stold(line);
	}

	for(unsigned int i = 0; i < numEdges; ++i){
		getline(file,line); // Saves the line in line.
		edges[i].srcID = std::stoll(line);
		getline(file,line);
		edges[i].dstID = std::stoll(line);
		getline(file,line);
		edges[i].weight = std::stold(line);
	}

	file.close(); // Closes file 
	initializeDSM(numVertices, numEdges); // Organize data in argo.
}


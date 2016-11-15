#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include <fstream> // Used for file reading
#include <array>
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"
#include <algorithm>
#include <sstream>
#include "test_functions.hpp"

Vertex* vertices; // All vertices in the graph
Vertex* activeVertex; 

Edge* edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty* vProperty; // property of nodes.
VertexProperty* vTempProperty; // new vProperty that been changed
VertexProperty* vConst; 

unsigned int totalVertexCount; // Number of nodes in the system.
unsigned int activeVertexCount; // Number of active nodes in the system.

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
 * Compare unsigned int with each other.
 */
bool unsignedIntCompare(unsigned int u1, unsigned int u2){
	if(u1 < u2){
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

  	// Init ActiveVertices
	if(isAllVerticesActive){ //If settings are set to use all vertices as active
		for(unsigned int i =0; i < numVertices; ++i) {
	    	activeVertex[i] = vertices[i];
	  	}
	  	activeVertexCount = numVertices; //Set that it exist this many active vertices
	}
	else{
		unsigned int activeNodesLength = 
		activeVertexCount = sizeof(startingNodes)/sizeof(startingNodes[0]); // Get number of elements of startingNodes and set number of starting active vertices
		std::cout << " THIS IS THE NUMBER OF ACTIVE VERTICES: " << activeVertexCount;
		std::sort(startingNodes,&startingNodes[activeNodesLength],unsignedIntCompare); // Sort it by ID
		// initialize the starting active vertices
		for(unsigned int i = 0; i < activeVertexCount; ++i) {
	    	activeVertex[i] = vertices[startingNodes[i]]; // Set active Vertex from startingNodes that hold ID of what vertices.
	  	}
	}

  	// Init EdgeIDTable
	setupEIT(numVertices, numEdges, vertices, edgeIDTable, edges);
	
	printVertices(numVertices, vertices);
	printEdges(numEdges, edges);
	printEdgeIDTable(numVertices, edgeIDTable, vertices);

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
   	// Get more info what this is? Do we get this from the data? Is this even needed? For now it does not looks like it.
}


/*
 * Read graph input data from a text file.
 */
void readTextFile(const char* filename){
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

	totalVertexCount = numVertices;
	initializeDSM(numVertices, numEdges); // Organize data in argo.
}

void readTextFileWithLineParsing(const char* filename) {
	// Local variable declaration
	std::ifstream file;
	std::string line;
	std::string item;
	char delimiter = ',';
	unsigned int numVertices;
	unsigned int numEdges;

	file.open(filename);

	// Read the number of vertices 
	getline(file, line);
	numVertices = std::stoll(line);
	std::cout << "numVertices: \t" << numVertices << std::endl;

	// Read the number of edges
	getline(file, line);
	numEdges = std::stoll(line);
	std::cout << "numEdges: \t" << numEdges << std::endl;

	setupDSM(numVertices,numEdges); // Make system ready to store data.

	// Read all the vertices
	for(unsigned int i = 0; i < numVertices; ++i){
		getline(file,line); // Saves the line in line.
		std::stringstream ss; // Create a new string stream
		ss.str(line); // Stream the current line
		std::getline(ss, item, delimiter); // Read the vertex ID
	    vertices[i].ID = std::stoll(item);
		std::getline(ss, item, delimiter); // Read the vertex property
	    vertices[i].prop.property = std::stold(item);
	}

	// Read all the edges
	for(unsigned int i = 0; i < numEdges; ++i){
		getline(file,line); // Saves the line in line.
		std::stringstream ss; // Create a new string stream
		ss.str(line); // Stream the current line
		std::getline(ss, item, delimiter); // Read the edge srcID
	    edges[i].srcID = std::stoll(line);
		std::getline(ss, item, delimiter); // Read the edge dstID
	    edges[i].dstID = std::stoll(line);
		std::getline(ss, item, delimiter); // Read the edge weight
	    edges[i].weight = std::stold(line);
	}

	// Close the file
	file.close();

	totalVertexCount = numVertices;
	initializeDSM(numVertices, numEdges); // Organize data in argo.
}

void setupEIT(unsigned int numVertices, unsigned int numEdges, Vertex* vertices, unsigned int* edgeIDTable, Edge* edges){
  
  unsigned int oldIndex = edges[0].srcID;
  unsigned int newIndex = oldIndex;
  edgeIDTable[oldIndex] = 1;
  for(unsigned int i = 1; i <numEdges; i++)
    {
      oldIndex = newIndex;
      newIndex = edges[i].srcID;
      if(newIndex != oldIndex)
	{
	  edgeIDTable[newIndex] = i+1;
	}
    }
	/*unsigned int edgeIndex = 0;
  
  for(unsigned int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++){
    unsigned int vertex_ID = vertices[vertexIndex].ID;	
    
    for(; edgeIndex < numEdges; edgeIndex++){
      unsigned int edgeSource = edges[edgeIndex].srcID;
      if(vertex_ID <= edgeSource){
	break;
      }
    }
    
    edgeIDTable[vertex_ID] = edgeIndex; 
    }*/
}

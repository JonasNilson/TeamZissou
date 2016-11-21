#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include <fstream> // Used for file reading
#include <array>
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include "test_functions.hpp"
#include <math.h>
#include <limits>

//Init value on starting nodes for different algorithms used.
void initAlgorithmProperty() {
	if(graphAlgorithm == "BFS"){ //Check if BFS is used
		// Init all vertex properties for BFS
		for(unsigned int i=0; i < totalVertexCount; ++i){
			// Property can't be larger than totalVertexCount
			vertices[i].prop.property = totalVertexCount;
		}

		//Set their property to 0.
		for(unsigned int i = 0; i < activeVertexCount; ++i){
			for(unsigned int j = 0; j < totalVertexCount; ++j) {
				if(startingNodes[i] == vertices[j].ID) { // startingNodes are ID's of active nodes
					vertices[j].prop.property = 0;
				}
			}
		}
	}

	if(graphAlgorithm == "SSSP"){ //Check if SSSP is used
		// Init all vertex properties for SSSP
		for(unsigned int i=0; i < totalVertexCount; ++i){
			// Each vertex property is set to the maximum allowed double value
			vertices[i].prop.property = std::numeric_limits<double>::max();
		}

		//Set their property to 0.
	    for(unsigned int i = 0; i < activeVertexCount; ++i){
		    for(unsigned int j = 0; j < totalVertexCount; ++j) {
				if(startingNodes[i] == vertices[j].ID) { // startingNodes are ID's of active nodes
					// std::cout << "PREPROCESS: initAlgorithmProperty: vertex index: " << j << std::endl;
					vertices[j].prop.property = 0;
				}
			}
		}
	}

	if(graphAlgorithm == "PR"){ //Check if Page rank is used
		//Const array gonna be used to keep degrees. Degree is number of edges away.
		for(unsigned int i = 0; i < totalVertexCount; ++i){
			vertices[i].prop.property = 0.3; // From GraphMat
			
			if(edgeIDTable[i] == 0){
				//Does not have edges
				vConst[i].property = 0;
				continue;
			}
			unsigned int counter = 0; // Counter to keep track of how many edges
			unsigned int startPos = edgeIDTable[i] - 1; // StartPos in edges where it start
			for(unsigned int j = startPos; j < totalEdgeCount; ++j){
				if(edges[j].srcID == vertices[i].ID){
					counter++;
				} 
				else {
					//No more edges to count
					break;
				}
			}
			vConst[i].property = counter;
		}
	}
}



/*
 * Collective allocations for the argoDSM system.
 * numVertices: total number of vertices in graph
 * numEdges: total number of edges in graph
 */
void setupDSM(unsigned int numVertices, unsigned int numEdges){
	std::cout << "setupDSM: Setup of the Argo DSM..." << std::endl;
	vertices = argo::conew_array<Vertex>(numVertices); 
	activeVertex = argo::conew_array<Vertex>(numVertices);
	
	edges = argo::conew_array<Edge>(numEdges); 
    edgeIDTable = argo::conew_array<unsigned int>(numVertices); // make it of size number of vertices

	vProperty = argo::conew_array<VertexProperty>(numVertices);
	vTempProperty = argo::conew_array<VertexProperty>(numVertices);
	vConst = argo::conew_array<VertexProperty>(numVertices);
	std::cout << "setupDSM: Setup was successfull" << std::endl;
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
	std::cout << "initializeDSM: preprocessing data..." << std::endl;
	// Sort edges after srcID and then dstID with function edgeCompare
	std::sort(edges,&edges[numEdges],edgeIDCompare);
	// Sort vertices after ID to make sure the edge ID table correspond to correct node.
	std::sort(vertices,&vertices[numVertices],vertexIDCompare);
  	
  	// Init totalVertexCount & totalEdgeCount
	totalVertexCount = numVertices;
	totalEdgeCount = numEdges;

	// Init activeVertexCount
	if(isAllVerticesActive){ //If settings are set to use all vertices as active
	  	activeVertexCount = numVertices; //Set that it exist this many active vertices
	}
	else{ // Setting case where a subset of vertices are active.
		activeVertexCount = numberOfStartingNodes;	
	}


  	// Init EdgeIDTable
	setupEIT(numVertices, numEdges, vertices, edgeIDTable, edges);

	
    // Init starting nodes depending on algorithm used.
	initAlgorithmProperty();


	// Init ActiveVertices
	if(isAllVerticesActive){ //If settings are set to use all vertices as active
		for(unsigned int i =0; i < numVertices; ++i) {
	    	activeVertex[i] = vertices[i];
	  	}
	}
	else{ // Setting case where it is given active starting nodes
		std::sort(startingNodes,&startingNodes[activeVertexCount],unsignedIntCompare); // Sort it by ID
		// Initialize the starting active vertices
		for(unsigned int i = 0; i < activeVertexCount; ++i) {
	    	activeVertex[i] = vertices[startingNodes[i]]; // Set active Vertex from startingNodes that hold ID of what vertices.	
	  	}
	
		// Free local node memory usage for init active vertices
		delete[] startingNodes;
	}

	//printVertices(numVertices, vertices);
	//printEdges(numEdges, edges);
	//printEdgeIDTable(numVertices, edgeIDTable, vertices);

  	// Init VProperty
	for(unsigned int i =0; i < numVertices; ++i) {
    	vProperty[i] = vertices[i].prop;
  	}

  	// Init VTempProperty
  	for(unsigned int i =0; i < numVertices; ++i) {
      	vTempProperty[i] = vertices[i].prop;
   	}

  	// TODO: Init VConst
   	// Get more info what this is? Do we get this from the data? Is this even needed? For now it does not looks like it.
	std::cout << "initializeDSM: done preprocessing data!" << std::endl;
}



/*
 * Read graph input data from a text file.
 */

void readGTgraphFile(const char* filename){
  std::ifstream file;
  std::string line;
  std::string item;
  unsigned int numVertices = 0;
  unsigned int numEdges = 0;
  char delimiter = ' ';
  char comp = 'c';

  file.open(filename);

  while(comp == 'c'){
	getline(file,line);
	std::stringstream ss; // Create a new string stream
	ss.str(line);
	std::getline(ss, item, delimiter);

	comp = item.c_str()[0];
  }

  /*	getline(file,line);
	std::getline(ss, item, delimiter);
	//comp = std::stoll(item);
	comp = item.c_str()[0];
  */
  std::cout << "comp borde vara p: " << comp << std::endl;
	if(comp == 'p'){
	  std::stringstream ss; // Create a new string stream
	  ss.str(line);
	  std::getline(ss, item, delimiter);
	  std::getline(ss, item, delimiter);
	  std::getline(ss, item, delimiter);
	  numVertices = std::stoll(item); // get number of vertices
	  std::cout << "numVertices: " << numVertices << std::endl;
	  std::getline(ss, item, delimiter);
	  numEdges = std::stoll(item);   // get number of edges
	  std::cout << "numEdges: " << numEdges << std::endl;
	}

	setupDSM(numVertices,numEdges); // Make system ready to store data.

	std::cout << "after setup: " << std::endl;

	for(unsigned int i=0; i < numEdges; ++i) {
	  getline(file,line);
	  std::stringstream ss; // Create a new string stream
	  ss.str(line);
	  std::getline(ss, item, delimiter);
	  comp = item.c_str()[0];
	  if('a' == comp){
		std::getline(ss, item, delimiter);
		edges[i].srcID = std::stoll(item) - 1; // NOTE: GTGraph IDs start from 1, hence -1
		std::getline(ss, item, delimiter);
		edges[i].dstID = std::stoll(item) - 1; // NOTE: GTGraph IDs start from 1, hence -1
		std::getline(ss, item, delimiter);
		edges[i].weight = std::stoll(item);
	  }
	}
	
	// Initialize all vertex ID's (starting from 0)
	for(unsigned int i=0; i < numVertices; ++i){
	  	vertices[i].ID = i;
	}

	// Update startingNodes IDs from settings file
	for(unsigned int i = 0; i < numberOfStartingNodes; ++i) {
		startingNodes[i] = startingNodes[i] - 1;
	}

	printVertices(numVertices, vertices);

	file.close(); // Closes file

	std::cout << "file closed" << std::endl;
	initializeDSM(numVertices, numEdges); // Organize data in argo.
}

/*
 * Sets up the edgeIDTable. 
 * For each vertex in vertices, the edgeIDTable will contain the edge ID of the
 * first occurring edge in edges that has that vertex as source ID.
 * If a vertex has no such edge in edges, the edgeIDTable contains a null value
 * (0) for that vertex to indicate this.
 */
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
}

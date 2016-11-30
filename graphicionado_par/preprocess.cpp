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
#include "pipelines.hpp"

//Init value on starting nodes for different algorithms used.
void initAlgorithmProperty(unsigned int numVertices, unsigned int numEdges) {
	if(graphAlgorithm == "BFS"){ //Check if BFS is used
		// Init all vertex properties for BFS
		for(unsigned int j=0; j < NODES; ++j){
			for(unsigned int i=0; i < totalVertexCount[j]; ++i){
				// Property can't be larger than totalVertexCount
				vertices[j][i].prop.property = numVertices;
			}
		}
	}

		//Set their property to 0. //Can be optimized. The id is key to which stream so no need to check everything.
	for(unsigned int k=0; k < NODES; ++k){
		for(unsigned int i = 0; i < activeVertexCount[k]; ++i){
			for(unsigned int j = 0; j < totalVertexCount[k]; ++j) {
				if(startingNodes[i] == vertices[k][j].ID) { // startingNodes are ID's of active nodes
					vertices[k][j].prop.property = 0;
				}
			}
		}
	}


	if(graphAlgorithm == "SSSP"){ //Check if SSSP is used
		// Init all vertex properties for SSSP
		for(unsigned int j=0; j < NODES; ++j){
			for(unsigned int i=0; i < totalVertexCount[j]; ++i){
				// Each vertex property is set to the maximum allowed double value
				vertices[j][i].prop.property = std::numeric_limits<double>::max();
			}
		}

		//Set their property to 0.
		for(unsigned int k=0; k < NODES; ++k){
			for(unsigned int i = 0; i < activeVertexCount[k]; ++i){
				for(unsigned int j = 0; j < totalVertexCount[k]; ++j) {
					if(startingNodes[i] == vertices[k][j].ID) { // startingNodes are ID's of active nodes
						vertices[k][j].prop.property = 0;
					}
				}
			}
		}
	}

	if(graphAlgorithm == "PR"){ //Check if Page rank is used

		//Set their property to 0.3
		for(unsigned int k=0; k < NODES; ++k){
			for(unsigned int j = 0; j < totalVertexCount[k]; ++j) {
				vertices[k][j].prop.property = 0.3;// From GraphMat
			}
		}


		//Const array gonna be used to keep degrees. Degree is number of edges away.
			for(unsigned int i = 0; i < numVertices; ++i){ //i = vertex id
				unsigned int stream = i % NODES;
				unsigned int counter = 0; // Counter to keep track of how many edges
				unsigned int startPos = edgeIDTable[i] - 1; // StartPos in edges where it start

				if(edgeIDTable[i] == 0){
					//Does not have edges
					vConst[stream][startPos].property = 0;
					continue;
				}
				for(unsigned int j = startPos; j < totalEdgeCount[stream]; ++j){
					if(edges[stream][j].srcID == i){
						counter++;
					} 
					else {
						//No more edges to count
						break;
					}
				}
				vConst[stream][startPos].property = counter;
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

	vertices = argo::conew_array<Vertex*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		vertices[i] = argo::conew_array<Vertex>(ceil(numVertices/NODES));
	} 

	activeVertex = argo::conew_array<Vertex*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		activeVertex[i] = argo::conew_array<Vertex>(ceil(numVertices/NODES));
	}

	edges = argo::conew_array<Edge*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		edges[i] = argo::conew_array<Edge>(numEdges); // TODO: NEED to be optimized, should not be needed to keep entire space for number of edges times NODES.
	} 
	
    edgeIDTable = argo::conew_array<unsigned int>(numVertices); // might need to change the structure of this one.

	vProperty = argo::conew_array<VertexProperty*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		vProperty[i] = argo::conew_array<VertexProperty>(ceil(numVertices/NODES));
	}
	
	vTempProperty = argo::conew_array<VertexProperty*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		vTempProperty[i] = argo::conew_array<VertexProperty>(ceil(numVertices/NODES));
	}

	vConst = argo::conew_array<VertexProperty*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		vConst[i] = argo::conew_array<VertexProperty>(ceil(numVertices/NODES));
	}

	outputQueue = argo::conew_array<DataCrossbar*>(NODES);
	for(unsigned int i = 0; i < NODES; ++i){
		vConst[i] = argo::conew_array<VertexProperty>(ceil(numVertices/NODES));
	}

	activeVertexCount = argo::conew_array<unsigned int>(NODES);

	totalVertexCount = argo::conew_array<unsigned int>(NODES);

	totalEdgeCount = argo::conew_array<unsigned int>(NODES);

	initPipelines(); // Initialize locks etc. for the pipelines

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
	for(unsigned int i = 0; i < NODES; ++i){
		std::sort(edges[i],&edges[i][totalEdgeCount[i]],edgeIDCompare);
	}
	
	// Sort vertices after ID to make sure the edge ID table correspond to correct node.
	for(unsigned int i = 0; i < NODES; ++i){
		std::sort(vertices[i],&vertices[i][totalVertexCount[i]],vertexIDCompare);
	}

	// Init activeVertexCount
	if(isAllVerticesActive){ //If settings are set to use all vertices as active
	  	for(unsigned int i = 0; i < NODES; ++i){
	  		activeVertexCount[i] = totalVertexCount[i];
	  	}
	}
	else{ // Setting case where a subset of vertices are active.
		for(unsigned int i = 0; i < numberOfStartingNodes; ++i){	
			unsigned int stream = startingNodes[i] % NODES;
			activeVertexCount[stream]++;
		}	
	}

  	// Init EdgeIDTable
	setupEIT(numVertices, numEdges, edgeIDTable);
	
	// Init starting nodes depending on algorithm used.
	initAlgorithmProperty(numVertices,numEdges);

	// Init ActiveVertices  //XXX
	if(isAllVerticesActive){ //If settings are set to use all vertices as active
		for(unsigned int i =0; i < NODES; ++i) {
			for(unsigned int j=0; j < totalVertexCount[i]; ++j){
				activeVertex[i][j] = vertices[i][j];	
			}
	  	}
	}
	else{ 
		// Setting case where it is given active starting nodes
		// Initialize the starting active vertices
		unsigned int counterAV[NODES];

		// Initialize the local array with 0's
		for(unsigned int i = 0; i < NODES; ++i) {
			counterAV[i] = 0;
		}

		// Initialize the activeVertex array
		for(unsigned int i = 0; i < numberOfStartingNodes; ++i) {
			unsigned int stream = startingNodes[i] % NODES;
	    	activeVertex[stream][counterAV[stream]] = vertices[stream][startingNodes[i]]; // Set active Vertex from startingNodes that hold ID of what vertices.
			counterAV[stream] = counterAV[stream] + 1;
	  	}

		// Sort the activeVertex array by ID
	  	for(unsigned int j=0; j < NODES; ++j){
	  			std::sort(activeVertex[j],&activeVertex[j][activeVertexCount[j]],vertexIDCompare); // Sort by ID
	  	}

		// Free local node memory usage for init active vertices
		delete[] startingNodes;
	}

  	// Init VProperty
	for(unsigned int j=0; j < NODES; ++j){
		for(unsigned int i =0; i < totalVertexCount[j]; ++i) {
	    	vProperty[j][i] = vertices[j][i].prop;
	  	}
  	}

  	// Init VTempProperty
  	for(unsigned int j=0; j < NODES; ++j){
	  	for(unsigned int i =0; i < totalVertexCount[j]; ++i) {
	      	vTempProperty[j][i] = vertices[j][i].prop;
	   	}
   	}

  	// TODO: Init VConst
   	// Get more info what this is? Do we get this from the data? Is this even needed? For now it does not looks like it.
	std::cout << "initializeDSM: done preprocessing data!" << std::endl;
}

/* readHeader(filename)
 * Read the graph text file header information (number of edges and vertices)
 * and set up the ArgoDSM allocations using setupDSM().
 */
void readHeader(const char* filename) {
	std::ifstream file;
	std::string line;
	std::string item;
	unsigned int numVertices = 0;
	unsigned int numEdges = 0;
	char delimiter = ' ';
	char comp = 'c';

	file.open(filename);

	// Skip all the lines starting with the character 'c'
	while(comp == 'c'){
		getline(file,line);
		std::stringstream ss; // Create a new string stream
		ss.str(line);
		std::getline(ss, item, delimiter);

		comp = item.c_str()[0];
	}

	// Read the line starting with the character 'p' containing info. about number of vert/edges
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

	// Close the file
	file.close();

	setupDSM(numVertices,numEdges); // Allocate space in ArgoDSM
}

/* readData(filename)
 * Read the edge data from the graph text file (filename) and initialize the allocated
 * array(s) using the data read.
 */
void readData(const char* filename) {
	std::ifstream file;
	std::string line;
	std::string item;
	unsigned int numVertices = 0;
	unsigned int numEdges = 0;
	char delimiter = ' ';
	char comp = 'c';

	file.open(filename);

	// Skip all the lines starting with the character 'c'
	while(comp == 'c'){
		getline(file,line);
		std::stringstream ss; // Create a new string stream
		ss.str(line);
		std::getline(ss, item, delimiter);

		comp = item.c_str()[0];
	}

	// Read the line starting with the character 'p' containing info. about number of vert/edges
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

	// Read all the coming lines, which contains edge data (srcID, dstID, weight)
	for(unsigned int i=0; i < numEdges; ++i) {
		getline(file,line);
		std::stringstream ss; // Create a new string stream
		ss.str(line);
		std::getline(ss, item, delimiter);
		comp = item.c_str()[0];
		if('a' == comp){ 
			std::getline(ss, item, delimiter);
		
			unsigned int stream = (std::stoll(item) - 1) % NODES;
			
			std::cout << "@@@ readData: stream: " << stream << std::endl;

			//Edge* currentEdges = edges[stream];
			//Edge currentEdge;
			//currentEdge = currentEdges[totalEdgeCount[stream]];
		
			unsigned int src = std::stoll(item);
			std::getline(ss, item, delimiter);
			unsigned int dst = std::stoll(item);
			std::getline(ss, item, delimiter);
			unsigned int weight = std::stoll(item);
			
			std::cout << "@@@ readData: " << src-1 << "->" << dst-1 << " : " << weight << std::endl;
			edges[stream][totalEdgeCount[stream]].srcID = src-1;
			edges[stream][totalEdgeCount[stream]].dstID = dst-1;
			edges[stream][totalEdgeCount[stream]].weight = weight;

			totalEdgeCount[stream]++;
		}
	}

	// Initialize all vertex ID's (starting from 0)
	for(unsigned int i=0; i < numVertices; ++i){
		unsigned int stream = i % NODES;
	  	vertices[stream][totalVertexCount[stream]].ID = i;
	  	totalVertexCount[stream]++;
	}

	// Update startingNodes IDs from settings file
	for(unsigned int i = 0; i < numberOfStartingNodes; ++i) {
		startingNodes[i] = startingNodes[i] - 1;
	}

	// Close the file
	file.close();

	std::cout << "file closed" << std::endl;
	
	initializeDSM(numVertices, numEdges); // Initialize the data in the allocated ArgoDSM memory
}

/*
 * Read graph data from a text file and initialize the arrays
 */
void readGTgraphFile(const char* filename){
	// All nodes needs to run the collective allocations
	readHeader(filename); // Read header info. and set up ArgoDSM with collective allocations
	
	argo::barrier();
	
	// Only one node needs to read the edge data and initialize the arrays with data
	if(argo::node_id() == 0) { 
		readData(filename); // Read edge data and save the information in the array(s)
		
		// DEBUG: print the arrays to confirm the data is written correctly
		for(unsigned int i = 0; i < NODES; ++i) {
			// printEdges(totalEdgeCount[i], edges[i]); // Print the edges for each node
			// printEdgeIDTable(totalVertexCount[i], edgeIDTable, vertices[i]);
			// printVertices(totalVertexCount[i], vertices[i]);
			// printVerticesProperties(totalVertexCount[i], vertices[i], vProperty[i]);
		}
	}

	argo::barrier(); // Wait for all nodes/threads to finish the reading
}

/*
 * Sets up the edgeIDTable. 
 * For each vertex in vertices, the edgeIDTable will contain the edge ID of the
 * first occurring edge in edges that has that vertex as source ID.
 * If a vertex has no such edge in edges, the edgeIDTable contains a null value
 * (0) for that vertex to indicate this.
 */

//TEST THIS FUNCTION TO SEE IF IT DO WHAT IT SUPPOSE TO DO.
void setupEIT(unsigned int numVertices, unsigned int numEdges, unsigned int* edgeIDTable){
	unsigned int edgesPosition[NODES];
	unsigned int previousID[NODES];
	
	// Initialize the local array(s) with 0's because they point to the stack
	for(unsigned int i = 0; i < NODES; ++i) {
		edgesPosition[i] = 0;
		previousID[i] = 0;
	}

	for(unsigned int i = 0; i < numVertices; ++i) {
		unsigned int stream = i % NODES;

		for(unsigned int j = edgesPosition[stream]; j < totalEdgeCount[stream]; ++j){	
			
			// std::cout << "@@@ setupEIT: loop j: " << j << std::endl;

			//Base case
			if(j == 0){
				//Exist edges for this vertex
				if(edges[stream][j].srcID == i){
					//Progress through number of edges with same source.
					continue;
				}
				else {
					//No more edges found.
					edgeIDTable[i] = edgesPosition[stream] + 1;
					previousID[stream] = i;
					edgesPosition[stream] = j;
					break;
				}
			}
			//Normal case
			else if(i == previousID[stream]+1){
				//Exist edges for this vertex
				if(edges[stream][j].srcID == i){
					//Progress through number of edges with same source.
					continue;
				}
				else {
					//No more edges found.
					edgeIDTable[i] = edgesPosition[stream] + 1;
					previousID[stream] = i;
					edgesPosition[stream] = j;
					break;
				}
			}
			else{
				//There is no edges for this vertex
				edgeIDTable[i] = 0;
				previousID[stream] = i;
			}
		}
    }



}

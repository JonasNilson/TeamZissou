
#include <iostream> // Used for output prints.
#include "graphicionado.hpp" // Data structures for graph problems
#include "loadSettings.hpp"


// Test behavior for different search logarithms.
void runTests(){
	std::cout << "Starting tests..." << std::endl;

	// Check algorithm flag if BFS is being used.
	if(graphAlgorithm == "BFS"){
		// Test Behavior of BFS 
		std::cout << "Run BFS tests..." << std::endl;
		if(testBFS() == 1){
			std::cout << "BFS tests: FAIL" << std::endl;
		}
		std::cout << "BFS tests: PASSED" << std::endl;
	}
}

// Test BFS behavior
int testBFS(int argc, char *argv[]){
	readData(argc,argv) // Init reading of graph etc.
	testInitDataOrginization(); // Check data ordering in current session
	/* PRE GRAPHICIONADO TESTS */




	graphicionado(); // Run the algorithm
	/* POST GRAPHICIONADO TESTS */




	terminateProgram(); // Cleanup when program has finished.
	return 0;
} 


// Test to see if all structures are ordered as they should be for all algorithms.
int testInitDataOrginization(){
	int errorCode = 0; // if 0 then everything is fine otherwise 1 if something went wrong.

	// Check if vertices is not in order
	for(unsigned int i = 1; i < totalVertexCount; ++i) {
		if(vertices[i-1].ID > vertices[i]){
			errorCode = 1;
			std::cout << "Vertices ordering: FAIL" << std::endl;
			break;
		} 
	}
	if(errorCode == 0) std::cout << "Vertices ordering: PASSED" << std::endl;

	//Check that vTempProperty is same as vProperty
	for(unsigned int i = 0; i < totalVertexCount; ++i) {
		if(vProperty[i] != vTempProperty[i]){
			errorCode = 1;
			std::cout << "vTempProperty and vProperty get initialization: FAIL" << std::endl;
			break;
		}
	}
	if(errorCode == 0) std::cout << "vTempProperty and vProperty get initialization: PASSED" << std::endl;

	//Check active Vertices ordering
	for(unsigned int i = 1; i < activeVertexCount; ++i) {
		if(activeVertex[i-1].ID > activeVertex[i]){
			errorCode = 1;
			std::cout << "Active vertices ordering: FAIL" << std::endl;
			break;
		} 
	}
	if(errorCode == 0) std::cout << "Active vertices ordering: PASSED" << std::endl;

	// Check if vertices correspond to correct EdgeIDTable positioning.
	Vertex v;
	for(unsigned int i = 0; i < totalVertexCount; ++i) {
		v = vertices[i];
		unsigned int eID = edgeIDTable[v.ID];
		
		if(eID == 0){
			// This vertex do not have any edges.
			continue; // Skip to next iteration
		}
		eID = eID - 1; //The IDs in edgeIDTable is shifted by 1 to give room to use 0/NULL for nodes without edges.
		if(v.ID != edges[eID].srcID){
			errorCode = 1;
			std::cout << "EdgeIDTable position: FAIL" << std::endl;
			break;
		}
	}
	if(errorCode == 0) std::cout << "EdgeIDTable position: PASSED" << std::endl;
	
	// Check if edges is correctly positioned
	//TODO: Variable length of edges does not exist. Might add it later if we feel its necessary test. For now I let it wait

	// How data is stored in memory check
	if(errorCode == 1){
		std::cout << "Test data organization: FAIL" << std::endl;
	}
	std::cout << "Test data organization: PASSED" << std::endl;
	return errorCode;
}
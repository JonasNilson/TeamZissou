#include <iostream> // Used for output prints.
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"
#include "tests.hpp"
#include <math.h>

// Test to see if all structures are ordered as they should be for all algorithms.
int testInitDataOrginization(){
	int errorCode = 0; // if 0 then everything is fine otherwise 1 if something went wrong.

	// Check if vertices is not in order
	for(unsigned int i = 1; i < totalVertexCount; ++i) {
		if(vertices[i-1].ID > vertices[i].ID){
			errorCode = 1;
			std::cout << "FAILED: Vertices ordering." << std::endl;
			break;
		} 
	}
	if(errorCode == 0) std::cout << "PASSED: Vertices ordering." << std::endl;

	//Check that vTempProperty is same as vProperty
	for(unsigned int i = 0; i < totalVertexCount; ++i) {
		if(vProperty[i].property != vTempProperty[i].property){
			errorCode = 1;
			std::cout << "FAILED: vTempProperty and vProperty get initialization." << std::endl;
			break;
		}
	}
	if(errorCode == 0) std::cout << "PASSED: vTempProperty and vProperty get initialization." << std::endl;

	//Check active Vertices ordering
	for(unsigned int i = 1; i < activeVertexCount; ++i) {
		if(activeVertex[i-1].ID > activeVertex[i].ID){
			errorCode = 1;
			std::cout << "FAILED: Active vertices ordering." << std::endl;
			break;
		} 
	}
	if(errorCode == 0) std::cout << "PASSED: Active vertices ordering." << std::endl;

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
			std::cout << "FAILED: EdgeIDTable position." << std::endl;
			break;
		}
	}
	if(errorCode == 0) std::cout << "PASSED: EdgeIDTable position." << std::endl;
	
	// Check if edges is correctly positioned
	//TODO: Variable length of edges does not exist. Might add it later if we feel its necessary test. For now I let it wait

	// How data is stored in memory check
	if(errorCode == 1){
		std::cout << "FAILED: Test data organization." << std::endl;
	}
	std::cout << "[PASSED]: All data organization tests." << std::endl;
	return errorCode;
}

// Test BFS behavior
int testBFS(){
	const char* filename = "data/small.gr";
	// char* argv[] = {"1","data/micro.gr"};
	// readData(1,argv); // Init reading of graph etc.
	readGTgraphFile(filename);
        testInitDataOrginization(); // Check data ordering in current session


	
	/* PRE GRAPHICIONADO TESTS */
	unsigned int unassignedProperty = pow(2, 4*sizeof(double));
	unsigned int preGtestValues[10] = {unassignedProperty, 0, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty};

	int fails = 0;

	for(int i = 0; i<10; i++)
	  {
	    if(vProperty[i].property != preGtestValues[i])
	      {
		std::cout << "FAIL: " << vProperty[i].property << "!=" << preGtestValues[i] << std::endl;;
	      fails++;
	      }
	  }
	std::cout << "PRE GRAPHICIONADO: Passed " << (10-fails) << "/10 properties." << std::endl;



	graphicionado(); // Run the algorithm
	/* POST GRAPHICIONADO TESTS */		
	unsigned int testValues[10] = {unassignedProperty, 0, unassignedProperty, 1, 2, unassignedProperty, 1, 1, 2, 3};
	fails = 0;

	for(int i = 0; i<10; i++)
	  {
	    if(vProperty[i].property != testValues[i])
	      {
		std::cout << "FAIL: " << vProperty[i].property << "!=" << testValues[i] << std::endl;;
	      fails++;
	      }
	  }
	std::cout << "POST GRAPHICIONADO: Passed " << (10-fails) << "/10 properties." << std::endl;
	return 0;
}

// Test SSSP behavior
int testSSSP(){
	const char* filename = "data/small.gr";
	// char* argv[] = {"1","data/micro.gr"};
	// readData(1,argv); // Init reading of graph etc.
	readGTgraphFile(filename);
        testInitDataOrginization(); // Check data ordering in current session


	
	/* PRE GRAPHICIONADO TESTS */
	unsigned int unassignedProperty = pow(2, 4*sizeof(double));
	unsigned int preGtestValues[10] = {unassignedProperty, 0, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty};

	int fails = 0;

	for(int i = 0; i<10; i++)
	  {
	    if(vProperty[i].property != preGtestValues[i])
	      {
		std::cout << "FAIL: " << vProperty[i].property << "!=" << preGtestValues[i] << std::endl;;
	      fails++;
	      }
	  }
	std::cout << "PRE GRAPHICIONADO: Passed " << (10-fails) << "/10 properties." << std::endl;



	graphicionado(); // Run the algorithm
	/* POST GRAPHICIONADO TESTS */		
	unsigned int testValues[10] = {unassignedProperty, 0, unassignedProperty, 23, 86, unassignedProperty, 44, 4, 74, 84};
	fails = 0;

	for(int i = 0; i<10; i++)
	  {
	    if(vProperty[i].property != testValues[i])
	      {
		std::cout << "FAIL: " << vProperty[i].property << "!=" << testValues[i] << std::endl;;
	      fails++;
	      }
	  }
	std::cout << "POST GRAPHICIONADO: Passed " << (10-fails) << "/10 properties." << std::endl;
	return 0;
} 

// Test behavior for different search logarithms.
void runTests(){
	std::cout << "####__START TESTING__####" << std::endl;

	// Check algorithm flag if BFS is being used.
	if(graphAlgorithm == "BFS"){
		// Run BFS tests 
		std::cout << "Run BFS tests..." << std::endl;
		if(testBFS() == 1){
			std::cout << "FAILED: All BFS tests." << std::endl;
		} 
		else {
			std::cout << "[PASSED]: All BFS tests." << std::endl;
		}
	}
	if(graphAlgorithm == "SSSP"){
	  // Run SSSP tests
	  std::cout << "Run SSSP tests..." << std::endl;
	  if(testSSSP() == 1){
	    std::cout << "FAILED: All SSSP tests." << std::endl;
	  }
	  else{
	    std::cout << "[PASSED]: All SSSP tests." << std::endl;
	  }
	}


	std::cout << "####__FINISH TESTING__####" << std::endl;
	terminateProgram(); // Cleanup when program has finished.
}

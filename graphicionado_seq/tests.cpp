#include <iostream> // Used for output prints.
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"
#include "tests.hpp"
#include <math.h>
#include <float.h>

// Test to see if all structures are ordered as they should be for all algorithms.
int testInitDataOrginization(){
	int errorCode = 0; // if 0 then everything is fine otherwise 1 if something went wrong.

	// Check if vertices are not in order
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
	Edge e = edges[0];
	for(unsigned int i = 1; i < totalEdgeCount; ++i) {
	  if((e.srcID > edges[i].srcID) || ((e.srcID == edges[i].srcID) && (e.dstID > edges[i].dstID)))
	    {
	      errorCode = 1;
	      std::cout << "FAILED: Edge position." << std::endl;
	      break;
	    }
	  e = edges[i];
	}

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
	readGTgraphFile(filename);

        if(testInitDataOrginization() == 1)
	  return -1; // Check data ordering in current session
	
	/* PRE GRAPHICIONADO TESTS */
	unsigned int unassignedProperty = totalVertexCount; // Initial value for properties.
	unsigned int preGraphicionadoV[10] = {unassignedProperty, 0, unassignedProperty, unassignedProperty, unassignedProperty
					   , unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty
					   , unassignedProperty};
	int preFails = 0; // Number of fails.

	for(int i = 0; i<10; i++) if(!assertUnsignedInt(vProperty[i].property, preGraphicionadoV[i])) preFails++;
	std::cout << "PRE GRAPHICIONADO: Passed " << (10-preFails) << "/10 properties." << std::endl;

	graphicionado(); // Run the algorithm
	/* POST GRAPHICIONADO TESTS */		
	unsigned int postGraphicionadoV[10] = {unassignedProperty, 0, unassignedProperty, 1, 2, unassignedProperty, 1, 1, 2, 3};
	int postFails = 0;

	for(int i = 0; i<10; i++) if(!assertUnsignedInt(vProperty[i].property, postGraphicionadoV[i])) postFails++;
	std::cout << "POST GRAPHICIONADO: Passed " << (10-postFails) << "/10 properties." << std::endl;
	return preFails+postFails;
}

// Test SSSP behavior
int testSSSP(){
	const char* filename = "data/small.gr";
	readGTgraphFile(filename);
        
	if(testInitDataOrginization() == 1)
	  return -1; // Check data ordering in current session
	
	/* PRE GRAPHICIONADO TESTS */
	double unassignedProperty = DBL_MAX;
	double preGraphicionadoV[10] = {unassignedProperty, 0, unassignedProperty, unassignedProperty, unassignedProperty
				     , unassignedProperty, unassignedProperty, unassignedProperty, unassignedProperty
				     , unassignedProperty};
	int preFails = 0;

	for(int i = 0; i<10; i++) if(!assertDouble(vProperty[i].property, preGraphicionadoV[i])) preFails++;
	std::cout << "PRE GRAPHICIONADO: Passed " << (10-preFails) << "/10 properties." << std::endl;

	graphicionado(); // Run the algorithm
	/* POST GRAPHICIONADO TESTS */		
	double postGraphicionadoV[10] = {unassignedProperty, 0, unassignedProperty, 23, 86, unassignedProperty, 44, 4, 74, 84};
	int postFails = 0;

	for(int i = 0; i<10; i++) if(!assertDouble(vProperty[i].property, postGraphicionadoV[i])) postFails++;
	std::cout << "POST GRAPHICIONADO: Passed " << (10-postFails) << "/10 properties." << std::endl;
	return preFails+postFails;
} 

// Test behavior for different search logarithms.
void runTests(){
	std::cout << "####__START TESTING__####" << std::endl;
	int fails = 0;
	// Check algorithm flag if BFS is being used.
	if(graphAlgorithm == "BFS"){
	  // Run BFS tests 
	  std::cout << "Run BFS tests..." << std::endl;
	  fails = testBFS();
	  switch(fails)
	    {
	    case -1: std::cout << "FAILED: Initialization" << std::endl;
	      break;
	    case 0: std::cout << "[PASSED]: All BFS tests." << std::endl;
	      break;
	    default: std::cout << "FAILED: " << fails << " BFS assertions." << std::endl;
	    }
	}
	if(graphAlgorithm == "SSSP"){
	  // Run SSSP tests
	  std::cout << "Run SSSP tests..." << std::endl;
	  fails = testSSSP();
	  switch(fails)
	    {
	    case -1: std::cout << "FAILED: Initialization" << std::endl;
	      break;
	    case 0: std::cout << "[PASSED]: All SSSP tests." << std::endl;
	      break;
	    default: std::cout << "FAILED: " << fails << " SSSP assertions." << std::endl;
	    }
	}

	std::cout << "####__FINISH TESTING__####" << std::endl;
	terminateProgram(); // Cleanup when program has finished.
}

bool assertUnsignedInt(unsigned int a, unsigned int b){
  bool statement = (a == b);
  if(statement) std::cout << "PASS: " << a << "=" << b << std::endl;
  else std::cout << "FAIL: " << a << "!=" << b << std::endl;
  return statement;
}

bool assertDouble(double a, double b){
  bool statement = (a == b);
  if(statement) std::cout << "PASS: " << a << "=" << b << std::endl;
  else std::cout << "FAIL: " << a << "!=" << b << std::endl;
  return statement;
}

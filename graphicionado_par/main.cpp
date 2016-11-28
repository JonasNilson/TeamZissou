/**
 * Sequential implementation of graphicionado
 */
#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include <array>
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"
#include "test_functions.hpp"
#include "tests.hpp"
#include "pipelines.hpp"

// Global variable declaration
unsigned int THREADS = 4; // Set number of threads
unsigned int NODES = 2;

Vertex** vertices; // All vertices in the graph
Vertex** activeVertex; 

Edge** edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty** vProperty; // property of nodes.
VertexProperty** vTempProperty; // new vProperty that been changed
VertexProperty** vConst; 

unsigned int* totalVertexCount; // Number of nodes in the system.
unsigned int* activeVertexCount; // Number of active nodes in the system.
unsigned int* totalEdgeCount;

// Make all cleanups needed before closing the program.
void terminateProgram(){
    std::cout << "Shutting down program! \n";
	cleanupPipelines(); // Cleanup the allocations from pipelines
    argo::finalize(); // Cleanup for this node when program has finished.
}

/*
* readData take data and init all data in the system so graphicionado can run.
*/
int readData(int argc, char *argv[]){
  if(argc>1) {
	const std::string tests = "tests";
    if(argv[1] == tests){
      runTests();
      return 2; // Return 2 when running tests
    }
    //Init all data organization 
    std::cout << "Reading graph from textfile: " << argv[1] << std::endl;
	readGTgraphFile(argv[1]);
  }
  else {
    // Error no argument with filename
    std::cout << "Missing argument: graph file. \n";
    terminateProgram();
    return 1;
  }
  return 0;
}

// ID is for node/thread
void graphicionado(unsigned int id){
    
  while(activeVertexCount != 0) {
    //A process edge
    processingPhaseSourceOriented(id); //
    argo::barrier(); // Synchronization
    processingPhaseDestinationOriented(id); //

    //B Apply Phase
    applyPhase(id);

    //Settings check if we should use max iteration implementation or not
    if(maxIterations != 0){ //If setting is set to 0 it will use infinity iteration possibility
      maxIterations--;
      if(maxIterations == 0){
        std::cout << "All iterations are done!" << std::endl;
        //Only enter this stage if it reached the maximum iteration count
        break; // break the entire loop
      }
    }
  }
}


/**
   Information about graphicionado
   * EdgeIDTable - is constructed and stored in memory. It is an array that store edge id of the first edge of each vertex. Sorted by srcID and then dstID.
   * VProperty - is an array that store vertex property (Unknown for now)
   * VTempProperty - same as VProperty but updated from the function reduce.
   * Edges - array of all edges. Is indexed (sorted) by edge ID E.g of an edge: (srcID, dstID, weight) (NOT SURE VISULISE TOMORROW)
   */
int main(int argc, char *argv[]){
  /* 
     Set up the argo environment, caches and global memory. 
     Init the total space that is shared between all nodes. 
  */
  argo::init(128 * 1024 * 1024); 

  // Local variable declaration
  unsigned int id = argo::node_id(); // get this node unique index number starting from 0
  //int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.

  // Make sure only node 0 run 
  if(id == 0){
    // Load the configuration settings from file (settings.cfg)
    loadSettings();
  }
  argo::barrier();
  // readData take input and organize the input
  int code = readData(argc,argv);
  std::cout << "DONE READING DATA: node id: " << id << std::endl;
  if(code != 0){
      if(code == 2){
		  // test run
		  return 0;
      }
      //Exist program something went wrong with reading of Data.
      return 1;
  }  
  
  std::cout << "BEFORE GRAPHICIONADO: node id: " << id << std::endl;
  argo::barrier(); // Synchronize after node 0 is done with the initialization.
  graphicionado(id);
  std::cout << "AFTER GRAPHICIONADO: node id: " << id << std::endl;
  argo::barrier(); // Synchronize before cleaning up
  //printVerticesProperties(totalVertexCount, vertices, vProperty); //Debug prints too see behavior
  terminateProgram(); // Cleanup for this node when program has finished.
  return 0;
}
